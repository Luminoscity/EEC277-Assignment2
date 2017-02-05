#version 430
layout(local_size_x = 16) in;
layout(binding = 0) buffer inputBuffer {
	double input[];
};
layout(binding = 1) buffer outputBuffer {
	double result[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = cos(1.0) + input[offset];
}