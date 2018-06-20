#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
}; 

struct Light {
	vec3 position; // doesn't matter with directional lighting
	// vec3 direction; // needed for directional lighting

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;


	// attenuation (light fading over distance) values
	float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
			light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	// vec3 lightDir = normalize(-light.direction); // for directional lighting
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}