// Node business logic

#ifndef NODE_LOGIC_HPP
#define NODE_LOGIC_HPP

#include <boost/foreach.hpp>

#include <iostream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <utility>
#include <vector>

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

bool DBG = false;

string bootstrap_ip;
string bootstrap_port;

struct node {
  int uuid;
  string ip;
  string port;
  node () {}
  node (int _uuid, string _ip, string _port) :
    uuid(_uuid), ip(_ip), port(_port) {}
  node (const ptree& json) {
    uuid = json.get<int>("uuid");
    if (key_exists(json, "ip")) {
      ip = json.get<string>("ip");
    }
    if (key_exists(json, "port")) {
      port = json.get<string>("port");
    }
  }
  bool operator<(const node& other) const {
    return uuid < other.uuid;
  }
} node_info;

// type = {parent, child, prev, next}
struct edge {
  edge() { ip = port = "null"; }
  edge(string _type) : type(_type) { ip = port = "null"; }
  edge(int _uuid, string _ip, string _port, string _type) :
    type(_type), ip(_ip), port(_port), uuid(_uuid) {}
  int uuid;
  string type;
  string ip;
  string port;
  bool exists() const { return ip != "null" && port != "null"; }
  bool operator==(edge other) const {
    if (other.uuid == uuid && other.type == type && other.ip == ip && other.port == port) {
      return true;
    }
    return false;
  }
  bool operator<(const edge& other) const {
    return uuid < other.uuid;
  }
};

std::ostream& operator<<(std::ostream& stream, const edge& rhs) {
  stream << rhs.uuid << " " << rhs.ip << " " << rhs.port << " " << rhs.type;
  return stream;
}

edge e_parent("parent");
edge e_prev("prev");
edge e_next("next");
vector<edge> e_children;

namespace network {
  inline void json_to_edge(const ptree& json, edge& out) {
    out.uuid = json.get<int>("uuid");
    out.ip = json.get<string>("ip");
    out.port = json.get<string>("port");
    out.type = json.get<string>("type");
  }

  inline edge json_to_edge(const ptree& json) {
    edge out;
    json_to_edge(json, out);
    return out;
  }

  inline void edge_to_json(const edge& edge, ptree& json) {
    json.put("uuid", edge.uuid);
    json.put("ip", edge.ip);
    json.put("port", edge.port);
    json.put("type", edge.type);
  }

  inline ptree edge_to_json(const edge& edge) {
    ptree out;
    edge_to_json(edge, out);
    return out;
  }

  inline edge get_old_edge(const string& type) {
    edge res;
    if (type == "parent" && e_parent.exists()) {
        res = e_parent;
    }
    if (type == "prev" && e_prev.exists()) {
        res = e_prev;
    } 
    if (type == "next" && e_next.exists()) {
        res = e_next;
    } 
    return res;
  }

  inline void set_new_edge(const edge& new_edge) {
    if (new_edge.type == "parent") {
      e_parent = new_edge;
    }
    if (new_edge.type == "prev") {
      e_prev = new_edge;
    }
    if (new_edge.type == "next") {
      e_next = new_edge;
    }
  }

  inline void add_edge(const edge& edge, ptree& out) {
    ptree json;
    if (edge.exists()) {
      edge_to_json(edge, json);
      out.push_back(std::make_pair("", json));
    }
  }

  inline void add_edge(const string& type, ptree& out, const string& edge_name) {
    ptree json;
    edge new_edge = get_old_edge(type);
    if (new_edge.exists()) {
      edge_to_json(new_edge, json);
    }
    if (!json.empty()) {
      out.add_child(edge_name, json);
    }
  }


  inline void add_edges(const vector<edge>& edges, ptree& out) {
    for (auto edge: edges) {
      add_edge(edge, out);
    }
  }
  inline void add_all_edges(ptree& edges) {
    add_edge(e_parent, edges);
    add_edge(e_next, edges);
    add_edge(e_prev, edges);
    add_edges(e_children, edges);
  }
} // network helpers

namespace requests { //wrapperi za request (treba ga surround sa try/catch)
  using namespace ::network;
  struct status {
    string msg;
    string code;
    status() {code = msg = "";}
    status(string _code, string _msg): msg(_msg), code(_code) {}
    status(const ptree& json) {
      code = json.get<string>("status");
      if (code == "error") {
        msg = json.get<string>("message");
      }
    }
  };
  pair<node, status> hello(string ip, string port, string node_ip, string node_port) {
    HttpClient client(make_addr(ip, port));
    ptree in, out;
    in.put("ip", node_ip);
    in.put("port", node_port);
    read_json(client.request("POST", "/api/hello", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    return make_pair(node(out), status(out));
  }
  status reset(string ip, string port) {
    HttpClient client(make_addr(ip, port));
    ptree out;
    read_json(client.request("GET", "/api/reset")->content, out);
    if (DBG) cout << json_to_string(out);
    return status(out);
  }
  status ok(string ip, string port) {
    HttpClient client(make_addr(ip, port));
    ptree out;
    read_json(client.request("GET", "/api/basic/ok")->content, out);
    if (DBG) cout << json_to_string(out);
    return status(out);
  }
  pair<node, status> info(string ip, string port) {
    HttpClient client(make_addr(ip, port));
    ptree out;
    read_json(client.request("GET", "/api/basic/info")->content, out);
    if (DBG) cout << json_to_string(out);
    return make_pair(node(out), status(out));
  }
  pair<string, status> check(string ip, string port, string check_ip, string check_port) {
    HttpClient client(make_addr(ip, port));
    ptree in, out;
    in.put("ip", check_ip);
    in.put("port", check_port);
    read_json(client.request("POST", "/api/basic/check", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    return make_pair(out.get<string>("alive"), status(out));
  }
  pair<edge, status> get_edge(string ip, string port, string type) {
    HttpClient client(make_addr(ip, port));
    ptree in, out;
    in.put("type", type);
    read_json(client.request("POST", "/api/network/get_edge", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    edge res;
    if (key_exists(out, "edge")) {
      json_to_edge(out.get_child("edge"), res);
    }
    return make_pair(res, status(out));
  }
  pair<edge, status> set_edge(string ip, string port, edge new_edge) {
    HttpClient client(make_addr(ip, port));
    ptree in, out;
    in.add_child("edge", edge_to_json(new_edge));
    read_json(client.request("POST", "/api/network/set_edge", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    edge res;
    if (key_exists(out, "oldedge")) {
      json_to_edge(out.get_child("oldedge"), res);
    }
    return make_pair(res, status(out));
  }
  pair<vector<edge>, status> edges(string ip, string port) {
    HttpClient client(make_addr(ip, port));
    ptree out;
    read_json(client.request("GET", "/api/network/edges")->content, out);
    if (DBG) cout << json_to_string(out);
    vector<edge> edges;
    if (key_exists(out, "edges")) {
      BOOST_FOREACH(ptree::value_type& it, out.get_child("edges")) {
        edges.push_back(edge(it.second.get<int>("uuid"),
                        it.second.get<string>("ip"),
                        it.second.get<string>("port"),
                        it.second.get<string>("type")));
      } 
    }
    return make_pair(edges, status(out));
  }
} // requests


#endif
