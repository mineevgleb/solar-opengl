//shader version
#version 330 core

uniform mat4 modelViewMatrix;

//inverse and transpose matrix for normals
uniform mat4 normalMatrix;

//projectionMatrix*modelViewMatrix
uniform mat4 modelViewProjectionMatrix;

//input vertex: position, normal, texture coordinates
in vec3 pos;
in vec3 nor;
in vec2 tex;

//output vertex to fragment shader
out VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} VertexOut;

void main()
{
	vec4 vertPos4 = modelViewMatrix * vec4(pos.xyz, 1);
	VertexOut.position = vertPos4.xyz / vertPos4.w;
	VertexOut.normal = vec3(normalMatrix * vec4(nor.xyz, 0));	
	VertexOut.texcoord = vec2(tex.xy);
	gl_Position = modelViewProjectionMatrix*vec4(pos.xyz,1);
}
