A Drupal 7 Webform Component which does the following:

1. Creates a commerce line item type: "Webform fee"
2. Creates a Webform component: "Commerce payment"

Creates rules which do the following:

1. After submitting the Webform, calculates the total cost
2. After checkout is complete:
   * Retrieves the Webform submission id (sid) from the submitted webform
   * Changes the status of the Webform from "unpaid" to "paid"

##FEATURES
* Simple setup for event registrations
* Ajax price calculation for users creating the form
* http://img.skitch.com/20111209-e8am6gm3rma3rpkwsq9ykxyeqg.png

##SETUP
1. Enable the module
2. Optional: create a product
3. Create a webform (either a "Webform" type or any node enabled for webforms)
4. Optionally add a "Select" component to the webform that you want to use as extra fees
5. Add a "Commerce product" component to the webform
   * Select the main product to be added to the webform
   * OR create a new product to be added
   * Set the price(s) for optional items
6. The product will be added to the cart upon submitting the webform
7. *IMPORTANT:* In order to view the new line items, you need to edit the Commerce Cart views to include "Webform extra fee" line items.
   * This is done by editing the "Filter" of each cart view.

##TODO
1. Add form handlers to the "remove" button in Commerce cart/review for the extra line items
2. Add some kind of handler to "edit" the Webform from the cart
3. ~~Add product creation to the Webform component itself - eliminating the need to set up the product separately.~~
4. Update the Cart views on install

