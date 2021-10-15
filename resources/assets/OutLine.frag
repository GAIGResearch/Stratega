#version 120

uniform sampler2D texture;
uniform float outlineThickness;
uniform vec2 textureSize;

void main()
{
    // This pixel
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    // Compute alpha-sum
    vec2 offx = vec2(outlineThickness / textureSize.x, 0.0);
    vec2 offy = vec2(0.0, outlineThickness / textureSize.y);
    float surroundingAlphaSum = texture2D(texture, gl_TexCoord[0].xy + offx).a +
        texture2D(texture, gl_TexCoord[0].xy - offx).a +
        texture2D(texture, gl_TexCoord[0].xy + offy).a +
        texture2D(texture, gl_TexCoord[0].xy - offy).a;

    // If one of the surrounding pixels is transparrent --> an edge is detected
    if (pixel.a <= 0.5 && surroundingAlphaSum > 0.1)
        gl_FragColor = gl_Color;
    else
        gl_FragColor = pixel;
}