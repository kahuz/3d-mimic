#version 130
attribute vec4 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

uniform vec4 uLightColor;
uniform vec4 uObjectColor;
uniform float uAmbientStrength;

varying vec3 vObjFragPos;
varying vec3 vNormal;
varying vec4 vLightColor;
varying vec4 vObjectColor;
varying float vAmbientStrength;

void main()
{
	vObjFragPos = vec3(uModel * aPosition);

	vLightColor = uLightColor;
	vObjectColor = uObjectColor;
	vNormal = aNormal;

	vAmbientStrength = uAmbientStrength;

	gl_Position = uProjection * uView * uModel * aPosition;
}