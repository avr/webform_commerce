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
        'base_price' => '',
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
  
  $form['extra']['pricing']['base_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Base price'),
    '#description' => t('Enter the base price.'),
    '#default_value' => $component['extra']['pricing']['base_price'],
    '#size' => 10,
    '#maxlength' => 255,
    '#element_validate' => array('element_validate_integer'),
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
    '#description' => t('Select the components that contain price values. The total value of these components will be charged to the user.'),
    '#process' => array('webform_component_select'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#all_checkbox' => FALSE,
    // '#required' => TRUE,
  );

  return $form;
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
  $value = array_intersect_key($value, drupal_map_assoc(array(
    'total',
    'status',
  )));

  return $value;
}