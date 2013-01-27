/*template source code referneced from:
 http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php*/
 
attribute vec2 tCoordinate;
varying vec3 V; //view
varying vec3 N; //normal

void main(void)
{  
    // Set up the normals for the vertex shader
    // or glfrustrum....
    V = vec3(gl_ModelViewMatrix * gl_Vertex);
    N = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// passing to Object.c
	gl_TexCoord[0].st = tCoordinate;

    /*Sphere map coords
    	r  = reflect (u,n)
    	r  = 2*(n dot u)*n - u
    	m = sqrt( r.x2 + r.y2 + (r.z + 1.0)2 )
    	
		s and t are the final texture coordinates:
		
		s = r.x / m + 0.5
		t = r.y / m + 0.5
    */
	vec3 r = reflect( V, N );
	float parameter = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
	gl_TexCoord[1].st = vec2( r.x/parameter + 0.5, r.y/parameter + 0.5);
}
