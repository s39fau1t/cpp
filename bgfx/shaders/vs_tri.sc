$input a_position, a_color0
$output v_color0

#include <bgfx/bgfx_shader.sh>

void main() {
  gl_Position = mul(u_model[0], vec4(a_position, 0, 1));

  v_color0 = a_color0;
}
