# snakebot

A bot for snake

## Dependencies

* NodeJS 9.5
* Python
* C++ compiler

Python and C++ compiler are already available if you have basic dev tools installed on Linux of MacOS. On windows run this
command:

```
npm install --global --production windows-build-tools
```

If you don't want to run an algorithm implemented in C++ then it should still work with just node; however
if you run the whole set of tests you'll get errors if it can't load the C++ library.

On Ubuntu you can get required tools with:

```
sudo apt-get install build-essential
npm install -g node-gyp mocha
```

## Building

You only need to build if you use a C++ algorithm:

```
npm run build
```

## Running tests

```
npm install
npm install -g mocha
npm run build
npm run test
```

The build command only needs to be run once, or each time you change a cpp/hpp file.

## Running a bot

```
node index.js <PORT> <ALGORITHM>
```

`<ALGORITHM>` should be the name of one of the files in the `algorithm/` directory without the extension. For example:

```
node index.js 5000 defensive_hungry
```

Port and algorithm are parameterized so that you can easily run multiple instances at the same time using different algorithms.

For bots impemented in C++ you should prefix the algorithm name with `cpp_`. For example, to run the `test` algorithm:

```
node index.js 5000 cpp_test
```

In code the algorithm is NOT named `cpp_test`. It's just `test`!

## Adding a new bot

Just add a new js file to the `algorithms/` directory and export `move`, `start`, and `meta`.

## Running pure C++ without node js

Install cmake and boost:

```bash
sudo apt-get install cmake libboost-all-dev
```

Do standard cmake build process:

```bash
cd nonode
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. # or Release
make -j8 # replace 8 with # of cores or threads
./snakebot <port> <algorithm> # do not use cpp_ prefix
```

## Running pure C++ tests

The commands above also make an exe called `tests` so just run it.

## Running pure C++ benchmarks

The commands above also make an exe called `bench` so just run it.

## License (MIT)

Copyright 2018 Graeme Hill and Chris Sand

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
