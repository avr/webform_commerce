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
        'product_to_add' => '',
        'price_components' => array(),
        'status' => 'unpaid',
      ),
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

  $form['extra']['pricing']['product_to_add'] = array(
    '#type' => 'textfield',
    '#title' => t('Product to place in the order'),
    '#default_value' => $component['extra']['pricing']['product_to_add'],
    // Here we're using Commerce Product's autocomplete path in an unusual way
    // just to get a product for the form.
    '#autocomplete_path' => 'commerce_product/autocomplete/any/any/any',
    '#required' => TRUE,
  );
  
  $price_components = webform_component_list($node, 'price', FALSE, FALSE);
  
  $form['extra']['pricing']['price_components'] = array(
    '#type' => 'select',
    '#title' => t('Price components'),
    '#options' => $price_components,
    '#default_value' => $component['extra']['pricing']['price_components'],
    '#multiple' => TRUE,
    '#size' => 10,
    '#description' => t('Select the components that contain price values. These can be either "select" or "textfield" items. The total value of these components will be charged to the user.'),
    '#process' => array('webform_component_select'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#all_checkbox' => FALSE,
  );
  
  return $form;
}

/**
 * Validation handler for commerce component.
 */
function _webform_edit_commerce_validate($form, &$form_state) {
  $value = array();
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
  // This is a merely a placeholder element, since pay forms need to be located
  // at a specific location in the form. This will be populated with the actual
  // form in webform_pay_prerender().
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
    if ($data['data'] == 'unpaid') {
      $paid++;
    }
    if ($data['no'] == 'total') {
      $total += $data['data'];
    }
    $submissions++;
  }

  $rows[0] = array(t('Submissions'), ($submissions));
  $rows[1] = array(t('Paid'), $paid);
  $rows[2] = array(t('Total'), commerce_currency_format($total, 'USD'));
  return $rows;
}