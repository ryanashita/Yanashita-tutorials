__global__ void multiply(const float* a, const float* b, float* c) { // function qualifier that marks the function as a GPU kernel: runs on the GPU, launched from the CPU
        *c = (*a) * (*b); // multiply the value stored at pointer a with pointer b. *a means to dereference
}
