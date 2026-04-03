# eave

A simple audio visualizer using OpenGL and PulseAudio.

## Get it to Work

First, compile the binary
```bash

mkdir build

cd build

cmake .. # generates all build files within the ./build directory

make # compiles
```

Then to run it:

In the build directory, run

```bash
./eave 
```


## Things left to do

- Make it look good (Not my strong suit)
- Improve performance
- Give it a full player UI

## Credits
This Project uses [KissFFT](https://github.com/mborgerding/kissfft) to perform fast fourier transforms on the raw audio data.

Really cool library. Check it out.