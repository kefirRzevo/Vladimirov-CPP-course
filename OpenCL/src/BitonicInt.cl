__kernel void bitonic(__global int *buf, uint stage, uint step) {
  uint thread = get_global_id(0);

  const uint step_len = 1 << step;
  const uint part_length = step_len * 2;
  const uint part_index = thread >> step;

  const uint i = thread - part_index * step_len;
  uint j = 0;
  if (stage == step) { // The first step in a stage
    j = part_length - i - 1;
  } else {
    j = i + step_len;
  }

  const uint offset = part_index * part_length;
  const uint first_index = offset + i;
  const uint second_index = offset + j;

  if (buf[first_index] > buf[second_index]) {
    int temp = buf[first_index];
    buf[first_index] = buf[second_index];
    buf[second_index] = temp;
  }
}