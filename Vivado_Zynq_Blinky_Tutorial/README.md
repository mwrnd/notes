** Work-In-Progress **

The Xilinx [Kria](https://www.xilinx.com/products/som/kria.html) [KV260](https://www.xilinx.com/products/som/kria/kv260-vision-starter-kit.html) is based on the [XCK26](https://www.xilinx.com/products/silicon-devices/soc/zynq-ultrascale-mpsoc.html#eg) ([rebranded XCZU5EV-SFVC784-2LV-C](https://en.wikipedia.org/w/index.php?title=List_of_Xilinx_FPGAs&oldid=1129244401#Alveo_and_Kria)).

This tutorial is for a simple Blinking LED PL (Programmable Logic) project with minimal PS (Processing System) functionality.



## PMOD Connector

For the [PMOD](https://digilent.com/reference/pmod/start) Connector, HDA11(H12) and HDA15(B10) are filtered. Pins HDA12(E10), HDA13(D10), HDA14(C11), HDA16_CC(E12), HDA17(D11), HDA18(B11) are not. Refer to [constraints_kv260.xdc](constraints_kv260.xdc) for the pin definitions.

![Kria KV260 PMOD Schematic](img/Kria_KV260_PMOD_Schematic.png)



## Design


### Add Zynq MPSoC


Add the [Zynq MPSoC IP](https://www.xilinx.com/content/dam/xilinx/support/documents/ip_documentation/zynq_ultra_ps_e/v3_4/pg201-zynq-ultrascale-plus-processing-system.pdf) to the Block Design. Refer to the [Embedded Design Tutorial](https://xilinx.github.io/Embedded-Design-Tutorials/docs/2021.1/build/html/docs/Introduction/ZynqMPSoC-EDT/3-system-configuration.html)

![Add Zynq MPSoC IP in Block Design](img/Block_Design_Add_IP_Zynq_MPSoC.png)



### Export Hardware for Vitis

Export Hardware Platform.

![Export Hardware Platform](img/Vivado_Export_Platform.png)

Select *Hardware* as the platform.

![Export Hardware Platform](img/Export_Hardware_Platform-Platform_Type.png)

Name it.

![Export Hardware Platform](img/Export_Hardware_Platform-Output_File.png)

This is a pre-synthesis project.

![Export Hardware Platform](img/Export_Hardware_Platform-Platform_State_Exclude_Bitstream.png)

This is a pre-synthesis project.

![Export Hardware Platform](img/Export_Hardware_Platform-Platform_State_Include_Bitstream.png)

Use `1.0` for the version.

![Export Hardware Platform](img/Export_Hardware_Platform-Platform_Properties.png)

Finish creation of the Hardware Platform.

![Finish Export Hardware Platform](img/Export_Hardware_Platform-Finish.png)



## Program the Zynq in Hardware Manager

In Hardware Manager, *Open Target* then *Auto Connect*.

![Open Target Auto Connect](img/Hardware_Manager_Open_Target_Auto_Connect.png)

Right-click on `xck26_0` and *Program Device*.

![Program Device](img/Hardware_Manager_Program_Device.png)

Program the Device using the generated Bitstream.

![](img/Program_Device.png)

Run `xsct` and `loadhw` the [XSA](https://docs.xilinx.com/r/en-US/ug1400-vitis-embedded/Creating-a-Hardware-Design-XSA-File) Archive.

![xsct and loadhw XSA Archive](img/Kria_XSCT_loadhw_XSA.png)



## Create Boot Image in Vitis

Create Boot Image in Vitis

![Create Boot Image in Vitis](img/Vitis_Create_Boot_Image.png)

