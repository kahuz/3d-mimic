#version 130
attribute vec4 vPosition;
uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

void main()
{
   gl_Position = uProjection * uView * uModel  * vPosition;
}