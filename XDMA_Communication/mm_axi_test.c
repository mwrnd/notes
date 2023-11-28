/*

Prerequisites:
 - Vivado XDMA Project with BRAM connected to M_AXI:
   github.com/mwrnd/notes/tree/main/XDMA_Communication
 - XDMA Drivers from github.com/xilinx/dma_ip_drivers
   Install Instructions at github.com/mwrnd/innova2_flex_xcku15p_notes


Compile with:

  gcc -Wall mm_axi_test.c -o mm_axi_test

Run with:

  sudo ./mm_axi_test

*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>




// Using 8 kbyte == 8192 byte array. Size was defined in the
// Vivado FPGA Project Block Diagram Address Editor as the Data Range for BRAM
// Each word is 32-Bits so there are 1/4th as many data words.
// On Linux, read/write can transfer at most 0x7FFFF000 (2,147,479,552) bytes
#define DATA_BYTES	8192
#define DATA_WORDS	(DATA_BYTES/4)




int main(int argc, char **argv)
{
	uint32_t buffer[DATA_WORDS];
	uint64_t address = 0xC0000000;
	int xdma_h2cfd = 0;
	int xdma_c2hfd = 0;
	ssize_t rc;

	// Fill the buffer with data
	for (int i = 0; i < DATA_WORDS; i++) { buffer[i] = (DATA_WORDS - i); }

	printf("Buffer Contents before H2C write: \n");
	printf("[0]=%04d, [4]=%04d, [%d]=%04d\n",
		(uint32_t)buffer[0], (uint32_t)buffer[4],
		(DATA_WORDS - 3), (uint32_t)buffer[(DATA_WORDS - 3)]);

	// Open M_AXI H2C Host-to-Card Device as Write-Only
	xdma_h2cfd = open("/dev/xdma0_h2c_0", O_WRONLY);

	// Write the full buffer to the FPGA design's BRAM
	rc = pwrite(xdma_h2cfd, buffer, DATA_BYTES, address);

	// Clear the buffer to make sure data was read from FPGA
	printf("\nClearing buffer.\n");
	for (int i = 0; i < DATA_WORDS ; i++) { buffer[i] = 0; }


	// Open M_AXI C2H Card-to-Host Device as Read-Only
	xdma_c2hfd = open("/dev/xdma0_c2h_0", O_RDONLY);

	// Read the full buffer from the FPGA design's BRAM
	rc = pread(xdma_c2hfd, buffer, DATA_BYTES, address);

	printf("\nBuffer Contents after C2H read: \n");
	printf("[0]=%04d, [4]=%04d, [%d]=%04d\n",
		(uint32_t)buffer[0], (uint32_t)buffer[4],
		(DATA_WORDS - 3), (uint32_t)buffer[(DATA_WORDS - 3)]);

	printf("\nrc = %ld = bytes read from FPGA's BRAM\n", rc);


	close(xdma_h2cfd);
	close(xdma_c2hfd);
	exit(EXIT_SUCCESS);
}

