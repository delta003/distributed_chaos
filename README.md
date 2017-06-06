# distributed_chaos
KIDS projekat

## Docs
- Projekat https://drive.google.com/file/d/0B6P2gDjQqRbDMVptM3dfZmNWbVU/view?usp=sharing
- Dizajn https://docs.google.com/document/d/1YzOaIMauOf1US_li-Wa6A-0cFraVSx2Tim-j10xJb0k/edit?usp=sharing

## AngularJs
- Each server should serve static files from `/angularjs/app` and routes with 
`/api` prefix should return Content-Type:application/json

## Java
- Install Maven
- Run `mvn package` to build
- See `dropwizard-template.yml` for server configuration
- Run `sh start_server.sh` to start server

## C++
- Install cmake, openssl, boost and gcc
```
brew install cmake openssl boost gcc
```
- Use cpp/start.sh and cpp/stop.sh to create or destroy nodes and bootstrap servers.
