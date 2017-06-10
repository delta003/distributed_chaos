// Bootstrap server.

#include "http/server_http.hpp"

#include "util.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <cstring>
#include <sstream>

using namespace std;
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

stringstream logstream;

int uuid = 0;
bool can_reset = true;
string last_ip;
string last_port;

namespace Handlers {
void reset(HttpServer& server) {
  server.resource["/api/reset"]["GET"] = [](
      shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      log(logstream, "reset_request", request->content.string());
      ptree out;
      out.put("can_reset", can_reset);
      if (can_reset) {
        last_ip = "";
        last_port = "";
        can_reset = false;
      }
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "reset_error", e.what());
      send_error(response, e.what());
    }
  };
}
void reset_done(HttpServer& server) {
  server.resource["/api/reset_done"]["GET"] = [](
      shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      log(logstream, "reset_done_request", request->content.string());
      ptree out;
      can_reset = true;
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "reset_done_error", e.what());
      send_error(response, e.what());
    }
  };
}
void hello(HttpServer& server) {
  server.resource["/api/hello"]["POST"] = [](
      shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      string req_str = request->content.string();
      log(logstream, "hello_request", req_str);
      ptree in;
      string_to_json(req_str, in);
      string ip = in.get<string>("ip");
      string port = in.get<string>("port");
      ptree out;
      if (!last_ip.empty() && !last_port.empty()) {
        out.put("ip", last_ip);
        out.put("port", last_port);
      }
      out.put("uuid", uuid);
      uuid++;
      last_ip = ip;
      last_port = port;
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "hello_error", e.what());
      send_error(response, e.what());
    }
  };
}
void logz(HttpServer& server) {
  server.resource["/logz"]["GET"] = [](
      shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    try {
      log(logstream, "logz_request", request->content.string());
      send(response, logstream.str());
    } catch (exception& e) {
      log(logstream, "logz_error", e.what());
      send_error(response, e.what());
    }
  };
}
void index(HttpServer& server) {
  server.default_resource["GET"] = [&server](
      shared_ptr<HttpServer::Response> response,
      shared_ptr<HttpServer::Request> request) {
    ptree out;
    send_ok(response, out);
  };
}

}  // Handlers

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  HttpServer server;
  server.config.port = atoi(argv[1]);

  Handlers::reset(server);
  Handlers::hello(server);
  Handlers::logz(server);
  Handlers::index(server);
  Handlers::reset_done(server);

  thread server_thread([&server]() { server.start(); });

  server_thread.join();

  return 0;
}
