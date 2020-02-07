# SRTM Viewer
Krzysztof Boroński

Command-line argumets
---------------------
You can start he program with no arguments or provide a path to a folder containing .hgt data files, followed by some of these options:
* `-dl <min> <max>` - use data only from tiles lying between `<min>` and `<max>` longitudes.
* `-sz <min> <max>` - use data only from tiles lying between `<min>` and `<max>` latitudes.

Note that latitude is always a positive number, 0 is the north pole.

Controls
--------
Press `TAB` to switch between orthogonal and perspective modes (the program starts in orthogonal mode).
Press `ENTER` to activate command prompt.
Press `ESC` to exit.

In orthogonal mode:
* Hold `Z` and drag your mouse pointer too zoom-in/zoom-out.
* Hold `SPACEBAR` and drag your mouse pointer to move the view.

In perspective (geosphere) mode:
* Use `W` `A` `S` `D` to move around geosphere.
* Look aroung using your mouse.

Runtime commands
----------------
Press `ENTER` to activate command prompt.
The following commands are supported:
* `load-data [path] <minlon maxlon minlat maxlat>` - lods data from `[path]` folder. Optional arguments sequence `<minlon maxlon minlat maxlat>` determines longitude and latitude ranges for which the tiles should be loaded. Note, that any previously loaded data will be gone after this operation.
* `goto [coordinates]` - navigates to given coordinates in orthogonal view. The coordinate format is the same as in .hgt file naming convention, eg.: `N45E2`. The order in which longitude and mognitude are provided as well as capitalization of letters does not matter.
* `set-light-amount-ortho [value]` - sets the amount af applied lighting in orthogonal view mode.
* `set-light-amount-perspective [value]` - sets the amount af applied lighting in perspective(geosphere) view mode.
* `set-light-direction-ortho [x y z]` - sets light direction vector in orthogoal view mode.
* `set-light-direction-perspective [x y z]` - sets light direction vector in perspective (geosphere) view mode.
* `set-lod-factor` - sets an Level-Of-Detail factor (this applies only to perspective mode). 0 is the lowest, 1 is default.
* `set-sky-mode [mode]` - sets sky rendering mode: `color` - single color (glClearColor), `perlin` - weird sky texture based on 3d perlin noise.

-----

**Note:** In the provided code the meanings of terms "longitude" and "latitude" are swapped. I always confuse them with each other.