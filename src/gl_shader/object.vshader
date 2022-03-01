#version 130
attribute vec4 aPosition;
uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

uniform vec4 uColor;
varying vec4 vColor;

void main()
{
	vColor = uColor;
	gl_Position = uProjection * uView * uModel * aPosition;
}