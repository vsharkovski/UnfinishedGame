#version 120
uniform sampler2D currentTexture;
uniform float time;

void main() {
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	/*
	// for pixel coordinates (0-1280), use gl_FragCoord.xy ... Then to get relative divide by resolution.xy
	vec2 coord = gl_TexCoord[0].xy; // relative coordinates (between 0 and 1) to texture
	vec4 pixel_color = texture2D(currentTexture, coord); // get the pixel at those coordinates
	gl_FragColor = vec4(1.0 - pixel_color.r, 1.0 - pixel_color.g, 1.0 - pixel_color.b, pixel_color.a);
	*/
}