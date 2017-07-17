#version 400

uniform sampler2D sampler0;

in vec2 vTexCod;
out vec4 outColor;

void main()
{
	outColor = texture2D(sampler0, vTexCod);
}
