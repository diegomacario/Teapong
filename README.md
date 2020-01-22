<p align="center">
  <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/teapong_logo.PNG"/>
</p>

# Teapong

A two-player game for fans of Pong and the [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot)!

<p align="center">
  <a href="https://www.youtube.com/watch?v=qzS9MX50a6k&feature=youtu.be">
    <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/teapot.png" href="https://www.youtube.com/watch?v=qzS9MX50a6k&feature=youtu.be">
  </a>
  <br></br>
  <em>Click on this image to see a two minute demo of Teapong!</em>
</p>

## Technical details

This is the first game that I ever make! :blush:

It was written using C++ and OpenGL, and it is currently supported on macOS and Windows.

The external libraries it uses and their purposes are the following:

- [GLFW](https://www.glfw.org/) is used to interact with the windowing system and to receive inputs.
- [GLAD](https://glad.dav1d.de/) is used to load pointers to OpenGL functions.
- [GLM](https://glm.g-truc.net/0.9.9/index.html) is used to perform 3D math.
- [Assimp](http://www.assimp.org/) is used to load 3D models.
- [stb_image](https://github.com/nothings/stb) is used to load textures.
- [irrKlang](https://www.ambiera.com/irrklang/) is used to play sounds.

The sources of the assets it uses are the following:

- The 3D models were created using [3ds Max](https://area.autodesk.com/3ds-max-indie/).
- The textures were created procedurally using 3ds Max and the [Open Shading Language](https://github.com/imageworks/OpenShadingLanguage).
- The sound effects can be found [here](https://freesound.org/).
- The background music is Filaments by Podington Bear, and it can be found [here](https://freemusicarchive.org/).

For a more detailed description of this project's code, see [this](https://github.com/diegomacario/Teapong/blob/master/documentation/making_of_teapong.pdf) presentation.

## Rules and controls

The rules are simple: the first player to score three points wins!

The controls are as follows:

- Press <kbd>Esc</kbd> to close the game.
- Press <kbd>F</kbd> to toggle between the full screen and the windowed modes. When in the windowed mode, you can manually resize the window using the mouse.
- Press <kbd>1</kbd>, <kbd>2</kbd>, <kbd>4</kbd> or <kbd>8</kbd> to set the number of samples used for anti-aliasing. By default, the game starts with 1 sample. The higher the number of samples, the better the game looks.
- When in the menu, press <kbd>Space</kbd> to start a game.
- When ready to play, press <kbd>Space</kbd> to launch a teapot.
- The left paddle is controled with <kbd>G</kbd> and <kbd>B</kbd>, while the right paddle is controlled with <kbd>Up</kbd> and <kbd>Down</kbd>.
- Press <kbd>P</kbd> to pause the game.
- Press <kbd>C</kbd> to toggle between the fixed and free camera modes. When the camera is free, you can position it using <kbd>W</kbd>, <kbd>A</kbd>, <kbd>S</kbd>, <kbd>D</kbd> and the mouse. You can also zoom in and out using the scroll wheel.
- Press <kbd>R</kbd> to reset the camera to its original position.

## Installation

### macOS

To run Teapong on macOS, you must follow these steps:
- Download or clone this repository and open a Terminal window in its root.
- Execute the following command to install GLFW, GLM and Assimp:
 ```sh
 $ brew install cmake assimp glm glfw
 ```
- Execute the following commands to install GLAD:
 ```sh
 $ cp dependencies/mac/inc/glad/glad.h /usr/local/include/
 $ cp dependencies/mac/inc/KHR/khrplatform.h /usr/local/include/
 ```
- Execute the following commands to install irrKlang:
 ```sh
 $ cp -r dependencies/mac/inc/irrklang /usr/local/include/
 $ cp -R dependencies/mac/lib/ /usr/local/lib/
 ```
- Download release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases), place the Teapong executable at the root of the repository and execute the following command to launch the game:
 ```sh
 $ ./Teapong
 ```
 
### Windows

To run Teapong on Windows, simply download release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases) and launch the game.

## Building
 
### macOS

To build Teapong on macOS, you must follow the same steps listed in the **Installation** section, except for the last one, which you must replace with the following:
- Execute the following command to build the game:
 ```sh
 $ make Teapong
 ```
Thanks to [Daniel Macario](https://github.com/macadev) for writing the Makefile!

### Windows

To build Teapong on Windows, simply download or clone this repository and use the Visual Studio 2019 solution file that is stored in the **VS2019_solution** directory.

## Learning resources

The following sources of information are the ones that helped me the most while working on this game:

- The external libraries I used are presented with excellent clarity in [this](https://learnopengl.com/) page.
- The design pattern I used for game state management was inspired by [this](http://www.ai-junkie.com/architecture/state_driven/tut_state1.html) article.
- The classes I wrote for resource management were inspired by [this](https://github.com/skypjack/entt/tree/master/src/entt/resource) code.
