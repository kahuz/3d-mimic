#version 130
precision mediump float;

varying float vAmbientStrength;
varying vec4 vLightColor;
varying vec4 vObjectColor;

void main()
{
  vec3 ambient = vAmbientStrength * vLightColor.xyz;
  gl_FragColor = vec4(ambient,1.0) * vObjectColor;
}