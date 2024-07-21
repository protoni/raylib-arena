#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;

// Light structures
#define MAX_LIGHTS 4
struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 lightDot = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 light = normalize(lights[i].position - lights[i].target);
            float NdotL = max(dot(normal, light), 0.0);
            lightDot += lights[i].color.rgb * NdotL;

            float specCo = 0.0;
            if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-light, normal))), 16.0);
            specular += specCo;
        }
    }

    finalColor = texelColor * ((colDiffuse + vec4(specular, 1.0)) * vec4(lightDot, 1.0));
    finalColor += texelColor * (ambient * colDiffuse);
    
    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}