# CPUBrotPlot
A horrifyingly inefficient CPU-based Mandlebrot set plotter written in C++.

## Usage

### Controls

 * Z - Zoom in
 * X - Zoom out
 * Arrow keys - Move Camera
 * F2 - Take Screenshot (Placed in same directory as executable or CWD)

## About
Wrote this code some time ago, it is a Mandlebrot set plotter that runs on the CPU. It determines the number of threads availiable on your PC, and then proceedes to hog them all in order to draw a beautiful 640x480 rendering of the the mandlebrot set.

**It is highly likely your PC will come to a standstill while running this program, so use with caution!**

## Screenshots

![frame_468_screenshot](https://user-images.githubusercontent.com/45643741/156910064-175a17d2-5a7b-4a43-bd78-3aff0b9fdadc.png)

![frame_2162_screenshot](https://user-images.githubusercontent.com/45643741/156910143-f332c8a8-2464-4adb-8c8f-ddf62f01f807.png)

*Copyright (C) 2022 Jacob Allen. Released under the MIT License.*
