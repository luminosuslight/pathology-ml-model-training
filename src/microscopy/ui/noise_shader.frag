#ifdef GL_ES
precision mediump float;
#endif
varying highp vec2 coord;
uniform sampler2D src;
uniform lowp float qt_Opacity;
uniform lowp float strength;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    lowp vec4 tex = texture2D(src, coord);
    tex.rgb = tex.rgb + rand(coord) * strength;
    gl_FragColor = tex * qt_Opacity;
}
