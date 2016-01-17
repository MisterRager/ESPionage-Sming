HttpServer server;

#include "http_server.hpp"

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

void onIndex(HttpRequest &request, HttpResponse &response) {
  TemplateFileStream *tmpl = new TemplateFileStream("index.html");
  auto &vars = tmpl->variables();
  //vars["counter"] = String(counter);
  response.sendTemplate(tmpl); // this template object will be deleted automatically
}

void http_server_init() {
  server.listen(80);
  http_add_route("/", onIndex);
  server.setDefaultHandler(onFile);

  Serial.println("\r\n=== WEB SERVER STARTED ===");
  Serial.println(WifiStation.getIP());
  Serial.println("==============================\r\n");
}

void http_add_route(const char * route_name, void (*f)(HttpRequest &request, HttpResponse &response)) {
  server.addPath(route_name, f);
}
