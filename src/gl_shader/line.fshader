precision lowp float;
uniform lowp vec4 uGlobalColor;
varying lowp vec4 vColor;

void main()
{
	gl_FragColor = vColor * uGlobalColor;
}