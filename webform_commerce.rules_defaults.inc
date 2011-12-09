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
  
  $rule = rules_reaction_rule();
  $rule->label = t("Update Webform Submission to Paid");
  $rule->active = TRUE;

  $rule
    ->event('commerce_checkout_complete')
    ->action('webform_commerce_update_status', array(
      'commerce_order' => 'commerce_order',
      ));

  $rules['rules_update_webform_to_paid'] = $rule;
 
 return $rules;
}