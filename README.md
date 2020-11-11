[![Build Status](https://travis-ci.com/ostis-dev/sc-machine.svg?branch=master)](https://travis-ci.com/ostis-dev/sc-machine)

## Documentation
[http://ostis-dev.github.io/sc-machine/](http://ostis-dev.github.io/sc-machine/)

## Clone repository

```sh
git clone git@github.com:ostis-dev/sc-machine.git
cd sc-machine
git submodule update --init --recursive
```

## Build on Ubuntu

### Dependencies

Install python dependencies:

__Ubuntu 18.04+__

```sh
sudo apt install python3-pip
pip3 install -r requirements.txt
```

__Ubuntu 18.04+__

```sh
sudo apt install librocksdb-dev libglib2.0-dev qtbase5-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev make cmake antlr gcc g++ llvm-7 libcurl4-openssl-dev libclang-7-dev libboost-python-dev
```

### Make

```sh
mkdir build
cd build
cmake ..
make
```

## Build on Linux Mint

### Dependencies

Install python dependencies:

__Linux Mint 19__

```sh
sudo apt install python3-pip python3-setuptools python3-wheel
pip3 install -r requirements.txt
```

__Linux Mint 20__

```sh
sudo apt install python3-pip
pip3 install -r requirements.txt
```

__Linux Mint 19+__

```sh
sudo apt install librocksdb-dev libglib2.0-dev qtbase5-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev make cmake antlr gcc g++ llvm-7 libcurl4-openssl-dev libclang-7-dev libboost-python-dev
```

### Make

```sh
mkdir build
cd build
cmake ..
make
```
