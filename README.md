# pilfflonk
 PIL-FFLONK repository

## Setup

### Clone repository
```sh
$ git clone git@github.com:0xPolygonHermez/pilfflonk.git
$ cd pilfflonk
$ git submodule init
$ git submodule update
```

### Compile
The following packages must be installed.
```sh
$ sudo apt update && sudo apt-get install build-essential cmake libgmp-dev libomp-dev libsodium-dev nlohmann-json3-dev nasm
# For testing purposes only
$ sudo apt-get install libgtest-dev
```

Run `make` to compile the project
```sh
$ make clean
$ make -j
```