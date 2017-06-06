// Klijenat za testiranje.

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

namespace Bootstrap {
  void start_bootstrap(string port) {
    string exec = "./start bs " + port + " > /dev/null";
    system(exec.c_str());
    this_thread::sleep_for(chrono::seconds(1));
  }

  void stop_bootstrap(string port) {
    string exec = "./stop bs " + port + " > /dev/null";

    system(exec.c_str());
  }

  string make_json(string ip, string port) {
    ptree json;
    json.put("port", port);
    json.put("ip", ip);
    string out;
    json_to_string(json, out);
    return out;
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
    assert(json.get<string>("status") == "OK");
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

int main(int argc, char *argv[]) {
  Bootstrap::test_hello();
  Bootstrap::test_reset();
  return 0;
}
