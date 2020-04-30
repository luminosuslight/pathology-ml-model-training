# Interactive Deep Learning Tool for Cell Segmentation and Analysis
An tool to enable biologists to train machine learning models for their own pathology images.

## Requirements

* Win / macOS / Linux (cross-compilation for Android and iOS should work, too)
* Qt 5.14
* QtCreator or qmake

You also need to create an SSL certificate (core/data/luminosus_websocket.cert and core/data/luminosus_websocket.key). Please use the standard commands you can find on the Internet to create those.

## Build Instructions

* `git submodule update --init --recursive`
* open `src/luminosus-microscopy.pro` in Qt Creator, configure and hit the green play button
