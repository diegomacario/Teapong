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
- Make the game cross-platform, so that it runs on macOS and Windows.
- Find a flexible way to manage resources (e.g. textures, models, shaders, etc.).
- Find an organized way to represent game states (e.g. menu, play, pause, etc.) and make them fully independent and encapsulated.

The first three goals were achieved successfully, but the last one wasn't, which led to code that doesn't scale well in the game state layer. This failure is explained in the "**Game state management**" subsection  below.

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

### Game assets

The sources of the assets used by this project are the following:

- The 3D models were created using [3ds Max](https://area.autodesk.com/3ds-max-indie/).
- The textures were created procedurally using 3ds Max and the [Open Shading Language](https://github.com/imageworks/OpenShadingLanguage).
- The sound effects can be found [here](https://freesound.org/).
- The background music is Filaments by Podington Bear, and it can be found [here](https://freemusicarchive.org/).

### Resource management

The resouce manager class used by this project was inspired by [this](https://github.com/skypjack/entt/tree/master/src/entt/resource) code from the [EnTT](https://github.com/skypjack/entt) library. It follows the following principles:

- The resource management code ([resource_manager.h](https://github.com/diegomacario/Teapong/blob/master/inc/resource_manager.h)) is separated from the resource loading code ([texture_loader.h](https://github.com/diegomacario/Teapong/blob/master/inc/texture_loader.h), [model_loader.h](https://github.com/diegomacario/Teapong/blob/master/inc/model_loader.h) and [shader_loader.h](https://github.com/diegomacario/Teapong/blob/master/inc/shader_loader.h)).
- A resource manager instance can only manage one type of resource (e.g. [texture.h](https://github.com/diegomacario/Teapong/blob/master/inc/texture.h), [model.h](https://github.com/diegomacario/Teapong/blob/master/inc/model.h) or [shader.h](https://github.com/diegomacario/Teapong/blob/master/inc/shader.h)).
- Resources are not deleted automatically if they are not being used. The user must make a request for them to be deleted.

The implementation of the resource manager is a bit complex because it makes use of variadic templates and perfect forwarding, but using it is super intutive:

```cpp
// Create a resource manager for models
ResourceManager<Model> modelManager;

// Load the teapot model by giving it a resource ID and passing the path to its .obj file
modelManager.loadResource<ModelLoader>("utah_teapot", "resources/models/teapot/teapot.obj");

// Get the teapot model using its resource ID and render it
std::shared_ptr<Model> teapot = modelManager.getResource("utah_teapot");
teapot->render(phongShader);

// Stop managing the resource
// If nobody is using it, it's deleted. Otherwise, it is deleted when all the shared_ptrs to it go out of scope
modelManager.stopManagingResource("utah_teapot");
```

### Game state management

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

## Running the game without building it

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

## Building the game
 
### macOS

To build Teapong on macOS, you must follow the same steps listed in the "**Running the game without building it**" section, except for the last one, which you must replace with the following:
- Execute the following command to build the game:
 ```sh
 $ make Teapong
 ```
Thanks to [Daniel Macario](https://github.com/macadev) for writing the Makefile!

### Windows

To build Teapong on Windows, simply download or clone this repository and use the Visual Studio 2019 solution file that is stored in the **VS2019_solution** directory.

## Learning resources

<p align="center">
 <img src="https://github.com/diegomacario/Teapong/blob/master/readme_images/no_idea.jpg"/>
 <p align="center">
  <em>How it felt when I started working on this project.</em>
 </p>
</p>

The following sources of information are the ones that helped me the most while working on this game:

- The external libraries I used are presented with excellent clarity in [this](https://learnopengl.com/) page.
- The design pattern I used for game state management was inspired by [this](http://www.ai-junkie.com/architecture/state_driven/tut_state1.html) article.
