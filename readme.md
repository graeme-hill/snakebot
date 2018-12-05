# snakebot

A bot for snake

## Dependencies

See `ununtuinit.sh`.

## Building

```
cd src
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. # or Release
make -j8 # replace 8 with # of cores or threads
./snakebot <port> <algorithm>
```

## Running tests

The commands above also make an exe called `tests` so just run it.

## Running benchmarks

The commands above also make an exe called `bench` so just run it.

## Running a bot

```
./snakebot <PORT> <ALGORITHM>
node index.js <PORT> <ALGORITHM>
```

`<ALGORITHM>` should be the name of one of the files in the `algorithm/` directory without the extension. For example:

```
./snakebot 5000 sim
```

## License (MIT)

Copyright 2018 Graeme Hill and Chris Sand

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
