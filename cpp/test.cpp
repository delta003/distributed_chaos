// Unit tests.
// Pokrenuti pomocu test makefajla.
// Videti test_config.hpp pre pokretanja.
// Potrebno je da portovi 2000-2099 budu slobodni.

#include "http/client_http.hpp"

#include "util.hpp"
#include "node.hpp"
#include "test_config.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>
#include <stdexcept>

using namespace std;

using config::Node::start_node;
using config::Node::stop_node;
using config::Bootstrap::start_bootstrap;
using config::Bootstrap::stop_bootstrap;
using config::print_response;
using config::bootstrap_skip;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

int failures = 0;
vector<string> failed_tests;
int oks = 0;

template <typename A, typename B>
void assert_eq(A a, B b, int line) {
  if (a == b) {
    return;
  }
  std::ostringstream oss;
  oss << "Mismatch @ line " << line << " got " << a << " expected " << b << "\n";
  throw std::runtime_error(oss.str());
}

template <typename A, typename B>
void assert_veq(vector<A> a, vector<B> b, int line) {
  sort(a.begin(), a.end());
  sort(b.begin(), b.end());
  if (a == b) {
    return;
  }
  std::ostringstream oss;
  oss << "Mismatch @ line " << line << " got " << a << " expected " << b << "\n";
  throw std::runtime_error(oss.str());
}

void assert_true(bool expression, int line) {
  if (!expression) {
    std::ostringstream oss;
    oss << "Mismatch @ line " << line << " epxression is false\n";
    throw std::runtime_error(oss.str());
  }
}

#define ASSERT_EQ(a, b) assert_eq(a, b, __LINE__)
#define ASSERT_VEQ(a, b) assert_veq(a, b, __LINE__)
#define ASSERT_TRUE(a) assert_true(a, __LINE__)
#define RUN_TEST(f) run(f, #f)

namespace Bootstrap {
void test_hello() {
  string port = "2000";
  start_bootstrap(port);
  this_thread::sleep_for(chrono::seconds(1));
  pair<node, status> response;
  string node_ip, node_port, old_ip, old_port;

  node_ip = "192.220.100.50";
  node_port = "30000";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 0);
  ASSERT_EQ(response.first.ip, "");
  ASSERT_EQ(response.first.port, "");

  old_ip = node_ip;
  old_port = node_port;
  node_ip = "100.220.100.50";
  node_port = "224";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 1);
  ASSERT_EQ(response.first.ip, old_ip);
  ASSERT_EQ(response.first.port, old_port);

  old_ip = node_ip;
  old_port = node_port;
  node_ip = "200.220.100.50";
  node_port = "324";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 2);
  ASSERT_EQ(response.first.ip, old_ip);
  ASSERT_EQ(response.first.port, old_port);

  auto reset_response = requests::reset("localhost", port);
  ASSERT_EQ(reset_response.second.code, "ok");
  ASSERT_EQ(reset_response.first, true);

  old_ip = node_ip;
  old_port = node_port;
  node_ip = "288.255.100.50";
  node_port = "5554";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 3);
  ASSERT_EQ(response.first.ip, "");
  ASSERT_EQ(response.first.port, "");

  old_ip = node_ip;
  old_port = node_port;
  node_ip = "123.255.101.50";
  node_port = "3004";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 4);
  ASSERT_EQ(response.first.ip, old_ip);
  ASSERT_EQ(response.first.port, old_port);

  stop_bootstrap(port);
}

void test_reset() {
  string port = "2000";
  start_bootstrap(port);
  this_thread::sleep_for(chrono::seconds(1));
  pair<bool, status> response = requests::reset("localhost", port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first, true);
  response = requests::reset("localhost", port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first, false);
  response = requests::reset("localhost", port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first, false);
  auto r2 = requests::reset_done("localhost", port);
  ASSERT_EQ(r2.code, "ok");
  response = requests::reset("localhost", port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first, true);
}

