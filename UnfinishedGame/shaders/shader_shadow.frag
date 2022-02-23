#version 130

#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D currentTexture;
uniform float time;

void main() {
	vec2 rxy = gl_TexCoord[0].xy; // relative coordinates (between 0 and 1)
	vec4 pixel_color = texture2D(currentTexture, rxy);
	gl_FragColor = gl_Color; //vec4(1.0, 0.0, 0.0, gl_Color.a);
}