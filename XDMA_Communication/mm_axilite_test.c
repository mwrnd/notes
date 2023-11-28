/*

Prerequisites:
 - Vivado XDMA Project with BRAM connected to M_AXI_LITE:
   github.com/mwrnd/notes/tree/main/XDMA_Communication
 - XDMA Drivers from github.com/xilinx/dma_ip_drivers
   Install Instructions at github.com/mwrnd/innova2_flex_xcku15p_notes


Compile with:

  gcc -Wall mm_axilite_test.c -o mm_axilite_test

Run with:

  sudo ./mm_axilite_test

*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


// The PCIe to AXI Translation Offset for the PCIe to AXI-Lite Interface
#define XDMA_PCIe_to_AXI_Translation_Offset 0x40000000


int main(int argc, char **argv)
{
	// Open M_AXI_LITE Device as Read-Write
	int xdma_userfd = 0;
	xdma_userfd = open("/dev/xdma0_user", O_RDWR);

	uint64_t address = 0x40010000 - XDMA_PCIe_to_AXI_Translation_Offset;
	uint32_t data_word = 0xAA55A55A;
	ssize_t rc;

	rc = pwrite(xdma_userfd, &data_word, 4, address);

	data_word = 0;

	rc = pread(xdma_userfd, &data_word, 4, address);

	printf("AXILite Address 0x%08lX after offset has data: 0x%08X",
		address, data_word);
	printf(", rc = %ld\n", rc);


	close(xdma_userfd);
	exit(EXIT_SUCCESS);
}

