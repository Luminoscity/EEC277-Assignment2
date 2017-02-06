Comments go before the first delimiter
All text before it will be ignored
Test shaders are expected with the follwing layout:
<delimiter>Name of Double Shader for test1<delimiter>
Shader Code using doubles
<delimiter>Name of Float Shader for test1<delimiter>
Shader Code using floats
<delimiter>Name of Double Shader for test1<delimiter>
Shader Code using doubles
<delimiter>Name of Float Shader for test1<delimiter>
Shader Code using floats
<delimiter>


<<>>1SinAddDoubleTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 0) buffer inputBuffer {
	double input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = sin(1.0) + input[offset];
}


<<>>1SinAddFloatTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 1) buffer inputBuffer {
	float input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = sin(1.0) + input[offset];
}


<<>>2CosAddDoubleTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 0) buffer inputBuffer {
	double input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = cos(1.0) + input[offset];
}


<<>>2CosAddFloatTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 1) buffer inputBuffer {
	float input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = cos(1.0) + input[offset];
}


<<>>3ExpAddDoubleTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 0) buffer inputBuffer {
	double input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = exp(exp(1.0)) + input[offset];
  
  
  //Angela: doesn't make use of all FP digits in 10k iterations
  //edit: Or maybe it does? After adding all up, very large number
  //Should be more concerned with sig figs than digits after decimal
  //input[offset] = exp(2.5) + input[offset];
  
  //Angela: turns out this approaches inf at iterations = 4
  //input[offset] = exp(input[offset]);
}


<<>>3ExpAddFloatTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 1) buffer inputBuffer {
	float input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  
  input[offset] = exp(exp(1.0)) + input[offset];
}


<<>>4LargeIntegerDoubleTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 0) buffer inputBuffer {
	double input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  const double base = 3.0;
  
  input[offset] = input[offset] * base;
}


<<>>4LargeIntegerFloatTest<<>>
#version 430
layout(local_size_x = 16) in;
layout(binding = 1) buffer inputBuffer {
	float input[];
};

void main() {
  const uint offset = gl_GlobalInvocationID.x;
  const float power = 16.0;
  
  input[offset] = pow(input[offset], power);
}
<<>>