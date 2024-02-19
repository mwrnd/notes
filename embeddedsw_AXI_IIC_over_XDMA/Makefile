

CC		:= gcc

COMMON_DIR	:= ./embeddedsw/lib/bsp/standalone/src/common
IIC_DIR		:= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src
COMMON_INC_DIR	:= ./embeddedsw/XilinxProcessorIPLib/drivers/common/src

INCLUDE_DIRS	:= -I . -I $(COMMON_DIR) -I $(IIC_DIR) -I $(COMMON_INC_DIR)

CFLAGS		 = $(INCLUDE_DIRS) -g -lm -Wall
CFLAGS		+= -DXPAR_IIC_0_GPO_WIDTH=0 -DXPAR_IIC_1_GPO_WIDTH=0 -D__LITTLE_ENDIAN__=0

PROGRAM	:= innova2_xdma_opencapi_iic_tc74_test

SRC_FILES	 = $(PROGRAM).c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/inbyte.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/outbyte.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/print.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_assert.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_mem.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_printf.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_testcache.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_testio.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_testmem.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_util.c
SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xpm_init.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_dyn_master.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_g.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_intr.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_l.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_master.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_multi_master.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_options.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_selftest.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_sinit.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_slave.c
SRC_FILES	+= ./embeddedsw/XilinxProcessorIPLib/drivers/iic/src/xiic_stats.c

# SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xplatform_info.c
# SRC_FILES	+= ./embeddedsw/lib/bsp/standalone/src/common/xil_sleepcommon.c


OBJECTFILES     := $(patsubst %.c,%.o,$(SRC_FILES))


all : $(OBJECTFILES)
	gcc -o $(PROGRAM) $(OBJECTFILES)


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean : 
	rm $(OBJECTFILES)
	rm $(PROGRAM)


.PHONY: all clean

