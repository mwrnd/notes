# Vivado XDMA and DDR Memory Project

This tutorial goes through the steps of generating a basic XDMA and DDR4 Block Diagram Design in Vivado for the [XCKU15P](https://www.xilinx.com/products/silicon-devices/fpga/kintex-ultrascale-plus.html) FPGA in the [Innova-2](https://github.com/mwrnd/innova2_flex_xcku15p_notes).

Under [Linux](https://github.com/mwrnd/innova2_flex_xcku15p_notes#system-setup), start Vivado with:
```
sudo /tools/Xilinx/Vivado/2021.2/bin/vivado
```

*Create Project*

![Create Project](img/Vivado_Create_Project.png)

Choose directory and project name.

![Directory and Project Name](img/Vivado_New_Project.png)

No source files are needed to start.

![No Source Files](img/RTL_Project_No_Sources.png)

Select the target FPGA, in this case the `xcku15p-ffve1517-2-i`.

![Target FPGA](img/Choose_Target_FPGA.png)

Project Summary.

![Project Summary](img/Project_Summary.png)

Click on **Create Block Design** in the Flow Navigator and give it a name.

![Create Block Design](img/Create_Block_Design.png)



## Add XDMA

In the Diagram editor window, click **Add IP** and search for `xdma`. Double-click **DMA/Bridge Subsystem for PCI Express**.

![DMA Bridge Subsystem for PCI Express](img/Add_IP_XDMA.png)

Under the *Basic* tab in XDMA Customization, choose **X0Y2** as the *PCIe Block Location*, **X8** as the *Lane Width*, **8.0 GT/s** as the *Maximum Link Speed*, and **AXI Memory Mapped** as the *DMA Interface Option*. The actual pins are defined later. This selects which [PCIe Block](https://docs.xilinx.com/r/en-US/pg213-pcie4-ultrascale-plus) will be controlling the [Gigabit GTY SERDES Transceivers](https://docs.xilinx.com/v/u/en-US/ug578-ultrascale-gty-transceivers). All other options can be left at their defaults.

![Customize Basic XDMA Settings](img/Customize_XDMA_IP_Basic.png)

Under the *PCIe ID* tab, choose **Memory controller** as the *Base Class Menu* and **Other memory controller** as the *Sub Class Interface Menu*.

![Customize PCIe ID XDMA Settings](img/Customize_XDMA_IP_PCIe_ID.png)

Under the *Miscellaneous* tab, uncheck *Configuration Management Interface* to disable it.

![Customize Miscellaneous XDMA Settings](img/Customize_XDMA_IP_Misc.png)

Under the *DMA* tab, select **4** as the number of DMA Read and Write Channels. Click *OK* to update the design in the Block Diagram.

![Customize DMA XDMA Settings](img/Customize_XDMA_IP_DMA.png)

Click *Run Block Automation* in the Diagram editor.

![Run Block Automation](img/Run_Block_Automation.png)

Choose the same options as earlier for the **IP Level** design; **X8**, **8.0 GT/s**, **AXI Memory Mapped**, **4** Read and Write Channels.

![Block Automation Options](img/Run_Block_Automation_Options.png)

Block Automation will generate an appropriate buffer for the clock and add external signals.

![Block Automation Complete](img/Block_Automation_Complete.png)



## Add AXI

In the Diagram editor window, click **Add IP** and search for `axi smart`. Double-click **AXI SmartConnect**. [SmartConnect](https://docs.xilinx.com/r/en-US/pg247-smartconnect) automates clock and reset connections between AXI **S**lave and **M**aster devices and communication among them.

![Add AXI SmartConnect IP](img/Add_IP_AXI_SmartConnect.png)

This design will require 1 **S**lave interface, 4 **M**aster interfaces, 2 clock inputs, and 1 asynchronous reset.

![Customize AXI SmartConnect](img/Customize_SmartConnect.png)

Connect the XDMA and AXI SmartConnect blocks in the Diagram Editor. **M_AXI** to **S00_AXI** and **axi_clk** to **aclk**.

![Connect XDMA and AXI SmartConnect](img/Connect_XDMA_and_AXI_SmartConnect.png)



## Add DDR4

In the Diagram editor window, click **Add IP** and search for `ddr4`. Double-click **DDR4 SDRAM (MIG)**. This is the [DDR4 Memory Interface Generator](https://www.xilinx.com/content/dam/xilinx/support/documents/ip_documentation/ultrascale_memory_ip/v1_4/pg150-ultrascale-memory-ip.pdf).

![Add DDR4 IP](img/Add_IP_DDR4.png)

Under the *Basic* tab in DDR4 Customization, write **1428** as the *Memory Device Interface Speed (ps)*, choose **9996 (100.04MHz)** as the *Reference Input Clock Speed (ps)*, choose `MT40A1G16WBU-083E` as the *Memory Part*, choose **64** as the *Data Width*, and **DM NO DBI** as the *Data Mask and DBI*. `1428ps=700MHz=1400MT` is slow for DDR4 and a 64-bit width prevents ECC from being enabled. This is meant to be used for testing. The Innova-2 has [`MT40A1G16KNR-075`](https://www.micron.com/products/dram/ddr4-sdram/part-catalog/mt40a1g16knr-075) ICs with [D9WFR FBGA Code](https://www.micron.com/support/tools-and-utilities/fbga?fbga=D9WFR#pnlFBGA) that should be able to handle up to `750ps=1333MHz=2666MT`. Note parts rated to `-083` should be able to handle up to `833ps=1200MHz=2400MT`.

![Customize Basic DDR4 Settings](img/Customize_DDR4_Basic.png)

Under the *AXI Options* tab, choose *Round Robin* as the Arbitration Scheme. Leave all other options at their defaults. Click *OK* to continue.

![Customize AXI DDR4 Settings](img/Customize_DDR4_AXI_Options.png)

Click *Run Connection Automation* in the Diagram editor.

![Run Connection Automation](img/DDR4_Run_Connection_Automation.png)

Select all the ports for automatic connection.

![Select All for Automation](img/DDR4_Run_Connection_Automation_Customize.png)

This will generate a [*Processor System Reset*](https://docs.xilinx.com/v/u/en-US/pg164-proc-sys-reset) Block that controls the Active-Low Asynchronous Reset (*aresetn*) for the design. Connect **peripheral_aresetn[0:0]** to **c0_ddr4_aresetn** and AXI SmartConnect **aresetn**. The peripheral reset and DDR4 UI Clock will be used for all the AXI blocks.

![Processor System Reset Controls aresetn](img/Processor_System_Reset_Controls_aresetn.png)

Connect the DDR4 User Interface Clock (**c0_ddr4_ui_clk**) to AXI SmartConnect **aclk1**. This will be the slowest clock in the design at 100MHz.

![DDR4 UI CLK is SmartConnect ACLK1](img/DDR4_UI_CLK_is_SmartConnect_ACLK1.png)

Block automation generated *reset_rtl_0_0* for System Reset (*sys_rst*) for the DDR4 Block but the Innova-2 does not have such a signal.

![DDR4 sys rst](img/DDR4_sys_rst.png)

Delete the DDR4 System Reset (*sys_rst*) signal and port (*reset_rtl_0_0*). A DDR4 reset signal will be added next.

![Delete DDR4 sys rst](img/DDR4_sys_rst_Delete.png)

In the Diagram editor window, click **Add IP** and search for `vector`. Double-click **Utility Vector Logic** to add a basic [inverter gate](https://docs.xilinx.com/r/2021.2-English/ug994-vivado-ip-subsystems/Utility-Vector-Logic).

![Add Utility Vector Logic IP](img/Add_IP_Vector_Logic.png)

Choose a bit width (*C_SIZE*) of **1** and the *C_OPERATION* as **not**.

![Customize to 1-Input NOT Gate](img/Customize_Utility_Vector_to_NOT_Gate.png)

Invert the the PCIe Reset Signal (*reset_rtl_0*) and connect it to the DDR4 System Reset (*sys_rst*). PCIe Reset is Active-Low while DDR4 Reset is Active-High. Both blocks are thus enabled when the board is powered-on and PCIe activated.

![DDR4 sys_rst is inverted pcie_reset](img/DDR4_sys_rst_is_inverted_pcie_reset.png)



## Add Blinking XDMA Status LED

In the Diagram editor window, click **Add IP** and search for `counter`. Double-click **Binary Counter**. This [Binary Counter](https://docs.xilinx.com/v/u/en-US/pg121-c-counter-binary) will connect to the XDMA Clock and blink an LED about once per second.

![Add Counter IP](img/Add_IP_Counter.png)

Set the *Output Width* as **28**. XDMA uses a 250MHz AXI Interface clock so `250000000/(2^28)~=0.9313Hz`

![Customize Counter](img/Customize_Counter.png)

Connect the counter *CLK* input to XDMA *axi_aclk*.

![Connect Counter to XDMA ACLK](img/Connect_Counter_to_XDMA_ACLK.png)

Only the 28th bit will be needed for the Status LED. In the Diagram editor window, click **Add IP** and search for `slice`. Double-click **Slice**. The [Slice](https://docs.xilinx.com/r/2021.2-English/ug994-vivado-ip-subsystems/Slice) block selects specified bits from a wide bus.

![Add Slice IP](img/Add_IP_Slice.png)

Customize the Slice Block to output the last bit, **27**, from the 28-Bit Wide input.

![Customize Slice](img/Customize_Slice.png)

Connect the Counter Output (*Q[27:0]*) to Slice Input (*Din[27:0]*). Note the widths match.

![Connect Counter to Slice](img/Connect_Counter_to_Slice.png)

Right-click on the Slice *Dout[0:0]* output and choose **Make External**.

![Slice Dout Output Make External](img/Slice_Output_Make_External.png)

This will create an external port for the blinking XDMA Status LED.
 
![Slice Dout External Signal](img/Slice_Dout_Make_External.png)



## Add GPIO Input for Internal Status Signals

In the Diagram editor window, click **Add IP** and search for `gpio`. Double-click **AXI GPIO**. This [GPIO](https://docs.xilinx.com/v/u/en-US/pg144-axi-gpio) will be used to read status signals over AXI.

![Add GPIO IP](img/Add_IP_GPIO.png)

Customize the GPIO for *All Inputs* with a *GPIO Width* of **8**.

![Customize GPIO for Input](img/Customize_GPIO_Input.png)

Most status-related signals in the design are single wires while the GPIO Block expects a bus. In the Diagram editor window, click **Add IP** and search for `concat`. Double-click **Concat** to allow [concatenating](https://docs.xilinx.com/r/2021.2-English/ug994-vivado-ip-subsystems/Concat) individual signals into a bus.

![Add Concat IP](img/Add_IP_Concat.png)

Customize the Concat Block into eight 1-Bit wide inputs.

![Customize Concat](img/Customize_Concat.png)

Click the Plus Sign on the GPIO Block to expand the signal port.

![GPIO Port Pins Expand](img/GPIO_Pins_Expand.png)

Connect the Concat Output (*dout[7:0]*) to the GPIO Input (*gpio_io_i[7:0]*).

![Connect GPIO Input to Concat](img/Connect_GPIO_Input_to_Concat.png)

Connect the XDMA Link Up signal (*user_lnk_up*) to the Concat block. This is useless as XDMA must be working to access the GPIO but it is a simple sanity check.

![Concat user_lnk_up](img/Concat_user_lnk_up.png)

Connect the DDR4 Calibration Complete signal (*c0_init_calib_complete*) to the Concat block.

![Concat init_calib_complete](img/Concat_init_calib_complete.png)

Connect the GPIO Block (*S_AXI*) to the SmartConnect Block (*M01_AXI*). Use SmartConnect *aclk* for *axi_aclk* and SmartConnect *aresetn* for *s_axi_aresetn*.

![Connect GPIO Input to SmartConnect](img/Connect_GPIO_Input_to_SmartConnect.png)



## Add GPIO Output for a Controllable LED

In the Diagram editor window, click **Add IP** and search for `gpio`. Double-click **AXI GPIO**.

![Add GPIO IP](img/Add_IP_GPIO.png)

Customize the GPIO for *All Outputs* with a *GPIO Width* of **1**. Set the *Default Output Value* to `0x00000000`. The Innova-2 has inverted LED outputs which means the LED will be ON with a value of 0.

![Customize GPIO for Output](img/Customize_GPIO_Output.png)

Right-click on *GPIO* and choose *Make External*.

![Make GPIO Output External](img/GPIO_Make_External.png)

Vivado should generate a *GPIO_0* port.
 
![GPIO_0 External Signal](img/Make_GPIO_Output_External.png)

Connect the GPIO Block (*S_AXI*) to the SmartConnect Block (*M02_AXI*). Use SmartConnect *aclk1* for *axi_aclk* and SmartConnect *aresetn* for *axi_aresetn*. The picture below is wrong, use **aclk1**!.

![Connect GPIO Output to SmartConnect](img/Connect_GPIO_Output_to_SmartConnect.png)



## Add BRAM for Basic Memory Testing

In the Diagram editor window, click **Add IP** and search for `axi bram`. Double-click **AXI BRAM Controller**. [BRAM](https://docs.xilinx.com/v/u/en-US/pg078-axi-bram-ctrl) is FPGA Fabric Memory and is useful here as a functionality check.

![Add BRAM IP](img/Add_IP_AXI_BRAM.png)

Customize the BRAM Controller to a *Data Width* of **1024** and **1** *BRAM Interfaces*.

![Customize BRAM Controller](img/Customize_BRAM_Controller.png)

Connect the BRAM Controller Block (*S_AXI*) to the SmartConnect Block (*M03_AXI*). Use SmartConnect *aclk1* for *axi_aclk* and SmartConnect *aresetn* for *axi_aresetn*.

![Connect BRAM Controller to AXI SmartConnect](img/Connect_BRAM_Controller.png)

Run Connection Automation.

![Run Connection Automation](img/Run_Connection_Automation_Block_Memory.png)

It should generate a [Block Memory Generator](https://docs.xilinx.com/v/u/en-US/pg058-blk-mem-gen) Block.

![Block Memory Generator Added](img/Block_Memory_Generator.png)

The Block Design is now complete. Note all the AXI Blocks use the DDR4 User Interface Clock. At 100MHz it is slower than the 250MHz XDMA Clock. This makes it easier to meet timing constraints.

![AXI Peripherals use DDR4 Clock](img/Peripheral_Clock_is_DDR4_Clock.png)



## Define AXI Addresses

Open the [Address Editor](https://docs.xilinx.com/r/en-US/ug994-vivado-ip-subsystems/Using-the-Address-Editor) tab. Right-click and choose *Assign All*. These are the addresses at which the AXI Blocks will be accessed over XDMA. Make sure there are no overlaps.

![Address Editor](img/Address_Editor.png)



## Validate the Design

Back in the Diagram editor, click [*Validate Design*](https://docs.xilinx.com/r/en-US/ug995-vivado-ip-subsystems-tutorial/Step-8-Validating-the-Design).

![Validate Design](img/Validate_Design.png)

This should take about a minute and complete with no errors.

![Validating Design](img/Validating_Design.png)



## Add Signal Pin Constraints

Open the Sources tab and click *Add Sources*. If you already have a constraints file then add it here.

![Add Source Files](img/Add_Sources.png)

*Add or create constraints*

![Add or Create Constraints](img/Add_Constraints.png)

*Create File*

![Create File](img/Add_Constraints_Create_File.png)

Create an XDC (Xilinx Design Constraints) file.

![Create an XDC File](img/Constraints_Create_File.png)

Finish Constraints File Creation.

![Finish Constraints File Creation](img/Finish_Constraints_File_Creation.png)

Double-click the file to open the text editor.

![Open Constraints File for Editing](img/Open_Constraints_File_for_Editing.png)

Add Memory Configuration constraints for the bitstream file.

![Bitstream Memory Configuration Constraints](img/Memory_Configuration_Constraints.png)

Note all the external signal ports in the Block diagram. Left side has system inputs and signals that control your design. Right side has system outputs and bidirectional IO controlled by your design.

![Block Diagram External Signals](img/Block_Diagram_External_Signals.png)

Add PCIe constraints. Rename signals to match the Block design names if necessary. Different versions of Vivado use different default names.

![PCIe Constraints](img/PCIe_Constraints.png)

Add GPIO and LED constraints. The Innova-2 has two LEDs.

![LED Constraints Match Signal Names](img/GPIO_Constraints_Match_Signal_Names.png)

Add DDR4 constraints.

![DDR4 Constraints](img/DDR4_Constraints.png)

The Vivado GUI can also be used for [Pin Assignment](https://docs.xilinx.com/r/en-US/ug899-vivado-io-clock-planning/I/O-Logic-and-Low-Speed-I/O-Planning%E2%80%8C) and [Configuration Memory Setup](https://docs.xilinx.com/r/en-US/ug908-vivado-programming-debugging/Changing-Device-Image-Properties).


### Using the GUI to define Signal-to-Pin Constraints

*Open Implemented Design* in the Flow Navigator.

![Open Implemented Design](img/Open_Implemented_Design.png)

Open the *IO Ports* tab, expand the signal hierarchy and choose pins for the various signals.

![IO Ports Tab](img/IO_Ports.png)

When you are finished assigning pins to signals, *File->Constraints->Save*.

![Save Constraints](img/Save_Constraints.png)



## Create HDL Wrapper

In the Sources tab:

![Design Sources Block Diagram](img/Design_Sources_Block_Diagram.png)

Right-click on the Block Design file and select [*Create HDL Wrapper*](https://docs.xilinx.com/r/en-US/ug995-vivado-ip-subsystems-tutorial/Step-9-Creating-and-Implementing-the-Top-Level-Design):

![Create HDL Wrapper](img/Create_HDL_Wrapper.png)

Allow Vivado to create and manage the HDL Wrapper. This is the top-level HDL file associated with the design and must be included in every project.

![Vivado Managed HDL Wrapper](img/Vivado_Managed_HDL_Wrapper.png)

The HDL wrapper file will appear in the project's design hierarchy.

![HDL Wrapper in Hierarchy](img/HDL_Wrapper_in_Hierarchy.png)



## Synthesize and Implement

Click *Generate Bitstream* which will Synthesize and Implement the Block design.

![Generate Bitstream for Synthesis and Implementation](img/Generate_Bitstream_Synthesis_Implementation.png)

After the Bitstream is successfully generated, *Write Configuration Memory File*.

![Generate Configuration Memory File](img/Vivado_Tools_Generate_Memory_Configuration_File.png)

Select *bin*, *mt25qu512_x1_x2_x4_x8*, *SPIx8*, *Load bitstream files*, and a location and name for the output binary files. The bitstream will end up, for example, in the `DESIGN_NAME/DESIGN_NAME.runs/impl_1` subdirectory as `SOMETHING.bit`. Vivado will add the `_primary.bin` and `_secondary.bin` extensions as the Innova-2 uses dual MT25QU512 FLASH ICs in x8 for high speed programming.

![Write Configuration Memory File](img/Write_Configuration_Memory_File.png)

Select the `mt25qu512-spi-x1_x2_x4_x8` as the *Memory Part*.

![Select Configuration Memory Part](img/Select_Configuration_Memory_Part.png)



## Generate Project TCL Script

A Project TCL Script can be used to regenerate a project in Vivado and is useful for version control and distribution of projects.

After generating and testing a bitstream, [File->Project->Write TCL](https://docs.xilinx.com/r/en-US/ug892-vivado-design-flows-overview/Creating-a-Tcl-Script-to-Recreate-the-Project).

![Write Project TCL Script](img/Write_Project_to_TCL.png)


### Simplify Path for constraints.xdc in Project TCL

Open the Project TCL File for editing and search for `constraints.xdc`.

![Project TCL constraints.xdc ](img/Simplify_Path_for_Constraints_XDC_1.png)

Shorten the path to `$origin_dir/constraints.xdc` at the start of the file.

![Simplify Path for Constraints.xdc in Project TCL](img/Simplify_Path_for_Constraints_XDC_2.png)

Search for the next instance of `constraints.xdc`.

![Simplify Path for Constraints.xdc in Project TCL](img/Simplify_Path_for_Constraints_XDC_3.png)

Shorten the path appropriately near the middle of the file.

![Simplify Path for Constraints.xdc in Project TCL](img/Simplify_Path_for_Constraints_XDC_4.png)

Copy the `constraints.xdc` file to the same directory as the Project TCL file.

![Project Files](img/Project_Files.png)



## Confirm Project TCL Script Works

Test your Project's TCL Script by `source`ing it in the Vivado [Tcl Console](https://docs.xilinx.com/r/2021.1-English/ug893-vivado-ide/Using-the-Tcl-Console).

![Source a Vivado TCL Project Script](img/Vivado_Source_Project_TCL.png)




# Porting the Design to Another FPGA

Under *Tools->Settings*, change the **Project Device**.

![Change Project Device](img/Change_Project_Device.png)

The project's IP will now be out-of-date. Run *Report IP Status*.

![Status of Project IP](img/Report_IP_Status_to_Convert_to_New_Device.png)

Select all the IP check boxes and run *Upgrade Selected*.

![Upgrade IP](img/Show_IP_Status_then_Upgrade_Selected.png)

The IP should upgrade successfully if it is not too different an FPGA. Note that constraints will also need to be updated if the package has changed.

![IP Successfully Upgraded](img/IP_Successfully_Upgraded.png)

Rerun *IP Status* to confirm everything has upgraded.

![Rerun IP Status](img/Rerun_IP_Status.png)