void test_logz() {
  string port = "2000";
  start_bootstrap(port);
  this_thread::sleep_for(chrono::seconds(1));
  HttpClient client("localhost:" + port);
  client.request("POST", "/api/hello", "error");
  client.request("POST", "/api/hello", "not a json");
  client.request("GET", "/api/reset", "resetuj se!!!");
  auto response = client.request("GET", "/logz");
  cout << response->content.rdbuf();
  stop_bootstrap(port);
}
}  // Bootstrap

namespace Node {
namespace basic {
void test_ok() {
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));
  status response = requests::ok("localhost", port);
  ASSERT_EQ(response.code, "ok");
  stop_node(port);
}
void test_info() {
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));
  pair<node, status> response = requests::info("localhost", port);
  ASSERT_EQ(response.first.uuid, -1);
  ASSERT_EQ(response.first.ip, "localhost");
  ASSERT_EQ(response.first.port, port);
  ASSERT_EQ(response.second.code, "ok");
  stop_node(port);
}
void test_check() {
  string port_1 = "2001";
  string port_2 = "2002";
  start_node(port_1, bootstrap_skip);
  start_node(port_2, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));
  pair<bool, status> response;

  response = requests::check("localhost", port_1, "localhost", port_2);
  ASSERT_EQ(response.first, true);
  ASSERT_EQ(response.second.code, "ok");

  stop_node(port_2);
  response = requests::check("localhost", port_1, "localhost", port_2);
  ASSERT_EQ(response.first, false);
  ASSERT_EQ(response.second.code, "ok");

  stop_node(port_1);
}
}  // basic
namespace network {
using namespace ::network;
void test_edges() {
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));
  pair<vector<edge>, status> response = requests::edges("localhost", port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.size(), 0);
  stop_node(port);
}
void test_get_edge() {
  pair<edge, status> response;
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  response = requests::get_edge("localhost", port, "prev");
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_TRUE(!response.first.exists());

  response = requests::get_edge("localhost", port, "next");
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_TRUE(!response.first.exists());

  response = requests::get_edge("localhost", port, "parent");
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_TRUE(!response.first.exists());
  stop_node(port);
}
void __test_set_edge_t1(string port, string type) {
  pair<edge, status> response;

  edge new_edge = edge(224, "1.1.1.5", "2013", type);
  response = requests::set_edge("localhost", port, new_edge);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_TRUE(!response.first.exists());

  response = requests::set_edge("localhost", port, new_edge);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_TRUE(response.first.exists());
  ASSERT_EQ(response.first, new_edge);  // oldedge == new_edge
}
void test_set_edge() {
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));
  __test_set_edge_t1(port, "parent");
  __test_set_edge_t1(port, "prev");
  __test_set_edge_t1(port, "next");
  stop_node(port);
}
void test_get_set_edges() {
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  edge edge_1(1, "1.1.2.4", "80", "parent");
  edge edge_2(2, "1.5.2.1", "69", "next");
  edge edge_3(3, "100.55.2.4", "777", "prev");
  requests::set_edge("localhost", port, edge_1);
  requests::set_edge("localhost", port, edge_2);
  requests::set_edge("localhost", port, edge_3);

  pair<edge, status> r1 = requests::get_edge("localhost", port, edge_1.type);
  ASSERT_EQ(r1.second.code, "ok");
  ASSERT_EQ(r1.first, edge_1);
  r1 = requests::get_edge("localhost", port, edge_2.type);
  ASSERT_EQ(r1.second.code, "ok");
  ASSERT_EQ(r1.first, edge_2);
  r1 = requests::get_edge("localhost", port, edge_3.type);
  ASSERT_EQ(r1.second.code, "ok");
  ASSERT_EQ(r1.first, edge_3);

  pair<vector<edge>, status> r2 = requests::edges("localhost", port);
  ASSERT_EQ(r2.second.code, "ok");
  ASSERT_EQ(r2.first.size(), 3);

  sort(r2.first.begin(), r2.first.end());
  ASSERT_EQ(r2.first[0], edge_1);
  ASSERT_EQ(r2.first[1], edge_2);
  ASSERT_EQ(r2.first[2], edge_3);

  stop_node(port);
}

