#version 130
precision mediump float;

varying vec3 vObjPosition;	// object vertex coordinate x,y,z
varying vec3 vNormal;		// object vertex normal coordinate x,y,z
varying vec2 vTexCoord;		// object vertex texture coordinate x,y

uniform vec3 uLightPosition;	// light start coordinate x,y,z
uniform vec3 uLightColor;		// light color
uniform vec3 uObjectColor;		// object color
uniform float uAmbientStrength;

void main()
{
	vec3 ambient = uAmbientStrength * uLightColor;
	vec3 lightDirection = normalize(uLightPosition - vObjPosition);
	vec3 pixelNormal = normalize(vNormal);
	vec3 diffuse = max(dot(pixelNormal, lightDirection), 0.0) * uLightColor;
	vec3 result = (ambient + diffuse) * uObjectColor;

	gl_FragColor = vec4(result, 1.0);
}