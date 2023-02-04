(2 pts) What are the three different configuration cases when determining the intersection of a pixel row with a triangle edge? In all three cases, what simple criterion can one use to determine whether the triangle edge overlaps the pixel row at all?

Horizontal (slope = 0), vertical (slope = inf), and any other slope. We can check the endpoints of the edge to determine if they intersect the pixel.

(3 pts) How might one use barycentric interpolation to determine whether or not a given point in space lies within the bounds of a triangle? In rasterization, would this method be more efficient than row bound checking for determining which pixels lie within a triangle? Why or why not?

You can use the three triangles defined by the point and a combination of 2 other triangle points to calculate the three subareas S1, S2, and S3. We can also calculate the area of the whole triangle using the same method. From there, we can check if the two sides of the barycentric interpolation formula match to determine if the point lies within the triangle. Row bound is more efficient because barycentric has to check every point in the triangle, but row bounding only checks using the y values and the endpoints.

(5 pts) Describe in detail the sequence of transformations that must occur in order to project a set of triangles given in 3D world space into the coordinate system of the pixelized screen.


First we apply the view matrix onto the world space coordinates to transform them to the camera space. Then, we apply the projection matrix and add the 4th term to transform them to the homogeneous screen space. Finally, we scale them up based on the dimensions of the screen to calculate the pixel positions. 