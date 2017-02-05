#version 430
layout(local_size_x = 16) in;
layout(binding = 0) buffer inputBuffer {
	float input[];
};
layout(binding = 1) buffer outputBuffer {
	float result[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  const float scale = 2.0;
  
  result[offset] = scale * input[offset];
}