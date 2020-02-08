#ifdef GL_ES
precision mediump float;
#endif
varying highp vec2 coord;
uniform sampler2D src;
uniform lowp float qt_Opacity;
uniform lowp float blackLevel;
uniform lowp float whiteLevel;
uniform lowp float gamma;
uniform lowp vec4 color;
void main() {
    lowp vec4 tex = texture2D(src, coord);
    vec3 val = (tex.rgb - blackLevel) / (whiteLevel - blackLevel);
    val = pow(val, vec3(gamma));
    gl_FragColor = vec4(val.r * color.r, val.g * color.g, val.b * color.b, tex.a * color.a) * qt_Opacity;
}
