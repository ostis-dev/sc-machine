[![Build Status](https://travis-ci.com/ostis-dev/sc-machine.svg?branch=master)](https://travis-ci.com/ostis-dev/sc-machine)

## Documentation
[http://ostis-dev.github.io/sc-machine/](http://ostis-dev.github.io/sc-machine/)

## Build on Ubuntu

### Dependencies

Install python dependencies:

```sh
pip3 install -r requirements.txt
```

__Ubuntu 18.04__ and __Ubuntu 20.04__

```sh
sudo apt install librocksdb-dev libglib2.0-dev qtbase5-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev make cmake antlr gcc g++ llvm-7 libcurl4-openssl-dev libclang-7-dev libboost-python-dev python3-pip
```

Install Python evelopment package

__Ubuntu 18.04__

```
sudo apt install python3.6-dev
```

__Ubuntu 20.04__

```sh
sudo apt install python3.8-dev
```

### Make

```sh
mkdir build
cd build
cmake ..
make
```
