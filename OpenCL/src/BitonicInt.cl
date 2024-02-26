__kernel void bitonic(__global int* buf, uint stage, uint step) {
    uint i = get_global_id(0);
    uint iStep = i ^ step;
    if (iStep > i) {
        if ((i & stage) == 0 && buf[i] > buf[iStep]) {
            int tmp = buf[i];
            buf[i] = buf[iStep];
            buf[iStep] = tmp;
        }
        if ((i & stage) != 0 && buf[i] < buf[iStep]) {
            int tmp = buf[i];
            buf[i] = buf[iStep];
            buf[iStep] = tmp;
        }
    }
}
