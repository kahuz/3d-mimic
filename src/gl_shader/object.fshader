#version 140
precision mediump float;

uniform vec3 uLightPos;
uniform vec3 uViewPos;

varying vec3 vObjFragPos; // object fragment position
varying vec3 vNormal;
varying vec4 vLightColor;
varying vec4 vObjectColor;
varying float vAmbientStrength;
varying float vSpecularStrength;

void main()
{
  // ambient
  vec3 ambient = vAmbientStrength * vLightColor.xyz;

  // diffuse
  vec3 norm = normalize(vNormal);
  vec3 lightDir = normalize(uLightPos - vObjFragPos);

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * vLightColor.xyz;

  vec3 viewDir = normalize(uViewPos - vObjFragPos);
  vec3 reflectDir = reflect(-lightDir, norm);

  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

  vec3 specular = vSpecularStrength * spec * vLightColor.xyz;

  vec3 result = (ambient + diffuse + vSpecularStrength) * vObjectColor.xyz;
  gl_FragColor =  vec4(result, 1.0);
}