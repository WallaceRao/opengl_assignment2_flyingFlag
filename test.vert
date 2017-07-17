#version 400

uniform float phase;
uniform mat4 mvp;

in vec3 VertexPosition;
in vec2 aTexCod;
out vec2 vTexCod;

void main()
{
  float x,y,z;
  x = VertexPosition.x; 
  y = VertexPosition.y; 
  z = VertexPosition.z;
  if(z > -0.5 && x >= 0.0)
	 y = 0.08*sqrt(x)*(sin(phase+24.0*x)-sin(phase));
  vec4 newPos = vec4(x, y, z, 1.0);
  gl_Position = mvp  * newPos;
  vTexCod = aTexCod;
}

