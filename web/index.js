$(function() {
  var max_bright = 31;

  var bright_input = $("input#brightness");
  $.get("/brightness", null, function(res) {
    bright_input.val(res.brightness + "");
  });
  bright_input.change(function(){
    var new_val = Math.min(
        Math.max(Number.parseInt(bright_input.val()), 0),
        255
        );

    console.log("New val is ", new_val);

    $.post("/brightness",
        {brightness: new_val},
        function(res) {
          bright_input.val((res.brightness) + "");
          console.log(res);
        }
        );
    return false;
  });

  var ssid_input = $("input#wifi_ssidfm");
  var pass_input = $("input#wifi_passwordfm");

  $("form#wififm").submit(function() {
    $.post("/wifi_credentials",
        {
          wifi_ssid: ssid_input.val(),
          wifi_password: pass_input.val()
        },
        function(res) {
          console.log(res);
        }
        );
    return false;
  });

  $.get("/wifi_credentials", null, function(res) {
    ssid_input.val(res.ssid);
  });

  var radios = $("input.led_type_radio");
  $.get("/led_type", null, function(res) {
    radios.filter("[value!=" + res.type +"]").prop("checked", false);
    radios.filter("[value=" + res.type +"]").prop("checked", true);

    radios.on("change", function() {
      $.post("/led_type", {type: Number.parseInt($("input.led_type_radio:checked").val())});
    });
  });
});
