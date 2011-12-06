(function ($) {

  Drupal.behaviors.webformCommerce = {
    attach: function (context, settings) {
      $('.ajax-fix', context).parent().css("overflow", "hidden");
    }
  };

}(jQuery));