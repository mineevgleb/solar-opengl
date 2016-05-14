//shader version
#version 330 core

uniform float ambient;
uniform float diffuse;
uniform float specular;
uniform float gloss;
uniform float alpha;

struct Light {
	vec3 intensity;
	vec3 position;
};

uniform Light light_source;

//texture object
uniform sampler2D texture;

//retrieve this data form vertex shader
in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} VertexIn;

//main function
		
void main()
{
	vec3 normal = normalize(VertexIn.normal);
	vec3 tex_color = texture2D(texture,VertexIn.texcoord).rgb;
	vec3 ambient_color = tex_color * light_source.intensity * ambient;
	vec3 dir_to_light = normalize(light_source.position - VertexIn.position); 
	vec3 refl = normalize(reflect(-dir_to_light, normal));
	vec3 diffuse_color = dot(dir_to_light, normal) * light_source.intensity * diffuse * tex_color;
	diffuse_color = clamp(diffuse_color, 0, 1);
	vec3 dir_to_observer = normalize(-VertexIn.position);
	vec3 specular_color = pow(max(dot(refl, dir_to_observer), 0), gloss) * specular * vec3(1,1,1);
	specular_color = clamp(specular_color, 0, 1);
	gl_FragColor = vec4(ambient_color + diffuse_color + specular_color, alpha);
	//gl_FragColor = vec4(normal.xyz, 1);
}
