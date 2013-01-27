# CS 418 SP12 liu275

This programe was built and compiled on Linux (in Siebel 0220).
The screenshot folder contains several final images. 

To compile, type in terminal:
>>  make
>>  ./mp3

=================Keyboard Controls:=================

Press 'e' or 'E' : environmental mapping only.
Press 't' or 'T' : texture mapping only.
Press 'c' or 'C' : the combination of both.

Press 'i' or 'I' to zoom in.
Press 'o' or 'O' to zoom out.

Keep pressing 'RIGHT' to rotate right. (about y-axis).
Keep pressing 'LEFT'  to rotate left.  (about y-axis).
Keep pressing 'UP'    to rotate up.    (about x-axis).
Keep pressing 'DOWN'  to rotate down.  (about x-axis).

Press 'Esc' to close the demo.

====================================================

The Object.c and Object.h are used to load the teapot.All the
face normals and texture coordinates are computed and set up 
for rendering.

The Phong lighting from 1 light source was implemented throngh:
texture.frag
environment.frag
combination.frag

The template source code for shaders are referenced from:
http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php


Enjoy, thanks.
