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

using config::Node::start_node_public;
using config::Node::stop_node;
using config::Bootstrap::start_bootstrap;
using config::Bootstrap::stop_bootstrap;

namespace config {
int port_base = 4000;  // portovi u opsegu [port_base, port_base + 100) ne smeju biti zauzeti

string bs_port = "9080";  // bootstrap port, ne sme biti zauzet

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
       << "  1) up <n> - spawnuje n novih nodova.\n  2) k|kill n - ubija cvor na portu n.\n"
       << "  3) k2|kill2 <n> <m> - ubija dva cvora istovremeno.\n  4) j|job <port> w h p n x_1 y_1 ... x_n y_n - pocetak posla\n"
       << "  5) q|quit - napusta program i stopira mrezu.\n\n";

  string cmd;
  while (1) {
    cin >> cmd;
    if (cmd == "up") {
      int broj;
      cin >> broj;
      vector<thread> pool;
      ostringstream oss;
      for (int i = 0; i < broj; i++) {
        oss << "Node started {ip = " << config::ip << ", port = " << port << "}\n";
        start_node_public(config::ip, to_string(port), config::ip, config::bs_port);
        ++port;
      }
      for (auto& t : pool) {
        t.join();
      }
      cout << oss.str();
    } else if (cmd == "kill" || cmd == "k") {
      string n;
      cin >> n;
      stop_node(n);
      cout << "Node stopped {ip = " << config::ip << ", port = " << n << "}\n";
    } else if (cmd == "kill2" || cmd == "k2") {
      string n, m;
      cin >> n >> m;
      stop_node(n);
      stop_node(m);
      cout << "Node stopped {ip = " << config::ip << ", port = " << n << "}\n";
      cout << "Node stopped {ip = " << config::ip << ", port = " << m << "}\n";
    } else if (cmd == "q" || cmd == "quit") {
      break;
    } else if (cmd == "j" || cmd == "job") {
      job_request request;
      int n;
      string port;
      cin >> port >> request.width >> request.height >> request.p >> n;
      while (n--) {
        double x, y;
        cin >> x >> y;
        request.points.push_back(point(x, y));
      }
      try {
        pair<string, status> response = requests::jobs_new(config::ip, port, request);
        if (response.second.code == "ok") {
          cout << "Started job " << response.first << " on {ip = " << config::ip << ", port = " << port << "}\n";
        } else {
          cout << "Start failed, got response: " << response.second.code << "\n";
        }
      } catch (exception& e) {
        cout << e.what() << endl;
      }
    }
  }
  clean_up();
  return 0;
}
