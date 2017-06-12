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
#include <thread>

using namespace std;
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

int WAIT = 1;

stringstream logstream;

namespace Handlers {
void index(HttpServer& server) {
  server.default_resource["GET"] = [&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      auto web_root_path = boost::filesystem::canonical("../angularjs/app");
      auto path = boost::filesystem::canonical(web_root_path / request->path);

      if (distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
          !equal(web_root_path.begin(), web_root_path.end(), path.begin())) {
        throw invalid_argument("path must be within root path");
      }
      if (boost::filesystem::is_directory(path)) {
        path /= "index.html";
      }

      std::string cache_control, etag;
      auto ifs = make_shared<ifstream>();
      ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

      if (*ifs) {
        auto length = ifs->tellg();
        ifs->seekg(0, ios::beg);
        *response << "HTTP/1.1 200 OK\r\n" << cache_control << etag << "Content-Length: " << length << "\r\n\r\n";
        default_resource_send(server, response, ifs);
      } else {
        throw invalid_argument("could not read file");
      }
    } catch (const exception& e) {
      string content = "Could not open path " + request->path + ": " + e.what();
      *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
    }
  };
}
void logz(HttpServer& server) {
  server.resource["/logz"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
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
  server.resource["/api/basic/ok"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      // if (WAIT) { send_wait(response); return; }
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
  server.resource["/api/basic/info"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      // if (WAIT) { send_wait(response); return; }
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
  server.resource["/api/basic/check"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      // if (WAIT) { send_wait(response); return; }
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
}  // basic
namespace network {
using namespace ::network;
void edges(HttpServer& server) {
  server.resource["/api/network/edges"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      log(logstream, "edges_request", request->content.string());
      ptree out;
      add_all_edges(out);
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "edges_error", e.what());
      send_error(response, e.what());
    }
  };
}
void get_edge(HttpServer& server) {
  server.resource["/api/network/get_edge"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
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
  server.resource["/api/network/set_edge"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
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
  server.resource["/api/network/adopt"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
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
        add_all_edges(out);
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
  server.resource["/api/network/reset"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      WAIT = 1;
      log(logstream, "reset_request", request->content.string());
      ptree out;
      add_all_edges(out);
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "reset_error", e.what());
      send_error(response, e.what());
    }
  };
}
void visualize(HttpServer& server) {
  server.resource["/api/network/visualize"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      log(logstream, "visualize_request", request->content.string());
      ptree out, edges, nodes;
      map<int, bool> visited;
      queue<node> Q;
      Q.push(node_info);
      while (!Q.empty()) {
        node curr = Q.front();
        Q.pop();
        if (visited[curr.uuid]) continue;
        visited[curr.uuid] = true;
        log(logstream, "visualize_bfs_curr_node", to_str(curr));
        nodes.push_back(make_pair("", node_to_json(curr)));
        vector<edge> neighbors;
        try {
          neighbors = requests::edges(curr.ip, curr.port).first;
          log(logstream, "visualize_bfs_neighbors", neighbors);
        } catch (exception& e) {
          log(logstream, "visualize_error", e.what());
        }
        for (edge& it : neighbors) {
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
}  // network
namespace jobs {
using namespace ::jobs;
using namespace ::network;
void jobs_add(HttpServer& server) {
  server.resource["/api/jobs/add/(.+)"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    job_mutex.lock();
    string jobid = request_param(request->path);
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_add_request_" + jobid, req_str);
      ptree in, out;
      string_to_json(req_str, in);
      new_job(jobid, json_to_job_request(in));
      add_all_edges(out);
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_add_error_" + jobid, e.what());
      send_error(response, e.what());
    }
    job_mutex.unlock();
  };
}
void jobs_new(HttpServer& server) {
  server.resource["/api/jobs/new"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_new_request", req_str);
      ptree in, out;
      string_to_json(req_str, in);
      string jobid = uuid4();
      job_request request = json_to_job_request(in);
      map<int, bool> visited;
      queue<node> Q;
      Q.push(node_info);
      while (!Q.empty()) {
        node curr = Q.front();
        Q.pop();
        if (visited[curr.uuid]) continue;
        visited[curr.uuid] = true;
        vector<edge> neighbors;
        try {
          neighbors = requests::jobs_add(curr.ip, curr.port, jobid, request).first;
          log(logstream, "bfs_neighbors", neighbors);
        } catch (exception& e) {
          log(logstream, "bfs_error", e.what());
        }
        for (edge& it : neighbors) {
          if (!visited[it.uuid]) {
            Q.push(edge_to_node(it));
          }
        }
      }
      out.put("jobid", jobid);
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_new_error", e.what());
      send_error(response, e.what());
    }
  };
}
void jobs_all(HttpServer& server) {
  server.resource["/api/jobs/all"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    job_mutex.lock();
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_all_request", req_str);
      ptree out, job_requests;
      for (auto& it : node_jobs) {
        ptree json = job_request_to_json(it.request);
        json.put("jobid", it.id);
        job_requests.push_back(make_pair("", json));
      }
      if (!job_requests.empty()) {
        out.put_child("jobs", job_requests);
      }
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_all_error", e.what());
      send_error(response, e.what());
    }
    job_mutex.unlock();
  };
}
void jobs_backup(HttpServer& server) {
  server.resource["/api/jobs/backup"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    job_mutex.lock();
    try {
      // if (WAIT) {
      //   send_wait(response);
      //   return;
      // }
      string req_str = request->content.string();
      log(logstream, "jobs_backup_request", req_str);
      ptree in, out;
      string_to_json(req_str, in);
      backup(json_to_backup_request(in));
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_backup_error", e.what());
      send_error(response, e.what());
    }
    job_mutex.unlock();
  };
}
void jobs_remove(HttpServer& server) {
  server.resource["/api/jobs/remove/(.+)"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    job_mutex.lock();
    string jobid = request_param(request->path);
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_remove_request_" + jobid, req_str);
      ptree out;
      remove_job(jobid);
      add_all_edges(out);
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_remove_error_" + jobid, e.what());
      send_error(response, e.what());
    }
    job_mutex.unlock();
  };
}
void jobs_kill(HttpServer& server) {
  server.resource["/api/jobs/kill/(.+)"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    string jobid = request_param(request->path);
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_kill_request_" + jobid, req_str);
      ptree out;
      map<int, bool> visited;
      queue<node> Q;
      Q.push(node_info);
      while (!Q.empty()) {
        node curr = Q.front();
        Q.pop();
        if (visited[curr.uuid]) continue;
        visited[curr.uuid] = true;
        vector<edge> neighbors;
        try {
          neighbors = requests::jobs_remove(curr.ip, curr.port, jobid).first;
        } catch (exception& e) {
          log(logstream, "bfs_error", e.what());
        }
        for (edge& it : neighbors) {
          if (!visited[it.uuid]) {
            Q.push(edge_to_node(it));
          }
        }
      }
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_kill_error_" + jobid, e.what());
      send_error(response, e.what());
    }
  };
}
void jobs_ids(HttpServer& server) {
  server.resource["/api/jobs/ids"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    job_mutex.lock();
    string jobid = request_param(request->path);
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_ids_request", req_str);
      log(logstream, "node_jobs_size", node_jobs.size());
      ptree out, ids;
      for (auto& it : node_jobs) {
        ptree json;
        json.put("", it.id);
        ids.push_back(make_pair("", json));
      }
      if (!ids.empty()) {
        out.put_child("jobids", ids);
      }
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_ids_error", e.what());
      send_error(response, e.what());
    }
    job_mutex.unlock();
  };
}
void jobs_data(HttpServer& server) {
  server.resource["/api/jobs/data/(.+)"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    job_mutex.lock();
    string jobid = request_param(request->path);
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_data_request_" + jobid, req_str);
      ptree out = job_data_to_json(jobid);
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_data_error_" + jobid, e.what());
      send_error(response, e.what());
    }
    job_mutex.unlock();
  };
}
void jobs_visualize(HttpServer& server) {
  server.resource["/api/jobs/visualize/(.+)"]["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
    string jobid = request_param(request->path);
    try {
      if (WAIT) {
        send_wait(response);
        return;
      }
      string req_str = request->content.string();
      log(logstream, "jobs_visualize_request_" + jobid, req_str);
      ptree out;
      job_mutex.lock();
      for (auto& it : node_jobs) {
        if (it.id == jobid) {
          out.add_child("startingpoints", points_to_json(it.starting_points));
          out.put("width", it.request.width);
          out.put("height", it.request.height);
          break;
        }
      }
      job_mutex.unlock();
      map<int, vector<point>> points;
      map<int, vector<point>> backup;
      map<int, bool> visited;
      queue<node> Q;
      Q.push(node_info);
      while (!Q.empty()) {
        node curr = Q.front();
        Q.pop();
        if (visited[curr.uuid]) continue;
        visited[curr.uuid] = true;
        job_data data;
        vector<edge> neighbors;
        try {
          data = requests::jobs_data(curr.ip, curr.port, jobid).first;
        } catch (exception& e) {
          log(logstream, "bfs_error", e.what());
        }
        try {
          log(logstream, "bfs_jobs_data_response", data);
          merge(data.points, points[data.uuid]);  // merge points
          for (auto& it : data.backup) {
            merge(it.second, backup[it.first]);  // merge backup points
          }
        } catch (exception& e) {
          log(logstream, "visualize_point_data_merge_error", e.what());
        }
        for (edge& it : data.edges) {
          if (!visited[it.uuid]) {
            Q.push(edge_to_node(it));
          }
        }
      }
      for (auto& it : backup) {  // merge backup points and points
        merge(it.second, points[it.first]);
      }
      out.add_child("points", point_map_to_json(points));
      send_ok(response, out);
    } catch (exception& e) {
      log(logstream, "jobs_visualize_error_" + jobid, e.what());
      send_error(response, e.what());
    }
  };
}
}  // jobs
}  // Handlers

