<?php

/**
 * @file
 * Rules integration for webform_commerce.
 *
 * @addtogroup rules
 * @{
 */


/**
* Implements hook_rules_condition_info().
*/
function webform_commerce_rules_condition_info() {
  $conditions = array();
  
  $conditions['webform_contains_pricing_components'] = array(
    'label' => t('Webform contains additional pricing components'),
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line item'),
       ),
    ),
    'group' => t('Webform Commerce'),
    'callbacks' => array(
      'execute' => 'webform_contains_pricing_components',
    ),
  );
  
  return $conditions;
}

/**
 * Rules condition: checks to see if line item has webform pricing components.
 */
function webform_contains_pricing_components($line_item) {
  if (isset($line_item->data['webform_price_components'])) {
    return TRUE;
  };
}


/**
* Implements hook_rules_action_info().
*/
function webform_commerce_rules_action_info() {
  $actions = array();

  $actions['webform_commerce_update_status'] = array(
    'label' => t('Update the status of a webform submission(s).'),
    'parameter' => array(
      'commerce_order' => array(
         'type' => 'commerce_order',
         'label' => t('Order to update'),
      ),
    ),
    'group' => t('Webform Commerce'),
      'callbacks' => array(
        'execute' => 'webform_commerce_update_status',
      ),
  );
  
  
  $actions['webform_commerce_calculate_price'] = array(
    'label' => t('Update price to include extra pricing options.'),
    'parameter' => array(
      'commerce_line_item' => array(
         'type' => 'commerce_line_item',
         'label' => t('Line item to update'),
      ),
      'component_name' => array(
        'type' => 'text',
        'label' => t('Price component type'),
        'description' => t('Price components track changes to prices made during the price calculation process, and they are carried over from the unit price to the total price of a line item. When an order total is calculated, it combines all the components of every line item on the order. When the unit price is altered by this action, the selected type of price component will be added to its data array and reflected in the order total display when it is formatted with components showing. Defaults to base price, which displays as the order Subtotal.'),
        'options list' => 'webform_commerce_price_component_options_list',
        'default value' => 'base_price',
      ),
    ),
    'group' => t('Webform Commerce'),
      'callbacks' => array(
        'execute' => 'webform_commerce_calculate_price',
      ),
  );

  return $actions;
}

/**
 * Options list callback: price component selection list.
 */
function webform_commerce_price_component_options_list() {
  return commerce_price_component_titles();
}

/**
* Rules action: updates an order's webform submissions as paid
*/
function webform_commerce_update_status($order) {
  $sids = $order->data['webform'];

  foreach($sids as $sid) {
    webform_commerce_status_update($sid['sid'], 'paid');
  }
}

function webform_commerce_calculate_price($line_item, $component_name) {
  foreach($line_item->data['webform_price_components'] as $item) {
    $amount = $item['amount'];
    $round_mode = COMMERCE_ROUND_HALF_UP;
    
    if (is_numeric($amount)) {

      // taken from commerce_line_item.rules.inc
      $wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
      $unit_price = commerce_price_wrapper_value($wrapper, 'commerce_unit_price', TRUE);

      // Calculate the updated amount and create a price array representing the
      // difference between it and the current amount.
      $current_amount = $unit_price['amount'];
      $updated_amount = commerce_round($round_mode, $current_amount + $amount);

      $difference = array(
        'amount' => $updated_amount - $current_amount,
        'currency_code' => $unit_price['currency_code'],
        'data' => array(),
      );
      
      // Set the amount of the unit price and add the difference as a component.
      $wrapper->commerce_unit_price->amount = $updated_amount;

      $wrapper->commerce_unit_price->data = commerce_price_component_add(
        $wrapper->commerce_unit_price->value(),
        $component_name,
        $difference,
        FALSE
      );
    }
  
  }
}

/**
 * @}
 */