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
#include <queue>

#include "util.hpp"

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

bool DBG = false;
const double eps = 1e-4;  // double comparison
const int WAIT_DELAY = 1000;

std::mutex job_mutex;

string bootstrap_ip;
string bootstrap_port;

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

struct node {
  int uuid;
  string ip;
  string port;
  node() { uuid = -1; }
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
  void clear() { ip = port = ""; }
  bool exists() const { return !ip.empty() && !port.empty(); }
  bool operator<(const node& other) const { return uuid < other.uuid; }
} node_info;

// type = {parent, child, prev, next}
struct edge {
  edge() {}
  edge(string _type) : type(_type) {}
  edge(int _uuid, string _ip, string _port, string _type) : type(_type), ip(_ip), port(_port), uuid(_uuid) {}
  edge(edge& other, string type) {
    *this = other;
    this->type = type;
  }
  int uuid;
  string type;
  string ip;
  string port;
  void clear() { ip = port = ""; }
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
  adopt_response() { redirect = create_level = false; }
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
  if (rhs.exists()) {
    stream << rhs.uuid << " " << rhs.ip << " " << rhs.port << " " << rhs.type;
  } else {
    stream << "null";
  }
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const node& rhs) {
  if (rhs.exists()) {
    stream << rhs.uuid << " " << rhs.ip << " " << rhs.port;
  } else {
    stream << "null";
  }
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

struct point {
  double x, y;
  point() {}
  point(double _x, double _y) : x(_x), y(_y) {}
  bool operator==(point other) const { return abs(x - other.x) < eps && abs(y - other.y) < eps; }
  bool operator<(const point& other) const {
    if (abs(x - other.x) > eps) {
      return x < other.x;
    }
    return y < other.y;
  }
};

std::ostream& operator<<(std::ostream& stream, const point& rhs) {
  stream << "(" << rhs.x << ", " << rhs.y << ")";
  return stream;
}

struct job_request {
  int width, height;
  double p;
  vector<point> points;
  job_request() {}
  bool operator==(job_request other) const {
    return width == other.width && height == other.height && abs(p - other.p) < eps && points == other.points;
  }
  bool operator<(const job_request& other) const {
    if (width != other.width) {
      return width < other.width;
    }
    return height < other.height;  // good enough
  }
};

std::ostream& operator<<(std::ostream& stream, const job_request& rhs) {
  stream << rhs.width << " " << rhs.height << " " << rhs.p << " " << rhs.points;
  return stream;
}

struct backup_request {
  backup_request() {}
  backup_request(int _uuid, string _jobid, point _point) : uuid(_uuid), jobid(_jobid), point(_point.x, _point.y) {}
  int uuid;  // uid cvora koji salje backup_request
  string jobid;
  point point;
};

struct job {
  string id;                       // uuid joba
  vector<point> starting_points;   // zahtev
  map<int, vector<point>> backup;  // backup
  vector<point> points;            // izracunate tacke
  bool operator==(job other) const { return id == other.id; }
  bool operator<(const job& other) const { return id < other.id; }
  job_request request;
};

map<string, double> p;
vector<job> node_jobs;
queue<backup_request> backup_queue;  // producer consumer queue

struct job_data {
  int uuid;
  vector<point> points;
  map<int, vector<point>> backup;
  vector<edge> edges;
  bool empty() { return points.empty() && backup.empty(); }
};

std::ostream& operator<<(std::ostream& stream, const job_data& rhs) {
  stream << rhs.uuid;
  stream << " points = { " << rhs.points << "} ";
  stream << "backup = {";
  for (auto& it : rhs.backup) {
    stream << it.first << " " << it.second << ", ";
  }

  stream << "}";
  return stream;
}

namespace network {
void add_all_edges(ptree& out);
void json_to_edges(ptree& json, vector<edge>& edges);
}  // network declarations

namespace requests {
status jobs_backup(string ip, string port, backup_request request);
}  // requests declarations

namespace jobs {
// json
inline ptree point_to_json(point& p) {
  ptree res;
  res.put("x", p.x);
  res.put("y", p.y);
  return res;
}
inline point json_to_point(ptree json) { return point(json.get<double>("x"), json.get<double>("y")); }
inline vector<point> json_to_points(ptree& json) {
  vector<point> res;
  BOOST_FOREACH (ptree::value_type& it, json) { res.push_back(json_to_point(it.second)); }
  return res;
}
inline backup_request json_to_backup_request(ptree& json) {
  backup_request res;
  res.uuid = json.get<int>("uuid");
  res.jobid = json.get<string>("jobid");
  res.point = json_to_point(json.get_child("point"));
  return res;
}
inline ptree backup_request_to_json(backup_request& bak) {
  ptree res;
  res.put("uuid", bak.uuid);
  res.put("jobid", bak.jobid);
  res.add_child("point", point_to_json(bak.point));
  return res;
}
inline ptree points_to_json(vector<point>& points) {
  ptree res;
  for (auto& it : points) {
    res.push_back(make_pair("", point_to_json(it)));
  }
  return res;
}
inline ptree job_request_to_json(job_request& request) {
  ptree out;
  out.put("width", request.width);
  out.put("height", request.height);
  out.put("p", request.p);
  out.add_child("points", points_to_json(request.points));
  return out;
}
inline job_request json_to_job_request(ptree& json) {
  job_request res;
  res.width = json.get<int>("width");
  res.height = json.get<int>("height");
  res.p = json.get<double>("p");
  res.points = jobs::json_to_points(json.get_child("points"));
  return res;
}
inline vector<job_request> json_to_job_requests(ptree& json) {
  vector<job_request> res;
  BOOST_FOREACH (ptree::value_type& it, json) { res.push_back(json_to_job_request(it.second)); }
  return res;
}
inline ptree job_requests_to_json(vector<job_request>& requests) {
  ptree res;
  for (auto& it : requests) {
    res.push_back(make_pair("", job_request_to_json(it)));
  }
  return res;
}
inline ptree point_map_to_json(map<int, vector<point>>& m) {
  ptree out;
  for (auto& it : m) {
    ptree json;
    json.put("uuid", it.first);
    json.add_child("points", points_to_json(it.second));
    out.push_back(make_pair("", json));
  }
  return out;
}
inline map<int, vector<point>> json_to_point_map(ptree& json) {
  map<int, vector<point>> out;
  BOOST_FOREACH (ptree::value_type& it, json) { out[it.second.get<int>("uuid")] = json_to_points(it.second.get_child("points")); }
  return out;
}

inline ptree job_data_to_json(const string& jobid) {
  ptree out;
  network::add_all_edges(out);
  out.put("uuid", node_info.uuid);
  for (int i = 0; i < node_jobs.size(); i++) {
    if (node_jobs[i].id == jobid) {
      if (!node_jobs[i].points.empty()) {
        out.add_child("points", points_to_json(node_jobs[i].points));
      }
      if (!node_jobs[i].backup.empty()) {
        out.add_child("backup", point_map_to_json(node_jobs[i].backup));
      }
      break;
    }
  }
  return out;
}
inline job_data json_to_job_data(ptree& json) {
  job_data out;
  if (key_exists(json, "uuid")) {
    out.uuid = json.get<int>("uuid");
  }
  if (key_exists(json, "edges")) {
    network::json_to_edges(json.get_child("edges"), out.edges);
  }
  if (key_exists(json, "points")) {
    out.points = json_to_points(json.get_child("points"));
  }
  if (key_exists(json, "backup")) {
    BOOST_FOREACH (ptree::value_type& it, json.get_child("backup")) {
      out.backup[it.second.get<int>("uuid")] = json_to_points(it.second.get_child("points"));
    }
  }
  return out;
}
void merge(const vector<point>& in, vector<point>& out) {
  for (auto it : in) {
    out.push_back(it);
  }
  sort(out.begin(), out.end());
  auto last = std::unique(out.begin(), out.end());
  out.erase(last, out.end());
}
inline point rand_point(int w, int h) { return point(abs(rand() % w), abs(rand() % h)); }
// job logic
void new_job(string jobid, job_request request) {
  if (p.count(jobid)) {
    return;  // posao vec postoji
  }
  p.insert(make_pair(jobid, request.p));
  job new_job;
  new_job.request = request;
  new_job.id = jobid;
  new_job.starting_points = request.points;
  new_job.points.push_back(rand_point(request.width, request.height));
  node_jobs.push_back(new_job);
  sort(node_jobs.begin(), node_jobs.end());
}
void remove_job(string jobid) {
  for (auto it = node_jobs.begin(); it != node_jobs.end(); it++) {
    if (it->id == jobid) {
      node_jobs.erase(it);
      break;
    }
  }
  p.erase(jobid);
}
void consume_backup_queue() {
  int backup_size = backup_queue.size();
  while (backup_size--) {
    backup_request request = backup_queue.front();
    for (int i = 0; i < node_jobs.size(); i++) {
      if (node_jobs[i].id == request.jobid) {
        node_jobs[i].backup[request.uuid].push_back(request.point);
        break;
      }
    }
    backup_queue.pop();
  }
}
void work() {
  if (node_jobs.size() == 0) return;  // no jobs
  int idx = node_info.uuid % node_jobs.size();
  double _p = p[node_jobs[idx].id];
  point X = node_jobs[idx].starting_points[rand() % node_jobs[idx].starting_points.size()];
  point P = node_jobs[idx].points.back();
  P.x = P.x + (X.x - P.x) * _p;
  P.y = P.y + (X.y - P.y) * _p;
  node_jobs[idx].points.push_back(P);
}
void send_backup() {
  if (node_jobs.size() == 0) return;  // no jobs
  int idx = node_info.uuid % node_jobs.size();
  backup_request request;
  request.uuid = node_info.uuid;
  request.jobid = node_jobs[idx].id;
  request.point = node_jobs[idx].points.back();
  try {
    requests::jobs_backup(e_next.ip, e_next.port, request);
  } catch (exception& e) {
  }
  try {
    requests::jobs_backup(e_prev.ip, e_prev.port, request);
  } catch (exception& e) {
  }
}
}  // jobs

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
inline void add_all_edges(ptree& out) {
  ptree edges;
  add_edges(e_children, edges);
  add_edge(e_parent, edges);
  add_edge(e_next, edges);
  add_edge(e_prev, edges);
  if (!edges.empty()) {
    out.add_child("edges", edges);
  }
}
}  // network helpers

