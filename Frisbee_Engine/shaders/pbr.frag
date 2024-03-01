#version 450

layout (location = 0) in vec3 worldPos;
layout (location = 1) in vec3 normalWorldSpace;
//layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 outColor;

// descriptor set
layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec3 lightPos;
    vec3 camPos;
} ubo;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

// lights
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

const float PI = 3.14159265359;

const vec3 albedo = vec3(0.8, 1.0, 0.8);
const float metallic = 0.0;
const float roughness = 0.3;
const float ao = 0.02;

const vec3 WorldPos = vec3(0.0, 0.0, 0.0);

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
   vec3 N = normalize(normalWorldSpace);
   vec3 V = normalize(ubo.camPos - worldPos);
   
   vec3 F0 = vec3(0.04); 
   F0 = mix(F0, albedo, metallic);
           
   // reflectance equation
   vec3 Lo = vec3(0.0);
   
   // calculate per-light radiance
   vec3 L = normalize(ubo.lightPos - worldPos);
   vec3 H = normalize(V + L);
   float dist    = length(ubo.lightPos - worldPos);
   float attenuation = 1.0 / (dist * dist);
   vec3 radiance     = vec3(lightColor * attenuation);        
       
   // cook-torrance brdf
   float NDF = DistributionGGX(N, H, roughness);        
   float G   = GeometrySmith(N, V, L, roughness);      
   vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
       
   vec3 kS = F;
   vec3 kD = vec3(1.0) - kS;
   kD *= 1.0 - metallic;	  
       
   vec3 numerator    = NDF * G * F;
   float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
   vec3 specular     = numerator / denominator;  
           
   // add to outgoing radiance Lo
   float NdotL = max(dot(N, L), 0.0);                
   Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
   
   vec3 ambient = vec3(0.03) * albedo * ao;
   vec3 color = ambient + Lo;
   
   color = color / (color + vec3(1.0));
   color = pow(color, vec3(1.0/2.2));  
   
    outColor = vec4(color, 1.0);
}  