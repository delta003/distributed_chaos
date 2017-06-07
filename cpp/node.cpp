// Node server.

#include "http/server_http.hpp"
#include "http/client_http.hpp"

#include "util.hpp"
#include "node.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;
using requests::status;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;


int WAIT = 1;

stringstream logstream;

namespace Handlers {
  void index(HttpServer& server) {
    server.default_resource["GET"]=[&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      send(response, "");
      // TODO: servirati staticke fajlove ovde
    };
  }
  void logz(HttpServer& server) {
    server.resource["/logz"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
        log(logstream, "logz_request", request->content.string());
        send(response, logstream.str());
      } catch (exception& e) {
        log(logstream, "logz_error", e.what());
        send_error(response, e.what());
      }
    };
  }
  namespace basic {
    void ok(HttpServer& server) {
    server.resource["/api/basic/ok"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);     
          log(logstream, "ok_request", request->content.string());
          ptree out;
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "ok_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void info(HttpServer& server) {
    server.resource["/api/basic/info"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          log(logstream, "info_request", request->content.string());
          ptree out;
          out.put("uuid", node_info.uuid);
          out.put("ip", node_info.ip);
          out.put("port", node_info.port);

          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "info_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void check(HttpServer& server) {
    server.resource["/api/basic/check"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "check_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          HttpClient client(make_addr(in));
          ptree out;
          try {
            read_json(client.request("GET", "/api/basic/ok")->content, out);
            out.put("alive", "true");
            if (key_exists(out, "message")) {
              log(logstream, "check_response_error", out.get<string>("message"));
            }
            send_ok(response, out);
          } catch (exception& e) {
            out.put("alive", "false");
            send_ok(response, out);
          }
        } catch (exception& e) {
          log(logstream, "check_error", e.what());
          send_error(response, e.what());
        }
      };
    }
  } // basic
  namespace network {
    using namespace ::network;
    void edges(HttpServer& server) {
    server.resource["/api/network/edges"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          log(logstream, "edges_request", request->content.string());
          ptree out;
          ptree edges;
          add_all_edges(edges);
          if (!edges.empty()) {
            out.add_child("edges", edges);
          }
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "edges_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void get_edge(HttpServer& server) {
    server.resource["/api/network/get_edge"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "get_edge_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          ptree out;
          add_edge(in.get<string>("type"), out, "edge");
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "get_edge_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void set_edge(HttpServer& server) {
    server.resource["/api/network/set_edge"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "set_edge_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          edge new_edge;
          json_to_edge(in.get_child("edge"), new_edge);
          ptree out;
          add_edge(in.get_child("edge").get<string>("type"), out, "oldedge");
          set_new_edge(new_edge);
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "set_edge_error", e.what());
          send_error(response, e.what());
        }
      };
    }
  } // network
} // Handlers

void worker() {
  this_thread::sleep_for(chrono::milliseconds(100));
  WAIT = 0;
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  node_info.ip = argv[1];
  node_info.port = argv[2];
  bootstrap_ip = argv[3];
  bootstrap_port = argv[4];

  HttpServer server;
  server.config.port=atoi(argv[2]);

  // / i /logz
  Handlers::index(server);
  Handlers::logz(server);
  // Basic
  Handlers::basic::ok(server);
  Handlers::basic::info(server);
  Handlers::basic::check(server);
  // Network
  Handlers::network::edges(server);
  Handlers::network::get_edge(server);
  Handlers::network::set_edge(server);

  thread server_thread([&server](){
      server.start();
  });

  thread worker_thread(worker);

  worker_thread.join();
  server_thread.join();

  return 0;
}
