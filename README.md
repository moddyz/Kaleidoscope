# Kaleidoscope

This is an implementation of the Kaleidoscope language, following the tutorial here:
https://llvm.org/docs/tutorial/MyFirstLanguageFrontend

## Table of Contents

- [Documentation](#documentation)
- [Building](#building)
  - [Requirements](#requirements)
- [Build Status](#build-status)

## Documentation

Documentation based on the latest state of master, [hosted by GitHub Pages](https://moddyz.github.io/Kaleidoscope/).

## Building

Quick build:
```bash
./build.sh <INSTALLATION_PATH>
```

### Requirements

- `>= CMake-3.17`
- `>= C++17`
- `doxygen` and `graphviz` (optional for documentation)
- \>= `llvm-9.0.1`

## Build Status

|       | master | 
| ----- | ------ | 
| macOS-10.14 | [![Build Status](https://travis-ci.com/moddyz/Kaleidoscope.svg?branch=master)](https://travis-ci.com/moddyz/Kaleidoscope) |

