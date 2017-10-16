# Distributed Game of Chaos
Self-organizing distributed system with nodes written in C++, Java and Python, that supports failover of upto 2 nodes at the same time.

The project was written for Concurrent and Distributed Systems class at Faculty of Computing (RAF), in Belgrade, as part of a team effort with Dragan Marković and Lazar Milenković.

## Introduction
- Project https://drive.google.com/file/d/0B6P2gDjQqRbDMVptM3dfZmNWbVU/view?usp=sharing
- Design https://docs.google.com/document/d/1YzOaIMauOf1US_li-Wa6A-0cFraVSx2Tim-j10xJb0k/edit?usp=sharing
- Network architecture https://imgur.com/a/yql0r

## Prerequisites for running (OS X)
##### Java
- Install maven.
```
brew install maven
```

##### C++
- Install gcc and make sure it's on your path variable.
```
brew install gcc
```
- Install boost to /usr/local/include/boost.
```
brew install boost
```
- Install command line tools.
```
xcode-select --install
```

##### Python
- Install Python 3.5
```
brew install python3
```
- Go to python folder
```
cd python
```
- Create virtual environment
```
python3.5 -m venv myvenv
```
- Install all required modules
```
myvenv/bin/pip3.5 install -r requirements.txt
```

## Running
Start `runner.py` for an interactive console.

