#version 130
precision mediump float;
varying vec4 vColor;

void main()
{
  gl_FragColor = vColor;
}