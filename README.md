# distributed_chaos
KIDS projekat

## Docs
- Projekat https://drive.google.com/file/d/0B6P2gDjQqRbDMVptM3dfZmNWbVU/view?usp=sharing
- Dizajn https://docs.google.com/document/d/1YzOaIMauOf1US_li-Wa6A-0cFraVSx2Tim-j10xJb0k/edit?usp=sharing

## AngularJs
- Each server should serve static files from `/angularjs/app` and routes with `/api` prefix should accept and return `Content-Type: application/json`

## Java
- Install Maven `mvn install`
- Run `./build` to build
- Run `./start_bootstrap` to start bootstrap server (on port 9080, to change port you must make changes in `config.yml` and `./start_bootstrap`)
- Run `./start_node $PORT` to start node server on port $PORT

## C++
- Install cmake, openssl, boost and gcc
```
brew install cmake openssl boost gcc
```
- Install Xcode Command Line tools 
```
with xcode-select --install
```
- Starting node: ./cpp/start node <node_ip> <node_port> <bootstrap_ip> <bootstrap_port>
- Killing node: ./cpp/stop node <node_port>
- Starting bootstrap: ./cpp/start bs <bootstrap_port>
- Killing bootstrap: ./cpp/stop bs <bootstrap_port>
- Alternatively run ./cpp/client for an interactive client.

## C++ Unit Tests
- Edit /cpp/test_config.hpp
- Run ./cpp/test
