varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void)
{
    vec3 yuv;
    vec3 rgb;    
	
#if 0	
    yuv.x = texture2D(tex_y, textureOut).r;
    yuv.y = texture2D(tex_u, textureOut).r - 0.5;
    yuv.z = texture2D(tex_v, textureOut).r - 0.5;
	rgb = mat3( 1,       1,         1,
                0,       -0.39465,  2.03211,
                1.13983, -0.58060,  0) * yuv;    
#else
    yuv.x = 1.1643*(texture2D(tex_y, textureOut).r - 0.0625);
    yuv.y = texture2D(tex_u, textureOut).r - 0.5;
    yuv.z = texture2D(tex_v, textureOut).r - 0.5;
	rgb = mat3( 1,       1,         1,
                0,       -0.39173,  2.017,
                1.5958,  -0.81290,  0) * yuv;     
#endif
    gl_FragColor = vec4(rgb, 1);
}
