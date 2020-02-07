precision mediump float;
varying highp vec2 coord;
uniform sampler2D src;
uniform lowp float qt_Opacity;
uniform lowp float blackLevel;
uniform lowp float whiteLevel;
uniform lowp float gamma;
uniform lowp vec4 color;
void main() {
    lowp vec4 tex = texture2D(src, coord);
    float val = ((tex.r + tex.g / 256.0) - blackLevel) / (whiteLevel - blackLevel);
    val = pow(val, gamma);
    gl_FragColor = vec4(val * color.r, val * color.g, val * color.b, tex.a * color.a) * qt_Opacity;
}
