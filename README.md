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
  <em>Click on the image above to see a two minute demo of Teapong on YouTube!</em>
</p>

## Technical details

This project was an attempt to write a simple game from scratch as cleanly as possible. Special focus was placed on the following goals:

- Use modern C++ and modern OpenGL.
- Keep the code cross-platform so that the game runs on macOS and Windows.
- Find a flexible way to manage resources (e.g. textures, models, shaders, etc.).
- Find an organized way to represent game states (e.g. menu, play, pause, etc.) and make them fully independent and encapsulated.

The first three goals were achieved successfully, but the last one was not, which led to code that does not scale well in the game state layer. This failure is explained in the "**Game state management**" subsection  below.

The other subsections provide information on the libraries used by this project and the sources of the game assets, as well as some details on resource management, shading, collision detection and how to make a teapot explode.

For additional information on this project's code, see [this](https://github.com/diegomacario/Teapong/blob/master/documentation/making_of_teapong.pdf) presentation.

### Libraries

The libraries used by this project and their purposes are the following:

- [GLFW](https://www.glfw.org/) is used to interact with the windowing system and to receive inputs.
- [GLAD](https://glad.dav1d.de/) is used to load pointers to OpenGL functions.
- [GLM](https://glm.g-truc.net/0.9.9/index.html) is used to perform 3D math.
- [Assimp](http://www.assimp.org/) is used to load 3D models.
- [stb_image](https://github.com/nothings/stb) is used to load textures.
- [irrKlang](https://www.ambiera.com/irrklang/) is used to play sounds.

[This](https://learnopengl.com/) website explains how to use each of the libraries listed above with excellent clarity.

### Game assets

The sources of the assets used by this project are the following:

- The 3D models and textures were created using [3ds Max](https://area.autodesk.com/3ds-max-indie/).
- The sound effects can be found [here](https://freesound.org/).
- The background music is Filaments by Podington Bear, and it can be found [here](https://freemusicarchive.org/).

### Resource management

The resouce manager used by this project was inspired by [this](https://github.com/skypjack/entt/tree/master/src/entt/resource) code from the [EnTT](https://github.com/skypjack/entt) library. It follows these principles:

- The resource management code ([resource_manager.h](https://github.com/diegomacario/Teapong/blob/master/inc/resource_manager.h)) is separated from the resource loading code ([texture_loader.h](https://github.com/diegomacario/Teapong/blob/master/inc/texture_loader.h), [model_loader.h](https://github.com/diegomacario/Teapong/blob/master/inc/model_loader.h) and [shader_loader.h](https://github.com/diegomacario/Teapong/blob/master/inc/shader_loader.h)).
- A resource manager instance can only manage one type of resource (e.g. [texture.h](https://github.com/diegomacario/Teapong/blob/master/inc/texture.h), [model.h](https://github.com/diegomacario/Teapong/blob/master/inc/model.h) or [shader.h](https://github.com/diegomacario/Teapong/blob/master/inc/shader.h)).
- Resources are not deleted automatically if they are not being used. The user must make a request for them to be deleted.

The implementation of the resource manager may seem a bit complex because it makes use of variadic templates and perfect forwarding, but it is thanks to those C++ features that it is super flexible and easy to use:

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/resource_manager_example.png"/>
</p>

### Game state management

The design pattern used by this project for game state management is a modified version of the State design pattern, which is explained brilliantly in [this](http://www.ai-junkie.com/architecture/state_driven/tut_state1.html) article.

The fundamental ideas behind my implementation of that pattern are the following:

- Each state is represented by a class (e.g. [menu_state.h](https://github.com/diegomacario/Teapong/blob/master/inc/menu_state.h), [play_state.h](https://github.com/diegomacario/Teapong/blob/master/inc/play_state.h), [pause_state.h](https://github.com/diegomacario/Teapong/blob/master/inc/pause_state.h) and [win_state.h](https://github.com/diegomacario/Teapong/blob/master/inc/win_state.h)).
- Each state is required (by inheriting from [state.h](https://github.com/diegomacario/Teapong/blob/master/inc/state.h)) to implement the three functions that are always called in the game loop: `processInput`, `update` and `render`.
- Each state only has access to the resources that it needs, and it can share its resources with other states to facilitate communication between states.
- Each state is responsible for checking the conditions that could lead to a state change, and it must notify the finite state machine that contains all the states ([finite_state_machine.h](https://github.com/diegomacario/Teapong/blob/master/inc/finite_state_machine.h)) to make a transition occur. 

The state diagram below illustrates the states that make up this project, and the events that cause the transitions between them to occur:

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/fsm.PNG"/>
</p>

So how did this design pattern lead to code that does not scale well? The root of the problem is the way I made states communicate with each other: by sharing resources.

To illustrate why this was a bad decision, consider the following situation:

- Let's say that state A has the teapot at position X.
- Now let's say that we need to transition to state B, where the teapot will be placed at position Y.
- If the teapot needs to go back to position X if we ever switch back to state A in the future, then state A needs to remember position X.
- But what if both states share the same teapot? As soon as the transition occurs, state B will change the position of the teapot to Y, and X will be lost.
- This means that the position of the teapot behaves as a global variable, so to avoid losing X, state A needs to maintain a variable external to the teapot where it stores that value.

Now imagine the same situation, but with dozens of states and shared resources. The code quickly becomes tangled and difficult to maintain.

To prevent this mess we must only share resources that do not require any external variables to be maintained, and we must use a different system to allow states to communicate with each other.

"What does that other system look like?" you might ask. That is a question that I am still trying to answer. I you have any suggestions, please let me know!

### Shading

Shading is done using the [Phong reflection model](https://en.wikipedia.org/wiki/Phong_reflection_model).

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/menu.gif"/>
</p>

### Collision detection

By pretending that the paddles are two-dimensional rectangles and that the teapot is a two-dimensional circle, collisions are detected using simple equations for Axis-Aligned Bounding Boxes (AABBs) and circles.

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/play.gif"/>
</p>

### How to make a teapot explode

A simple [geometry shader](https://github.com/diegomacario/Teapong/blob/master/resources/shaders/game_object_3D_explosive.gs) is used to make the teapot explode by pushing each polygon along its normal.

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/win.gif"/>
</p>

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

## How to run Teapong

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
- Download **Teapong_macOS.zip** from release [1.0.0](https://github.com/diegomacario/Teapong/releases/tag/v1.0.0), open a Terminal in its root directory and execute the following command to launch the game:
 ```sh
 $ ./Teapong
 ```
 
### Windows

To run Teapong on Windows, simply download **Teapong_Windows.zip** from release [1.0.0](https://github.com/diegomacario/Teapong/releases/tag/v1.0.0) and double click **Teapong.exe**.

## How to build Teapong
 
### macOS

To build Teapong on macOS, you must follow the same steps listed in the "**How to run Teapong**" section, except for the last one, which you must replace with the following:
- Execute the following command to build the game:
 ```sh
 $ make Teapong
 ```
Thanks to [Daniel Macario](https://github.com/macadev) for writing the Makefile!

### Windows

To build Teapong on Windows, simply download or clone this repository and use the Visual Studio 2019 solution file that is stored in the **VS2019_solution** directory.

## Conclusion

I hope that this project helps anyone who decides to embark on a similar adventure.

All my life I have wanted to build a game, and I always knew that when I finally did it, I would do it from scratch, to understand every single little detail about it. It is not easy or practical to build a game this way, but I think it is the most insightful and rewarding way.

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/no_idea.jpg"/>
 <p align="center">
  <em>How I felt when I started working on this project.</em>
 </p>
</p>
