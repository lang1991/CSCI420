About this project:
I used modern OpenGL(3.2+) APIs in this assignment; therefore, I included GLEW library in this project.
Instead of GLUT, I used GLFW for my window creating and it also comes with a higher precision timer so that I can create my animation with better precision.
I also used GLM for some math operation since I am using shaders, I use the math library to calculate my Model-View-Projection matrix then pass it to the shader.

Controls:
Rotate: Do not hold any key, use the left or middle mouse button to rotate the mesh
Translate: Hold down Left Control and left or middle mouse button to translate the mesh
Scale: Hold down Left Shift and left or middle mouse button to scale the mesh
Point Mode: Press P to toggle to point rendering mode
Line Mode: Press L to toggle to line rendering mode
Triangle Mode: Press T to toggle to triangle rendering mode

