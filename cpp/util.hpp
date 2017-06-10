#ifndef UTIL_HPP
#define UTIL_HPP

#include <sstream>
#include <cstring>
#include "http/server_http.hpp"

#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

void json_to_string(const ptree& json, string& out);

inline bool key_exists(const ptree& json, const string& key) {
  ptree::const_assoc_iterator it = json.find(key);
  return it != json.not_found();
}

inline string make_json(const string& ip, const string& port) {
  ptree json;
  json.put("port", port);
  json.put("ip", ip);
  string out;
  json_to_string(json, out);
  return out;
}

inline string make_json(const ptree& json) {
  string out;
  json_to_string(json, out);
  return out;
}

inline void json_to_string(const ptree& json, string& out) {
  std::ostringstream oss;
  write_json(oss, json);
  out = oss.str();
}

inline string json_to_string(const ptree& json) {
  std::ostringstream oss;
  write_json(oss, json);
  return oss.str();
}

inline void string_to_json(const string& str, ptree& out) {
  std::istringstream iss(str);
  read_json(iss, out);
}

inline string make_addr(const ptree& json) { return json.get<string>("ip") + ":" + json.get<string>("port"); }

inline string make_addr(const string& ip, const string& port) { return ip + ":" + port; }

inline void log(stringstream& logz, const string& handler, const string& error_msg) { logz << handler << " : " << error_msg << "\n"; }

void send(shared_ptr<HttpServer::Response> response, const string& body) {
  *response << "HTTP/1.1 200 OK\r\nContent-Length: " << body.size() << "\r\n\r\n" << body;
}

void send_ok(shared_ptr<HttpServer::Response> response, ptree& json) {
  json.put("status", "ok");
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

void send_wait(shared_ptr<HttpServer::Response> response) {
  ptree json;
  json.put("status", "wait");
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

void send_error(shared_ptr<HttpServer::Response> response, const string& error_msg) {
  ptree json;
  json.put("status", "error");
  json.put("message", error_msg);
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

void send_error(shared_ptr<HttpServer::Response> response, ptree& json, const string& error_msg) {
  json.put("status", "error");
  json.put("message", error_msg);
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

void default_resource_send(const HttpServer& server, const shared_ptr<HttpServer::Response>& response, const shared_ptr<ifstream>& ifs) {
  // read and send 128 KB at a time
  static vector<char> buffer(131072);  // Safe when server is running on one thread :(
  streamsize read_length;
  if ((read_length = ifs->read(&buffer[0], buffer.size()).gcount()) > 0) {
    response->write(&buffer[0], read_length);
    if (read_length == static_cast<streamsize>(buffer.size())) {
      server.send(response, [&server, response, ifs](const boost::system::error_code& ec) {
        if (!ec)
          default_resource_send(server, response, ifs);
        else
          cerr << "Connection interrupted" << endl;
      });
    }
  }
}

#endif
