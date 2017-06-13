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
- You should install the Xcode Command Line tools with xcode-select --install to get a version of clang that searches /usr/local by default.
- Run ./cpp/start and ./cpp/stop to create or destroy nodes and bootstrap servers.
- Alternatively run ./cpp/client for an interactive client.

## Unit Tests

- Install homebrew, openssl, cmake, boost and gcc
- Edit /cpp/test_config.hpp
- Run ./cpp/test
