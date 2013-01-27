This program was built on EWS LINUX (Siebel 0220)
The screenshoot folder includes a group of output images.

To compile and run, type in terminal:
~> make
~> ./mp4


================ KEY BOARD INSTRUCTIONS =================

=> Keep pressing 'Z' (or 'z') mutiple times: switch between the orignial, first-level and second-level subdivision of the BLOCK I.  

=> Press 'C' (or 'c') to stop or move camera along the Bezier-curve.

=> Press 'P' (or 'p') to see the outline of the mesh. Press 'P' agin to switch back.

=> Press default keyboard 'UP','DOWN', 'LEFT','RIGHT' to rotate the BLOCK I.

=> Press ESC to close the demo.

==========================================================


I draw the orignial 3D block I and stored all its vertex/face information in 'base.txt'. 
I implemented the catmul-clark subdivision algorthim (two levels) to create smoother and more detailed surface.

The camera moves in the path of Bezier-curve, where the lookat point is located at the centroid of I.

Also there is a texture mapped onto the Block I to create more realistic 3D effect.


Enjoy, thanks.
  
