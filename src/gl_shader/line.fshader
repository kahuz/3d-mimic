#version 130

varying vec4 vLightColor;

void main()
{
	gl_FragColor = vLightColor;
}