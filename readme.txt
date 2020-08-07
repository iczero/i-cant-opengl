OCTREE-BASED N-BODY SIMULATION
==============================
This application implements an octree and the Barnes-Hut approximation to
efficiently simulate gravitational interactions.

To compile:
-----------
Fetch submodules first:
$ git submodule init --update --recursive

Ensure meson, ninja, and cmake are installed, then run:
$ meson build
$ ninja -C build -v
Compiled output will be in build/main.

Usage:
------
$ build/main generate <count>
  to randomly generate [count] objects, or

$ build/main load <csv-file>
  to load a csv file containing objects.

CSV files are in the format of:
x-position,y-position,z-position,x-velocity,y-velocity,z-velocity,mass

Some csv files are provided in the data/ folder.

Click in the application to capture the mouse. Move the mouse around to look
around. Hit Escape to release the mouse. Hit Escape again to exit the
application.
Use W/A/S/D to move around and Space/Left Shift to move up and down respectively.
Hold Left Control to move faster.


This application was written for COP3503 (Data Structures and Algorithms).
