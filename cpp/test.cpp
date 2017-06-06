// Unit tests.

#include "http/client_http.hpp"

#include "util.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <chrono>
#include <thread>

using namespace std;

typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

string cwd() {
  string file = __FILE__;
  while (file.back() != '/') {
    file.pop_back();
  }
  return file;
}

namespace Bootstrap {
  void start_bootstrap(string port) {
    string exec = cwd() + "start bs " + port + " > /dev/null";
    system(exec.c_str());
    this_thread::sleep_for(chrono::seconds(1));
  }

  void stop_bootstrap(string port) {
    string exec = cwd() + "stop bs " + port + " > /dev/null";
    system(exec.c_str());
  }

  void test_hello() {
    string port = "2000";
    start_bootstrap(port);
    HttpClient client("localhost:" + port);

    ptree json;
    read_json(client.request("POST", "/api/hello", make_json("1.1.1.1", "224"))->content, json);
    assert(json.get<int>("uuid") == 0);

    read_json(client.request("POST", "/api/hello", make_json("2.1.1.1", "225"))->content, json);
    assert(json.get<int>("uuid") == 1);
    assert(json.get<string>("ip") == "1.1.1.1");
    assert(json.get<string>("port") == "224");

    read_json(client.request("POST", "/api/hello", make_json("3.1.1.1", "226"))->content, json);
    assert(json.get<int>("uuid") == 2);
    assert(json.get<string>("ip") == "2.1.1.1");
    assert(json.get<string>("port") == "225");

    client.request("GET", "/api/reset");
    read_json(client.request("POST", "/api/hello", make_json("4.1.1.1", "227"))->content, json);
    assert(json.get<int>("uuid") == 0);

    read_json(client.request("POST", "/api/hello", make_json("1.1.1.1", "224"))->content, json);
    assert(json.get<int>("uuid") == 1);
    assert(json.get<string>("ip") == "4.1.1.1");
    assert(json.get<string>("port") == "227");

    stop_bootstrap(port);

    cout << "Boostrap Hello Test: OK" << endl;
  }

  void test_reset() {
    string port = "2000";
    start_bootstrap(port);
    HttpClient client("localhost:" + port);
    ptree json;
    read_json(client.request("GET", "/api/reset")->content, json);
    assert(json.get<string>("status") == "ok");
    stop_bootstrap(port);

    cout << "Boostrap Reset Test: OK" << endl;
  }

  void test_logz() {
    string port = "2000";
    start_bootstrap(port);
    HttpClient client("localhost:" + port);
    client.request("POST", "/api/hello", "error");
    client.request("POST", "/api/hello", "not a json");
    client.request("GET", "/api/reset", "resetuj se!!!");
    auto response = client.request("GET", "/logz");
    cout << response->content.rdbuf() << endl;
    stop_bootstrap(port);
  }

} // Bootstrap

namespace Node {
  void start_node(string node_port, string boot_port) {
    string exec = cwd() + "start node localhost " + node_port + " localhost " + boot_port + " > /dev/null";
    system(exec.c_str());
  }

  void stop_node(string node_port) {
    string exec = cwd() + "stop node " + node_port + " > /dev/null";
    system(exec.c_str());
  }

  namespace basic {
    void test_ok() {
      string port = "2000";
      start_node(port, "300");
      this_thread::sleep_for(chrono::seconds(1));
      HttpClient client("localhost:" + port);
      ptree json;
      read_json(client.request("GET", "/api/basic/ok")->content, json);
      assert(json.get<string>("status") == "ok");
      stop_node(port);
      cout << "Node Basic Ok Test: OK" << endl;
    }
    void test_info() {
      string port = "2000";
      start_node(port, "300");
      this_thread::sleep_for(chrono::seconds(1));
      HttpClient client("localhost:" + port);
      ptree json;
      read_json(client.request("GET", "/api/basic/info")->content, json);
      assert(json.get<int>("uuid") == 0);
      assert(json.get<string>("ip") == "localhost");
      assert(json.get<string>("port") == port);
      stop_node(port);
      cout << "Node Basic Info Test: OK" << endl;
    }
    void test_check() {
      string port_1 = "2016";
      string port_2 = "2224";
      start_node(port_1, "300");
      start_node(port_2, "300");
      this_thread::sleep_for(chrono::seconds(1));
      HttpClient client(make_addr("localhost", port_1));
      ptree in;
      in.put("ip", "localhost");
      in.put("port", port_2);
      ptree out;
      read_json(client.request("POST", "/api/basic/check", make_json(in))->content, out);
      assert(out.get<string>("alive") == "true");
      stop_node(port_2);
      read_json(client.request("POST", "/api/basic/check", make_json(in))->content, out);
      assert(out.get<string>("alive") == "false");
      stop_node(port_1);
      cout << "Node Check Test: OK" << endl;
    }
  }

};

int main(int argc, char *argv[]) {
  // Bootstrap::test_hello();
  // Bootstrap::test_reset();
  // Node::basic::test_ok();
  // Node::basic::test_info();
  Node::basic::test_check();
  return 0;
}
