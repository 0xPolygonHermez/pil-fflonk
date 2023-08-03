# Pil-Fflonk
 PIL-FFLONK repository

### Setup
The repository can be initialized as follows: 

```sh
$ git clone git@github.com:0xPolygonHermez/pilfflonk.git
$ cd pilfflonk
$ git submodule init
$ git submodule update
$ cd depends/ffiasm && npm i 
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
$ make init 
$ make -j
```

### Building proof

The prover is compiled in the build directory.

You can either store all the required files in the config folder or provide them as a command.

It can be called in two different ways:

```sh
./build/pfProver false <proof.json> <public.json> <pilfflonk.zkey> <pilfflonk.fflonkinfo.json> <pilfflonk.commit> 
```

```sh
./build/pfProver true <proof.json> <public.json> <pilfflonk.zkey> <pilfflonk.fflonkinfo.json> <pilfflonk.exec> <circuit.dat> <circuit.proof.zkin.json> 
```

If the files are already stored in the config folder under the name pilfflonk.*, it will be enough to call the prover like

```sh
./build/pfProver true
```
or
```sh
./build/pfProver false
```
depending if you want to execute the proof using the .commit file or generate the commits using the .exec file

### Generating setup

Zkey file can also be generated in C++. 

In order to do it, you'll need to first compile the setup

```sh
$ make -j setup
```

And then run the setup as follows:

```sh
$ ./build/pfSetup <powersOfTau.ptau> <pilfflonk.zkey> <pilfflonk.shkey.json> <pilfflonk.fflonkinfo.json> <pilfflonk.const>
```

### Notes

ShKey and FflonkInfo files can be generated using pil-stark repository. In order to do that, one must connect to `pilfflonk` branch