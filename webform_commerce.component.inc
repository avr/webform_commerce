<?php

/**
 * @file
 * Webform module pay component.
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_commerce() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'pricing' => array(
        'type' => 'ref',
        'product_to_add' => '',
        'amount' => '',
      ),
      'price_components' => array(),
      'status' => 'unpaid',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_commerce($component) {

  $form = array();

  $form['extra']['description'] = array(
    '#type' => 'value',
    '#value' => '',
  );
  
  $node = node_load($component['nid']);
  
  $form['extra']['pricing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pricing and cost'),
    '#description' => t('Establish the cost of purchasing this webform.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['extra']['pricing']['type'] = array(
    '#type' => 'select',
    '#options' => array('ref' => 'Reference an existing product', 'new' => 'Create a new product'),
    '#default_value' => $component['extra']['pricing']['type'],
  );

  // Create reusable states
  $ref_product = array(
    'visible' => array(
      '#edit-extra-pricing-type' => array('value' => 'ref'),
    ),
  );
  
  $add_product = array(
    'visible' => array(
      '#edit-extra-pricing-type' => array('value' => 'new'),
    ),
    'required' => array(
      '#edit-extra-pricing-type' => array('value' => 'new'),
    ),
  );

  if ($product = commerce_product_load_by_sku($component['extra']['pricing']['product_to_add'])) {
    $amount = commerce_product_calculate_sell_price($product);
    $currency_code = $amount['currency_code'];
  }

  $display_amount = isset($amount) ? $amount['amount'] : 0;
  
  $form['extra']['pricing']['product_to_add'] = array(
    '#type' => 'textfield',
    '#title' => t('Product to place in the order'),
    '#description' => t('This product will be added to the cart when the user clicks the "Submit" button on the form.'),
    '#default_value' => $component['extra']['pricing']['product_to_add'],
    '#autocomplete_path' => 'commerce_product/autocomplete/any/any/any',
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => '_webform_edit_commerce_update_pricing',
      'wrapper' => 'amount',
    ),
    '#states' => $ref_product,
  );
  
  $form['extra']['pricing']['amount'] = array(
    '#type' => 'item',
    '#title' => t('Current base price'),
    '#prefix' => '<div id="amount">',
    '#suffix' => '</div>',
    '#markup' => '<span class="cost">' . commerce_currency_format($display_amount, 'USD') . '</span>',
    '#states' => $ref_product,
  );
  
  
  $types = commerce_product_types();
  
  $form['extra']['pricing']['product_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of product to be created'),
    '#description' => t('New product types can be created !link.', array('!link' => l('here', 'admin/commerce/products/types'))),
    '#options' => drupal_map_assoc(array_keys($types)),
    '#states' => $add_product,
  );
  
  $form['extra']['pricing']['product_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Product title'),
    '#default_value' => t('Webform product'),
    '#states' => $add_product,
  );
  $form['extra']['pricing']['product_sku'] = array(
    '#type' => 'textfield',
    '#title' => t('Product sku'),
    '#default_value' => '',
    '#description' => t('Unique id for this product'),
    '#states' => $add_product,
  );
  
  $form['extra']['pricing']['product_price'] = array(
    '#title' => t('Product price'),
    '#type' => 'textfield',
    '#description' => t('A price in decimal format, without a currency symbol'),
    '#default_value' => '100.00',
    '#states' => $add_product,
  );
  
  $price_components = webform_component_list($node, 'price', FALSE, FALSE);

  $form['extra']['price_components'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional pricing'),
    '#description' => t('Select the components that contain price values. These can be either "select" or "textfield" items. Each of these components will be added to the amount of the base product.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  foreach($price_components as $pc => $pc_title) {
    $extra_price = $component['extra']['price_components'];

    $form['extra']['price_components'][$pc] = array(
      '#type' => 'container',
    );

    $default_value = isset($extra_price[$pc]) ? $extra_price[$pc]['expense'] : NULL;

    $form['extra']['price_components'][$pc]['expense'] = array(
      '#type' => 'checkbox',
      '#title' => $pc_title,
      '#default_value' => $default_value,
    );

    $component_value = $node->webform['components'][$pc];

    if ($component_value['type'] == 'select') {
      //  Reusable states
      $add_price = array(
        'visible' => array(
          '#edit-extra-price-components-' . $pc . '-expense' => array('checked' => TRUE),
        ),
      );

      $options = _webform_select_options($component_value);

      foreach($options as $option => $title) {

        $option_value = isset($extra_price[$pc]) ? $extra_price[$pc]['amount'][$option] : NULL;

        $form['extra']['price_components'][$pc]['amount'][$option] = array(
          '#type' => 'textfield',
          '#title' => $title,
          '#size' => 10,
          '#field_prefix' => "$",
          '#default_value' => $option_value,
          '#states' => $add_price,
        );
      }
    }
  }

  return $form;
}

function _webform_edit_commerce_update_pricing(&$form, $form_state) {
  $commands = array();

  // Adjust the display of the main "Total"
  $product = commerce_product_load_by_sku($form_state['input']['extra']['pricing']['product_to_add']);
  $wrapper = entity_metadata_wrapper('commerce_product', $product);
  $amount = $wrapper->commerce_price->amount->value();
  $currency_code = 'USD';
  
  $commands[] = ajax_command_html("#amount .cost", commerce_currency_format($amount, $currency_code));

  return array('#type' => 'ajax', '#commands' => $commands);
}

function webform_commerce_form_webform_component_edit_form_alter(&$form, &$form_state, $form_id) {
  // This is required in order to return the cost of the refrenced product by ajax.
  form_load_include($form_state, 'inc', 'webform', 'includes/webform.components');
}

/**
 * Render a Webform component to be part of a form.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   If editing an existing submission or resuming a draft, this will contain
 *   an array of values to be shown instead of the default in the component
 *   configuration. This value will always be an array, keyed numerically for
 *   each value saved in this field.
 * @param $filter
 *   Whether or not to filter the contents of descriptions and values when
 *   rendering the component. Values need to be unfiltered to be editable by
 *   Form Builder.
 *
 * @see _webform_client_form_add_component()
 */

