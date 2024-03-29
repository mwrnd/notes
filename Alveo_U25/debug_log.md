**Work-In-Progress**

**These notes have moved to [https://github.com/mwrnd/alveo_u25_notes](https://github.com/mwrnd/alveo_u25_notes/blob/main/debug_log.md).**

# Alveo U25 Debug Log

See also the [Vitis Debug Log](debug_log_vitis.md).

For testing, I have the U25 in a [PCIe x16 to x1 Riser Extender](https://www.google.com/search?q=PCIe+x16+to+x1+Riser+Extender) and use some [PCIe 8-Pin Power Extension Cables](https://www.google.com/search?q=pcie+8+pin+extension+cable) to work at a distance from the host computer. I am using a [WaveShare Platform Cable USB Clone](https://www.waveshare.com/platform-cable-usb.htm) for JTAG and a [74AVC4T774PW](https://www.trustedparts.com/en/search/74AVC4T774PW) Voltage Translator on a [TSSOP-16](https://www.ti.com/lit/pdf/mpds361a)-to-DIP adapter ([1](https://www.trustedparts.com/en/search/PA0193), [2](https://www.trustedparts.com/en/search/LCQT-TSSOP16)) to translate the U25's 1.8V UART to 3.3V for the [MCP2221A](https://www.trustedparts.com/en/part/adafruit/4471), which also handles the U25's 3.3V I2C communication.

![U25 Test Setup](img/U25_Test_Setup.jpg)




## Figuring out the JTAG Debug Connector

Initial work was done by *astronomy8* and *victhor393* on the [ServeTheHome Forums](https://forums.servethehome.com/index.php?threads/identifying-some-curious-xilinx-solarflare-cards.35111/). The official [Alveo Programming Cable and Adapter User Guide](https://www.xilinx.com/content/dam/xilinx/support/documents/boards_and_kits/accelerator-cards/ug1377-alveo-programming-cable-user-guide.pdf) is useless.

The [Alveo U25 Member Page](https://www.xilinx.com/member/u25.html) has terms which require you to not discuss the U25 so I am attempting to figure the board out.

I began by taking pictures and annotating Config, Reset, and Status pins with pinouts from the [ug1075 Zynq Ultrascale+ Packaging and Pinouts](https://docs.xilinx.com/v/u/en-US/ug1075-zynq-ultrascale-pkg-pinout) guide. Luckily the board was designed with exposed vias for testing.

![U25 Annotated Pins](img/U25_Underside_Annotated_Pins.jpg)

I then managed to trace the JTAG signals to the Alveo U25 Debug Connector through 22-ohm series resistors and a [SN74AVC4T774](https://www.ti.com/product/SN74AVC4T774)([ZVK Part Marking](https://www.ti.com/packaging/en/docs/partlookup.tsp?partmarking=ZVK)) voltage translating buffer IC.

![JTAG Connection Tracing](img/U25_JTAG_Connection_Tracing.jpg)

I traced **PS_PROG_B** and **PS_INIT_B** signals. Also, PS_ERROR_OUT(EO) connects through a [LSF0102](https://www.ti.com/product/LSF0102)([RV Part Marking](https://www.ti.com/packaging/en/docs/partlookup.tsp?partmarking=RV)) voltage translator to the [ATSAMD20J16](https://www.microchip.com/en-us/product/ATSAMD20J16)(Pin3) which acts as a Board Management Controller.

![Debug Connector PS_PROG_B and PS_INIT_B](img/U25_PS_PROG_B_and_PS_INIT_B_connect_to_Debug_Connector.jpg)

I created a simple DIY adapter for the U25 Debug Connector using a [MEC8](https://www.samtec.com/products/mec8-113-02-l-d-ra1)([TrustedParts](https://www.trustedparts.com/en/search/MEC8-113-02-L-D-RA1)) connector, [30AWG wire](https://www.digikey.com/en/products/detail/belden-inc/9978-013100/7389141), and a 0.1" Pin Header. See the [AlveoU25_JTAG_Adapter](https://github.com/mwrnd/AlveoU25_JTAG_Adapter) project for progress on the design of an Alveo Debug Connector to JTAG board.

![DIY JTAG Adapter](img/U25_DIY_JTAG_Adapter.jpg)




### Alveo U25 Debug Connector Pinout

Note that the JTAG pins run through a voltage translating buffer and operate at 3.3V while the `PS_POR_B`, `PS_INIT_B`, `RXD`, and `TXD` signals have direct connections to the Zynq IC and operate at 1.8V so they cannot be driven by the same circuit as the JTAG signals. They require voltage translation. The Operating Voltage is the voltage measured on the pad to GND when the board is powered on.

| Pad  | Signal         | Operating Voltage  |
| ---- |:--------------:|:------------------:|
| A1   | `GND`          |    0V              |
| A2   | `TCK`          |  3.3V              |
| A3   | `TDI`          |  3.3V              |
| A4   | `TMS`          |  3.3V              |
| A5   | `TDO`          |  3.3V              |
| A6   | `3.3V`         |  3.3V              |
| A7   | `GND`          |    0V              |
| A8   | `RXD=PS_MIO22` |  1.8V              |
| A9   | `TXD=PS_MIO23` |  1.8V              |
| A10  | `SDA`          |  3.3V              |
| A11  | `SCL`          |  3.3V              |
| A12  | `nALERT`       |  3.3V              |
| A13  | `1.8V`         |  1.8V              |
| B1   | `GND`          |    0V              |
| B2   | `PS_POR_B`     |  1.8V              |
| B3   | `PS_INIT_B`    |  0.1V              |
| B4   | `???`          |  2.2V              |
| B5   | `???`          |  0.6V              |
| B6   | `???`          |  0.6V              |
| B7   | `GND`          |    0V              |
| B8   | `SFP_PWR`      |  3.4V (0V-3.4V-0V) |
| B9   | `???`          |  3.3V              |
| B10  | `???`          |  1.2V              |
| B11  | `???`          |  2.5V              |
| B12  | `???`          |  1.2V              |
| B13  | `MGTAVCC`      |  0.9V              |




## Config, Reset, and Status Pin States

I measured the voltages on all the Config, Reset, and Status pins.

![Reset and Config Pins Measured](img/U25_Reset_and_Config_Pins_Measured.png)

For the LEDs, D1 (PWR GOOD) is lit Green, D2 is off, D3 (DONE) is off, D4 is lit Yellow, D5 has both Red+Green LEDs lit, D6 has both Red+Blue LEDs lit, and D7 is off.

![LED States](img/U25_LEDs_D4-green_D5-redgreen_D6-redblue.jpg)

Measured reset and config voltages when powered:

| Pad                   | Operating Voltage  |
| --------------------- |:------------------:|
|`PS_DONE` (DN)         | 0.0V               |
|`PS_ERROR_OUT` (EO)    | 1.8V               |
|`PS_ERROR_STATUS` (ES) | 0.0V               |
|`PS_INIT_B` (IN)       | 0.0V               |
|`PS_POR_B` (PR)        | 1.8V               |
|`PS_PROG_B` (PG)       | 1.8V               |
|`PS_REF_CLK` (RC)      | 0.85V              |
|`PS_SRST_B` (SR)       | 1.8V               |
|`POR_OVERRIDE` (13)    | 0.72V==VCCINT      |
|`PUDC_B` (15)          | 1.8V               |

`POR_OVERRIDE_B` and `PUDC_B` needed to be traced as well:

![POR_OVERRIDE_B and PUDC_B Annotated](img/U25_Annotated_Pins_POR_OVERRIDE_B_and_PUDC_B.jpg)




## Tracing I2C Signals

I was able to trace the Debug Connector Signals A10, A11, and A12 to SDA, SCL, and nALERT, respectively. They operate at 3.3V. The SCL line has a 4.7k-ohm resistor in series to the [TMP411](https://www.ti.com/product/TMP411) temperature sensor SCL pin.

![U25 Debug Connector I2C](img/U25_Debug_Connector_A10_A11_A12-I2C.jpg)




## Tracing Debug Connector B8

B8 measures 3.4V and connects to the `IN` pin of an [AP2182A](https://www.diodes.com/assets/Datasheets/AP2182A_92A.pdf) 1.5A dual channel current-limited power switch.

![B8 Connects to AP2182A IN](img/U25_Debug_B8_Connects_to_AP2182A_IN.jpg)

The AP2182A's outputs power the SFP Connectors.

![AP2182A U17 Powers SFP Connectors](img/AP2182A_U17_Powers_SFP_Connectors.jpg)

The 3.4V rail power cycles every so often.

![B8 Power Cycles](img/U25_Debug_B8_Power_Cycles.jpg)




## Tracing Debug Connector B13

B13 seems to connect to the `MGTAVCC` voltage rail which is used by the FPGA PL (Programmable Logic) SERDES.

![B13 Connects to MGTAVCC](img/U25_Debug_B13_Connects_to_MGTAVCC.jpg)




## Tracing A8 A9 Debug Connector UART Signals

The pins measure 1.8V. The [U25N Notes](https://xilinx.github.io/U25N-SmartNIC-Solution/docs/build/html/docs/ug1534-shellprogramming.html) mention a UART and [astronomy8 on the ServeTheHome Forum found A8 and A9 are being used](https://forums.servethehome.com/index.php?threads/identifying-some-curious-xilinx-solarflare-cards.35111/post-369295).

After [enabling JTAG](https://github.com/mwrnd/notes/tree/main/Alveo_U25#connect-jtag-adapter-and-allow-vivado-to-update-platform-cable-usb-ii-firmware) I was able to use UrJTAG to test pins A8 and A9. A8 connects to `PS_MIO22` (`AH32`) which is **RXD** and A9 connects to `PS_MIO23` (`AG35`) which is **TXD**.

I used a [74AVC4T774PW](https://www.ti.com/product/SN74AVC4T774) Voltage Translator on a [TSSOP-16](https://www.ti.com/lit/pdf/mpds361a)-to-DIP adapter ([1](https://www.trustedparts.com/en/search/PA0193), [2](https://www.trustedparts.com/en/search/LCQT-TSSOP16)).

![U25 UART Voltage Translator](img/U25_UART-Level-Shifter_I2C_MCP2221.jpg)

[74AVC4T774PW](https://www.ti.com/lit/ds/symlink/sn74avc4t774.pdf) connections:

![U25 UART Voltage Translator](img/U25_UART_MCP2221_Voltage_Translator.png)

UrJTAG Session:

![PS_MIO22 is RXD and PS_MIO23 is TXD](img/UrJTAG_PS_MIO22_RXD__PS_MIO23_TXD.jpg)

I was also able to use a [CH341A](https://github.com/stahir/CH341-Store/tree/5b4fda3add3d492f14f73a8376c580644f6c8195) USB-UART module set to TTL Mode (Pins 2-3 connected) and a 1.8V Adapter.

![Testing 1.8V UART](img/U25_Testing_UART-Debug_Connector_A8_A9.jpg)

A fixed-function 1.8V adapter such as one based on a [74ALVC164245](https://assets.nexperia.com/documents/data-sheet/74ALVC164245.pdf) is preferred over a bidrectional voltage translator such as the [TXB0108](https://www.ti.com/lit/ds/symlink/txb0108.pdf).

![1.8V Adapter](img/1.8V_Adapter.jpg)




## Tracing SAMD20 Board Controller Reset and Serial Wire Debug SWD

The [SAMD20J16](https://www.microchip.com/en-us/product/ATSAMD20J16)'s [Serial Wire Debug SWD](https://developer.arm.com/documentation/ihi0031/a/The-Serial-Wire-Debug-Port--SW-DP-/Introduction-to-the-ARM-Serial-Wire-Debug--SWD--protocol) pins connect to the B-Side of the Auxiliary Debug Connector.

| Pad  | Signal    | Operating Voltage  |
| ---- |:---------:|:------------------:|
| ...  |  `...`    |   ...              |
| B1   |  `???`    |  3.3V              |
| B2   |  `???`    |  3.3V              |
| B3   |  `SWDIO`  |  3.3V              |
| B4   |  `SWCLK`  |  3.3V              |
| ...  |  `...`    |   ...              |

![SAMD20 Reset and Serial Wire Debug SWD](img/U25_SAMD20_SWD_Signals.jpg)

The [SOT23-3](https://en.wikipedia.org/wiki/Small-outline_transistor#SOT23-3,_SOT323,_SOT416) transistors marked `W4T` are [BSH111BK](https://assets.nexperia.com/documents/data-sheet/BSH111BK.pdf) N-Channel MOSFETs. `?4T` is the [BSH111BK](https://assets.nexperia.com/documents/data-sheet/BSH111BK.pdf) marking. From the [PBHV8540T](https://web.archive.org/web/20120512162654/http://www.nxp.com/documents/data_sheet/PBHV8540T.pdf) datasheet, `W` is a site code.

![SOT23-3 with W4T Marking is BSH111BK](img/Transistor_MOSFET_NCH_BSH111BK_SOT23-3_W4T-Marking.png)

The markings match a recently purchased BSH111BKR.

![BSH111BK has W4T Marking](img/BSH111BKR_W4T-Marking.jpg)

I was unable to trace the Reset line to any external connector.

![SAMD20 Reset](img/U25_SAMD20_Reset.png)




## Attempt to Trace QSPI Signals

I tried tracing the QSPI signals to determine if they can be accessed on the PCB but unfortunately they seem to be routed using buried vias.

![Alveo U25 QSPI Signals NOT Exposed on PCB](img/U25_QSPI_Pin_Annotation_Attempt.jpg)




## JTAG Access to the Zynq APU


### Attempt to Program Configuration Memory Device

This programming attempt will put the Zynq into a state that will allow further JTAG exploration. The QSPI cannot be programmed over JTAG.

Start Vivado Hardware Manager. Any recent version of [Vivado Lab Edition](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2021-2.html) or full Vivado is enough for this task.

Start a Connection, `Open Target -> Auto Connect`

![Hardware Manager Auto Connect](img/U25_Configuration_Memory_Step0-Hardware_Manager_AutoConnect.png)

A *"xczu19_0 PL Power Status OFF, cannot connect PL TAP. Check POR_B signal"* error shows up when you connect via JTAG.

![POR_B Error](img/U25_Hardware_Manager_POR_B_Error.png)

Right-click on `xczu19_0` and select `Add Configuration Memory Device ...`

![Add Configuration Memory Device](img/U25_Configuration_Memory_Step1-Add_Device.png)

Select `mt25qu01g-qspi-x4-single`.

![Select mt25qu01g-qspi-x4-single](img/U25_Configuration_Memory_Step2-Choose_Device.png)

Right click on the `mt25qu01g-qspi-x4-single` and then `Program Configuration Memory Device ...`

![Program Complete Memory Image](img/U25_Configuration_Memory_Step3-Program.png)

Choose any valid U25-related `.bin` and `.elf` files.

![Select Files](img/U25_Configuration_Memory_Step4-Select_Files.png)

Programming fails but it puts the Zynq into a useful state.

![Programming Fails](img/U25_Configuration_Memory_Step5-Programming_Fails.png)

Close Target to disconnect Vivado Hardware Manager from the JTAG Adapter.

![Vivado Hardware Manager Close Target](img/Vivado_Hardware_Manager_Close_Target.png)



### xsdb JTAG Access to Zynq

The above attempt to program the Configuration Memory Device should have left the Zynq in a state that will now allow `xsdb` access.

Run Xilinx's `xsdb` which comes with Vivado:
```
source /tools/Xilinx/Vivado/2021.2/settings64.sh
xsdb
```

In `xsdb` run `connect`, then `targets`. If things went well, the **APU** should be in the **Power On Reset** state and the first Cortex-A53 should be in the **Reset Catch, EL3(S)/A64** state.

Select the first Cortex-A53 core for communication with `target 9`. `rrd pc` reads the Program Counter Register. Registers can be read all at once with `rrd`. The command `mrd 0xFFD80528 1` reads the `PMU_GLOBAL.CSU_BR_ERR` register which contains the BootROM Error Codes.

![xsdb Zynq Communication](img/U25_JTAG_xsdb_Zynq_Communication.png)

The value `80002424` pictured means the current and previous [Error Code](https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/CSU-BootROM-Error-Codes) was `0x24` which indicates a fault in the QSPI Configuration Memory device.

![BootROM Error Codes](img/Zynq_BootROM_Error_Codes.png)

PMU Register addresses are defined in [`/tools/Xilinx/Vitis/2021.2/data/embeddedsw/lib/sw_apps/zynqmp_pmufw/src/pmu_global.h`](https://github.com/Xilinx/embeddedsw/blob/836d748f91eda280bac23d0200df8a3bef61c0c3/lib/sw_apps/zynqmp_pmufw/src/pmu_global.h#L2306).
```
#define PMU_GLOBAL_BASEADDR     ((u32)0xFFD80000U)
#define PMU_GLOBAL_REQ_PWRUP_STATUS ((PMU_GLOBAL_BASEADDR)+((u32)0X00000110U))
#define PMU_GLOBAL_ERROR_POR_MASK_1 ((PMU_GLOBAL_BASEADDR)+((u32)0X00000550U))
#define PMU_GLOBAL_CSU_BR_ERROR     ((PMU_GLOBAL_BASEADDR)+((u32)0X00000528U))
```

*Table 11-9* in the [ug1085 Zynq Technical Reference Manual](https://www.xilinx.com/content/dam/xilinx/support/documents/user_guides/ug1085-zynq-ultrascale-trm.pdf) explains the [BootROM Error Codes](https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/CSU-BootROM-Error-Codes) for the `PMU_GLOBAL.CSU_BR_ERR` register.

![CSU BootROM Error Codes](img/Zynq_CSU_BootROM_Error_Codes.png)




### Zynq Boot Halts on Tamper Detection

During a debug attempt, `mrd 0xFFD80528 1` (the [`PMU_GLOBAL.CSU_BR_ERR` register](https://github.com/Xilinx/embeddedsw/blob/836d748f91eda280bac23d0200df8a3bef61c0c3/lib/sw_apps/zynqmp_pmufw/src/pmu_global.h#L2306)) had BootROM [Error Code](https://docs.xilinx.com/r/en-US/ug1085-zynq-ultrascale-trm/CSU-BootROM-Error-Codes) `0x92` as the previous error.

![CSU Tamper Detect](img/xsdb_CSU_Tamper_Detect.png)

Further exploration did not yield any additional information as the various tamper registers only store the current BootROM Error Code.
```
mrd 0xFFD80528 1
mrd 0xFFCA0018 1
mrd 0xFFCA5000 1
mrd 0xFFCA500C 1
```

![Previous BootROM Error was 0x92](img/xsdb_Previous_BootROM_Error_was_0x92.png)




### Zynq Boot Halts on Tamper Detection when U25 Powered with JTAG Connected

The U25 has a Tamper Detected BootROM Error Code if it is powered while JTAG is connected. The simplest solution is to power the JTAG adapter _AFTER_ powering up the U25.

![Tamper Detected when Powered with JTAG Connected](img/Tamper_Status_if_U25_Powered_when_JTAG_Connected.png)




## Tracing PCIe Connections

The Alveo U25 has an x16 PCI Express connection that needs to be bifurcated into two x8 connections to allow both the [Solarflare SFC9250 X2 GbE Controller](https://www.xilinx.com/products/boards-and-kits/x2-series.html) and Zynq FPGA SoC to communicate with the host.

The PCIe Clock and Reset signals are each buffered into two seperate signals.

![PCIe Clock and Reset](img/U25_PCIe_PERST_CLK.jpg)

The PCIe clock is buffered into two signal pairs using an IC marked `AOZ LG`, U18.

![PCIe Clock](img/U25_PCIe_CLOCK.jpg)

The Reset signal is buffered into two 1.8V traces using an IC marked `A5AA`, U27.

![PCIe U27 PERSTn Closeup](img/U25_PCIe_U27_PERSTn_Closeup.jpg)

The clock signal pair can further be traced to pins AK11 and AK12. The 9th PCIe Lane, RX08, can be traced to AM7 and AM8, which are a transmit pair.

![PCIe Lane 8](img/U25_PCIe_9thRX_Lane08.jpg)

In Vivado, the following constraints can be used when setting up PCIe:

![U25 PCIe x1 Constraints](img/U25_PCIe_x1_Pin_Constraints.png)

The closest `PCIE4` block to Banks 224 and 225 is X1Y0.

![Quads 224 225 Closest to X1Y0 PCIE4](img/U25_Quads_224_225_Closest_to_X1Y0_PCIE4.png)




### Using UrJTAG to find PCIe Reset Signal Pin

Plug the Alveo U25 into a PCIe Extension Riser board but only connect power, no signal header.

Enable JTAG using the [JTAG Access to the Zynq APU](#jtag-access-to-the-zynq-apu) process above. Refer to the [JTAG Using UrJTAG](https://github.com/mwrnd/notes/tree/main/Alveo_U25#jtag-using-urjtag) notes for UrJTAG setup. Run `sudo jtag` in a terminal then the following commands at the `jtag>` prompt. Note `cable xpc_ext` is for a [Xilinx Platform Cable USB II](https://docs.xilinx.com/v/u/en-US/ds593) adapter or [clone](https://www.waveshare.com/platform-cable-usb.htm). The [`getallio`](getallio) script reads the status of all the `IO_` pins. A similar script is also included for `PS_MIO_` pins, [`getallpsmio`](getallpsmio).

```
cable xpc_ext
detect
print chain
part 1
instruction EXTEST
shift ir
shift dr
dr
script /home/user/getallio
```

Copy the output into a file.

Connect the PCIe PERSTn signal, the resistor below U27, to 3.3V.

![Connect PERSTn To 3.3V](img/Connect_PERSTn_To_3.3V_Before_Running_get-all-io.jpg)

Run the [`getallio`](getallio) script again and copy the results to a new file.
```
script /home/user/getallio
```

![Run script getallio](img/UrJTAG_run_script-get-all-io.png)

Do this several times, waiting different amounts between the second call to `getallio`. Pin `IO_AM25` will be the only signal that consistently changes. It must be the PCIe Reset signal.

![getallio Output Diffs](img/U25_UrJTAG_script-get-all-io_output_diffs.png)




## Save a Copy of the IS25WP128 SFC9250 Firmware

If you have access to a [`flashrom`](https://www.flashrom.org/Flashrom) compatible SPI Programmer (such as a [CH341A Programmer](https://github.com/stahir/CH341-Store/tree/5b4fda3add3d492f14f73a8376c580644f6c8195)) with a 1.8V Adapter, consider saving the [IS25WP128-JBLE](https://www.issi.com/WW/pdf/IS25WP032-064-128.pdf) 1.8V FLASH IC contents.  **This is a dangerous procedure that can damage your U25**. Please do not make this your first attempt at FLASH IC programming. Consider a practice run on some other less important device or [purchase a IS25WP128](https://www.trustedparts.com/en/search/IS25WP128-JBLE) IC to test with. The U25 should **not** be powered when reading the IS25WP128.
 
![IS25WP128 flashrom read Using CH341A with 1.8V Adapter](img/flashrom_Read_and_Erase_Using_CH341A_with_1.8V_Adapter.jpg)

```
sudo flashrom -p ch341a_spi -r u25_25f128_read1.bin
sudo flashrom -p ch341a_spi -r u25_25f128_read2.bin
sha256sum u25_25f128_read*
```

![IS25WP128 FLASH Read then Erase](img/U25_IS25WP128_FLASH_Read_then_Erase.jpg)




## Add Jumper to Mode1 Pin to Allow JTAG Booting

The Mode1 Pin (AC28) is connected via a 0-ohm resistor to 1.8V to set QSPI as the Boot Mode.

![Mode1 Pin](img/Alveo_U25_Mode1_Pin_AC28.jpg)

Prepare a [4-Pin 2-mm header](https://www.digikey.com/en/products/detail/adam-tech/2PH1-04-UA/9830490) by bending two pins on one end to right-angles. Also, prepare a single conductor of [0.050" 7-strand ribbon cable](https://www.digikey.com/en/products/detail/3m/3365-06-300/9478336) by exposing the strands and trimming all but one. The idea is to have a very delicate connection to the Mode1 pad so that if it gets pulled out it will not damage the PCB traces.

![Mode1 Jumper Mod Parts](img/U25_Mode1_Jumper_Mod-Parts.jpg)

This is a very delicate modification but I managed to replace the 0201-size 0-ohm resistor with a 0402-size 10k-ohm resistor and then solder the single conductor of 0.050" 7-strand ribbon cable wire to the Mode1 side of the resistor.

![Mode1 Jumper Mod](img/U25_Mode1_Jumper_Mod-Mode1_Connection.jpg)

I used the J15 (NO_FLASH) jumper footprint to mount the 4-pin 2mm header. One of the J15 pads has a 10k-ohm resistor to GND. I soldered one of the header legs to the GND pad of the resistor. The other leg I soldered the Mode1 wire.

![Mode1 Jumper Mod Header](img/U25_Mode1_Jumper_Mod-Jumper.jpg)

I can now short the Mode1 pin to GND using a [2mm jumper](https://www.digikey.com/en/products/detail/sullins-connector-solutions/SPN02SYBN-RC/927356) to boot the U25 in JTAG Mode which allows programming the QSPI Flash memory.

![Mode1 Jumper Mod JTAG Active](img/U25_Mode1_Jumper_Mod-JTAG_Active.jpg)

Without the jumper, QSPI boot is active.

![Mode1 Jumper Mod QSPI Active](img/U25_Mode1_Jumper_Mod-QSPI_Boot_and_Debug.jpg)




## Debug Connector I2C Scanning

I used an [Adafruit MCP2221A](https://www.trustedparts.com/en/part/adafruit/4471) breakout board and [i2c-tools](https://manpages.debian.org/unstable/i2c-tools/i2cdetect.8.en.html).

```
sudo apt-get install i2c-tools
```

![Debug Connector I2C Using MCP2221](img/U25_Debug_Connector_I2C_Using_MCP2221.jpg)

On my system the MCP2221 showed up as I2C bus `5`. Use [`i2cdetect`](https://manpages.debian.org/unstable/i2c-tools/i2cdetect.8.en.html) to scan all 7-bit I2C addresses. `0x4D` and `0x4E` are the [TMP411](https://www.ti.com/product/TMP411) temperature sensors [found earlier](#tracing-i2c-signals).
```
lsusb | grep Microchip
sudo i2cdetect -l | grep MCP2221
sudo i2cdetect -r 5 0x03 0x77
```

![I2C Scan](img/U25_Debug_Connector_I2C_Scan.png)




### I2C Bus Device Register Dumps

Use `i2cdump` on all the devices that were found.

![I2C Dump of 0x0C](img/U25_Debug_Connector_I2C_Dump_0x0C.png)

![I2C Dump of 0x30](img/U25_Debug_Connector_I2C_Dump_0x30.png)

![I2C Dump of 0x31](img/U25_Debug_Connector_I2C_Dump_0x31.png)

![I2C Dump of 0x32](img/U25_Debug_Connector_I2C_Dump_0x32.png)

![I2C Dump of 0x49](img/U25_Debug_Connector_I2C_Dump_0x49.png)

![I2C Dump of 0x4D](img/U25_Debug_Connector_I2C_Dump_0x4D_TMP411.png)

![I2C Dump of 0x4E](img/U25_Debug_Connector_I2C_Dump_0x4E_TMP411.png)




## SAMD20 Board Controller D4 LED

The D4 LED connects VDDIO to PB14, `VDDIO->330ohm->D4LED->PB14`.

![PB14 Sinks D4](img/SAMD20_Pin27_PB14_Connects_to_D4.jpg)

SAMD20 IO Pins are Tri-State (High-Impedance) after Power-On Reset so if the LED is on, some firmware must have run.

![SAMD20 IO Pins are Tri-State After Reset](img/SAMD20_IO_Pins_after_POR_Reset.png)

Tri-state has lowest current leakage.

![Tri-State Has Lowest Current Leakage](img/SAMD20_Unused_Pins.png)




## Programmed partition.bin and FSBL

I ran the following commands in the [Vitis XSCT Console](https://docs.xilinx.com/r/en-US/ug1400-vitis-embedded/XSCT-Commands) to enable the Zynq for programming and Debug by getting it out of the `L2 Cache Reset` state.
```
connect
after 10000
targets
targets -set -nocase -filter {name =~ "*PSU*"}
mwr 0xff5e0200 0x0100
rst -system
after 1000
targets -set -nocase -filter {name =~ "*APU*"}
mwr 0xffff0000 0x14000000
mask_write 0xFD1A0104 0x501 0x0
targets -set -nocase -filter {name =~ "*A53*#0"}
stop
targets
```

![xsct Console](img/U25_Vitis_Step_08-Run_Commands_in_xsct_Console.png)

I then started Vivado Hardware Manager and added a Configuration Memory Device:

![Hardware Manager Add Configuration Memory Device](img/Hardware_Manager_Add_Memory_Device_to_xczu19.png)

I chose `mt25qu01g-qspi-x4-single` as the memory device.

![Select mt25qu01g-qspi-x4-single](img/Select_mt25qu01g-qspi-x4-single.png)

I then programmed the Configuration Memory Device using an FSBL (First-Stage Boot Loader) from a recent project and `partition.bin` generated by an install of the [U25 FPGA Utility](https://support.lenovo.com/us/en/downloads/ds547996-xilinx-u25-fpga-utility-for-unknown-os)([2.8.372](https://download.lenovo.com/servers/mig/2020/12/23/43128/xlnx-lnvgy_utl_nic_u25-2.8.372_linux_x86-64.tgz)). Note that the U25 is a [Two Stage Platform](https://xilinx.github.io/XRT/master/html/platforms_partitions.html#two-stage-platforms) and [requires `BOOT.bin` and `fsbl_flash_prog.elf`](https://xilinx.github.io/U25N-SmartNIC-Solution/docs/build/html/docs/ug1534-shellprogramming.html) from the [Official U25 Member Page](https://www.xilinx.com/member/u25.html).

![Program Configuration Memory Device](img/Program_Configuration_Memory_Device.png)

Programming took about 25 minutes and seemed to work but failed at the last moment.

![Hardware Manager Programmed but Boot Fails](img/Hardware_Manager_Programmed_but_Boot_Fails.png)

However, something **was** programmed! An old version of the FSBL, from `partition.bin`, is running.

![Old FSBL Running](img/Boot_Failing_but_FSBL_Running_from_QSPI.png)

The Zynq FSBL I used during programming has a recent compile date and version, not `2019.2`.

![Zynq FSBL is from May7](img/Zynq_FSBL_Used_is_from_May7_2023.png)




## Forcing sfc Driver Attach Crashes Computer

It is possible to force a device to bind to a driver. I found the U25's PCI bus address using `lspci -D` and then forced it to bind to the [`sfc` driver](https://www.xilinx.com/support/download/nic-software-and-drivers.html).

```
lspci -D
sudo su
modprobe sfc
lsmod | grep sfc
echo 0000:01:00.0 > /sys/bus/pci/drivers/sfc/bind
```

The result is that the system crashes. After reboot, the following:

```
[name@localhost ~]$ sudo su
[sudo] password for name: 
ABRT has detected 1 problem(s). For more info run: abrt-cli list --since 1688149458

[root@localhost name]# abrt-cli list --since 1688149458
id 1bc2b3f1596ce1d056a6f8d47e90440d00c0ddd4
reason:         BUG: unable to handle kernel NULL pointer dereference at 0000000000000520
time:           Fri 30 Jun 2023 02:28:22 PM EDT
uid:            0 (root)
count:          1
Directory:      /var/spool/abrt/vmcore-127.0.0.1-2023-06-30-14:27:29
Reported:       cannot be reported

The Autoreporting feature is disabled. Please consider enabling it by issuing
'abrt-auto-reporting enabled' as a user with root privileges
```




## lspci Output Does Not Change Regardless of Firmware Attempt

I reprogrammed the [IS25WP128-JBLE](https://www.issi.com/WW/pdf/IS25WP032-064-128.pdf) 1.8V FLASH IC with the partial firmware files from [U25 X2 NIC Firmware](https://support.lenovo.com/us/en/downloads/ds548057-xilinx-u25-x2-nic-firmware-for-unknown-os)([8.1.3.1011-1](https://download.lenovo.com/servers/mig/2021/01/06/43174/xlnx-lnvgy_fw_nic_u25-8.1.3.1011-1_linux_x86-64.tgz)) and there was no change in `lspci -D -vnn -d 1924:` output. Same as when I erased the FLASH IC completely. `[1924:0b13]` is the default built-in X2 firmware.

```
0000:01:00.0 Ethernet controller [0200]: Solarflare Communications Device [1924:0b13] (rev 01)
	Subsystem: Solarflare Communications Device [1924:0001]
	Flags: fast devsel, IRQ 255
	Memory at 8d000000 (64-bit, non-prefetchable) [disabled] [size=1M]
	Memory at 8d100000 (64-bit, non-prefetchable) [disabled] [size=16K]
	Capabilities: [40] Power Management version 3
	Capabilities: [50] MSI: Enable- Count=1/1 Maskable- 64bit+
	Capabilities: [70] Express Endpoint, MSI 00
	Capabilities: [b0] MSI-X: Enable- Count=64 Masked-
	Capabilities: [d0] Vital Product Data
	Capabilities: [100] Advanced Error Reporting
	Capabilities: [148] Device Serial Number 00-0f-53-ff-ff-00-00-00
	Capabilities: [158] Power Budgeting <?>
	Capabilities: [168] Alternative Routing-ID Interpretation (ARI)
	Capabilities: [178] #19
	Capabilities: [1a8] Single Root I/O Virtualization (SR-IOV)
	Capabilities: [1e8] Transaction Processing Hints
	Capabilities: [274] Access Control Services
	Capabilities: [27c] Latency Tolerance Reporting
	Capabilities: [284] L1 PM Substates
	Capabilities: [294] #16
	Capabilities: [2c4] #22
	Capabilities: [2d0] Vendor Specific Information: ID=0001 Rev=1 Len=038 <?>
	Capabilities: [308] Precision Time Measurement
	Capabilities: [314] Vendor Specific Information: ID=0003 Rev=1 Len=054 <?>
```




## QSPI Boot Image Search

![Zynq Golden Image Search](img/Zynq_Golden_Image_Search.png)




## Additional Board Notes

PCIe JTAG signals are connected to somewhere on the board through a [SN74AVC4T774](https://www.ti.com/product/SN74AVC4T774)([ZVK Part Marking](https://www.ti.com/packaging/en/docs/partlookup.tsp?partmarking=ZVK)) voltage translating buffer IC.

![PCIe JTAG Pins are Used on the Board](img/U25_PCIe_JTAG_is_Connected.jpg)

The Zynq JTAG pins have two buffer ICs connected to them. There are two possible JTAG sources for the Zynq.

![Multiple JTAG Sources](img/U25_Programming_Header_has_two_JTAG_Sources.jpg)




## Software and System Setup

**TODO**

[U25 X2 NIC Driver](https://support.lenovo.com/us/en/downloads/ds547997-xilinx-u25-x2-nic-driver-for-unknown-os) ([4.15.10.1003](https://download.lenovo.com/servers/mig/2020/12/23/43129/xlnx-lnvgy_dd_nic_u25-4.15.10.1003_linux_x86-64.tgz))

[U25 X2 NIC Firmware](https://support.lenovo.com/us/en/downloads/ds548057-xilinx-u25-x2-nic-firmware-for-unknown-os) ([8.1.3.1011-1](https://download.lenovo.com/servers/mig/2021/01/06/43174/xlnx-lnvgy_fw_nic_u25-8.1.3.1011-1_linux_x86-64.tgz))

[U25 FPGA Utility](https://support.lenovo.com/us/en/downloads/ds547996-xilinx-u25-fpga-utility-for-unknown-os) ([2.8.372](https://download.lenovo.com/servers/mig/2020/12/23/43128/xlnx-lnvgy_utl_nic_u25-2.8.372_linux_x86-64.tgz))




## Setting Boot Mode

[Set JTAG Boot](https://xilinx.github.io/kria-apps-docs/creating_applications/2022.1/build/html/docs/bootmodes.html#setting-bootmodes) regardless of the `PS_MODE` pins. [ug1087 describes `BOOT_MODE_USER`](https://docs.xilinx.com/r/en-US/ug1087-zynq-ultrascale-registers/BOOT_MODE_USER-CRL_APB-Register) which is defined in [`/tools/Xilinx/Vivado/2021.2/data/embeddedsw/lib/sw_apps/zynqmp_pmufw/src/crl_apb.h`](https://github.com/Xilinx/embeddedsw/blob/836d748f91eda280bac23d0200df8a3bef61c0c3/lib/sw_apps/zynqmp_pmufw/src/crl_apb.h#L1988).

```
targets -set -nocase -filter {name =~ "*PSU*"}
stop
mwr 0xFFCA0010 0x0
mwr 0xFF5E0200 0x0100
rst -system
```




### Test with Vitis-AI

**TODO**

```
git clone --branch 1.3.2 --recursive --single-branch https://github.com/Xilinx/Vitis-AI.git
cd Vitis-AI
git checkout 1.3.2
git describe --tags

docker pull xilinx/vitis-ai-cpu:1.3.598
./docker_run.sh xilinx/vitis-ai-cpu:1.3.598
```






