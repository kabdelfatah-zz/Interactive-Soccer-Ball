# Interactive-Socccer-Ball

1-OpenGL codes for interactive Socccer Ball.

2-By hitting the 'h' you can kick the ball to move in a straight line on the grass-land field. (you can notice that the ball motion will start with high speed then this speed will decrease until the ball stops.)

3-By hitting the 'v' key you can kick the ball off the land and then moved along a parabola orbit (I modeled the bouncing process). (when press the 'v' key the camera position and looking will be changed to give a better view.)

4-The ball is always move within the frustum view. However the user can Interactively tuned the Camera Position to get closer looking. (using keyboard keys).

***Note: When the ball reaches the end of the field it will change the direction of its motion (modeling that the ball hits a wall and changed its direction and speed).


Please see the following key function

***********************************
Ball Motion:
***********************************
 1-By pressing 'h' you can kick the ball to move in a straight line on the grass-land field.
 2-By pressing the 'v' you can kick the ball off the land and then moved along a parabola orbit


***********************************
Updating the Camera properties:
***********************************
By pressing one of the following keys you can specify the properties of the Camera:

	1-By pressing 'q' and 'a' you can increase or decrease respectively the camera Position in X direction.

	2-By pressing 'w' and 's' you can increase or decrease respectively the camera Position in Y direction.

	3-By pressing 'e' and 'd' you can increase or decrease respectively the camera Position in Z direction.

	4-By pressing 'r' and 'f' you can increase or decrease respectively the camera Looking in X direction.

	5-By pressing 't' and 'g' you can increase or decrease respectively the camera Looking in Y direction.

	6-By pressing 'y' and 'u' you can increase or decrease respectively the camera Looking in Z direction.


********************************
Reseting Positions:
********************************
By pressing (l) or (L) you can reset the position of the ball and the camera.




*******************************
Simple Scenario:
*******************************
1-First, hit the 'h' key to kick the ball and watch the motion of the ball in a stright line.

2-Then, you can get a closer look to the field and the ball by changing the camera position as following:

	I.   press the 's' key (for many times or long press) which it will decrease the camera position in Y direction.

	II.  press the 'a' then 'd' which they will adjuct the view to right and left.

	III. If there any wrong in the view you can press 'l' to reset the position of the camera and the ball.

3-Then, you can press the 'v' key to watch the bouncing motion.

------------------------------------------
Compilation Environment:
------------------------------------------
This code is compiled in Lab 1D39 on machine 129.252.130.121 ashepoo.cse.sc.edu # Dell Precision T3400

------------------------------------
The content of the working folder:
-----------------------------------
1-main.cpp file

2-Makefile

3-Readme

4-images for textures (black.bmp, white.bmp, and field.bmp)


developed by: ABDELFATAH, KAREEM EZZELDEEN RABEA 

