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

### Subject

In PON, the optical signal is delivered to the city region over a single Input cable. In order for Clients to get the internet at their homes, the signal must be split among them using a combination of Splitter devices. They have multiple outputs, allowing the connection of additional cables while reducing the signal strength.

There are many kinds of Splitters, each with different split and signal reduction factors. The number of Clients to be connected to a single Input could also grow to hundreds in densely populated cities. This makes the number of possible network layouts enormous.

### Workflow

A typical PONC workflow consists of an engineer designing or refining existing PON layouts, with the goal of finding the cheapest Splitter combination to match the number of Clients.

To achieve this, PONC provides instant reaction to changes, analytical views, and an automatic layout Calculator that could find the perfect combination in seconds.

The resulting network layouts are then implemented by the field engineers using real devices.

## Features

- Build and manage PON layouts
- Automatic flow updates and visualization
- Analytical views and navigation
- Find the most optimal layouts given the constraints
- Convenience functionality

## Installation

### Requirements

#### Linux

- OpenGL

#### Windows

- Windows 7
- DirectX 11

No installation is required. Unpack the release archive and run the executable.

## Build

Build with CMake on any supported platform.

```sh
cmake -B ./build
cmake --build ./build --config Release
```

See .github/workflows for build samples.

## Third-party components

### C++

- [Dear ImGui](https://github.com/ocornut/imgui) - GUI framework
- [Node Editor in ImGui](https://github.com/thedmd/imgui-node-editor) - node editor framework
- [imgui-filebrowser](https://github.com/AirGuanZ/imgui-filebrowser) - file browser

### Style

- [Google Fonts](https://fonts.google.com/) - fonts
- [tailwindcss](https://tailwindcss.com/docs/customizing-colors) - color palette

## License

[MIT](https://opensource.org/license/mit/)
