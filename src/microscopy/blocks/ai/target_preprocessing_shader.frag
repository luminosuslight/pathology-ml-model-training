varying highp vec2 coord;
uniform sampler2D src1;
uniform sampler2D src2;
uniform sampler2D src3;
uniform lowp float qt_Opacity;

float max3(vec3 v) {
  return max(max(v.x, v.y), v.z);
}

void main() {
    lowp vec4 tex = vec4(0.0, 0.0, 0.0, 1.0);
    tex.r = texture2D(src1, coord).r;
    tex.g = texture2D(src2, coord).g;
    tex.b = texture2D(src3, coord).b;
    gl_FragColor = tex * qt_Opacity;
}
