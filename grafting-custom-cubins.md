# Grafting Custom GPU sections (.cubin files) into a fatbin file
This is a tutorial on how to graft custom GPU sections into a fatbin file. By custom GPU sections, I am talking about .cubin files (cuda binaries). 
These .cubin files are target-machine-specific, and in this tutorial they are specific to sm_75 (Nvidia GPU architecture with compute capability 7.5).
The reason for doing this is to understand how to edit & write cubin files and run custom fatbin files using Nvidia's driver API. 
Grafting in this context means taking a existing fatbin and cubin, modifying the cubin, and rebuilding the fatbin with the modified cubin file. 
The SASS assembly code inside the .cubin can be modified to perform a computation that differs from the computation of the kernel that was complied with NVCC. 

For this tutorial, I will compile a cuda .cu file add.cu with the command nvcc --keep -arch=sm_75 -c add.cu. The --keep command is to keep all 
intermediate files used in NVCC's full compilation process. The add.sm_75.cubin, add.fatbin, and add.o files will be particularly useful. 

## Understanding .cubin, .fatbin, and SASS asm 
1. Extract nv_fatbin binary from the add.o object file using the command objdump -s -j nv_fatbin add.o.
2. View the hexdump of the add.fatbin file with command hexdump -C add.fatbin
3. Comparing the outputs from steps (1) and (2), it is clear that they are identical, meaning the fatbin portion in the final object file is unchanged from the fatbin file
created intermediately by the compiler. This is because the fatbin contains only information and data relevant to the GPU computation, nothing executed by the CPU.
This is important because everything we need to modify the GPU's computation / create custom GPU sections are contained in the fatbin. 
It's type is also PROGBITs (more on this later). 
4. Either open the existing add.sm_75.cubin file with nvdisasm add.sm_75.cubin, or run the command cuobjdump -xelf add1.sm_75.cubin add.fatbin
(the cubin file's name can be anything, I added the '1' to differentiate between the two otherwise identical files). Looking at the contents of the disassembled cubin file
(run the nvdisasm command in this step), it can be seen that the FADD instruction (which we want to modify), is at a specific address. Running the command readelf -S add.sm_75.cubin
shows that the SASS asm where the FADD instruction resides is at a specific offset from the top of the cubin file. Adding the two addresses gets us the correct,
specific address of the FADD instruction: address = offset of .text section + address of FADD within the .text section.
5. Find a NOP instruction inside the .cubin file and save its hexadecimal sequence somewhere. We will print this to the cubin file and overwrite the FADD instruction.
To do this, run the command xxd -g 8 -l 8 -s 0x7c0 add.sm_75.cubin. The hexademical value '0x7c0' is the address I calculated of the NOP instruction I found. Save the hexadecimal
value that is outputted. For example, the NOP instruction I found and will use is 18 79 00 00 00 00 00 00.

## Modifying the .cubin file by overwriting FADD instruction with a NOP instruction
7. Run the command printf 'HEXADECIMAL INSTRUCTION SEQUENCE HERE' | dd of=add.sm_75.cubin bs=1 seek=$((0x760)), where 0x760 is the address of the FADD instruction I want to overwrite
with a NOP. 
8. If you want to check that the instruction was indeed overwritten, run command nvdisasm --print-line-info add.sm_75.cubin
9. rebuild the fatbin with the cubin we just modified using the command nvcc -cubin add.sm_75.cubin -arch=sm_75 -o patched.fatbin (renamed the fatbin to decrease confusion).

## Running the modified .fatbin file using the Nvidia Driver API and not using NVCC!
10. //TODO


