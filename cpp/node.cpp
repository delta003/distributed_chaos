// Noded server.

#include "http/server_http.hpp"
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
  string uuid;
  string node_ip;
  string node_port;
  string bootstrap_ip;
  string bootstrap_port;
} node_info;

namespace Handlers {

} // Handlers

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  node_info.node_ip = argv[1];
  node_info.node_port = argv[2];
  node_info.bootstrap_ip = argv[3];
  node_info.bootstrap_port = argv[4];

  HttpServer server;
  server.config.port=atoi(argv[2]);

  // Handlers::index(server);

  thread server_thread([&server](){
      server.start();
  });
    
  server_thread.join();

  return 0;
}
