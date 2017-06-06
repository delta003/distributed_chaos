// Bootstrap

#include "server_http.hpp"
#include "util.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

int uuid = 0;
string last_ip = "null";
string last_port = "null";

stringstream logstream;

namespace Handlers {
  void reset(HttpServer& server) {
  server.resource["/api/reset"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
        ptree out;
        uuid = 0;
        last_ip = "null";
        last_port = "null";
        send_ok(response, out);
      } catch (exception& e) {
        log(logstream, "reset", e.what());
      }
    };
  }
  void hello(HttpServer& server) {
    server.resource["/api/hello"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
        ptree in;
        read_json(request->content, in);
        string ip = in.get<string>("ip");
        string port = in.get<string>("port");

        ptree out;
        out.put("ip", last_ip);
        out.put("port", last_port);
        out.put("uuid", uuid);

        send_ok(response, out);
      } catch (exception& e) {
        log(logstream, "hello", e.what());
        send_error(response, e.what());
      }
    };
  }
  void logz(HttpServer& server) {
    server.resource["/logz"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
        send(response, logstream.str());
      } catch (exception& e) {
        log(logstream, "logz", e.what());
        send_error(response, e.what());
      }
    };
  }
} // Handlers

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  HttpServer server;
  server.config.port=atoi(argv[1]);

  Handlers::reset(server);
  Handlers::hello(server);
  Handlers::logz(server);

  thread server_thread([&server](){
      server.start();
  });
    
  server_thread.join();

  return 0;
}
