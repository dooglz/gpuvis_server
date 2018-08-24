//#define MG_ENABLE_HTTP_WEBSOCKET 0
#include <mongoose.h>
#include <chrono>
#include <iostream>
#include <string>
extern void test();
static const char *s_http_port = "8000";
static uintmax_t reqs;
static auto last_req = std::chrono::steady_clock::now();

std::string intToIp(const unsigned long &ip) {
  char buff[10];
  snprintf(buff, sizeof(buff), "%d.%d.%d.%d", ip & 0xFF, (ip >> 8) & 0xFF,
           (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
  return std::string(buff);
}

static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    reqs++;
    last_req = std::chrono::steady_clock::now();
    auto *hm = static_cast<struct http_message *>(p);
    std::cout << reqs << '\t' << intToIp(c->sa.sin.sin_addr.s_addr)
              << std::endl;
    const std::string str(R"({"id":1,"error":null})");
    mg_send_head(c, 200, str.length(), "Content-Type: application/json");
    mg_send(c, str.c_str(), str.length());
  }
}

int main() {
//  test();
  struct mg_mgr mgr;
  struct mg_connection *c;
  std::cout << "Start Webserver on port " << s_http_port << std::endl;
  mg_mgr_init(&mgr, nullptr);
  c = mg_bind(&mgr, s_http_port, ev_handler);
  if(!c){
    std::cerr << "Can't Start Webserver" << std::endl;
    return 1;
  }
  mg_set_protocol_http_websocket(c);
  reqs = 0;
  bool go = true;
  std::cout << "Server Running" << std::endl;
  while (go) {
    const auto dt = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - last_req);
    int waittime = 1000;
    if (dt.count() < 2) {
      waittime = 1;
    }
    if (dt.count() > 10) {
      waittime = 3000;
    }
    mg_mgr_poll(&mgr, waittime);
  }
  mg_mgr_free(&mgr);

  return 0;
}
