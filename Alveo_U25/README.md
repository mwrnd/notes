**Work-In-Progress**

**These notes have moved to [https://github.com/mwrnd/alveo_u25_notes](https://github.com/mwrnd/alveo_u25_notes).**

# Alveo U25 Setup and Usage Notes

Currently focused on trying to [debug a faulty Alveo U25](debug_log.md).

The [Alveo U25](https://www.xilinx.com/content/dam/xilinx/publications/product-briefs/alveo-u25-product-brief.pdf) is based on the [XCU25](https://en.wikipedia.org/w/index.php?title=List_of_Xilinx_FPGAs&oldid=1129244401)==[XCZU19EG-FFVC1760](https://www.xilinx.com/products/silicon-devices/soc/zynq-ultrascale-mpsoc.html#eg).

![Alveo U25 Overview](img/AlveoU25_Overview.png)



## JTAG Using UrJTAG

[UrJTAG](http://urjtag.org) is a low-level tool for communicating with JTAG devices. It supports [Xilinx Platform Cable USB II](https://docs.xilinx.com/v/u/en-US/ds593) adapters and [clones](https://www.waveshare.com/platform-cable-usb.htm). Main use is *EXTEST* pin toggling although it is theoretically possible to program the FPGA using an [SVF file generated from the Vivado Tcl Console](https://docs.xilinx.com/r/2021.2-English/ug908-vivado-programming-debugging/Using-the-Command-Line?tocId=L2PEfMkFFNtSZI~r7QbNVg).


### Compile and Install UrJTAG

```Shell
cd ~
wget https://downloads.sourceforge.net/project/urjtag/urjtag/2021.03/urjtag-2021.03.tar.xz
sha256sum urjtag-2021.03.tar.xz
echo b0a2eaa245513af096dc4d770109832335c694c6c12aa5e92fefae8685416f1c should be the SHA256 Checksum
tar -xvf urjtag-2021.03.tar.xz
cd urjtag-2021.03/
./configure
make
sudo make install
sudo ldconfig
```


### Create UrJTAG-Compatible JTAG Definition Files from BSDL Files

Xilinx's [Zynq Ultrascale+ BSDL Files](https://www.xilinx.com/member/forms/download/sim-model-eval-license-xef.html?filename=bsdl_zynquplus_2021_2.zip) include `STD_1149_6_2003.all` definitions that UrJTAG's `bsdl2jtag` cannot process and must therefore be removed. The included [xczu19eg_ffvc1760.jtag](xczu19eg_ffvc1760.jtag) and [zynqu_arm_dap.jtag](zynqu_arm_dap.jtag) files are processed and tested to work with UrJTAG.


### Add XCZU19EG FFVC1760 JTAG Bit Definitions to UrJTAG

From the directory containing the `.jtag` files run the following commands which create *PART* and *STEPPINGS* files for the XCZU19EG. These commands assume UrJTAG installed support files to the default `/usr/local/share/` directory. Values were found by running the UrJTAG `detect` command which reads the `Device Id` from the JTAG chain. First 4 bits (`0001`) are the *STEPPING*, next 16 bits (`0100011101011000`) are the *PART*, last 12 bits (`01000111011`) are the *MANUFACTURER*.
```Shell
sudo su
echo "# Zynq Ultrascale+ (XCZUxxEG)" >>/usr/local/share/urjtag/xilinx/PARTS
echo -e "0100011101011000\txczu19eg_1760\txczu19eg_ffvc1760" >>/usr/local/share/urjtag/xilinx/PARTS
mkdir /usr/local/share/urjtag/xilinx/xczu19eg_1760
touch /usr/local/share/urjtag/xilinx/xczu19eg_1760/STEPPINGS
echo "0001 xczu19eg_1760 1" >>/usr/local/share/urjtag/xilinx/xczu19eg_1760/STEPPINGS
echo "0000 xczu19eg_1760 0" >>/usr/local/share/urjtag/xilinx/xczu19eg_1760/STEPPINGS
cp xczu19eg_ffvc1760.jtag /usr/local/share/urjtag/xilinx/xczu19eg_1760/xczu19eg_1760
echo -e "01000111011\tarm\t\tARM"  >>/usr/local/share/urjtag/MANUFACTURERS
mkdir /usr/local/share/urjtag/arm
touch /usr/local/share/urjtag/arm/PARTS
echo -e "1011101000000000\tzynqu_arm_dap\tzynqu_arm_dap" >>/usr/local/share/urjtag/arm/PARTS
mkdir /usr/local/share/urjtag/arm/zynqu_arm_dap
touch /usr/local/share/urjtag/arm/zynqu_arm_dap/STEPPINGS
echo "0101 zynqu_arm_dap 5" >>/usr/local/share/urjtag/arm/zynqu_arm_dap/STEPPINGS
cp zynqu_arm_dap.jtag /usr/local/share/urjtag/arm/zynqu_arm_dap/zynqu_arm_dap
exit
```

![Configuring UrJTAG for the Zynq XCZU19EG](img/UrJTAG-Configuring_for_Zynq.png)




#### Connect JTAG Adapter and Allow Vivado to Update Platform Cable USB II Firmware

Connect your JTAG Adapter to the Alveo U25. The Alveo needs to be powered over its PCIe connection.

![JTAG Connected](img/AlveoU25_JTAG_Connected.jpg)

If you are using a Platform Cable USB II compatible adapter it will show up under `lsusb` as `03fd:0013 Xilinx, Inc.` In this state it _CANNOT_ be used for JTAG.

![03fd 0013 Xilinx Inc](img/Xilinx_Platform_USB_Cable_II_lsusb_Initial.png)

Run Xilinx's `xsdb` which comes with Vivado:
```
source /tools/Xilinx/Vivado/2021.2/settings64.sh
xsdb
```

In `xsdb` run the following commands to bring the Zynq out of the `L2 Cache Reset` state and into the `Power On Reset` and `External Debug Request` state. The 10 second delay (`after 10000`) allows for Xilinx's `hw_server` to update the Platform Cable II Firmware in the background.
```
connect
after 10000
targets
targets -set -nocase -filter {name =~ "*APU*"}
mwr 0xffff0000 0x14000000
mask_write 0xFD1A0104 0x501 0x0
targets -set -nocase -filter {name =~ "*A53*#0"}
stop
targets
```

![JTAG Enabled when Cortex-A53 in External Debug Request](img/JTAG_xsdb_xsct_L2_Cache_Reset_to_External_Debug_Request.png)

`exit` from `xsdb`. `lsusb` should now show `03fd:0008 Xilinx, Inc. Platform Cable USB II`. The JTAG adapter is ready to be used by UrJTAG.

![03fd 0008 Xilinx Inc Platform Cable USB II](img/Xilinx_Platform_USB_Cable_II_lsusb_After_Update.png)




#### Begin a UrJTAG Session

`sudo jtag` to start UrJTAG with access to USB. You should see the `jtag> ` command prompt. A [JTAG Overview](https://www.xjtag.com/about-jtag/jtag-a-technical-overview/) may be helpful before you begin. Note that `shift ir` and `shift dr` need to be run to change the debug target's state. Running `instruction` commands only changes UrJTAG's internal state.

Valid signal names can be found in the [xczu19eg_ffvc1760.jtag](https://github.com/mwrnd/notes/blob/f52bbadc220d22232c5bf39dd608829c7e002867/Alveo_U25/xczu19eg_ffvc1760.jtag#L1054) file.

`cable xpc_ext` selects the Xilinx Platform Cable II, external JTAG chain

`detect` finds all devices in the JTAG chain

`print chain` prints all devices in the JTAG chain

`part 1` selects the Zynq for JTAG communication. Good practise to always make this explicit.

`instruction EXTEST` select the External Test Function

`shift ir` shifts EXTEST into the JTAG Instruction Register to put the device into EXTEST mode

`shift dr` shifts the Data Register containing all pin states into UrJTAG memory

`dr` prints the full contents of the JTAG Data Register.

It is more useful to read individual signals:

`get signal PS_MODE0` displays the value of pin PS_MODE0(AA27)

`get signal PS_MODE1` displays the value of pin PS_MODE1(AC28)

`get signal PS_MODE2` displays the value of pin PS_MODE2(AA28)

`get signal PS_MODE3` displays the value of pin PS_MODE3(AB28)

The above MODE pins define Quad-SPI as the [Boot Mode](https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/Boot-Modes).

`get signal PS_ERROR_OUT` indicates a Power, Hardware, or PMU Exception when =1.

`get signal PS_INIT_B` indicates a CRC Error in the PL Bitstream when =1.

`get signal PS_ERROR_STATUS` indicates a lockdown state or PMU Status Error when =1.

`get signal PS_DONE` indicates PL Configuration completed when =1.

`reset` resets the JTAG chain and enters Bypass Mode

`quit` exits UrJTAG

![UrJTAG Session](img/UrJTAG_Session_with_Zynq.png)


The included UrJTAG scripts [`getallio`](getallio) and [`getallpsmio`](getallpsmio) will return the states of all the Programmable Logic (PL) I/O pins and Processing System (PS) Multiplexed I/O pins, respectively.
```
include /DOWNLOAD_DIRECTORY/getallio
```

A sequence of instructions like the following can be used to set IO pins. Be careful which signals you set. They should be wired appropriately on the board. Refer to [xczu19eg_ffvc1760.jtag](https://github.com/mwrnd/notes/blob/f52bbadc220d22232c5bf39dd608829c7e002867/Alveo_U25/xczu19eg_ffvc1760.jtag#L1054) for signal names.

```
instruction SAMPLE/PRELOAD
shift ir
shift dr
get signal IO_??

instruction EXTEST
shift ir
set signal IO_?? out 0
shift dr

instruction SAMPLE/PRELOAD
shift ir
shift dr
get signal IO_??

instruction EXTEST
shift ir
set signal IO_?? in
shift dr

instruction SAMPLE/PRELOAD
shift ir
shift dr
get signal IO_??
```




## Reading Back the Zynq Configuration Memory

In the Vivado Tcl Console, run:
```
readback_hw_device [current_hw_device]  -readback_file zu19eg_u25_r.rbd  -bin_file zu19eg_u25_b.bin
```

![Configuration Memory Readback](img/Hardware_Manager_Configuration_Memory_Readback.png)




## Miscallaneous Notes

[XCU25==XCZU19EG-FFVC1760](https://en.wikipedia.org/w/index.php?title=List_of_Xilinx_FPGAs&oldid=1129244401) Banks from [Zynq Ultrascale+ Device Packaging and Pinouts](https://docs.xilinx.com/v/u/en-US/ug1075-zynq-ultrascale-pkg-pinout).

![XCU25 XCZU19EG FFVC1760 Banks](img/XCU25_XCZU19EG_FFVC1760_Banks_ug1075.png)




## Useful Links

[Zynq Ultrascale+ MPSoC Overview](https://docs.xilinx.com/v/u/en-US/ds891-zynq-ultrascale-plus-overview)

[Zynq Ultrascale+ MPSoC Design Hub](https://xilinx.com/support/documentation-navigation/design-hubs/dh0070-zynq-mpsoc-design-overview-hub.html)

[Zynq Ultrascale+ Technical Reference Manual](https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm)

[Zynq Ultrascale+ Device Packaging and Pinouts](https://docs.xilinx.com/v/u/en-US/ug1075-zynq-ultrascale-pkg-pinout)

[Zynq Ultrascale+ MPSoC Datasheet](https://www.xilinx.com/content/dam/xilinx/support/documents/data_sheets/ds925-zynq-ultrascale-plus.pdf)

[Zynq UltraScale+ Devices Register Reference](https://docs.xilinx.com/r/en-US/ug1087-zynq-ultrascale-registers/Overview)([HTML](https://www.xilinx.com/htmldocs/registers/ug1087/ug1087-zynq-ultrascale-registers.html)) is [Downloadable](https://www.xilinx.com/html_docs/registers/ug1087/ug1087-zynq-ultrascale-registers.zip)

[ServeTheHome Forum](https://forums.servethehome.com/index.php?threads/identifying-some-curious-xilinx-solarflare-cards.35111) post about the Alveo U25

[Zynq Boot Process Overview](https://docs.xilinx.com/r/en-US/ug1137-zynq-ultrascale-mpsoc-swdev/Boot-Process-Overview)

[JTAG Boot Image Format](https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/Boot-Image-Format)

[Bare Metal Application Development](https://docs.xilinx.com/r/2021.1-English/ug1137-zynq-ultrascale-mpsoc-swdev/Bare-Metal-Application-Development)

[XRT Documentation](https://xilinx.github.io/XRT/2020.2/html/platforms_partitions.html#single-stage-platforms)

[XRT Dual-Stage Platform](https://xilinx.github.io/XRT/master/html/platforms_partitions.html#two-stage-platforms) - U25 shell stage is included in the [U25 FPGA Utility](https://support.lenovo.com/us/en/downloads/ds547996-xilinx-u25-fpga-utility-for-unknown-os) ([2.8.372](https://download.lenovo.com/servers/mig/2020/12/23/43128/xlnx-lnvgy_utl_nic_u25-2.8.372_linux_x86-64.tgz)) package. Base stage is not public.

[Flashing Shell to U25N](https://xilinx.github.io/U25N-SmartNIC-Solution/docs/build/html/docs/ug1534-shellprogramming.html)

[Flashing Shell to U25 Support Forum Post](https://support.xilinx.com/s/question/0D52E00006hpYjRSAU/flashing-shell-to-u25?language=en_US)

[Alveo Platform Partitions](https://xilinx.github.io/XRT/master/html/platforms_partitions.html)

[Alveo U25 Member Page](https://www.xilinx.com/member/u25.html)

[U25 Xilinx Lenovo Page](https://www.xilinx.com/products/boards-and-kits/alveo/oem-partner/lenovo-servers.html) has a [Production Package](https://www.xilinx.com/content/dam/xilinx/publications/products/board-kits/u25_production_package_nov20.zip) and a [Utilities Package](https://www.xilinx.com/content/dam/xilinx/support/download/lenovo/xn-200528-ls-3-x2522-25g-u25-lenovo-linux-utilities-rpm-64-bit.zip)

[U25 X2 NIC Driver](https://support.lenovo.com/us/en/downloads/ds547997-xilinx-u25-x2-nic-driver-for-unknown-os) ([4.15.10.1003](https://download.lenovo.com/servers/mig/2020/12/23/43129/xlnx-lnvgy_dd_nic_u25-4.15.10.1003_linux_x86-64.tgz))

[U25 X2 NIC Firmware](https://support.lenovo.com/us/en/downloads/ds548057-xilinx-u25-x2-nic-firmware-for-unknown-os) ([8.1.3.1011-1](https://download.lenovo.com/servers/mig/2021/01/06/43174/xlnx-lnvgy_fw_nic_u25-8.1.3.1011-1_linux_x86-64.tgz))

[U25 FPGA Utility](https://support.lenovo.com/us/en/downloads/ds547996-xilinx-u25-fpga-utility-for-unknown-os) ([2.8.372](https://download.lenovo.com/servers/mig/2020/12/23/43128/xlnx-lnvgy_utl_nic_u25-2.8.372_linux_x86-64.tgz))

[U25 Cloud Instance](https://vmaccel.com/xilinxtrial/)

[Alveo U25 Product Brief](https://www.xilinx.com/content/dam/xilinx/publications/product-briefs/alveo-u25-product-brief.pdf)

[Alveo U25N Product Brief](https://www.xilinx.com/content/dam/xilinx/publications/product-briefs/xilinx-u25N-product-brief.pdf) is almost identical but lacks the FPGA to PCIe connections.

[U25N Install Notes](https://xilinx.github.io/U25N-SmartNIC-Solution/docs/build/html/docs/ug1534-installation.html)

[MT40A512M16LY-062E](https://media-www.micron.com/-/media/client/global/documents/products/data-sheet/dram/ddr4/8gb_ddr4_sdram.pdf)([D9WFJ FBGA Code](https://www.micron.com/support/tools-and-utilities/fbga?fbga=D9WFJ#pnlFBGA)) is the DDR4 memory IC used on the U25.

[MT25QU01](https://media-www.micron.com/-/media/client/global/documents/products/data-sheet/nor-flash/serial-nor/mt25q/die-rev-b/mt25q_qlkt_u_01g_bbb_0.pdf)([RW188 FBGA Code](https://www.micron.com/support/tools-and-utilities/fbga?fbga=RW188#pnlFBGA)) is the QSPI Configuration Memory flash IC used on the U25.


