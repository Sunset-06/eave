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

You need to actually add your monitor to MY_SOURCE in `aud.c`

To find your monitor:
```bash
pactl list short sources | grep monitor # will list out all monitors
```

After this paste the monitor in the MY_SOURCE macro.

## Things left to do

- Make it look good (Not my strong suit)
- Dynamically select monitors
- Improve performance (especially around that pesky little buffer)

## Credits
This Project uses [KissFFT](https://github.com/mborgerding/kissfft) to perform fast fourier transforms on the raw audio data.

Really cool library. Check it out.