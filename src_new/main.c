#include <stdio.h>
#include <stdlib.h>

#include "arch/arch.h"

int main(int argc, char **argv) {
	struct u8_arch *arch = malloc(sizeof(struct u8_arch));

	u8_step(arch);

	return 0;
}