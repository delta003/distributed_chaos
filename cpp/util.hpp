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

inline void json_to_string(const ptree& json, string& out) {
  std::ostringstream oss;
  write_json(oss, json);
  out = oss.str();
}

inline void string_to_json(const string& str, ptree& out) {
  std::istringstream iss(str);
  read_json(iss, out);
}


inline void log(stringstream& logz, const string& handler, const string& error_msg) {
  logz << "@" << handler << " : " << error_msg << "\n"; 
}

void send(shared_ptr<HttpServer::Response> response, const string& body) {
  *response << "HTTP/1.1 200 OK\r\nContent-Length: " << body.size() << "\r\n\r\n" << body;
}

void send_ok(shared_ptr<HttpServer::Response> response, ptree& json) {
  json.put("status", "OK");
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

void send_wait(shared_ptr<HttpServer::Response> response, ptree& json) {
  json.put("status", "WAIT");
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

void send_error(shared_ptr<HttpServer::Response> response, const string& error_msg) {
  ptree json;
  json.put("status", "ERROR");
  json.put("message", error_msg);
  string json_str;
  json_to_string(json, json_str);
  send(response, json_str);
}

#endif 
