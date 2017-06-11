// Klijent za startovanje cvorova (interaktivni).
// Koriscenje: ./client

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

using config::Node::start_node;
using config::Node::start_node_public;
using config::Node::stop_node;
using config::Bootstrap::start_bootstrap;
using config::Bootstrap::stop_bootstrap;

namespace config {
int port_base = 2000;  // treba imati slobodno sledecih 100 portova

string bs_port = "9080";  // bootstrap port

string ip = "localhost";  // ip masine

}  // config

int port = config::port_base;  // port poslednjeg noda

void clean_up() {
  for (int i = 0; i < 100; i++) {
    string port = to_string(config::port_base + i);
    stop_bootstrap(port);
    stop_node(port);
  }
  stop_bootstrap(config::bs_port);
}

int main(int argc, char* argv[]) {
  clean_up();
  start_bootstrap(config::bs_port);
  cout << "Bootstrap startovan na portu: " << config::bs_port << "\n\n";

  cout << "Koriscenje:\n"
       << "  1) up <n> - spawnuje n novih nodova.\n  2) k|kill n - ubija cvor sa uuid-om n.\n"
       << "  3) k2|kill2 <n> <m> - ubija dva cvora istovremeno.\n  4) q|quit - napusta program i stopira mrezu.\n\n";

  string cmd;
  while (1) {
    cin >> cmd;
    if (cmd == "up") {
      int broj;
      cin >> broj;
      vector<thread> pool;
      ostringstream oss;
      for (int i = 0; i < broj; i++) {
        oss << "Node " << i << " started {ip = " << config::ip << ", port = " << port << "}\n";
        start_node_public(config::ip, to_string(port), config::ip, config::bs_port);
        ++port;
      }
      for (auto& t : pool) {
        t.join();
      }
      cout << oss.str();
    } else if (cmd == "kill" || cmd == "k") {
    } else if (cmd == "kill2" || cmd == "k2") {
    } else if (cmd == "q" || cmd == "quit") {
      break;
    }
  }
  clean_up();
  return 0;
}
