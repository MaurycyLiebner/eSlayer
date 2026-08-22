# eSlayer

eSlayer is an open-source 2D isometric hack-and-slash video game demo.

## How To Play

To play eSlayer you can [download the latest release files from GitHub](https://github.com/MaurycyLiebner/eZeus/releases) and build/download the binary files.

**Windows**: you can download a [prebuilt eSlayer binaries](https://github.com/MaurycyLiebner/eZeus/releases).

**Linux**: you have to build eSlayer yourself - it should be pretty straightforward.

## Build instructions

### Build on Linux

[Here](https://wiki.libsdl.org/SDL3/README-linux) SDL3 dependencies are listed.

Install SDL3 dependencies (provided command for Ubuntu):

```
sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev libusb-1.0-0-dev
```
Install zlib:
```
sudo apt install zlib1g-dev
```

Clone the eSlayer repository:
```
git clone https://github.com/MaurycyLiebner/eSlayer
```

Enter the eSlayer directory:
```
cd eSlayer
```

Create and enter a build directory:
```
mkdir -p build
cd build
```

Configure the project:
```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Compile (replace **6** with the number of CPU cores):
```
cmake --build . -j 6
```

After the build is finished you should have **build/eSlayerGame/eSlayerGame** executable file.
Put the executable in **Bin/Bin/** directory of the downloaded release files and run it.

## Dependencies

eSlayer is build using following libraries:

[SDL3](https://github.com/libsdl-org/SDL)
[nlohmann/json](https://github.com/nlohmann/json)
[RapidCSV](https://github.com/d99kris/rapidcsv.git)
[libzip](https://github.com/nih-at/libzip.git)
[tinyxml2](https://github.com/leethomason/tinyxml2.git)

## Code

The code was written without the use of AI.

## Graphics

The Slayer character was created using [Blender 5.2](https://www.blender.org/) and [MakeHuman](http://www.makehumancommunity.org).

The remaining graphics is AI generated with varying amount of editing.

## Audio

eSlayer currently has no audio.

## Authors
**Maurycy Liebner** - 2026 - [MaurycyLiebner](https://github.com/MaurycyLiebner)