void test_adopt() {
  string ip = "localhost";
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  edge e_next = edge(100, "1.1.1.2", "2001", "next");
  requests::set_edge(ip, port, e_next);

  vector<edge> children{edge(1, "0.0.0.1", "2002", "child"), edge(2, "5.0.0.1", "2003", "child"), edge(3, "6.0.0.1", "2004", "child"),
                        edge(4, "22.0.0.1", "2005", "child"), edge(50, "66.0.0.1", "2006", "child")};

  auto r = requests::adopt(ip, port, children[0], true).first;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, false);

  r = requests::adopt(ip, port, children[1], true).first;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, false);

  r = requests::adopt(ip, port, children[2], true).first;
  ASSERT_EQ(r.redirect, true);
  ASSERT_EQ(r.next, e_next);

  r = requests::adopt(ip, port, children[2], false).first;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, false);

  r = requests::adopt(ip, port, children[3], false).first;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, false);

  r = requests::adopt(ip, port, children[4], false).first;
  auto r2 = r.edges;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, true);

  sort(r2.begin(), r2.end());  // sort by uuid
  ASSERT_EQ(r2[0], children[0]);
  ASSERT_EQ(r2[1], children[1]);
  ASSERT_EQ(r2[2], children[2]);
  ASSERT_EQ(r2[3], children[3]);

  r2 = requests::edges(ip, port).first;
  sort(r2.begin(), r2.end());
  ASSERT_EQ(r2[0], children[0]);
  ASSERT_EQ(r2[1], children[1]);

  stop_node(port);
}
}  // network
namespace jobs {
double __rand_double(double fmin, double fmax) {
  double f = (double)rand() / RAND_MAX;
  return fmin + f * (fmax - fmin);
}
job_request __job(int n) {
  job_request out;
  out.width = rand() % 600 + 600;
  out.height = rand() % 600 + 600;
  out.p = __rand_double(0.2, 0.8);
  for (int i = 0; i < n; i++) {
    out.points.push_back(::jobs::rand_point(out.width, out.height));
  }
  return out;
}
void add_remove_ids_test() {
  string ip = "localhost";
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  vector<string> ids = {"id1", "asdasdsafasgadgA", "asfn2140i150130--21591205i105iq-sas0-ii129431025959z98-931-58130531059405a-a09-9134-9"};
  for (auto& it : ids) {
    requests::jobs_add(ip, port, it, __job(3));
  }
  vector<string> ids2;

  ids2 = requests::jobs_ids(ip, port).first;
  ASSERT_VEQ(ids2, ids);

  requests::jobs_remove(ip, port, ids[1]);
  ids2 = requests::jobs_ids(ip, port).first;
  ASSERT_EQ(ids2.size(), 2);

  requests::jobs_add(ip, port, ids[1], __job(5));
  ids2 = requests::jobs_ids(ip, port).first;
  ASSERT_VEQ(ids2, ids);

  requests::jobs_remove(ip, port, ids[1]);
  ids.erase(ids.begin() + 1);
  ids2 = requests::jobs_ids(ip, port).first;
  ASSERT_VEQ(ids2, ids);

  requests::jobs_remove(ip, port, ids[1]);
  ids.erase(ids.begin() + 1);
  ids2 = requests::jobs_ids(ip, port).first;
  ASSERT_VEQ(ids2, ids);

  requests::jobs_remove(ip, port, ids[0]);
  ids.erase(ids.begin());
  ids2 = requests::jobs_ids(ip, port).first;
  ASSERT_VEQ(ids2, ids);
  ASSERT_EQ(ids2.size(), 0);
}
void new_kill_all_test() {
  string ip = "localhost";
  string p1 = "2000";
  string p2 = "2001";
  string p3 = "2002";
  start_node(p1, bootstrap_skip);
  start_node(p2, bootstrap_skip);
  start_node(p3, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  requests::set_edge(ip, p1, edge(1, ip, p2, "next"));
  requests::set_edge(ip, p2, edge(2, ip, p3, "next"));

  vector<string> ids;
  vector<job_request> jobs = {__job(3), __job(4)};

  ids.push_back(requests::jobs_new(ip, p1, jobs[0]).first);
  ids.push_back(requests::jobs_new(ip, p1, jobs[1]).first);
  this_thread::sleep_for(chrono::milliseconds(100));  // wait for propagation

  auto response = requests::jobs_all(ip, p3);
  ASSERT_VEQ(response.first, jobs);
  ASSERT_VEQ(response.second, ids);

  requests::jobs_kill(ip, p1, ids[0]);
  ids.erase(ids.begin());
  jobs.erase(jobs.begin());
  this_thread::sleep_for(chrono::milliseconds(100));  // wait for propagation

  response = requests::jobs_all(ip, p2);
  ASSERT_VEQ(response.first, jobs);
  ASSERT_VEQ(response.second, ids);

  requests::jobs_kill(ip, p1, ids[0]);
  this_thread::sleep_for(chrono::milliseconds(100));  // wait for propagation

  response = requests::jobs_all(ip, p1);
  ASSERT_EQ(response.first.size(), 0);
  ASSERT_EQ(response.second.size(), 0);
}
void data_test() {
  string ip = "localhost";
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  vector<string> ids;
  vector<job_request> jobs = {__job(3), __job(4)};

  ids.push_back(requests::jobs_new(ip, port, jobs[0]).first);
  ids.push_back(requests::jobs_new(ip, port, jobs[1]).first);
  this_thread::sleep_for(chrono::seconds(1));

  auto r1 = requests::jobs_data(ip, port, "ne_postojeci_jobid");
  ASSERT_TRUE(r1.first.empty());  // points and backup empty
  ASSERT_EQ(r1.second.code, "ok");
}
void backup_test() {
  string ip = "localhost";
  string port = "2000";
  start_node(port, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));

  string id = requests::jobs_new(ip, port, __job(3)).first;
  vector<point> p200 = {point(1., 2.), point(2.5, 22.4), point(340, 33.33)};
  vector<point> p400 = {point(224.2, 600.252525), point(2444.44444, 555.5555)};
  vector<point> p600 = {point(22.5, 34.5)};
  auto add = [&id, &ip, &port](vector<point> points, int uuid) {
    for (auto& it : points) {
      requests::jobs_backup(ip, port, backup_request(uuid, id, it));
    }
  };
  add(p200, 200);
  add(p400, 400);
  add(p600, 600);
  auto r1 = requests::jobs_data(ip, port, id);
  ASSERT_EQ(r1.first.backup.size(), 3);
  ASSERT_VEQ(r1.first.backup[200], p200);
  ASSERT_VEQ(r1.first.backup[400], p400);
  ASSERT_VEQ(r1.first.backup[600], p600);
}
void visualize_test() {
  string ip = "localhost";
  string bs_port = "2055";
  string p1 = "2000";
  string p2 = "2001";
  string p3 = "2002";
  start_bootstrap(bs_port);
  this_thread::sleep_for(chrono::seconds(1));  // wait for bootstrap
  start_node(p1, bs_port);
  start_node(p2, bs_port);
  start_node(p3, bs_port);
  this_thread::sleep_for(chrono::seconds(1));  // wait for nodes to form network

  vector<string> ids;
  vector<job_request> jobs = {__job(3), __job(4)};

  ids.push_back(requests::jobs_new(ip, p1, jobs[0]).first);
  ids.push_back(requests::jobs_new(ip, p3, jobs[1]).first);
  this_thread::sleep_for(chrono::seconds(10));  // wait for nodes to calculate a few points

  auto r1 = requests::jobs_data(ip, p2, "ne_postojeci_jobid");
  ASSERT_TRUE(r1.first.empty());
  ASSERT_EQ(r1.second.code, "ok");

  if (ids[0] > ids[1]) {
    swap(ids[0], ids[1]);
    swap(jobs[0], jobs[1]);
  }                                          // lexi sort
  int eps = 3;                               // points calculated during new job start
  r1 = requests::jobs_data(ip, p3, ids[1]);  // this node isn't doing this job
  ASSERT_TRUE(r1.first.points.size() < eps);
  ASSERT_TRUE(!r1.first.backup.empty());

  r1 = requests::jobs_data(ip, p1, ids[0]);
  ASSERT_TRUE(!r1.first.points.empty());
  ASSERT_TRUE(!r1.first.backup.empty());

  auto r2 = requests::jobs_visualize(ip, p2, ids[0]);  // nodes 0 and 2 are doing job 0
  ASSERT_VEQ(r2.first, jobs[0].points);
  ASSERT_TRUE(!r2.second[0].empty());
  ASSERT_TRUE(r2.second[1].size() < eps);
  ASSERT_TRUE(!r2.second[2].empty());

  r2 = requests::jobs_visualize(ip, p3, ids[1]);  // node 1 is doing job 1
  ASSERT_VEQ(r2.first, jobs[1].points);
  ASSERT_TRUE(r2.second[0].size() < eps);
  ASSERT_TRUE(!r2.second[1].empty());
  ASSERT_TRUE(r2.second[2].size() < eps);
}
}  // jobs
}  // Node

