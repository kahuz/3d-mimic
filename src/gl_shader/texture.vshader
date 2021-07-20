#version 100
attribute lowp vec4 aPosition;
attribute lowp vec2 aTexCoord;
varying lowp vec4 vColor;
varying lowp vec2 vTexCoord;
uniform lowp mat4 uProjection;
uniform lowp vec4 uGlobalColor;

void main()
{
    vTexCoord = aTexCoord;
    vColor = uGlobalColor;
    gl_Position = uProjection * aPosition;
}
