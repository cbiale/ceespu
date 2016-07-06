#pragma once
#define VMEM_SIZE (2 << 13)

typedef union _Data  {
	unsigned int _dword;
    unsigned short _words[2];
} Data;

typedef struct _VirtualMem {
	Data mem[VMEM_SIZE];
} VirtualMem;

typedef struct _VirtualMachine {
	unsigned short RegFile [31];		// the register file
	VirtualMem *vmem;	// the virtual memory
	int PC;				// the program counter
	unsigned short stack_pointer; // the stack pointer
	unsigned short carry_flag;
} VirtualMachine;