void clean_up() {
  for (int i = 0; i < 100; i++) {
    string port = to_string(2000 + i);
    stop_bootstrap(port);
    stop_node(port);
  }
}
void output_results() {
  cout << "Failed: " << failures << " | Passed: " << oks << "\n";
  if (failures > 0) {
    cout << "Failed tests: ";
    for (int i = 0; i < failed_tests.size(); i++) {
      if (i != 0) cout << ", ";
      cout << failed_tests[i];
    }
    cout << "" << endl;
  }
}

void run(function<void()> const& test, string test_name) {
  try {
    cout << "Running " << test_name << "\n";
    test();
    cout << test_name << ": OK\n\n";
    ++oks;
  } catch (exception& e) {
    cout << e.what();
    cout << test_name << ": FAIL\n";
    ++failures;
    failed_tests.push_back(test_name);
  }
  clean_up();
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  DBG = print_response;

  RUN_TEST(Bootstrap::test_reset);  // boostrap/api/reset
  RUN_TEST(Bootstrap::test_hello);  // boostrap/api/hello

  RUN_TEST(Node::basic::test_ok);     // node/api/basic/ok
  RUN_TEST(Node::basic::test_info);   // node/api/basic/info
  RUN_TEST(Node::basic::test_check);  // node/api/basic/check

  RUN_TEST(Node::network::test_edges);          // node/api/network/edges
  RUN_TEST(Node::network::test_get_edge);       // node/api/network/get_edge
  RUN_TEST(Node::network::test_set_edge);       // node/api/network/set_edge
  RUN_TEST(Node::network::test_get_set_edges);  // get_edge + set_edge + edges
  RUN_TEST(Node::network::test_adopt);          // node/api/network/adopt

  RUN_TEST(Node::jobs::add_remove_ids_test);  // add + remove + ids
  RUN_TEST(Node::jobs::new_kill_all_test);    // new + kill + all
  RUN_TEST(Node::jobs::data_test);            // data
  RUN_TEST(Node::jobs::backup_test);          // data + backup
  RUN_TEST(Node::jobs::visualize_test);       // visualize

  if (oks + failures > 0) {
    output_results();
  }
  return 0;
}
