// Draw fragment shader
#version 130
uniform sampler2D uTexture0;
varying lowp vec4 vColor;
varying lowp vec2 vTexCoord;

void main()
{
	gl_FragColor = texture2D(uTexture0, vTexCoord) * vColor;
}