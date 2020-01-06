<p align="center">
  <img src=""/>
</p>

# Teapong

A two-player game for fans of the [Utah teapot](https://en.wikipedia.org/wiki/Utah_teapot)!

<p align="center">
  <a href="youtube">
    <img src="" href="youtube" width="450">
  </a>
  <br></br>
  <em>Click on this image to see a one minute demo of Teapong.</em>
</p>

## Technical details

This project was written using C++ and OpenGL, with a strong focus on making the code clear, organized and cross-platform.

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

## Justification

When one encounters such a strange piece of software as Teapong, it is normal to ask: why would anyone make this?

The answer to that question, however, is quite disappointing. I just wanted to work on something that would allow me to:

- Deepen my understanding of real-time rendering and the linear algebra it involves.
- Improve my C++ and OpenGL skills.
- Explore new design patterns and libraries.

Teapong allowed me to do all those things, and it gave me something to laugh at in the end.

## Installation
 
### macOS

To run Teapong on macOS, you must first install all the necessary dependencies.
- To install GLFW, GLM and Assimp, you must execute the following command:
 ```sh
 $ brew install cmake assimp glm glfw
 ```
- To install GLAD, you must do the following:
  - Copy the _/dependencies/mac/inc/glad_ directory to _/usr/local/include_.
  - Copy the _/dependencies/mac/inc/KHR_ directory to _/usr/local/include_.
- To install irrKlang, you must do the following:
  - Copy the _/dependencies/mac/inc/irrKlang_ directory to _/usr/local/include_.
  - Copy the dylibs stored in the _/dependencies/mac/lib/irrKlang_ directory to _/usr/local/lib_.
Once you have completed the steps listed above, you can build the game using the Makefile included in this project.

### Windows

To install the application globally, download the executable named _pfox_windows.exe_ included in release [1.0.0](https://github.com/diegomacario/Poor-Fox/releases), rename it as _pfox.exe_ and place it in a folder that's on the PATH environment variable.

## Usage

__Note__: The application features clear error messages that will guide you when you make mistakes.

__1.__ Create your own expense categories:
 ```sh
 $ pfox new -Groceries -Restaurants -Movies -Shows -Clothes -Books -Photography -Climbing
 ```
 - The application automatically generates codes that you can use to log expenses into each category. 
 - Codes normally consist of the first 3 letters of each category.
 - Don't worry if 2 or more categories start with the same 3 letters (e.g. -Transportation -Travelling), the application will always generate unique codes.  

__2.__ Log the money you spent on a particular day by specifying its date in the format _day/month/year_:
 ```sh
 $ pfox log -d=1/12/2016 -gro=64.32 -boo=36.22 -mov=12.25
 
 $ pfox log -d=16/12/2016 -mov=12.25 -cli=21.34
 ```
 <p align="center">
  <img src="https://github.com/diegomacario/Poor-Fox/blob/master/readme_images/expense_report.png"/>
</p>
 
 - The application automatically displays an expense report for the month you specified.
    - The table at the top displays the expenses that have been logged into each category separated by day.
    - The second table and the bar graph display the total spent in each category.
    - The global value at the bottom is the sum of all the totals.
 - If you do not specify a date, today's date is selected by default.
 - You can log expenses using the same date more than once. The expenses you enter will be added to the existing ones.
 - You can log negative values to make corrections when you enter incorrect amounts.
 - Dates are sorted automatically.
