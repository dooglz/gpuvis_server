//#define MG_ENABLE_HTTP_WEBSOCKET 0
#define MG_ENABLE_HTTP_STREAMING_MULTIPART 1
#include <chrono>
#include <iostream>
#include <mongoose.h>
#include <string>
extern void InitBridge();
extern void ShutdownBridge();
std::string inputFile(const std::string &ip);

static const char *s_http_port = "8000";
static uintmax_t reqs;
static auto last_req = std::chrono::steady_clock::now();

std::string intToIp(const unsigned long &ip) {
  char buff[10];
  snprintf(buff, sizeof(buff), "%lu.%lu.%lu.%lu", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
  return std::string(buff);
}

static void ev_handler(struct mg_connection *c, int ev, void *p) {

  static struct mg_serve_http_opts s_http_server_opts;
  s_http_server_opts.document_root = "./static/"; // Serve current directory

  switch (ev) {

  case MG_EV_HTTP_REQUEST: {
    mg_serve_http(c, (http_message *)p, s_http_server_opts);
    break;
    // reqs++;
    // last_req = std::chrono::steady_clock::now();
    // auto *hm = static_cast<struct http_message *>(p);
    // std::cout << reqs << '\t' << intToIp(c->sa.sin.sin_addr.s_addr) << std::endl;
    // const std::string str(R"({"id":1,"error":null})");
    // mg_send_head(c, 200, str.length(), "Content-Type: application/json");
    // mg_send(c, str.c_str(), str.length());
    // break;
  }
  case MG_EV_HTTP_PART_BEGIN:
  case MG_EV_HTTP_PART_DATA:
  case MG_EV_HTTP_PART_END: {
    const auto cstr = mg_file_upload_handler_string(c, ev, p);
    if (cstr != NULL) {
      const auto str = std::string(cstr);
      std::cout << "File upload done: \n\n" << str << "\n\n" << std::endl;
      try {
        inputFile(str);
      } catch (const std::exception &e) {
        std::cerr << "THROW: " << e.what() << std::endl;
      } catch (...) {
        std::cerr << "THROW...: " << std::endl;
      }
    }
    free(cstr);
    break;
  }
  }
}

int main() {
  InitBridge();
  struct mg_mgr mgr;
  struct mg_connection *c;
  struct mg_serve_http_opts s_http_server_opts;

  std::cout << "Start Webserver on port " << s_http_port << std::endl;
  mg_mgr_init(&mgr, nullptr);
  c = mg_bind(&mgr, s_http_port, ev_handler);
  if (!c) {
    std::cerr << "Can't Start Webserver" << std::endl;
    return 1;
  }
  mg_set_protocol_http_websocket(c);
  reqs = 0;
  bool go = true;
  std::cout << "Server Running" << std::endl;
  while (go) {
    const auto dt = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_req);
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
  ShutdownBridge();
  return 0;
}
