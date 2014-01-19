uniform float t, k, w, A;
varying vec4 p;
uniform sampler2D sky, leaf;
uniform bool perFrag;

void main()
{
    //u += sin(t);
    //v += t;
    // gl_FragColor = vec4(sin(40.*u),sin(40.*v),0.0,1.0);

    vec4 skyColor;

    if ( perFrag )
    {
        vec3 u = normalize( vec3(gl_ModelViewMatrix * p) );
        float d = length(p.xy);
        vec2 nxy = - p.xy * k * A / d * sin( w * t + k * d );
        vec3 n = normalize( gl_NormalMatrix * vec3(nxy, 1.0) );

        vec3 r = reflect( u, n );

        float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );

        vec2 skyTexCoord = r.xy / m + 0.5;

        skyColor = texture2D( sky, skyTexCoord );
    }
    else
    {
        skyColor = texture2D( sky, gl_TexCoord[0].st );
    }

    float k = 0.2;

    float s2 = k * p.x - floor( k * p.x );
    float t2 = k * p.y - floor( k * p.y );
    vec2 st2 = vec2( s2, t2 );
    vec4 leafColor = texture2D( leaf, st2 );

    gl_FragColor = leafColor * leafColor.a + skyColor * ( 1. - leafColor.a );
}
