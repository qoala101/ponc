# Passive Optical Network Calculator (PONC)

[![Build Linux](https://github.com/qoala101/ponc/actions/workflows/build-linux.yml/badge.svg)](https://github.com/qoala101/ponc/actions/workflows/build-linux.yml)
[![Build Windows](https://github.com/qoala101/ponc/actions/workflows/build-windows.yml/badge.svg)](https://github.com/qoala101/ponc/actions/workflows/build-windows.yml)
[![Static Analysis](https://github.com/qoala101/ponc/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/qoala101/ponc/actions/workflows/static-analysis.yml)
[![CodeFactor](https://www.codefactor.io/repository/github/qoala101/ponc/badge)](https://www.codefactor.io/repository/github/qoala101/ponc)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![GitHub Releases](https://img.shields.io/github/release/qoala101/ponc.svg)](https://github.com/qoala101/ponc/releases)

## Description

PONC is the graphical tool used by telecommunication engineers to design, manage, and optimize [Passive Optical Network](https://en.wikipedia.org/wiki/Passive_optical_network) layouts.

PONC performs complex calculations on the fly and serves as fast user-friendly replacement of existing spreadsheet-based workflows which require hours of manual work.

## Features

- Build and manage PON layouts
- Automatic flow updates and visualization
- Analysis views and navigation
- Find the most optimal layouts given the constraints
- In-place editing and convenience functionality

## Installation

### Requirements

#### Linux

- OpenGL

#### Windows

- Windows 7
- DirectX 11

No installation required. Unpack release archive and run executable.

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
