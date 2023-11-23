#version 410 core

out vec4 color;

in VS_OUT
{
    vec3 vertex;
    vec3 normals;
    vec2 tc;
} fs_in;

uniform sampler2D tex;
uniform mat4 model_matrix;

uniform float u_time;

#define PI 3.1415926535

// TODO: made the lighting much smoother.
void main(void){
    vec2 u_resolution = vec2(1000.0, 1000.0);
vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;
    
    float speed = 1.;
    float frequency = 8.;
    float intensity = 0.07;

    float ti = u_time*speed;
    float tii = u_time*speed;
    float t = (ti-floor(ti))*PI;
    
    
    //the core of the code, distorts the image making it wobbly.
    //Use an random number for the speed of the two wobble funcitons to
    //ensure they are unsynced for maximum randomness.
    //If you wish for the wooble to be lower you can change those values.
    //I recommend between 1.0-5.0.
    
    float wobble = cos((st.y+st.x)*frequency*2.+t) * intensity * sin(st.x*frequency + t);
    vec2 p = vec2(st.x-wobble, st.y-wobble*0.5)+vec2(-0.5, -0.5);
    float c = sin( ( ((p.x)*p.y*2.)*109.))+-0.084;

    color = texture(tex, fs_in.tc)+vec4(c*0., c*0.164+0.3, c*0.188+0.5,1.0);
}