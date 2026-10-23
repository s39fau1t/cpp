$input a_position, a_texcoord0
$output v_texcoord0

#include <bgfx/bgfx_shader.sh>

void main() {
  gl_Position = mul(u_model[0], vec4(a_position, 0, 1));

  v_texcoord0 = a_texcoord0;
}
