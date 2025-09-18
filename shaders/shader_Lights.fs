#version 330 core
out vec4 FragColor;

#define NUMBER 2

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

//uniform sampler2D texture_diffuse1;
//struct Material
//{
//    sampler2D diffuse;
//    sampler2D specular;
//    float shininess;
//};

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight[NUMBER];
uniform SpotLight spotLight;
//uniform Material material;

uniform sampler2D material_diffuse;
uniform sampler2D material_specular;
uniform float material_shininess;
uniform vec3 material_color_diffuse; // Nuevo: color difuso sólido
uniform bool hasDiffuseTexture;      // Nuevo: bandera para indicar si hay textura difusa

// Function prototypes
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir );
vec4 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir );

void main()
{    
    //Properties needed to lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);

    //Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    //Point Light
    for(int i = 0; i < NUMBER; i++)
    {
        result  += CalcPointLight(pointLight[i], norm, FragPos, viewDir);
    }

    // Spot light
    //result += CalcSpotLight( spotLight, norm, FragPos, viewDir );
    
    vec4   texColor = vec4( result, (hasDiffuseTexture ? texture( material_diffuse, TexCoords).a : 1.0) );
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}

// Calculates the color when using a directional light.
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir )
{
    vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material_shininess );
    
    // Combine results
    vec3 ambient = light.ambient * (hasDiffuseTexture ? vec3( texture( material_diffuse, TexCoords ).rgb ) : material_color_diffuse);
    vec3 diffuse = light.diffuse * diff * (hasDiffuseTexture ? vec3( texture( material_diffuse, TexCoords ).rgb ) : material_color_diffuse);
    vec3 specular = light.specular * spec * (hasDiffuseTexture ? vec3( texture( material_specular, TexCoords ).rgb ) : material_color_diffuse);
   
   vec3 result = ambient + diffuse + specular;

    return (result);
}

// Calculates the color when using a point light.
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material_shininess );
    
    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );
    
    // Combine results
    vec3 ambient = light.ambient * (hasDiffuseTexture ? vec3( texture( material_diffuse, TexCoords ).rgb ) : material_color_diffuse);
    vec3 diffuse = light.diffuse * diff * (hasDiffuseTexture ? vec3( texture( material_diffuse, TexCoords ).rgb ) : material_color_diffuse);
    vec3 specular = light.specular * spec * (hasDiffuseTexture ? vec3( texture( material_specular, TexCoords ).rgb ) : material_color_diffuse);
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result=ambient + diffuse + specular;

    return (result);
    
}

// Calculates the color when using a spot light.
vec4 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
     vec3 lightDir = normalize(light.position - FragPos);
    
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light.ambient * (hasDiffuseTexture ? texture(material_diffuse, TexCoords).rgb : material_color_diffuse);
        
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * (hasDiffuseTexture ? texture(material_diffuse, TexCoords).rgb : material_color_diffuse);  
        
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
        vec3 specular = light.specular * spec * (hasDiffuseTexture ? texture(material_specular, TexCoords).rgb : material_color_diffuse);  
        
        // attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   
            
        //vec3 result = ambient + diffuse + specular;
        //FragColor = vec4(result, 1.0);
        vec4 result = vec4(ambient + diffuse + specular, (hasDiffuseTexture ? texture(material_diffuse, TexCoords).a : 1.0)) ;
        if(result.a < 0.1)
            discard;
        FragColor = result;
         return (result);
    }
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        //FragColor = vec4(light.ambient * texture(material_diffuse, TexCoords).rgb, 1.0);
        vec4 result = vec4(light.ambient * (hasDiffuseTexture ? texture(material_diffuse, TexCoords).rgb : material_color_diffuse), (hasDiffuseTexture ? texture(material_diffuse, TexCoords).a : 1.0));
        if(result.a < 0.1)
            discard;
        FragColor = result;
         return (result);
    }

   
}