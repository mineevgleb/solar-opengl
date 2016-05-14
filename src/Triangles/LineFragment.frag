//shader version
#version 150 core

uniform vec3 color;

//retrieve this data form vertex shader

//main function

//TODO: you should use VertexIn.normal value to evaluate Phong Lightning for this pixel
// 
		
void main()
{
	gl_FragColor = vec4(color, 1);
}
