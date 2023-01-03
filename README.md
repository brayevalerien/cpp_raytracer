# C++ raytracer
First iteration of a simplistic ray tracer render engine written in C++ and running on the CPU, following the book Computer graphics from scratch, available at: https://www.gabrielgambetta.com/computer-graphics-from-scratch/ .

# How to run the renderer
If you already have the necessary .dll installed, it should be possible to simply run [main.exe](./main.exe). <br>
Otherwise, you can compile the code ([main.cpp](./main.cpp)) using any C++ compiler and linking the `gdi32.lib` library. <br>
When executed, the rendered should produce an image representing the default test scene, composed of 3 spheres (one red, one green and one blue) on a grey ground.

# Disclaimer
This project is far from being finished and many features need to be added, such as:
- Properly working shadows for all types of lights (point and directional)
- Specular highlighting
- Reflections
- Transparency and refraction
- Other primitive objects (box, triangle...)
- ... etc.