namespace network {
void join() {
  try {
    e_parent.clear();
    e_next.clear();
    e_prev.clear();
    e_children.clear();
    node bs_node = requests::hello(bootstrap_ip, bootstrap_port, node_info.ip, node_info.port).first;
    if (node_info.uuid == -1) {
      node_info.uuid = bs_node.uuid;
    }
    e_next = node_to_edge(node_info, "next");
    e_prev = node_to_edge(node_info, "prev");
    if (!bs_node.exists()) {
      return;
    }
    node x = requests::info(bs_node.ip, bs_node.port).first;
    log(logstream, "join-node-bootstrap-returned", x);
    vector<edge> edges = requests::edges(x.ip, x.port).first;
    log(logstream, "join-bootstrap-edges", edges);
    if (!get_edge(edges, "parent").exists()) {
      log(logstream, "join-line", __LINE__);
      log(logstream, "join-edges-next", get_edge(edges, "next"));
      log(logstream, "join-edges-prev", get_edge(edges, "prev"));
      if (edge_equals_ignore_type(get_edge(edges, "prev"), get_edge(edges, "next"))) {  // 1 ili 2 u top levelu
        log(logstream, "join-line", __LINE__);
        edge x_next = requests::set_edge(x.ip, x.port, node_to_edge(node_info, "next")).first;
        log(logstream, "join-x_next", x_next);
        requests::set_edge(x_next.ip, x_next.port, node_to_edge(node_info, "prev"));
        e_prev = node_to_edge(x, "prev");
        e_next = edge(x_next, "next");
      } else {  // prvi u drugom levelu
        log(logstream, "join-line", __LINE__);
        requests::adopt(x.ip, x.port, node_to_edge(node_info, "child"), false);
        e_parent = node_to_edge(x, "parent");
      }
    } else {
      log(logstream, "join-line", __LINE__);
      edge parent = get_edge(edges, "parent");
      log(logstream, "join-parent", parent);
      adopt_response response = requests::adopt(parent.ip, parent.port, node_to_edge(node_info, "child"), true).first;
      log(logstream, "join-adopt_response", response);
      if (response.redirect == true) {
        log(logstream, "join-line", __LINE__);
        parent = response.next;
        response = requests::adopt(response.next.ip, response.next.port, node_to_edge(node_info, "child"), false).first;
        log(logstream, "join-adopt_response", response);
      }
      if (response.create_level == false) {
        e_parent = edge(parent, "parent");
        log(logstream, "join-line", __LINE__);
        edge x_next = requests::set_edge(x.ip, x.port, node_to_edge(node_info, "next")).first;
        log(logstream, "join-x_next", x_next);
        requests::set_edge(x_next.ip, x_next.port, node_to_edge(node_info, "prev"));
        log(logstream, "join-this_parent", e_parent);
        e_prev = node_to_edge(x, "prev");
        e_next = edge(x_next, "next");
      } else {  // T.T
        log(logstream, "join-line", __LINE__);
        log(logstream, "join-new-level-edges", response.edges);
        edge this_prev_0 = get_edge(response.edges, "child", 0);
        edge this_prev_1 = get_edge(response.edges, "child", 1);
        edge this_prev_2 = get_edge(response.edges, "child", 2);
        edge this_prev_3 = get_edge(response.edges, "child", 3);
        log(logstream, "join-this_prev_0", this_prev_0);
        log(logstream, "join-this_prev_1", this_prev_1);
        log(logstream, "join-this_prev_2", this_prev_2);
        log(logstream, "join-this_prev_3", this_prev_3);
        edge new_next = requests::get_edge(x.ip, x.port, "next").first;
        edge new_prev = requests::get_edge(this_prev_2.ip, this_prev_2.port, "prev").first;
        log(logstream, "join-this_new_prev_1_next", new_next);
        log(logstream, "join-this_new_prev_2_next", new_prev);
        requests::set_edge(new_next.ip, new_next.port, new_prev, "prev");
        requests::set_edge(new_prev.ip, new_prev.port, new_next, "next");
        requests::adopt(this_prev_0.ip, this_prev_0.port, this_prev_2, false);
        requests::set_edge(this_prev_2.ip, this_prev_2.port, this_prev_0, "parent");
        requests::adopt(this_prev_0.ip, this_prev_0.port, this_prev_3, false);
        requests::set_edge(this_prev_3.ip, this_prev_3.port, this_prev_0, "parent");
        requests::adopt(this_prev_1.ip, this_prev_1.port, node_to_edge(node_info, "child"), false);
        e_parent = edge(this_prev_1, "parent");
        requests::set_edge(this_prev_3.ip, this_prev_3.port, node_to_edge(node_info, "next"));
        requests::set_edge(this_prev_2.ip, this_prev_2.port, node_to_edge(node_info, "prev"));
        e_next = edge(this_prev_2, "next");
        e_prev = edge(this_prev_3, "prev");
      }
    }
  } catch (exception& e) {
    log(logstream, "JOIN NETWORK FAILED", e.what());
  }
}
int fail_cnt = 0;
void failover() {
  try {
    requests::ok(e_next.ip, e_next.port);
    fail_cnt = 0;
  } catch (exception& e) {
    fail_cnt++;
    log(logstream, "ping next failed", e.what());
  }
  if (fail_cnt == 5) {  // 5s proslo
    try {
      requests::check(e_prev.ip, e_prev.ip, e_next.ip, e_next.port);
    } catch (exception& e) {  // cvor pao rekonfiguracija mreze
      auto can_reset = requests::reset(bootstrap_ip, bootstrap_port).first;
      if (!can_reset) return;  // reset je u toku
      map<int, bool> visited;
      queue<node> Q;
      Q.push(node_info);
      while (!Q.empty()) {
        node curr = Q.front();
        Q.pop();
        if (visited[curr.uuid]) continue;
        visited[curr.uuid] = true;
        log(logstream, "recover_bfs_curr_node", to_str(curr));
        vector<edge> neighbors;
        try {
          neighbors = requests::node_reset(curr.ip, curr.port).first;
          log(logstream, "recover_bfs_neighbors", neighbors);
        } catch (exception& e) {
          log(logstream, "recover_error", e.what());
        }
        for (edge& it : neighbors) {
          if (!visited[it.uuid]) {
            Q.push(edge_to_node(it));
          }
        }
      }
      this_thread::sleep_for(chrono::seconds(5));  // for small number of nodes
      requests::reset_done(bootstrap_ip, bootstrap_port);
    }
    fail_cnt = 0;
  }
}
}  // network

