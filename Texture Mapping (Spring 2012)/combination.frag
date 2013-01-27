/* template source code referneced from: 
http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php*/

varying vec3 N;
varying vec3 V;
#define MAX_LIGHTS 3 

//Need to be keept the same!!!!
uniform sampler2D environment;
uniform sampler2D texture;
uniform sampler2D combination;

void main (void)
{  
   vec4 textureColor = texture2D(texture, gl_TexCoord[0].xy);
   vec4 combineColor = texture2D(combination, gl_TexCoord[0].xy);
   vec4 environmentColor = texture2D(environment, gl_TexCoord[1].xy);
   
   // light, eye, lookat
   vec3 L = normalize(gl_LightSource[0].position.xyz - V);   
   vec3 E = normalize(-V);  // we are in Eye Coordinates, so EyePos is (0,0,0)
   vec3 R = normalize(-reflect(L,N));  

   float average = (combineColor.r + combineColor.g + combineColor.b) / 3.0;
   vec4 finalColor = mix(textureColor, environmentColor, average);
 
   //calculate Ambient Term:  
   vec4 Iamb = gl_FrontLightProduct[0].ambient;

   //calculate Diffuse Term:  
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);
   Idiff = clamp(Idiff, 0.0, 1.0);     
   
   // calculate Specular Term:
   vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E),0.0), gl_FrontMaterial.shininess);
   Ispec = clamp(Ispec, 0.0, 1.0); 
   
   //==================:)))) ======================
   vec4 color = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;  
   average = (color.r + color.g + color.b) / 3.0;
   vec4 black = vec4(0, 0, 0, 1.0);
   gl_FragColor = mix(black, finalColor, average);

}
