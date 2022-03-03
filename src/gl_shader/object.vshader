#version 130
attribute vec4 aPosition;
attribute vec4 aNormal; // added normal vector for light

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

uniform vec4 uLightColor;
uniform vec4 uObjectColor;
varying vec4 vLightColor;
varying vec4 vObjectColor;

uniform float uAmbientStrength;
varying float vAmbientStrength;

void main()
{
	vLightColor = uObjectColor;
	vObjectColor = uLightColor;

	vAmbientStrength = uAmbientStrength;

	gl_Position = uProjection * uView * uModel * aPosition;
}