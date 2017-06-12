// Unit tests config.
// Treba promeniti sledeće metode:
//  Bootstrap::start_bootstrap() i Bootstrap::stop_bootstrap() - pokretanje i
//  stopiranje bootstrap-a.
//  Node::start_node_public() i Node::stop_node() - pokretanje i stopiranje noda.
//  print_response = true | false
//  bootstrap_skip = <string>

#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP

string git_root() {
  auto up = [](string file) { return file.substr(0, file.rfind("/")); };
  return up(up(__FILE__));
}

namespace config {

// Da li stampa odgovore (json) koje dobija od servera.
bool print_response = true;

// Ovo se salje nodu kao <bootstrap_port> argument
// prilikom startovanja nekih testova.
// Ako je node primio ovu vrednost <bootstrap_porta> treba SAMO da napravi api-e, umesto da radi
// standardni proces startovanja cvora - pridruzivanje u mrezu, detekcija, failovanja suseda itd itd.
string bootstrap_skip = "1";

namespace Bootstrap {
void start_bootstrap(string port) {
  string exec = git_root() + "/cpp/start bs " + port + " > /dev/null 2>&1";
  system(exec.c_str());
}

void stop_bootstrap(string port) {
  string exec = git_root() + "/cpp/stop bs " + port + " > /dev/null 2>&1";
  system(exec.c_str());
}
}  // Bootstrap

namespace Node {
void start_node_public(string node_ip, string node_port, string bootstrap_ip, string bootstrap_port) {
  string exec = git_root() + "/cpp/start node " + node_ip + " " + node_port + " " + bootstrap_ip + " " + bootstrap_port + " > /dev/null 2>&1";
  system(exec.c_str());
}
void start_node(string node_port, string bootstrap_port) { start_node_public("localhost", node_port, "localhost", bootstrap_port); }

void stop_node(string node_port) {
  string exec = git_root() + "/cpp/stop node " + node_port + " > /dev/null 2>&1";
  system(exec.c_str());
}
}  // Node
}  // config

#endif
