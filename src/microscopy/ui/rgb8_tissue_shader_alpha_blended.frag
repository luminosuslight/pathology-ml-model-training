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

float max3 (vec3 v) {
  return max (max (v.x, v.y), v.z);
}

void main() {
    lowp vec4 tex = texture2D(src, coord);
    vec3 val = (tex.rgb - blackLevel) / (whiteLevel - blackLevel);
    val = pow(val, vec3(gamma));
    val = vec3(val.r * color.r, val.g * color.g, val.b * color.b);
    gl_FragColor = vec4(val.rgb, tex.a * color.a * max3(val)) * qt_Opacity;
}
