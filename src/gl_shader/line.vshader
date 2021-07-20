attribute lowp vec4 aPosition;
attribute lowp vec4 aColor;
varying lowp vec4 vColor;
uniform lowp mat4 uProjection;

void main()
{
    vColor = aColor;
    gl_Position = uProjection * aPosition;
}