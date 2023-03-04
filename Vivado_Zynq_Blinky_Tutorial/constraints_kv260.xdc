
# Xilinx Kria KV260 (XCK26 == XCK26-SFVC784-2LV-C == XCZU5EV-SFVC784-2LV-C)
# References: Kria_K26_SOM_Rev1.xdc and Kria_KV_CarrierCard_Rev1.xdc



# PMOD pins are in Bank 45 and use VCCO from SOM240_1_B13
# PMOD Pin == Schematic Label == SOM Pin == FPGA IO == Package Pin

# J2 PMOD Pin1 == HDA11 == SOM240_1_A17 == IO_L4N_AD12N_45 == XCK26 Pin H12
set_property PACKAGE_PIN  H12       [get_ports {Dout_0[0]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[0]}]
set_property DRIVE        4         [get_ports {Dout_0[0]}]
set_property SLEW         SLOW      [get_ports {Dout_0[0]}]

# J2 PMOD Pin3 == HDA12 == SOM240_1_D20 == IO_L7P_HDGC_45 == XCK26 Pin E10
set_property PACKAGE_PIN  E10       [get_ports {Dout_0[1]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[1]}]
set_property DRIVE        4         [get_ports {Dout_0[1]}]
set_property SLEW         SLOW      [get_ports {Dout_0[1]}]

# J2 PMOD Pin5 == HDA13 == SOM240_1_D21 == IO_L7N_HDGC_45 == XCK26 Pin D10
set_property PACKAGE_PIN  D10       [get_ports {Dout_0[2]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[2]}]
set_property DRIVE        4         [get_ports {Dout_0[2]}]
set_property SLEW         SLOW      [get_ports {Dout_0[2]}]

# J2 PMOD Pin7 == HDA14 == SOM240_1_D22 == IO_L9P_AD11P_45 == XCK26 Pin C11
set_property PACKAGE_PIN  C11       [get_ports {Dout_0[3]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[3]}]
set_property DRIVE        4         [get_ports {Dout_0[3]}]
set_property SLEW         SLOW      [get_ports {Dout_0[3]}]

# J2 PMOD Pin2 == HDA15 == SOM240_1_B20 == IO_L9N_AD11N_45 == XCK26 Pin B10
set_property PACKAGE_PIN  B10       [get_ports {Dout_0[4]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[4]}]
set_property DRIVE        4         [get_ports {Dout_0[4]}]
set_property SLEW         SLOW      [get_ports {Dout_0[4]}]

# J2 PMOD Pin4 == HDA16_CC == SOM240_1_B21 == IO_L8P_HDGC_45 == XCK26 Pin E12
set_property PACKAGE_PIN  E12       [get_ports {Dout_0[5]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[5]}]
set_property DRIVE        4         [get_ports {Dout_0[5]}]
set_property SLEW         SLOW      [get_ports {Dout_0[5]}]

# J2 PMOD Pin6 == HDA17 == SOM240_1_B22 == IO_L8N_HDGC_45 == XCK26 Pin D11
set_property PACKAGE_PIN  D11       [get_ports {Dout_0[6]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[6]}]
set_property DRIVE        4         [get_ports {Dout_0[6]}]
set_property SLEW         SLOW      [get_ports {Dout_0[6]}]

# J2 PMOD Pin8 == HDA18 == SOM240_1_C22 == IO_L10P_AD10P_45 == XCK26 Pin B11
set_property PACKAGE_PIN  B11       [get_ports {Dout_0[7]}]
set_property IOSTANDARD   LVCMOS33  [get_ports {Dout_0[7]}]
set_property DRIVE        4         [get_ports {Dout_0[7]}]
set_property SLEW         SLOW      [get_ports {Dout_0[7]}]