/**
 * Implements _webform_render_component().
 */
function _webform_render_commerce($component, $value = NULL, $filter = TRUE) {
  $element = array(
    '#value' => $value,
    '#weight' => $component['weight'],
    '#webform_component' => $component,
    '#theme_wrapper' => array('webform_element'),
  );

  return $element;
}

/**
 * Implements _webform_submit_component().
 */
function _webform_submit_commerce($component, $value) {
  // $value is set in the the custom form validate from webform_commerce.module
  $value = array_intersect_key($value, drupal_map_assoc(array(
    'total',
    'status',
  )));

  return $value;
}

/**
 * Implements _webform_display_component().
 */

/**
 * Display the result of a submission for a component.
 *
 * The output of this function will be displayed under the "Results" tab then
 * "Submissions". This should output the saved data in some reasonable manner.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the webform_submitted_data database table schema.
 * @param $format
 *   Either 'html' or 'text'. Defines the format that the content should be
 *   returned as. Make sure that returned content is run through check_plain()
 *   or other filtering functions when returning HTML.
 * @return
 *   A renderable element containing at the very least these properties:
 *    - #title
 *    - #weight
 *    - #component
 *    - #format
 *    - #value
 *   Webform also uses #theme_wrappers to output the end result to the user,
 *   which will properly format the label and content for use within an e-mail
 *   (such as wrapping the text) or as HTML (ensuring consistent output).
 */
function _webform_display_commerce($component, $value, $format) {
  $element = array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#format' => $format,
    // '#theme' => 'webform_display_address',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#sorted' => TRUE,
    '#webform_component' => $component,
    '#value' => $value,
    'status' => array(
      '#type' => 'item',
      '#title' => t('Status'),
      '#markup' => $value['status'],
    ),
    'amount' => array(
      '#type' => 'item',
      '#title' => t('Amount'),
      '#markup' => commerce_currency_format($value['total'], 'USD'),
    ),
  );
  
  return $element;
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_commerce($component, $value) {
  $output = '';
  $output .= "Status: " . ucfirst($value['status']) . "<br />";
  $output .= "Amount: " . commerce_currency_format($value['total'], 'USD');
  
  return $output;
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_commerce($component, $sids = array(), $single = FALSE) {
  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('no', 'data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid']);

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  $submissions = 0;
  $paid = 0;
  $total = 0;

  $result = $query->execute();
  foreach ($result as $data) {
    if ($data['data'] == 'paid') {
      $paid++;
      if ($data['no'] == 'total') {
        $total += $data['data'];
      }
    }
    $submissions++;
  }

  $rows[0] = array(t('Submissions'), ($submissions));
  $rows[1] = array(t('Paid'), $paid);
  $rows[2] = array(t('Total'), commerce_currency_format($total, 'USD'));
  return $rows;
}