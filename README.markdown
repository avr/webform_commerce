A Drupal 7 Webform Component which does the following:

1. Creates a commerce line item type: "Webform extra fee"
   * A "component" is also created.  This is currently not used.
2. Creates a webform component: "Commerce payment"

Then has a rule which do the following:

1. After checkout is completed it:
   * Retrieves the webform submission id (sid) from the submitted webform
   * Changes the status of the webform from "unpaid" to "paid"

##FEATURES
* Simple setup for event registrations
* Ajax price calculation for users creating the form
* http://img.skitch.com/20111209-e8am6gm3rma3rpkwsq9ykxyeqg.png

##SETUP
1. Enable the module
2. Create a product
3. Create a webform (either a "Webform" type or any node enabled for webforms)
4. Optionally add a "Select" or "Text" component to the webform that you want to use as extra fees
   * For a "select component", the options must be numeric.  For example:
      "0|No"
      "10|Yes"
5. Add a "Commerce payment" component to the webform
   * Select the main product to be added to the webform
   * Select the items created in step 4 that will affect price
6. The product & extra line items will be added to the cart upon submitting the webform
7. *IMPORTANT:* In order to view the new line items, you need to edit the Commerce Cart views to include "Webform extra fee" line items.
   * This is done by editing the "Filter" of each cart view.

##TODO
1. Add form handlers to the "remove" button in commerce cart/review for the extra line items
2. Add some kind of handler to "edit" the webform from the cart
3. Add product creation to the webform component itself - eliminating the need to set up the product separately.
4. Update the Cart views on install

