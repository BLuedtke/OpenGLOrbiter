#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

layout(location=0) out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform sampler2D EmissiveTexture;

const int MAX_LIGHTS=14;

struct Light
{
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

uniform Lights 
{
int LightCount;
Light lights[MAX_LIGHTS];
};


float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

vec3 calcDirLight(Light dirLight, vec3 N, vec3 E, vec4 DiffTex){	
	vec3 L = normalize(-dirLight.Direction);
	vec3 H = normalize(L + E);

	float diff = sat(dot(N,L));
	float spec = pow(sat(dot(N,H)), SpecularExp);

	vec3 diffuse = dirLight.Color * DiffuseColor * diff;
	vec3 specular = dirLight.Color * SpecularColor * spec;
	return ((diffuse + AmbientColor) * DiffTex.rgb + specular);
}

vec3 calcPointLight(Light pointLight, vec3 N, vec3 E, vec4 DiffTex){
	vec3 L = normalize(pointLight.Position - Position);
	vec3 H = normalize(L + E);

	float diff = sat(dot(N,L));
	float spec = pow(sat(dot(N,H)), SpecularExp);	
	float dist = length(pointLight.Position - Position);
	float att = 1.0 / (pointLight.Attenuation.x + pointLight.Attenuation.y * dist + pointLight.Attenuation.z * dist * dist);

	vec3 diffuse = (pointLight.Color * DiffuseColor * diff) * att;
	vec3 specular = (pointLight.Color * SpecularColor * spec) * att;
	vec3 ambient = (AmbientColor) * att;	

	return ((ambient + diffuse) * DiffTex.rgb + specular);
}

vec3 calcSpotLight(Light spotLight, vec3 N, vec3 E, vec4 DiffTex){	
	float innerAngle = spotLight.SpotRadius.x;
	float outerAngle = spotLight.SpotRadius.y;

	vec3 L = normalize(spotLight.Position-Position);
	vec3 H = normalize(L + E);

	
	float dist = length(spotLight.Position - Position);	
	float att = 1.0 / (spotLight.Attenuation.x + spotLight.Attenuation.y * dist + spotLight.Attenuation.z * dist * dist);
	float angleToMiddle = acos(dot(spotLight.Direction,-L));
	float attWA = att * (1 - sat( (angleToMiddle - innerAngle)/( outerAngle - innerAngle) ));

	float diff = sat(dot(N,L));
	float spec = pow(sat(dot(N,H)), SpecularExp);	

	vec3 diffuse = (spotLight.Color * DiffuseColor * diff) * attWA;
	vec3 specular = (spotLight.Color * SpecularColor * spec) * attWA;
	vec3 ambient = (AmbientColor) * attWA;	
	return ((ambient + diffuse) * DiffTex.rgb + specular);
}


void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    vec4 EmissTex = texture(EmissiveTexture,Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 E = normalize(EyePos-Position);

    vec3 combinedColor;
    combinedColor += EmissTex.xyz;
    for(int i = 0; i < LightCount; i++){
    	if(lights[i].Type == 0){
    		combinedColor += calcPointLight(lights[i],N,E,DiffTex);
    	} else if(lights[i].Type == 1){
    		combinedColor += calcDirLight(lights[i],N,E,DiffTex);
    	} else if(lights[i].Type == 2){
    		combinedColor += calcSpotLight(lights[i],N,E,DiffTex);
    	}
    }
    FragColor = vec4(combinedColor, DiffTex.a);
}





