<p align="center">
  <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/teapong_logo.png"/>
</p>

# Teapong

A two-player game for fans of Pong and the [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot)!

<p align="center">
  <a href="youtube">
    <img src="" href="youtube" width="450">
  </a>
  <br></br>
  <em>Click on this image to see a one minute demo of Teapong.</em>
</p>

## Rules and controls

The first player to score three points wins.

The controls are as follows:

- Press <kbd>Esc</kbd> to close the game.
- Press <kbd>F</kbd> to toggle between the full screen and the windowed modes.
- When in the menu, press <kbd>Space</kbd> to start a game.
- When ready to play, press <kbd>Space</kbd> to launch a teapot.
- The left paddle is controled with <kbd>G</kbd> and <kbd>B</kbd>, while the right paddle is controlled with <kbd>Up</kbd> and <kbd>Down</kbd>.
- Press <kbd>P</kbd> to pause the game.
- Press <kbd>C</kbd> to toggle between the fixed and free camera modes. When the camera is free, you can position it using <kbd>W</kbd><kbd>A</kbd><kbd>S</kbd><kbd>D</kbd> and the mouse.
- Press <kbd>R</kbd> to reset the camera to its original position.

## Technical details

This is the first 3D game I ever make. It was written using C++ and OpenGL, with a strong focus on making the code clear, organized and cross-platform. It is currently supported on macOS and Windows.

The external libraries it uses and their purposes are the following:

- [GLFW](https://www.glfw.org/) is used to interact with the windowing system and to receive inputs.
- [GLAD](https://glad.dav1d.de/) is used to load pointers to OpenGL functions.
- [GLM](https://glm.g-truc.net/0.9.9/index.html) is used to perform 3D math.
- [Assimp](http://www.assimp.org/) is used to load 3D models.
- [stb_image](https://github.com/nothings/stb) is used to load textures.
- [irrKlang](https://www.ambiera.com/irrklang/) is used to play sounds.

The sources of the assets it uses are the following:

- The 3D models were created using [3ds Max](https://area.autodesk.com/3ds-max-indie/).
- The textures can be found in [3dtextures.me](https://3dtextures.me/).
- The sound effects can be found in [freesound.org](https://freesound.org/).
- The background music is Filaments by Podington Bear, and it can be found in [freemusicarchive.org](https://freemusicarchive.org/).

For a more detailed description of this project's code, see [this](https://github.com/diegomacario/Teapong/blob/master/documentation/making_of_teapong.pdf) presentation.

## Installation

### macOS

To run Teapong on macOS, you must follow these steps:
- Download this repository.
- Install GLFW, GLM and Assimp by executing the following command:
 ```sh
 $ brew install cmake assimp glm glfw
 ```
- Install GLAD by doing the following:
  - Copy the `/dependencies/mac/inc/glad` directory to `/usr/local/include`.
  - Copy the `/dependencies/mac/inc/KHR` directory to `/usr/local/include`.
- Install irrKlang by doing the following:
  - Copy the `/dependencies/mac/inc/irrKlang` directory to `/usr/local/include`.
  - Copy the dylibs stored in the `/dependencies/mac/lib/irrKlang` directory to `/usr/local/lib`.
- Download release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases), place the Teapong executable inside the repository and launch the game.
 
### Windows

To run Teapong on Windows, simply download release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases) and launch the game.

## Building
 
### macOS

To build Teapong on macOS, you must follow the same steps listed in the **Installation** section, except for the last one, which you must replace with the following:
- Open a Terminal window, navigate to this repository, and execute the following command:
 ```sh
 $ make Teapong
 ```
Thanks to [Daniel Macario](https://github.com/macadev) for writing the Makefile!

### Windows

To build Teapong on Windows, you can download this repository and use the Visual Studio solution file stored in `VS2019_solution`.

## Learning resources

Listing all the pages I studied to make this game would be impossible, so I will limit myself to the three most influential ones:

- The external libraries I used are presented with excellent clarity in [this](https://learnopengl.com/) page.
- The design pattern I used for game state management was inspired by [this](http://www.ai-junkie.com/architecture/state_driven/tut_state1.html) article.
- The classes I wrote for resource management were inspired by [this](https://github.com/skypjack/entt/tree/master/src/entt/resource) code.

Thank you to the authors of those great sources of information!
