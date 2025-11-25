in vec3 fragPosWS;
in vec3 normalWS;
in vec2 fragUV; // trebuie s? se potriveasc? cu vertex shader
out vec4 outColor;

uniform vec3 lightDir;      
uniform vec3 lightColor;    // culoarea luminii
uniform vec3 baseColor;     // culoarea materialului
uniform vec3 cameraPos;     // pozitia camerei (world space)    

uniform sampler2D albedoGrass;
uniform sampler2D albedoMud;
uniform sampler2D albedoRock;
uniform sampler2D albedoPureGrass;

uniform sampler2D roughnessGrass;
uniform sampler2D roughnessMud;
uniform sampler2D roughnessRock;
uniform sampler2D roughnessPureGrass;

float ambientStrength = 0.8;

void main()
{
    vec3 albedoColor;
    float roughness;
    if(fragPosWS.y < 2.0){
        albedoColor = texture(albedoGrass, fragUV).rgb;
        roughness = texture(roughnessGrass, fragUV).r;
    } else if(fragPosWS.y < 5.0){
        albedoColor = texture(albedoMud, fragUV).rgb;
        roughness = texture(roughnessMud, fragUV).r;
    } else if(fragPosWS.y<12.0){
        albedoColor = texture(albedoPureGrass, fragUV).rgb;
        roughness = texture(roughnessPureGrass, fragUV).r;
    }else{
         albedoColor = texture(albedoPureGrass, fragUV).rgb;
        roughness = texture(roughnessPureGrass, fragUV).r;
        //albedoColor = texture(albedoRock, fragUV).rgb;
        //roughness = texture(roughnessRock, fragUV).r;
    }
    roughness = clamp(roughness, 0.0, 1.0);
    vec3 norm = normalize(normalWS);
    vec3 L = normalize(-lightDir);

    vec3 ambient = ambientStrength * lightColor;
    float diff = max(dot(norm, L), 0.0);
    vec3 diffuse = diff * lightColor;

    float shininess = mix(128.0, 4.0, roughness);   // neted ? aspru
    float specularStrength = mix(1.0, 0.1, roughness);

    vec3 viewDir = normalize(cameraPos - fragPosWS);
    vec3 reflectDir = reflect(-L, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse) * albedoColor + specular;
    outColor = vec4(result, 1.0);
    //outColor = vec4(fragPosWS.y / 20.0, 0.0, 0.0, 1.0);
}
