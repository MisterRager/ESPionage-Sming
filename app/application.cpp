#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "apa102.cpp"

struct Color {
  uint8_t brightness;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

Color currentColor = {
  0xFF,
  0,
  99,
  10
};

size_t LED_LENGTH = 200;
HttpServer server;

void updateLights() {
  showColor(
    LED_LENGTH,
    currentColor.brightness,
    currentColor.blue,
    currentColor.green,
    currentColor.red
  );
}

void onFile(HttpRequest &request, HttpResponse &response) {
  String file = request.getPath();
  if (file[0] == '/')
    file = file.substring(1);

  if (file[0] == '.')
    response.forbidden();
  else
  {
    response.setCache(86400, true); // It's important to use cache for better performance.
    response.sendFile(file);
  }
}

void ajaxGetColor(HttpRequest &request, HttpResponse &response) {
  JsonObjectStream* stream = new JsonObjectStream();
  JsonObject& json = stream->getRoot();

  json["brightness"] = currentColor.brightness;
  json["red"] = currentColor.red;
  json["blue"] = currentColor.blue;
  json["green"] = currentColor.green;

  response.sendJsonObject(stream);
}

void ajaxSetColor(HttpRequest &request, HttpResponse &response) {
  int red = request.getQueryParameter("red").toInt();
  int green = request.getQueryParameter("green").toInt();
  int blue = request.getQueryParameter("blue").toInt();

  Serial.printf("\r\nRed: [%d] Green: [%d] Blue: [%d]", red, green, blue);
  currentColor.red = (uint8_t) red;
  currentColor.green = (uint8_t) green;
  currentColor.blue = (uint8_t) blue;

  currentColor.brightness = (uint8_t) request.getQueryParameter("brightness").toInt();

  Serial.printf(
    "\r\nNew values: r[%d] g[%d] b[%d] brightness[%d]",
    currentColor.red,
    currentColor.green,
    currentColor.blue
  );

  updateLights();
  ajaxGetColor(request, response);
}

void ajaxGetLength(HttpRequest &request, HttpResponse &response) {
  JsonObjectStream* stream = new JsonObjectStream();
  JsonObject& json = stream->getRoot();
  json["length"] = LED_LENGTH;

  response.sendJsonObject(stream);
}

void ajaxSetLength(HttpRequest &request, HttpResponse &response) {
  LED_LENGTH = (size_t) request.getQueryParameter("length").toInt();
  ajaxGetLength(request, response);
}

void onIndex(HttpRequest &request, HttpResponse &response) {
  TemplateFileStream *tmpl = new TemplateFileStream("index.html");
  auto &vars = tmpl->variables();
  //vars["counter"] = String(counter);
  response.sendTemplate(tmpl); // this template object will be deleted automatically
}

void startWebServer() {
  server.listen(80);
  server.addPath("/", onIndex);
  server.addPath("/color/set", ajaxSetColor);
  server.addPath("/color/get", ajaxGetColor);
  server.addPath("/length/set", ajaxSetLength);
  server.addPath("/length/get", ajaxGetLength);
  server.setDefaultHandler(onFile);

  Serial.println("\r\n=== WEB SERVER STARTED ===");
  Serial.println(WifiStation.getIP());
  Serial.println("==============================\r\n");
}

void init()
{
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.systemDebugOutput(true); // Allow debug print to serial
  SPI.begin();
  updateLights();

  //Ajax server
  spiffs_mount(); // Mount file system, in order to work with files
  WifiStation.enable(true);
  WifiStation.config(WIFI_SSID, WIFI_PWD);
  WifiAccessPoint.enable(false);

  WifiStation.waitConnection(startWebServer);
}
