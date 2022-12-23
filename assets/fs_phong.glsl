
#version 430

out vec4 FragColor;

in VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    vec3 emission;
};

uniform Light light;
uniform Material material;

uniform sampler2D texture_diffuse;
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(texture_diffuse, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.5 * color * material.ambient;

    // diffuse
    vec3 lightDir = normalize(light.position - fs_in.WorldPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0001);
    vec3 diffuse = light.color * diff * material.diffuse * color;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.WorldPos);
    float spec = 0.0;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0001), material.shininess);
    vec3 specular = spec * material.specular;

    vec3 result = material.emission + ambient + (diffuse + specular) * light.intensity;
    FragColor = vec4(result, 1.0);
} 
