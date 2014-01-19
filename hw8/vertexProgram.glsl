uniform float t, k, w, A;
const float pi = 3.1415926535;
uniform bool perFrag;

varying float u, v;
varying vec4 p;

void main()
{
    // Convert from the [-5,5]x[-5,5] range provided into radians
    // between 0 and 2*theta
    /*
    u = (gl_Vertex.x + 5.0) / 10.0 * 2. * pi;
    v = (gl_Vertex.y + 5.0) / 10.0 * 2. * pi;
    float r = sin(4.*u+t)/4.+1.;
    float R = cos(6.*cos(u)-t)/4.+3.;

    float a = R+r*cos(v);
    vec3 world = vec3(a*cos(u), a*sin(u), r*sin(v));
    */

    float d = sqrt( gl_Vertex.x * gl_Vertex.x + gl_Vertex.y * gl_Vertex.y );

    p = vec4( gl_Vertex.x, gl_Vertex.y, A * cos( w*t + k*d ), 1.0 );
    
    if ( ! perFrag )
    {
        vec3 u = normalize( vec3(gl_ModelViewMatrix * p) );

        float n0 = - k * A / d * sin( w*t + k*d );
        float nx = p.x * n0;
        float ny = p.y * n0;
        vec3 n = normalize( gl_NormalMatrix * vec3(nx, ny, 1.0) );

        vec3 r = reflect( u, n );

        float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );

        gl_TexCoord[0].s = r.x/m + 0.5;
        gl_TexCoord[0].t = r.y/m + 0.5;
    }

    gl_Position = gl_ModelViewProjectionMatrix * p;
}