namespace requests {  // wrapperi za request (treba ga surround sa try/catch)
using namespace ::network;
using namespace ::jobs;
pair<node, status> hello(string ip, string port, string node_ip, string node_port) {
  HttpClient client(make_addr(ip, port));
  ptree in, out;
  in.put("ip", node_ip);
  in.put("port", node_port);
  do {
    read_json(client.request("POST", "/api/hello", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return make_pair(node(out), status(out));
}
pair<bool, status> reset(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/reset")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return make_pair(out.get<bool>("can_reset"), status(out));
}
status reset_done(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/reset_done")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return status(out);
}
status ok(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/basic/ok")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return status(out);
}
pair<node, status> info(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/basic/info")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
    json_to_edges(out.get_child("edges"), res.edges);
  }
  return make_pair(res, status(out));
}
pair<vector<edge>, status> node_reset(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/network/reset")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  vector<edge> edges;
  if (key_exists(out, "edges")) {
    json_to_edges(out.get_child("edges"), edges);
  }
  return make_pair(edges, status(out));
}
pair<visualize_response, status> visualize(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/network/visualize")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
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
pair<vector<edge>, status> jobs_add(string ip, string port, string jobid, job_request request) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  ptree in = job_request_to_json(request);
  do {
    read_json(client.request("POST", "/api/jobs/add/" + jobid, make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  vector<edge> edges;
  if (key_exists(out, "edges")) {
    json_to_edges(out.get_child("edges"), edges);
  }
  return make_pair(edges, status(out));
}
pair<string, status> jobs_new(string ip, string port, job_request request) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  ptree in = job_request_to_json(request);
  do {
    read_json(client.request("POST", "/api/jobs/new", make_json(in))->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return make_pair(out.get<string>("jobid"), status(out));
}
pair<vector<job_request>, vector<string>> jobs_all(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/jobs/all")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  vector<job_request> requests;
  vector<string> ids;
  if (key_exists(out, "jobs")) {
    requests = json_to_job_requests(out.get_child("jobs"));
    BOOST_FOREACH (ptree::value_type& it, out.get_child("jobs")) { ids.push_back(it.second.get<string>("jobid")); }
  }
  return make_pair(requests, ids);
}
status jobs_backup(string ip, string port, backup_request request) {
  HttpClient client(make_addr(ip, port));
  ptree out, in;
  in = backup_request_to_json(request);
  read_json(client.request("POST", "/api/jobs/backup", make_json(backup_request_to_json(request)))->content, out);
  if (DBG) cout << json_to_string(out);
  return status(out);
}
pair<vector<edge>, status> jobs_remove(string ip, string port, string jobid) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/jobs/remove/" + jobid)->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  vector<edge> edges;
  if (key_exists(out, "edges")) {
    json_to_edges(out.get_child("edges"), edges);
  }
  return make_pair(edges, status(out));
}
status jobs_kill(string ip, string port, string jobid) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/jobs/kill/" + jobid)->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return status(out);
}
pair<vector<string>, status> jobs_ids(string ip, string port) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/jobs/ids")->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  vector<string> ids;
  if (key_exists(out, "jobids")) {
    BOOST_FOREACH (ptree::value_type& it, out.get_child("jobids")) { ids.push_back(it.second.get<string>("")); }
  }
  return make_pair(ids, status(out));
}
pair<job_data, status> jobs_data(string ip, string port, string jobid) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/jobs/data/" + jobid)->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  return make_pair(json_to_job_data(out), status(out));
}
pair<vector<point>, map<int, vector<point>>> jobs_visualize(string ip, string port, string jobid) {
  HttpClient client(make_addr(ip, port));
  ptree out;
  do {
    read_json(client.request("GET", "/api/jobs/visualize/" + jobid)->content, out);
    if (DBG) cout << json_to_string(out);
    if (out.get<string>("status") == "wait") this_thread::sleep_for(chrono::milliseconds(WAIT_DELAY));
  } while (out.get<string>("status") == "wait");
  vector<point> startingpoints;
  map<int, vector<point>> point_map;
  if (key_exists(out, "startingpoints")) {
    startingpoints = json_to_points(out.get_child("startingpoints"));
  }
  if (key_exists(out, "points")) {
    point_map = json_to_point_map(out.get_child("points"));
  }
  return make_pair(startingpoints, point_map);
}
}  // requests

#endif
