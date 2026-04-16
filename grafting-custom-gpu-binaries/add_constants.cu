#include <cstdio>

__global__ void add() {
	float a = 17.0f;
    float b = 21.0f;
    float c = a + b;

    // write to a device-side global so result is observable
    extern __shared__ float tmp[];  // optional scratch
    tmp[0] = c;
}

int main() { // main function of the cuda program
	// float cpu_c = 0.0f; // host values a,b,c. all floats 

	// float *gpu_c; // variable declaration of a pointer to a float. no memory allocated yet, no value assigned. 

	// cudaMalloc(&gpu_a, sizeof(float)); // allocate memory for gpu_a on the GPU's memory. & a pointer means a pointer to a pointer, so float**
	// cudaMalloc(&gpu_b, sizeof(float)); // cudaMalloc modifies the pointer, store the resulting GPU address of the memory that was just allocated so the CPU can pass it to GPU kernels
	// cudaMalloc(&gpu_c, sizeof(float)); // that somewhere is gpu_x. 
	
	// gpu_x is a host variable, and lives in CPU memory. its value (that it points to) is intended to be a GPU address. &gpu_x is a float**, address of the variable gpu_a in CPU memory
	// cudaMalloc writes a GPU pointer value into the memory location given, which is &gpu_x.

	// cudaMemcpy(gpu_a, &cpu_a, sizeof(float), cudaMemcpyHostToDevice); // cudaMemcpy does a data transfer. copy bytes from CPU memory location &cpu_a into GPU global memory at address gpu_a
	// cudaMemcpy(gpu_b, &cpu_b, sizeof(float), cudaMemcpyHostToDevice); 

	add<<<1, 1>>>(); // this is the kernel launch syntax func<<<number of blocks on the grid: gridDim, number of threads per block: blockDim>>>(a,b,c);
       					    // 1,1 means 1 block on the grid, 1 thread on the block
					    // launching 1 thread on the GPU to run the add kernel
					    // numBlocks, threadsPerBlock

	// block is a group of threads that can cooperate and have shared memory, sync. in CUDA can have up to thousands of threads on a block
 	// in the kernel, access local thread index and block size	
	
	// cudaMemcpy(&cpu_c, gpu_c, sizeof(float), cudaMemcpyDeviceToHost); // data transfer. copy bytes from GPU global memory at address gpu_c into CPu memory location &cpu_c

	printf("Result: %f\n", tmp[0]); 

	// cudaFree(gpu_a); //free the three float variables which were malloced
	// cudaFree(gpu_b);
	cudaFree(gpu_c);

	return 0; 
}
