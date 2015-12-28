document.App = (function() {
  var uiElementsFn = function (type, id) {
    var elemId = id;
    var elemType = type;
    return function (parentElem) {
      return parentElem.find(elemType + "#" + elemId);
    };
  };

  UI = {
    length: uiElementsFn("input", "length"),
    brightness: uiElementsFn("input", "brightness"),
    red: uiElementsFn("input", "red"),
    green: uiElementsFn("input", "green"),
    blue: uiElementsFn("input", "blue")
  };

  var State = {
    color: {
      brightness: 0,
      red: 0,
      green: 0,
      blue: 0
    },
    length: 72
  };

  var API = {
    color: {
      get: function(colorForm) {
        $.get("/color/get", null, function(color) {
          State.color = color;
          var red = UI.red(colorForm);
          var green = UI.green(colorForm);
          var blue = UI.blue(colorForm);
          var brightness = UI.brightness(colorForm);

          red.val(State.color.red.toString());
          green.val(State.color.green.toString());
          blue.val(State.color.blue.toString());
          brightness.val(State.color.brightness.toString());
        });
      },
      set: function() {
        $.get("/color/set", State.color, function(color) {
          State.color = color;
        });
      }
    },
    length: {
      get: function(lengthForm) {
        $.get("/length/get", null, function(res) {
          State.length = res.length;
          UI.length(lengthForm).val(State.length + "");
        });
      },
      set: function() {
        $.get("/length/set", {length: State.length}, function(res) {
          State.length = res.length;
        });
      }
    }
  };

  return {
    UI: UI,
    State, State,
    API: API,
    initColor: function(colorForm) {
      colorForm.submit(function(ev) {
        var red = UI.red(colorForm);
        var green = UI.green(colorForm);
        var blue = UI.blue(colorForm);
        var brightness = UI.brightness(colorForm);
        State.color = {
          red: Number.parseInt(red.val()),
          green: Number.parseInt(green.val()),
          blue: Number.parseInt(blue.val()),
          brightness: Number.parseInt(brightness.val())
        };
        API.color.set();
        return false;
      });
      API.color.get(colorForm);
    },
    initLength: function(lengthForm) {
      lengthForm.submit(function(ev) {
        var length = UI.length(lengthForm);
        var newVal = length.val();
        State.length = Number.parseInt(newVal);
        API.length.set();
        return false;
      });
      API.length.get(lengthForm);
    }
  };
})();
