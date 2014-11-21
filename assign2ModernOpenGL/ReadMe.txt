Before you compile, please make sure you have Visual Studio 2012 and open my project in VS 2012

My program uses GLFW, GLEW and GLM libraries to complement OpenGL 3.2+. So you won't see any old, deprecated OpenGL API(glBegin, glEnd) in my code. As a result, I also use shaders for the rendering.

The very cool thing about my program is that I wrote a FBX model/animation exporter so I can export any model/animation from Maya into my scene. For this assignment, I had a dragon, helicopter, tank and wood house. I did not do any animations since skeletal animation is such a pain to write the code for...................

I also have double track for the rollercoaster's rails. 

I have simulated physics using the conservation of kinetic energy law. Basically, I specify an initial height then use the formula mgh0 = mgh + 0.5mv^2 to calculate speed of the rollercoaster.

When computing the track, I use the recursive subdivision algorithm.


To sum up, here are the extra credits I did:

1. Model importing, Dragon, Tank, Helicopter, Wood House
2. Double rail for the track
3. Simple Physics for simulating speed
4. Recursive subdivision algorithm for computing the track


Please make sure you run my project in VS2012; otherwise it will not build..........................







Open-ended questions:

1. The seams in the sky:
I did not solve the problem perfectly. However, I think if the UV texture mode is set to Clap or Mirror, the seam could be less obvious.

2. Determining the camera normal:
I solved my problem by trial and error. 