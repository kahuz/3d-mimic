#version 130
attribute vec3 aPosition;
attribute vec3 aNormal;			// object vertex normal coordinate x,y,z
attribute vec2 aTexCoord;		// object vertex texture coordinate x,y

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

uniform mat4 uModelTransform;
uniform mat4 uInverseModelTransform;	// # version 330 transpose(inverse(uModelTransform)) 

varying vec3 vObjPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;

void main()
{
	vNormal = (uInverseModelTransform * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;
	vObjPosition = (uModelTransform * vec4(aPosition, 1.0)).xyz;

	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0f);
}