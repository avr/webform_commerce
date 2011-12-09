(function ($) {

  Drupal.behaviors.webformCommerce = {
    attach: function (context, settings) {
      $('.ajax-processed', context).parent().css("overflow", "hidden");
    }
  };

}(jQuery));