/*

Prerequisites:
 - Xilinx XDMA AXI-Stream Project:
   github.com/mwrnd/notes/tree/main/XDMA_Communication
 - XDMA Drivers from github.com/xilinx/dma_ip_drivers
   Install Instructions at github.com/mwrnd/innova2_flex_xcku15p_notes

Compile with:

  gcc -Wall stream_test.c -o stream_test -lm

Run with:

  sudo ./stream_test

*/

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// With a PCIe x1 Design, AXI Width = 64-Bit=8-Byte
// Both H2C (Host-to-Card) and C2H (Card-to-Host) have FIFO space for 
// for two transfers each. Data flows from H2C to C2H.
#define DATA_SIZE 64
#define H2C_FLOAT_COUNT (DATA_SIZE / 4)
#define C2H_FLOAT_COUNT (H2C_FLOAT_COUNT / 2)



int main(int argc, char **argv)
{
	int xdma_fd_read;
	int xdma_fd_wrte;
	float h2c_data[H2C_FLOAT_COUNT];
	float c2h_data[C2H_FLOAT_COUNT];
	ssize_t rc = 0;


	xdma_fd_wrte = open("/dev/xdma0_h2c_0", O_WRONLY);
	xdma_fd_read = open("/dev/xdma0_c2h_0", O_RDONLY);


	printf("H2C_FLOAT_COUNT = %d, C2H_FLOAT_COUNT = %d\n",
		H2C_FLOAT_COUNT, C2H_FLOAT_COUNT);

	// fill the write data buffer with floating point values
	for (int i = 0; i < H2C_FLOAT_COUNT; i++) { h2c_data[i]=(3.14*(i+1)); }



	// write data buffer to the AXI Stream - a float is 4-bytes
	rc = pwrite(xdma_fd_wrte, h2c_data, (H2C_FLOAT_COUNT * 4), 0);
	printf("Write returned rc = %ld = number of bytes sent\n", rc);

	// read data from the AXI Stream into buffer - a float is 4-bytes
	rc = pread (xdma_fd_read, c2h_data, (C2H_FLOAT_COUNT * 4), 0);
	printf("Read  returned rc = %ld = number of bytes received\n", rc);


	// print the data in the return data (C2H) buffer
	uint32_t j = 0;
	float expected = 0;
	printf("\n");
	for (int i = 0 ; i < H2C_FLOAT_COUNT; i=i+2)
	{
	
		j = floor((i / 2));
		printf("%-2d, %-2d, h2c[%02d]*[%02d]=c2h[%02d] = %f*%f = %f",
		i, j, i, (i+1), j, h2c_data[i],
		h2c_data[(i+1)], c2h_data[j]);
		if (fabs((h2c_data[i] * h2c_data[(i+1)]) - c2h_data[j]) > 0.01)
		{
			expected = (h2c_data[i] * h2c_data[(i+1)]);
			printf(" -- ERROR, was expecting %f", expected);
		}
		printf("\n");
	}




	close(xdma_fd_wrte);
	close(xdma_fd_read);
	exit(EXIT_SUCCESS);
}

