#version 130

uniform sampler2D mapColor;
uniform sampler2D mapLight;
uniform sampler2D lightTex;

uniform float rotX;
uniform float rotY;
uniform float distToPlanet;
uniform float time;

float pi = 3.1415926;
float ipi = 1.0/pi;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

float getSpecularColor(vec3 p, vec3 normal, vec3 light_pos)
{
	vec3 v_p_light = normalize(light_pos-p);
	float c = dot(v_p_light, normalize(normal));
	if (c>0.0) c = pow(c, 20); else c = 0;
	return c;
}

void main()
{
	vec3 eyePosition = vec3(0, 0, -1);
	vec2 screenPos = gl_TexCoord[0].xy-vec2(0.5, 0.5);
	vec3 viewVec = vec3(screenPos.x, screenPos.y, 0) - eyePosition;
	viewVec = normalize(viewVec);
		
	float planetRadius = 1.00000;
	vec3 planetPosition = vec3(0, 0, distToPlanet);
	vec3 viewPlanet = planetPosition-eyePosition;
	
	float v = dot(viewPlanet, viewVec);
	float disc = planetRadius*planetRadius - (dot(viewPlanet, viewPlanet) - v*v);
	
	vec3 lightPos = planetPosition+vec3(10*cos(time), -0.25, 10*sin(time));

	
	if (disc > 0)
	{
		float d = sqrt(disc);
		vec3 intersectionPoint = eyePosition + (v-d)*viewVec;
		
		vec2 mapCoord;
		vec3 vn = vec3(0, -cos(rotY), sin(rotY));
		
		vec4 veRotate = vec4(1, 0, 0, 0)*rotationMatrix(vn, rotX);
		vec3 ve = veRotate.xyz;
		
		vec3 vp = intersectionPoint-planetPosition;
		
		float phi = acos(-dot(vn, vp));
		
		mapCoord.y = phi * ipi;
		
		float theta = acos( dot(vp, ve) / sin( phi )) / (2.0*pi);
		if ( dot( cross(vn, ve), vp) > 0)
			mapCoord.x = theta;
		else
			mapCoord.x = 1.0 - theta;
		
		if (abs(mapCoord.x-0.5) < 0.0025) mapCoord.x = 0;
		
		vec4 color = texture2D(mapColor, mapCoord);
		
		//Lighting
		
		vec3 normalVec = normalize(intersectionPoint - planetPosition);
		vec3 lightVec = normalize(lightPos - intersectionPoint);
		float intensity = dot(normalVec, lightVec);
		
		//specular
		
		gl_FragColor = color*max(intensity, 0)+getSpecularColor(intersectionPoint, vp, lightPos)*texture2D(mapLight, mapCoord);
	}
	else if (dot(viewVec, lightPos-eyePosition)>0)
	{
		vec2 lightTexCoord = lightPos.xy-screenPos;
		lightTexCoord.x *= 0.5;
		lightTexCoord.y *= 0.5*620.0/348.0;
		
		lightTexCoord += vec2(0.5, 0.5);
		gl_FragColor = 1.1*texture2D(lightTex, lightTexCoord);
	}
	else
	{
		gl_FragColor = vec4(0, 0, 0, 1);
	}
	
}