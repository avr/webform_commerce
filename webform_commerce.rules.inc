<?php

/**
 * @file
 * Rules integration for webform_commerce.
 *
 * @addtogroup rules
 * @{
 */

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
  
  return $actions;
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

/**
 * @}
 */