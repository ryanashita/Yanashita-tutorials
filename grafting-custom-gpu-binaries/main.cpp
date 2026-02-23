#include <cuda.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

int main() {
	CUresult res = cuInit(0); 
	if (res != 0) {
		std::cerr << "failed to initialize: " << res << std::endl;
		return 1; 
	}

	CUdevice device; 
	res = cuDeviceGet(&device, 0); 
	if (res != 0) {
		std::cerr << "failed to get device: " << res << std::endl;
	        return 1; 	
	}

	CUcontext context; 
	res = cuCtxCreate(&context, nullptr, 0, device);
       	if (res != 0) {
		std::cerr << "failed to create context: " << res << std::endl;
	       return 1; 	
	}	
	
	//read contents of fatbin into a buffer pointer
	std::ifstream input("patched2.fatbin", std::ios::binary );
	std::vector<unsigned char> buffer(
			std::istreambuf_iterator<char>(input), {}); 	

	CUmodule module; 
	res = cuModuleLoadFatBinary(&module, buffer.data()); 
	if (res != 0) {
		std::cerr << "failed to load fatbin: " << res << std::endl;
		return 1; 
	}
	
	CUfunction function; 
	res = cuModuleGetFunction(&function, module, "_Z3addPKfS0_Pf"); 	
	if (res != 0) {
		std::cerr << "failed to return function handle: " << res << std::endl;
		return 1; 
	}
	
	res = cuFuncLoad(function);
	if (res != 0) {
		std::cerr << "failed to load function: " << res << std::endl;
		return 1; 
	}
		
	CUfunctionLoadingState state;
        std::cerr << "after state init" << std::endl; 	
	cuFuncIsLoaded(&state, function); 	
	if (res != 0) {
		std::cerr << "function is not loaded: " << res << std::endl;
		return 1; 
	}

	//kernel parameters
	int N = 1024;
	//values to use in computation
        float a = 4.5f, b = 2.5f, c = 0.0;	

	CUdeviceptr d_a, d_b, d_c; 
	cuMemAlloc(&d_a, sizeof(float)); 
	cuMemAlloc(&d_b, sizeof(float)); 
	cuMemAlloc(&d_c, sizeof(float)); 
	 	
	cuMemcpyHtoD(d_a, &a, sizeof(float));
	cuMemcpyHtoD(d_b, &b, sizeof(float));

	void* args[3] = {&d_a, &d_b, &d_c}; 	
	res = cuLaunchKernel(function, 1, 1, 1, 1, 1, 1, 0, 0, args, NULL); 	
	if (res != 0) {
		std::cerr << "kernel not launched: " << res << std::endl;
		return 1; 
	}

	//cuCtxSynchronize();
	
	cuMemcpyDtoH(&c, d_c, sizeof(float));
       		
	std::cout << "successfully launched kernel" << std::endl; 
	std::cout << "Originally added the two values 2.5 + 4.5 = 7.0f. Patched fatbin multiplies the two values instead of adding, expected value is 2.5 * 4.5 = 11.25. Computed value: " << std::to_string(c) << std::endl;
	
	cuMemFree(d_a); // free memory that was allocated earlier 
	cuMemFree(d_b); 
	cuMemFree(d_c); 
	
	return 0;
}
