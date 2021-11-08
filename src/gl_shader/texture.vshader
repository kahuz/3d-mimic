#version 130
attribute vec4 aPosition;
attribute vec2 aTexCoord;
varying vec4 vColor;
varying vec2 vTexCoord;
uniform mat4 uProjection;
uniform vec4 uGlobalColor;

void main()
{
    vTexCoord = aTexCoord;
    vColor = uGlobalColor;
    gl_Position = uProjection * aPosition;
}
