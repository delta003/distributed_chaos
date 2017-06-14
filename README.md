# Distributed Game of Chaos
## Docs
- Project https://drive.google.com/file/d/0B6P2gDjQqRbDMVptM3dfZmNWbVU/view?usp=sharing
- Design https://docs.google.com/document/d/1YzOaIMauOf1US_li-Wa6A-0cFraVSx2Tim-j10xJb0k/edit?usp=sharing

## AngularJs
- Each server should serve static files from `/angularjs/app` and routes with `/api` prefix should accept and return `Content-Type: application/json`

## Java
- Install Maven
- Run `./build` to build
- Run `./start_bootstrap` to start bootstrap server (on port 9080, to change port you must make changes in `config.yml` and `./start_bootstrap`)
- Run `./start_node $PORT` to start node server on port $PORT

## C++
##### Installation
- Install gcc and make sure it's on your path variable. On OS X:
```
brew install gcc
```

- Install boost to /usr/local/include/boost. On OS X:
```
brew install boost
```

- Make sure boost is on your path variable. On OS X:
```
xcode-select --install
```

##### Building
- Run ./cpp/build, however building is not necessary. Scripts that spawn nodes will use md5 to hash your files, and call ./cpp/build if the project hasn't been built yet, or if some of the source code was changed.

##### Running
- Starting node: ./cpp/start node <node_ip> <node_port> <bootstrap_ip> <bootstrap_port>
- Killing node: ./cpp/stop node <node_port>
- Starting bootstrap: ./cpp/start bs <bootstrap_port>
- Killing bootstrap: ./cpp/stop bs <bootstrap_port>
- Alternatively run ./cpp/client for an interactive client

##### Unit Tests
- Edit /cpp/test_config.hpp
- Run ./cpp/test

## Python
##### Installation
todo

##### Running
todo
