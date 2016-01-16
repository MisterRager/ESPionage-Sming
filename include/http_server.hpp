typedef void (*HttpCallback)(HttpRequest &req, HttpResponse &res);

void onFile(HttpRequest &request, HttpResponse &response);
void onIndex(HttpRequest &request, HttpResponse &response);
void http_server_init();
void http_add_route(const char * route_name, HttpCallback f);
