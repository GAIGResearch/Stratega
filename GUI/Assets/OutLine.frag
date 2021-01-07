#version 120
uniform sampler2D texture;

uniform float textureOffset;
uniform vec4 targetCol;

const float offset = 2.0 / 512.0;//size of the texture
varying vec2 v_texCoords;
uniform sampler2D u_texture;

void main()
{
  vec2 v_texCoords = gl_TexCoord[0].xy;
  //vec4 col = texture2D(texture, v_texCoords);
 
 vec4 col = texture2D(texture, v_texCoords);
	if (col.a > 0.5)
		gl_FragColor = col;
	else {
		float a = texture2D(u_texture, vec2(v_texCoords.x + offset, v_texCoords.y)).a +
			texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y - offset)).a +
			texture2D(u_texture, vec2(v_texCoords.x - offset, v_texCoords.y)).a +
			texture2D(u_texture, vec2(v_texCoords.x, v_texCoords.y + offset)).a;
		if (col.a < 1.0 && a > 0.1)
			gl_FragColor = targetCol;
		else
			gl_FragColor = col;
	}

//  if (col.a == 1)
//    gl_FragColor = col;
//  else if(col.a == 0){
//    float au = texture2D(texture, vec2(v_texCoords.x , v_texCoords.y - textureOffset)).a;
//    float ad = texture2D(texture, vec2(v_texCoords.x , v_texCoords.y + textureOffset)).a;
//    float al = texture2D(texture, vec2(v_texCoords.x - textureOffset, v_texCoords.y)).a;
//    float ar = texture2D(texture, vec2(v_texCoords.x + textureOffset, v_texCoords.y)).a;
//
//    if (au == 1.0 || ad == 1.0 || al == 1.0 || ar == 1.0)
//      gl_FragColor = targetCol;
//    else
//      gl_FragColor = col;
 // }
}