/*

Prerequisites:
 - PCIe XDMA AXI using OpenCAPI Connector Demo Project:
   github.com/mwrnd/innova2_xdma_opencapi
 - XDMA Drivers from github.com/xilinx/dma_ip_drivers
   Install Instructions at github.com/mwrnd/innova2_flex_xcku15p_notes


Compile with:

  make

Run with:

  sudo ./innova2_xdma_opencapi_iic_tc74_test

*/

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "xiic.h"




// The PCIe to AXI Translation Offset for the PCIe to AXI Lite Interface
#define XDMA_PCIe_to_AXI_Translation_Offset 0x40000000
#define AXI_IIC_SIZE 65536
// The AXI IIC Block Address from the Vivado Block Diagram Address Editor
#define AXI_IIC_ADDRESS 0x40060000
#define AXI_IIC_ADDRESS_ADJUSTED (AXI_IIC_ADDRESS - XDMA_PCIe_to_AXI_Translation_Offset)




// Global struct for XDMA Device files
struct _XDMA {
	char *userfilename;
	int userfd;
} xdma;




void print_iic_registers(void* iic_mem_map, uint8_t level, char* label)
{
	if ((level & 0x01))
	{
		printf("\nAXI IIC Registers in Hexadecimal - %s:\n", label);
		printf("      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
		for (int i = 0; i < 336; i++) {
			if ((i >= 0x50) && (i < 0x100)) {
				// skip 0x50 to 0xF0
			} else {
				if ((i % 16) == 0) {
					printf("\n%03X: ", i);
				}

				printf("%02X ", ((uint8_t *)iic_mem_map)[i]);
			}
		}
		printf("\n");
	}


	if ((level & 0x02))
	{
		printf("\nIIC Registers - %s:\n", label);

		// Byte Addresses need to be /4 as using word (32-bit) indexing
		printf("GIE(1C) = 0x%08X", ((u32 *)iic_mem_map)[0x1C/4]);
		printf("\t\tISR(20) = 0x%08X", ((u32 *)iic_mem_map)[0x20/4]);
		printf("\t\tIER(28) = 0x%08X", ((u32 *)iic_mem_map)[0x28/4]);
		printf("\n");
		printf("SOFTR(40) = 0x%08X", ((u32 *)iic_mem_map)[0x40/4]);
		printf("\t\tCR(100) = 0x%08X", ((u32 *)iic_mem_map)[0x100/4]);
		printf("\t\tSR(104) = 0x%08X", ((u32 *)iic_mem_map)[0x104/4]);
		printf("\n");
		printf("TX_FIFO(108) = 0x%08X", ((u32 *)iic_mem_map)[0x108/4]);
		printf("\tRX_FIFO(10C) = 0x%08X", ((u32 *)iic_mem_map)[0x10C/4]);
		printf("\tADR(110) = 0x%08X", ((u32 *)iic_mem_map)[0x110/4]);
		printf("\n");
		printf("TX_FIFO_OCY(114) = 0x%08X", ((u32 *)iic_mem_map)[0x114/4]);
		printf("\tRX_FIFO_OCY(118) = 0x%08X", ((u32 *)iic_mem_map)[0x118/4]);
		printf("\n");
		printf("TEN_ADDR(11C) = 0x%08X", ((u32 *)iic_mem_map)[0x11C/4]);
		printf("\tRX_FIFO_PIRQ(120) = 0x%08X", ((u32 *)iic_mem_map)[0x120/4]);
		printf("\tGPO(124) = 0x%08X", ((u32 *)iic_mem_map)[0x124/4]);
		printf("\n");
		printf("TSUSTA(128) = 0x%08X", ((u32 *)iic_mem_map)[0x128/4]);
		printf("\tTSUSTO(12C) = 0x%08X", ((u32 *)iic_mem_map)[0x12C/4]);
		printf("\tTHDSTA(130) = 0x%08X", ((u32 *)iic_mem_map)[0x130/4]);
		printf("\n");
		printf("TSUDAT(134) = 0x%08X", ((u32 *)iic_mem_map)[0x134/4]);
		printf("\tTBUF(138) = 0x%08X", ((u32 *)iic_mem_map)[0x138/4]);
		printf("\t\tTHIGH(13C) = 0x%08X", ((u32 *)iic_mem_map)[0x13C/4]);
		printf("\n");
		printf("TLOW(140) = 0x%08X", ((u32 *)iic_mem_map)[0x140/4]);
		printf("\t\tTHDDAT(144) = 0x%08X", ((u32 *)iic_mem_map)[0x144/4]);
		printf("\n");
	}
}




int main(int argc, char **argv)
{
	ssize_t rc1 = 0;
	ssize_t rc2 = 0;




	// Open M_AXI_LITE Device as Read-Write
	xdma.userfilename = "/dev/xdma0_user";

	xdma.userfd = open(xdma.userfilename, O_RDWR);

	if (xdma.userfd < 0) {
		fprintf(stderr, "unable to open device %s, %d.\n",
			xdma.userfilename, xdma.userfd);
		perror("File Open");
		exit(EXIT_FAILURE);
	}




	// Map the AXI IIC Portion of the M_AXI_LITE Device to Memory
	void* iic_mem_mapped = mmap(NULL, AXI_IIC_SIZE, (PROT_READ | PROT_WRITE),
		MAP_SHARED, xdma.userfd, AXI_IIC_ADDRESS_ADJUSTED);
	
	if (iic_mem_mapped == MAP_FAILED) {
		fprintf(stderr, "unable to memory map %s, %d.\n",
			xdma.userfilename, xdma.userfd);
		perror("mmap Failed");
		exit(EXIT_FAILURE);
	}




	// Current State of the AXI IIC Registers
	print_iic_registers(iic_mem_mapped, 2, "Initial");




	// TC74A0-3.3VAT has I2C Address 1001 000_ = 0x48
	u8 TempSensorAddress = 0x48;
	u8 temp = 0;
	u8 TxMsg[2];
	TxMsg[0] = 0;
	TxMsg[1] = 0;


	printf("\n\nTC74 Temperature Sensor at I2C Address 0x%02X:\n",
		TempSensorAddress);


	// Read TC74 Temperature Sensor Configuration
	TxMsg[0] = 0x01; // Command Byte = 0x01 = RWCR = Read/Write Config Reg
	rc1 = XIic_Send((UINTPTR)iic_mem_mapped, TempSensorAddress, TxMsg, 1, XIIC_REPEATED_START);
	rc2 = XIic_Recv((UINTPTR)iic_mem_mapped, TempSensorAddress, &temp, 1, XIIC_STOP);

	// D[7], 1 = standby, 0 = normal operation
	uint8_t Standby = (temp & 0x80) >> 7;
	// D[6], 1 = ready, 0 = not ready
	uint8_t Data_Ready = (temp & 0x40) >> 6;

	printf("Data_Ready = %d, Standby = %d\trc1=%ld, rc2=%ld\n",
		Data_Ready, Standby, rc1, rc2);




	// Read TC74 Temperature Value
	TxMsg[0] = 0x00; // Command Byte = 0x00 = RTR = Read Temperature
	rc1 = XIic_Send((UINTPTR)iic_mem_mapped, TempSensorAddress, TxMsg, 1, XIIC_REPEATED_START);
	rc2 = XIic_Recv((UINTPTR)iic_mem_mapped, TempSensorAddress, &temp, 1, XIIC_STOP);

	printf("Temperature = %d = 0x%02X\t\trc1=%ld, rc2=%ld\n",
		(int8_t)temp, temp, rc1, rc2);


	printf("\n\n");


	// Current State of the AXI IIC Registers
	print_iic_registers(iic_mem_mapped, 2, "End");




	munmap(iic_mem_mapped, AXI_IIC_SIZE);
	close(xdma.userfd);
	exit(EXIT_SUCCESS);
}

