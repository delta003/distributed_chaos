// Node server.

#include "http/server_http.hpp"
#include "http/client_http.hpp"

#include "util.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <cstring>
#include <sstream>

using namespace std;
using namespace boost::property_tree;

struct node_info {
  int uuid;
  string ip;
  string port;
  string bootstrap_ip;
  string bootstrap_port;
} node_info;

// type = {parent, child, prev, next}
struct edge {
  edge() {}
  edge(string _type) : type(_type) { ip = port = "null"; }
  edge(string _type, string _ip, string _port) :
    type(_type), ip(_ip), port(_port) {}
  string type;
  string ip;
  string port;
  bool exists() const { return ip != "null" && port != "null"; }
};

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

stringstream logstream;

edge e_parent("parent");
edge e_prev("prev");
edge e_next("next");
vector<edge> e_children;

inline void edge_to_json(const edge& edge, ptree& json) {
  json.put("uuid", node_info.uuid);
  json.put("ip", edge.ip);
  json.put("port", edge.port);
  json.put("type", edge.type);
}

inline void add_edge(const edge& edge, ptree& out) {
  ptree json;
  if (edge.exists()) {
    edge_to_json(edge, json);
    out.push_back(json);
  }
}

inline void add_edge(const string& type, ptree& out) {
  if (type == "parent" && parent.exist()) {
    if (parent.exists()) {
      edge_to_json("out");
    } 
  }
}

inline void add_edges(const vector<edge>& edges, ptree& out) {
  for (auto edge: edges) {
    add_edge(edge, out);
  }
}

namespace Handlers {
  void index(HttpServer& server) {
    server.default_resource["GET"]=[&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      send(response, "");
      // TODO: servirati staticke fajlove ovde
    };
  }
  namespace basic {
    void ok(HttpServer& server) {
    server.resource["/api/basic/ok"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
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
          string req_str = request->content.string();
          log(logstream, "check_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          HttpClient client(make_addr(in));
          ptree out;
          read_json(client.request("GET", "/api/basic/ok")->content, out);
          out.put("alive", "true");
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "check_error", e.what());
          ptree out;
          out.put("alive", "false");
          send_error(response, out, e.what());
        }
      };
    }
  } // basic
  namespace network {
    void edges(HttpServer& server) {
    server.resource["/api/network/edges"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          log(logstream, "edges_request", request->content.string());
          ptree out;
          ptree edges;
          add_edge(e_parent, edges);
          add_edge(e_next, edges);
          add_edge(e_prev, edges);
          add_edges(e_children, edges);
          out.add_child("edges", edges);
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "edges_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void get_edge(HttpServer& server) {
    server.resource["/api/network/edges"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          string req_str = request->content.string();
          log(logstream, "get_edge_request", req_str);
          ptree in;
          string_to_json(req_str, in);ptree out;
          ptree out;
          add_edge(out, )
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "edges_error", e.what());
          send_error(response, e.what());
        }
      };
    }
  } // network
} // Handlers

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  node_info.ip = argv[1];
  node_info.port = argv[2];
  node_info.bootstrap_ip = argv[3];
  node_info.bootstrap_port = argv[4];

  HttpServer server;
  server.config.port=atoi(argv[2]);

  Handlers::index(server);
  Handlers::basic::ok(server);
  Handlers::basic::info(server);
  Handlers::basic::check(server);

  thread server_thread([&server](){
      server.start();
  });
    
  server_thread.join();

  return 0;
}
