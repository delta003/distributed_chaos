// Node server.

#include "http/server_http.hpp"
#include "http/client_http.hpp"

#include "util.hpp"
#include "node.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#include <queue>
#include <map>
#include <algorithm>
#include <cstdlib>

using namespace std;
using namespace boost::property_tree;
using requests::status;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

int WAIT = 1;
int RESET = 0;

stringstream logstream;

namespace Handlers {
  void index(HttpServer& server) {
    server.default_resource["GET"]=[&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
        auto web_root_path=boost::filesystem::canonical("../angularjs/app");
        auto path=boost::filesystem::canonical(web_root_path/request->path);

        if (distance(web_root_path.begin(), web_root_path.end())>distance(path.begin(), path.end()) ||
            !equal(web_root_path.begin(), web_root_path.end(), path.begin())) {
          throw invalid_argument("path must be within root path");
        }
        if (boost::filesystem::is_directory(path)) {
          path/="index.html";
        }

        std::string cache_control, etag;
        auto ifs=make_shared<ifstream>();
        ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

        if (*ifs) {
            auto length=ifs->tellg();
            ifs->seekg(0, ios::beg);
            *response << "HTTP/1.1 200 OK\r\n" << cache_control << etag << "Content-Length: " << length << "\r\n\r\n";
            default_resource_send(server, response, ifs);
        } else {
          throw invalid_argument("could not read file");
        }
      } catch(const exception &e) {
          string content="Could not open path "+request->path+": "+e.what();
          *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
      }
    };
  }
  void logz(HttpServer& server) {
    server.resource["/logz"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
        send(response, logstream.str());
      } catch (exception& e) {
        log(logstream, "logz_error", e.what());
        send_error(response, e.what());
      }
    };
  }
  namespace basic {
    void ok(HttpServer& server) {
    server.resource["/api/basic/ok"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);     
          log(logstream, "ok_request", request->content.string());
          ptree out;
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "ok_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void info(HttpServer& server) {
    server.resource["/api/basic/info"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          log(logstream, "info_request", request->content.string());
          ptree out;
          out.put("uuid", node_info.uuid);
          out.put("ip", node_info.ip);
          out.put("port", node_info.port);

          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "info_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void check(HttpServer& server) {
    server.resource["/api/basic/check"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "check_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          HttpClient client(make_addr(in));
          ptree out;
          try {
            read_json(client.request("GET", "/api/basic/ok")->content, out);
            out.put("alive", "true");
            if (key_exists(out, "message")) {
              log(logstream, "check_response_error", out.get<string>("message"));
            }
            send_ok(response, out);
          } catch (exception& e) {
            out.put("alive", "false");
            send_ok(response, out);
          }
        } catch (exception& e) {
          log(logstream, "check_error", e.what());
          send_error(response, e.what());
        }
      };
    }
  } // basic
  namespace network {
    using namespace ::network;
    void edges(HttpServer& server) {
    server.resource["/api/network/edges"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          log(logstream, "edges_request", request->content.string());
          ptree out;
          ptree edges;
          add_all_edges(edges);
          if (!edges.empty()) {
            out.add_child("edges", edges);
          }
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "edges_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void get_edge(HttpServer& server) {
    server.resource["/api/network/get_edge"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "get_edge_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          ptree out;
          add_edge(in.get<string>("type"), out, "edge");
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "get_edge_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void set_edge(HttpServer& server) {
    server.resource["/api/network/set_edge"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "set_edge_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          edge new_edge;
          json_to_edge(in.get_child("edge"), new_edge);
          ptree out;
          add_edge(in.get_child("edge").get<string>("type"), out, "oldedge");
          set_new_edge(new_edge);
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "set_edge_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void adopt(HttpServer& server) {
    server.resource["/api/network/adopt"]["POST"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          string req_str = request->content.string();
          log(logstream, "set_adopt_request", req_str);
          ptree in;
          string_to_json(req_str, in);
          edge new_edge;
          bool can_redirect = in.get<bool>("can_redirect");
          json_to_edge(in.get_child("edge"), new_edge);
          ptree out;
          int size = e_children.size();
          if (size != 2) {
            e_children.push_back(new_edge);
          }
          if (size == 0 || size == 1 || size == 3) {
            out.put("redirect", "false");
            out.put("create_level", "false");
          }
          if (size == 2) {
            out.put("redirect", can_redirect);
            if (can_redirect == true) {
              add_edge("next", out, "next");
            } else {
              e_children.push_back(new_edge);
              out.put("create_level", "false");
            }
          }
          if (size == 4) {
            out.put("redirect", "false");
            out.put("create_level", "true");
            ptree edges;
            add_all_edges(edges);
            out.add_child("edges", edges);
            e_children.resize(2);
          }
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "set_adopt_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void reset(HttpServer& server) {
    server.resource["/api/network/reset"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          log(logstream, "reset_request", request->content.string());
          RESET = WAIT = 1;
          ptree out, edges;
          add_all_edges(edges);
          out.add_child("edges", edges);
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "reset_error", e.what());
          send_error(response, e.what());
        }
      };
    }
    void visualize(HttpServer& server) {
    server.resource["/api/network/visualize"]["GET"]=[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
      try {
          if (WAIT) send_wait(response);
          log(logstream, "visualize_request", request->content.string());
          ptree out, edges, nodes;
          map<int, bool> visited;
          queue<node> Q;
          Q.push(node_info);
          while (!Q.empty()) {
            node curr = Q.front(); Q.pop();
            log(logstream, "visualize_bfs_curr_node", to_str(curr));
            visited[curr.uuid] = true;
            nodes.push_back(make_pair("", node_to_json(curr)));
            vector<edge> neighbors;
            try {
              neighbors = requests::edges(curr.ip, curr.port).first;
            } catch (exception& e) {
              log(logstream, "visualize_error", e.what());
            }
            for (edge& it: neighbors) {
              edges.push_back(make_pair("", vedge_to_json(edge_to_vedge(it, curr.uuid))));
              if (!visited[it.uuid]) {
                Q.push(edge_to_node(it));
              }
            }
          }
          out.add_child("nodes", nodes);
          out.add_child("edges", edges);
          send_ok(response, out);
        } catch (exception& e) {
          log(logstream, "visualize_error", e.what());
          send_error(response, e.what());
        }
      };
    }
  } // network
} // Handlers

namespace network {
  void join() {
    try {

    } catch (exception& e) {
      cout << e.what() << endl;
      cout << "JOIN NETWORK FAILED" << endl;
      exit(1);
    }
  }
  void recover() {

  }
} // network 

void worker() {
  if (bootstrap_port == "1") {
    this_thread::sleep_for(chrono::milliseconds(100));
    WAIT = 0;
    return;
  }
  network::join();
  WAIT = 0;
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  node_info.ip = argv[1];
  node_info.port = argv[2];
  bootstrap_ip = argv[3];
  bootstrap_port = argv[4];

  HttpServer server;
  server.config.port=atoi(argv[2]);
  server.config.thread_pool_size = 2;

  // / i /logz
  Handlers::index(server);
  Handlers::logz(server);
  // Basic
  Handlers::basic::ok(server);
  Handlers::basic::info(server);
  Handlers::basic::check(server);
  // Network
  Handlers::network::edges(server);
  Handlers::network::get_edge(server);
  Handlers::network::set_edge(server);
  Handlers::network::adopt(server);
  Handlers::network::reset(server);
  Handlers::network::visualize(server);

  thread server_thread([&server](){
      server.start();
  });

  thread worker_thread(worker);

  worker_thread.join();
  server_thread.join();

  return 0;
}
