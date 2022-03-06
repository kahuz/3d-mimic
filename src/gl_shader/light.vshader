#version 130
attribute vec4 aPosition;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

uniform vec4 uLightColor;
varying vec4 vLightColor;

void main()
{
	vLightColor = uLightColor;
	gl_Position = uProjection * uView * uModel * aPosition;
}