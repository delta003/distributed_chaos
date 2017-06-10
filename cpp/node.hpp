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
  node() {}
  node(int _uuid, string _ip, string _port) : uuid(_uuid), ip(_ip), port(_port) {}
  node(const ptree& json) {
    uuid = json.get<int>("uuid");
    if (key_exists(json, "ip")) {
      ip = json.get<string>("ip");
    }
    if (key_exists(json, "port")) {
      port = json.get<string>("port");
    }
  }
  bool exists() const { return !ip.empty() && !port.empty(); }
  bool operator<(const node& other) const { return uuid < other.uuid; }
} node_info;

// type = {parent, child, prev, next}
struct edge {
  edge() {}
  edge(string _type) : type(_type) {}
  edge(int _uuid, string _ip, string _port, string _type) : type(_type), ip(_ip), port(_port), uuid(_uuid) {}
  int uuid;
  string type;
  string ip;
  string port;
  bool exists() const { return !ip.empty() && !port.empty(); }
  bool operator==(edge other) const {
    if (other.uuid == uuid && other.type == type && other.ip == ip && other.port == port) {
      return true;
    }
    return false;
  }
  bool operator<(const edge& other) const { return uuid < other.uuid; }
};

struct vedge {  //
  vedge() {}
  vedge(int _start_uuid, int _end_uuid, string _type) : start_uuid(_start_uuid), end_uuid(_end_uuid), type(_type) {}
  int start_uuid;
  int end_uuid;
  string type;
};

struct adopt_response {
  adopt_response() {}
  bool redirect, create_level;
  vector<edge> edges;
  edge next;
};

struct visualize_response {
  visualize_response() {}
  vector<node> nodes;
  vector<vedge> vedges;
};

std::ostream& operator<<(std::ostream& stream, const edge& rhs) {
  stream << rhs.uuid << " " << rhs.ip << " " << rhs.port << " " << rhs.type;
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const node& rhs) {
  stream << rhs.uuid << " " << rhs.ip << " " << rhs.port;
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const adopt_response& rhs) {
  stream << rhs.redirect << " " << rhs.create_level << " " << rhs.edges << " " << rhs.next;
  return stream;
}

template <typename T>
string to_str(T object) {
  ostringstream oss;
  oss << object;
  return oss.str();
}

edge e_parent("parent");
edge e_prev("prev");
edge e_next("next");
vector<edge> e_children;

namespace network {
void add_edge(const edge& edge, ptree& out);

inline void json_to_edges(ptree& json, vector<edge>& edges) {
  BOOST_FOREACH (ptree::value_type& it, json) {
    edges.push_back(edge(it.second.get<int>("uuid"), it.second.get<string>("ip"), it.second.get<string>("port"), it.second.get<string>("type")));
  }
}

inline bool edge_equals_ignore_type(edge e1, edge e2) { return e1.ip == e2.ip && e1.port == e2.port; }

inline vector<edge> get_all_edges() {
  vector<edge> result;
  if (e_prev.exists()) result.push_back(e_prev);
  if (e_next.exists()) result.push_back(e_next);
  if (e_parent.exists()) result.push_back(e_parent);
  for (auto& it : e_children) {
    if (it.exists()) result.push_back(it);
  }
  return result;
}

inline edge get_edge(const vector<edge>& edges, string type, int idx = 0) {
  for (auto it : edges) {
    if (it.type == type) {
      if (idx == 0) {
        return it;
      } else {
        --idx;
      }
    }
  }
  return edge();
}

inline void edges_to_json(const vector<edge>& edges, ptree& json) {
  for (auto& it : edges) {
    add_edge(it, json);
  }
}

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

inline ptree node_to_json(const node& node) {
  ptree json;
  json.put("uuid", node.uuid);
  json.put("ip", node.ip);
  json.put("port", node.port);
  return json;
}

inline node edge_to_node(const edge& edge) { return node(edge.uuid, edge.ip, edge.port); }
inline edge node_to_edge(const node& node, string type) { return edge(node.uuid, node.ip, node.port, type); }

inline node json_to_node(const ptree& json) { return node(json.get<int>("uuid"), json.get<string>("ip"), json.get<string>("port")); }

inline void json_to_nodes(ptree& json, vector<node>& out) {
  BOOST_FOREACH (ptree::value_type& it, json) {
    out.push_back(node(it.second.get<int>("uuid"), it.second.get<string>("ip"), it.second.get<string>("port")));
  }
}

inline ptree vedge_to_json(const vedge& vedge) {
  ptree json;
  json.put("start_uuid", vedge.start_uuid);
  json.put("end_uuid", vedge.end_uuid);
  json.put("type", vedge.type);
  return json;
}

inline vedge json_to_vedge(const ptree& json) { return vedge(json.get<int>("start_uuid"), json.get<int>("end_uuid"), json.get<string>("type")); }

inline void json_to_vedges(ptree& json, vector<vedge>& out) {
  BOOST_FOREACH (ptree::value_type& it, json) {
    out.push_back(vedge(it.second.get<int>("start_uuid"), it.second.get<int>("end_uuid"), it.second.get<string>("type")));
  }
}

inline vedge edge_to_vedge(const edge& edge, int start_uuid) { return vedge(start_uuid, edge.uuid, edge.type); }

inline void edge_to_json(const edge& edge, ptree& json) {
  json.put("uuid", edge.uuid);
  json.put("ip", edge.ip);
  json.put("port", edge.port);
  json.put("type", edge.type);
}

inline ptree edge_to_json(const edge& edge) {
  ptree out;
  edge_to_json(edge, out);
  return std::move(out);
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
  for (auto edge : edges) {
    add_edge(edge, out);
  }
}
inline void add_all_edges(ptree& edges) {
  add_edges(e_children, edges);
  add_edge(e_parent, edges);
  add_edge(e_next, edges);
  add_edge(e_prev, edges);
}
}  // network helpers