void worker() {
  if (bootstrap_port == "1") {
    this_thread::sleep_for(chrono::milliseconds(100));
    WAIT = 0;
    return;
  } else {
    network::join();
    try {
      if (e_parent.exists()) {
        auto response = requests::jobs_all(e_parent.ip, e_parent.port);
        for (int i = 0; i < response.first.size(); i++) {
          jobs::new_job(response.second[i], response.first[i]);
        }
      }
    } catch (exception& e) {
      log(logstream, "GETTING_JOB_DATA_FROM_PARENT_FAILED", e.what());
    }
  }
  WAIT = 0;
  while (1) {
    network::failover();
    if (WAIT) {  // re-join
      network::join();
      WAIT = 0;
    }
    {  // generisanje tacke
      job_mutex.lock();
      jobs::work();
      jobs::send_backup();
      job_mutex.unlock();
    }
    this_thread::sleep_for(chrono::seconds(1));
  }
}

int main(int argc, char* argv[]) {
  if (argc != 5) {
    cout << "Pogresan broj parametra!\n";
    return 1;
  }
  srand(time(NULL));
  node_info.ip = argv[1];
  node_info.port = argv[2];
  bootstrap_ip = argv[3];
  bootstrap_port = argv[4];

  HttpServer server;
  server.config.port = atoi(argv[2]);
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
  // Jobs
  Handlers::jobs::jobs_add(server);
  Handlers::jobs::jobs_new(server);
  Handlers::jobs::jobs_all(server);
  Handlers::jobs::jobs_backup(server);
  Handlers::jobs::jobs_remove(server);
  Handlers::jobs::jobs_kill(server);
  Handlers::jobs::jobs_ids(server);
  Handlers::jobs::jobs_data(server);
  Handlers::jobs::jobs_visualize(server);

  thread server_thread([&server]() { server.start(); });

  thread worker_thread(worker);

  worker_thread.join();
  server_thread.join();

  return 0;
}
