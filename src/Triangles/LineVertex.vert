//shader version
#version 150 core


//projectionMatrix*modelViewMatrix
uniform mat4 modelViewProjectionMatrix;

//input vertex: position, normal, texture coordinates
in vec3 pos;

//output vertex to fragment shader


void main()
{
	gl_Position = modelViewProjectionMatrix*vec4(pos.xyz,1);
}