namespace requests {  // wrapperi za request (treba ga surround sa try/catch)
using namespace ::network;
struct status {
  string msg;
  string code;
  status() { code = msg = ""; }
  status(string _code, string _msg) : msg(_msg), code(_code) {}
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
  do {
    read_json(client.request("POST", "/api/hello", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return make_pair(node(out), status(out));
}
pair<bool, status> reset(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/reset")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return make_pair(out.get<bool>("can_reset"), status(out));
}
status reset_done(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/reset_done")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return status(out);
}
status ok(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/basic/ok")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return status(out);
}
pair<node, status> info(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/basic/info")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return make_pair(node(out), status(out));
}
pair<bool, status> check(string ip, string port, string check_ip, string check_port) {
  HttpClient client(make_addr(ip, port));
  ptree in, out;
  in.put("ip", check_ip);
  in.put("port", check_port);
  do {
    read_json(client.request("POST", "/api/basic/check", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return make_pair(out.get<bool>("alive"), status(out));
}
pair<edge, status> get_edge(string ip, string port, string type) {
  HttpClient client(make_addr(ip, port));
  ptree in, out;
  in.put("type", type);
  do {
    read_json(client.request("POST", "/api/network/get_edge", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  edge res;
  if (key_exists(out, "edge")) {
    json_to_edge(out.get_child("edge"), res);
  }
  return make_pair(res, status(out));
}
pair<edge, status> set_edge(string ip, string port, edge new_edge, string type = "") {
  if (!type.empty()) {
    new_edge.type = type;
  }
  HttpClient client(make_addr(ip, port));
  ptree in, out;
  in.add_child("edge", edge_to_json(new_edge));
  do {
    read_json(client.request("POST", "/api/network/set_edge", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  edge res;
  if (key_exists(out, "oldedge")) {
    json_to_edge(out.get_child("oldedge"), res);
  }
  return make_pair(res, status(out));
}
pair<vector<edge>, status> edges(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/network/edges")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  vector<edge> edges;
  if (key_exists(out, "edges")) {
    json_to_edges(out.get_child("edges"), edges);
  }
  return make_pair(edges, status(out));
}
pair<adopt_response, status> adopt(string ip, string port, edge new_edge, bool can_redirect) {
  HttpClient client(make_addr(ip, port));
  ptree in, out;
  in.add_child("edge", edge_to_json(new_edge));
  in.put("can_redirect", can_redirect);
  do {
    read_json(client.request("POST", "/api/network/adopt", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  adopt_response res;
  if (key_exists(out, "redirect")) {
    res.redirect = out.get<bool>("redirect");
  }
  if (key_exists(out, "create_level")) {
    res.create_level = out.get<bool>("create_level");
  }
  if (key_exists(out, "next")) {
    res.next = json_to_edge(out.get_child("next"));
  }
  if (key_exists(out, "edges")) {
    json_to_edges(out, res.edges);
  }
  return make_pair(res, status(out));
}
status node_reset(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/network/reset")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  return status(out);
}
pair<visualize_response, status> visualize(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/network/visualize")->content, out);
    if (DBG) cout << json_to_string(out);
  } while (out.get<string>("status") == "wait");
  vector<vedge> vedges;
  vector<node> nodes;
  if (key_exists(out, "edges")) {
    json_to_vedges(out.get_child("edges"), vedges);
  }
  if (key_exists(out, "nodes")) {
    json_to_nodes(out.get_child("nodes"), nodes);
  }
  visualize_response resp;
  resp.vedges = vedges;
  resp.nodes = nodes;
  return make_pair(resp, status(out));
}
}  // requests

#endif
