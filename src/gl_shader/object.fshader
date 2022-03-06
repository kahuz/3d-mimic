#version 130
precision mediump float;

uniform vec3 uLightPos;

varying vec3 vObjFragPos; // object fragment position
varying vec3 vNormal;
varying vec4 vLightColor;
varying vec4 vObjectColor;
varying float vAmbientStrength;

void main()
{
  // ambient
  vec3 ambient = vAmbientStrength * vLightColor.xyz;

  // diffuse
  vec3 norm = normalize(vNormal);
  vec3 lightDir = normalize(uLightPos - vObjFragPos);

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * vLightColor.xyz;

  vec3 result = (ambient + diffuse) * vObjectColor.xyz;
  gl_FragColor =  vec4(result, 1.0);
}