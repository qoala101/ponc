# Passive Optical Network Calculator (PONC)

[![Build Linux](https://github.com/qoala101/ponc/actions/workflows/build-linux.yml/badge.svg)](https://github.com/qoala101/ponc/actions/workflows/build-linux.yml)
[![Build Windows](https://github.com/qoala101/ponc/actions/workflows/build-windows.yml/badge.svg)](https://github.com/qoala101/ponc/actions/workflows/build-windows.yml)
[![Static Analysis](https://github.com/qoala101/ponc/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/qoala101/ponc/actions/workflows/static-analysis.yml)
[![CodeFactor](https://www.codefactor.io/repository/github/qoala101/ponc/badge)](https://www.codefactor.io/repository/github/qoala101/ponc)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![GitHub Releases](https://img.shields.io/github/release/qoala101/ponc.svg)](https://github.com/qoala101/ponc/releases)

## Description

PONC is the graphical tool used by telecommunication engineers to design, manage, and optimize [Passive Optical Network](https://en.wikipedia.org/wiki/Passive_optical_network) layouts.

PONC performs complex calculations on the fly and serves as a fast, user-friendly replacement for existing spreadsheet-based approaches, which require hours of manual work.

![Header](https://github.com/qoala101/ponc/assets/97284265/bc15e1ca-c095-4154-b909-5a1530119822)

### Subject

In PON, the optical signal is delivered to the city region over a single Input cable. In order for Clients to get the internet at their homes, the signal must be split among them using a combination of Splitter devices. They have multiple outputs, allowing the connection of additional cables while reducing the signal strength.

There are many kinds of Splitters, each with different split and signal reduction factors. The number of Clients to be connected to a single Input could also grow to hundreds in densely populated cities. This makes the number of possible network layouts enormous.

### Workflow

A typical PONC workflow consists of an engineer designing or refining existing PON layouts, with the goal of finding the cheapest Splitter combination to match the number of Clients.

To achieve this, PONC provides instant reaction to changes, analytical views, and an automatic layout Calculator that could find the perfect combination in seconds.

The resulting network layouts are then implemented by the field engineers using real devices.

## Features

- Build and manage PON layouts

  ![Feature 1](https://github.com/qoala101/ponc/assets/97284265/d5f44488-eb61-45e7-ad88-3d79ab3d5bbb)

- Automatic flow updates and visualization

  ![Feature 2](https://github.com/qoala101/ponc/assets/97284265/ad61419e-7fcd-49e8-8c96-f0acbcb26ad7)

- Analytical views and navigation

  ![Feature 3](https://github.com/qoala101/ponc/assets/97284265/e87ac7f2-0776-40f4-a26d-3030cca2c138)

- Find the most optimal layouts given the constraints

  ![Feature 4](https://github.com/qoala101/ponc/assets/97284265/ad26d16a-fd83-4af5-8c0b-4fdca24d3635)

- Convenience functionality

  ![Feature 5](https://github.com/qoala101/ponc/assets/97284265/d627a22c-de49-4e0a-88de-a5b6cf1e169c)

## Requirements

### Linux

- OpenGL

### Windows

- Windows 7
- DirectX 11

No installation is required. Unpack the release archive and run the executable.

## Build

Build with CMake on any supported platform.

```sh
cmake -B ./build
cmake --build ./build --config Release
```

See **.github/workflows** for build samples.

## Third-party components

### C++

- [Dear ImGui](https://github.com/ocornut/imgui) - UI framework
- [Node Editor in ImGui](https://github.com/thedmd/imgui-node-editor) - node editor framework
- [imgui-filebrowser](https://github.com/AirGuanZ/imgui-filebrowser) - file browser

### Style

- [Google Fonts](https://fonts.google.com) - fonts
- [tailwindcss](https://tailwindcss.com/docs/customizing-colors) - color palette

## License

[MIT](https://opensource.org/license/mit)
