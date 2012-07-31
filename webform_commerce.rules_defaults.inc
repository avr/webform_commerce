<?php
/**
 * @file
 * webform_commerce.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
 
function webform_commerce_default_rules_configuration() {
  $rules = array();

  $rule = entity_import('rules_config', '{
    "update_webform_to_paid" : {
      "LABEL" : "Update Webform Submission to Paid",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "webform_commerce", "commerce_checkout" ],
      "ON" : [ "commerce_checkout_complete" ],
      "DO" : [
        { "webform_commerce_update_status" : { "commerce_order" : [ "commerce-order" ] } }
      ]
    }
  }');

  $rules['update_webform_to_paid'] = $rule;

  $rule = entity_import('rules_config', '{
    "update_webform_commerce_price" : {
      "LABEL" : "Update price of webform submission",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "webform_commerce", "commerce_product_reference" ],
      "ON" : [ "commerce_product_calculate_sell_price" ],
      "IF" : [
        { "webform_contains_pricing_components" : { "commerce_line_item" : [ "commerce_line_item" ] } }
      ],
      "DO" : [
        { "webform_commerce_calculate_price" : {
            "commerce_line_item" : [ "commerce_line_item" ],
            "component_name" : "webform_fee",
            "round_mode" : "1"
          }
        }
      ]
    }
  }');

  $rules['add_components_to_line_item'] = $rule;

  return $rules;
}