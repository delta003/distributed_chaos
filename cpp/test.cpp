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
#include <thread>

using namespace std;

using config::Node::start_node;
using config::Node::stop_node;
using config::Bootstrap::start_bootstrap;
using config::Bootstrap::stop_bootstrap;
using config::print_response;
using config::bootstrap_skip;
using requests::status;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

void clean_up();

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

void assert_true(bool expression, int line) {
  if (!expression) {
    std::ostringstream oss;
    oss << "Mismatch @ line " << line << " epxression is false\n";
    throw std::runtime_error(oss.str());
  }
}

#define ASSERT_EQ(a, b) assert_eq(a, b, __LINE__)
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

  old_ip = node_ip;
  old_port = node_port;
  node_ip = "288.255.100.50";
  node_port = "5554";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 0);
  ASSERT_EQ(response.first.ip, "");
  ASSERT_EQ(response.first.port, "");

  old_ip = node_ip;
  old_port = node_port;
  node_ip = "123.255.101.50";
  node_port = "3004";
  response = requests::hello("localhost", port, node_ip, node_port);
  ASSERT_EQ(response.second.code, "ok");
  ASSERT_EQ(response.first.uuid, 1);
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
  ASSERT_EQ(response.first.uuid, 0);
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
                        edge(4, "22.0.0.1", "2005", "child"), edge(5, "66.0.0.1", "2006", "child")};

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

  r = requests::adopt(ip, port, children[3], true).first;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, false);

  r = requests::adopt(ip, port, children[4], true).first;
  ASSERT_EQ(r.redirect, false);
  ASSERT_EQ(r.create_level, true);

  auto r2 = requests::edges(ip, port).first;
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
void test_visualize() {
  string ip = "localhost";
  string port_1 = "2000";
  string port_2 = "2001";
  string port_3 = "2002";
  start_node(port_1, bootstrap_skip);
  start_node(port_2, bootstrap_skip);
  start_node(port_3, bootstrap_skip);
  this_thread::sleep_for(chrono::seconds(1));
  requests::set_edge(ip, port_1, edge(1, ip, port_2, "parent"));
  requests::set_edge(ip, port_2, edge(2, ip, port_3, "next"));
  auto r = requests::visualize(ip, port_1);
  ASSERT_EQ(r.second.code, "ok");
  ASSERT_EQ(r.first.nodes.size(), 3);
  ASSERT_EQ(r.first.vedges.size(), 2);
  stop_node(port_1);
  stop_node(port_2);
  stop_node(port_3);
}
}  // network
};

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
    clean_up();
  }
}

std::mutex mtx;
int port = 2000;

// interaktivni end to end test
void end_to_end_test() {
  clean_up();
  string ip = "localhost";
  string bs_port = "2099";
  start_bootstrap(bs_port);

  cout << "Koriscenje:\n"
       << "  1) up n - spawnuje n novih nodova.\n  2) k|kill n - ubija cvor sa uuid-om n.\n"
       << "  3) k2|kill2 n m - ubija dva cvora istovremeno.\n  4) q|quit - napusta program i stopira mrezu.\n\n";

  string cmd;
  while (1) {
    cin >> cmd;
    if (cmd == "up") {
      int broj;
      cin >> broj;
      vector<thread> pool;
      ostringstream oss;
      for (int i = 0; i < broj; i++) {
        oss << "Node " << i << " started {ip = " << ip << ", port = " << port << "}\n";
        pool.push_back(thread(start_node, to_string(port), bs_port));
        ++port;
      }
      for (int i = 0; i < broj; i++) {
        pool[i].join();
      }
      cout << oss.str();
    } else if (cmd == "kill" || cmd == "k") {
    } else if (cmd == "kill2" || cmd == "k2") {
    } else if (cmd == "q" || cmd == "quit") {
      break;
    }
  }
  clean_up();
}

int main(int argc, char* argv[]) {
  DBG = print_response;

  // RUN_TEST(Bootstrap::test_reset);  // boostrap/api/reset
  // RUN_TEST(Bootstrap::test_hello); // boostrap/api/hello

  // RUN_TEST(Node::basic::test_ok); // node/api/basic/ok
  // RUN_TEST(Node::basic::test_info); // node/api/basic/info
  // RUN_TEST(Node::basic::test_check); // node/api/basic/check

  // RUN_TEST(Node::network::test_edges); // node/api/network/edges
  // RUN_TEST(Node::network::test_get_edge); // node/api/network/get_edge
  // RUN_TEST(Node::network::test_set_edge); // node/api/network/set_edge
  // RUN_TEST(Node::network::test_get_set_edges); // get_edge + set_edge + edges
  // RUN_TEST(Node::network::test_adopt); // node/api/network/adopt
  // RUN_TEST(Node::network::test_visualize); // node/api/network/visualize

  end_to_end_test();

  if (oks + failures > 0) {
    output_results();
  }
  return 0;
}
