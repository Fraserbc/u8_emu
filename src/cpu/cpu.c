#include <stdio.h>

#include "cpu.h"
#include "instr.h"

int main() {
	u8_cpu cpu;

	// Set register values
	reg_set_r(&cpu, 0, 0x10);
	reg_set_r(&cpu, 1, 0x5);

	printf("%x\n", reg_get_er(&cpu, 0));

	return 0;
}