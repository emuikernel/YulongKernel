  /* Register map header file for S2202, Family 0x00, Revision 0x00 */

  /* Automatically generated on 2012-Feb-27  23:37:19, do not edit */

#ifndef SYNA_REGISTER_MAP_H
  #define SYNA_REGISTER_MAP_H 1

  /*      Register Name                                      Address     Register Description */
  /*      -------------                                      -------     -------------------- */
  #define SYNA_F34_FLASH_DATA00                              0x0000   /* Block Number Low */
  #define SYNA_F34_FLASH_DATA01                              0x0001   /* Block Number High */
  #define SYNA_F34_FLASH_DATA02_00                           0x0002   /* Block Data 0 */
  #define SYNA_F34_FLASH_DATA02_01                           0x0003   /* Block Data 1 */
  #define SYNA_F34_FLASH_DATA02_02                           0x0004   /* Block Data 2 */
  #define SYNA_F34_FLASH_DATA02_03                           0x0005   /* Block Data 3 */
  #define SYNA_F34_FLASH_DATA02_04                           0x0006   /* Block Data 4 */
  #define SYNA_F34_FLASH_DATA02_05                           0x0007   /* Block Data 5 */
  #define SYNA_F34_FLASH_DATA02_06                           0x0008   /* Block Data 6 */
  #define SYNA_F34_FLASH_DATA02_07                           0x0009   /* Block Data 7 */
  #define SYNA_F34_FLASH_DATA02_08                           0x000A   /* Block Data 8 */
  #define SYNA_F34_FLASH_DATA02_09                           0x000B   /* Block Data 9 */
  #define SYNA_F34_FLASH_DATA02_10                           0x000C   /* Block Data 10 */
  #define SYNA_F34_FLASH_DATA02_11                           0x000D   /* Block Data 11 */
  #define SYNA_F34_FLASH_DATA02_12                           0x000E   /* Block Data 12 */
  #define SYNA_F34_FLASH_DATA02_13                           0x000F   /* Block Data 13 */
  #define SYNA_F34_FLASH_DATA02_14                           0x0010   /* Block Data 14 */
  #define SYNA_F34_FLASH_DATA02_15                           0x0011   /* Block Data 15 */
  #define SYNA_F34_FLASH_DATA03                              0x0012   /* Flash Control */
  #define SYNA_F01_RMI_DATA00                                0x0013   /* Device Status */
  #define SYNA_F01_RMI_DATA01_00                             0x0014   /* Interrupt Status */
  #define SYNA_F11_2D_DATA00_00                              0x0015   /* 2D Finger State */
  #define SYNA_F11_2D_DATA00_01                              0x0016   /* 2D Finger State */
  #define SYNA_F11_2D_DATA01_00                              0x0017   /* 2D X Position (11:4) Finger 0 */
  #define SYNA_F11_2D_DATA02_00                              0x0018   /* 2D Y Position (11:4) Finger 0 */
  #define SYNA_F11_2D_DATA03_00                              0x0019   /* 2D Y/X Position (3:0) Finger 0 */
  #define SYNA_F11_2D_DATA04_00                              0x001A   /* 2D Wy/Wx Finger 0 */
  #define SYNA_F11_2D_DATA05_00                              0x001B   /* 2D Z Finger 0 */
  #define SYNA_F11_2D_DATA01_01                              0x001C   /* 2D X Position (11:4) Finger 1 */
  #define SYNA_F11_2D_DATA02_01                              0x001D   /* 2D Y Position (11:4) Finger 1 */
  #define SYNA_F11_2D_DATA03_01                              0x001E   /* 2D Y/X Position (3:0) Finger 1 */
  #define SYNA_F11_2D_DATA04_01                              0x001F   /* 2D Wy/Wx Finger 1 */
  #define SYNA_F11_2D_DATA05_01                              0x0020   /* 2D Z Finger 1 */
  #define SYNA_F11_2D_DATA01_02                              0x0021   /* 2D X Position (11:4) Finger 2 */
  #define SYNA_F11_2D_DATA02_02                              0x0022   /* 2D Y Position (11:4) Finger 2 */
  #define SYNA_F11_2D_DATA03_02                              0x0023   /* 2D Y/X Position (3:0) Finger 2 */
  #define SYNA_F11_2D_DATA04_02                              0x0024   /* 2D Wy/Wx Finger 2 */
  #define SYNA_F11_2D_DATA05_02                              0x0025   /* 2D Z Finger 2 */
  #define SYNA_F11_2D_DATA01_03                              0x0026   /* 2D X Position (11:4) Finger 3 */
  #define SYNA_F11_2D_DATA02_03                              0x0027   /* 2D Y Position (11:4) Finger 3 */
  #define SYNA_F11_2D_DATA03_03                              0x0028   /* 2D Y/X Position (3:0) Finger 3 */
  #define SYNA_F11_2D_DATA04_03                              0x0029   /* 2D Wy/Wx Finger 3 */
  #define SYNA_F11_2D_DATA05_03                              0x002A   /* 2D Z Finger 3 */
  #define SYNA_F11_2D_DATA01_04                              0x002B   /* 2D X Position (11:4) Finger 4 */
  #define SYNA_F11_2D_DATA02_04                              0x002C   /* 2D Y Position (11:4) Finger 4 */
  #define SYNA_F11_2D_DATA03_04                              0x002D   /* 2D Y/X Position (3:0) Finger 4 */
  #define SYNA_F11_2D_DATA04_04                              0x002E   /* 2D Wy/Wx Finger 4 */
  #define SYNA_F11_2D_DATA05_04                              0x002F   /* 2D Z Finger 4 */
  #define SYNA_F11_2D_DATA28                                 0x0030   /* 2D Extended Status */
  #define SYNA_F11_2D_DATA35_00                              0x0031   /* 2D Wy/Wx High Finger 0 */
  #define SYNA_F11_2D_DATA35_01                              0x0032   /* 2D Wy/Wx High Finger 1 */
  #define SYNA_F11_2D_DATA35_02                              0x0033   /* 2D Wy/Wx High Finger 2 */
  #define SYNA_F11_2D_DATA35_03                              0x0034   /* 2D Wy/Wx High Finger 3 */
  #define SYNA_F11_2D_DATA35_04                              0x0035   /* 2D Wy/Wx High Finger 4 */
  #define SYNA_F34_FLASH_CTRL00_00                           0x0036   /* Customer Defined Config ID 0 */
  #define SYNA_F34_FLASH_CTRL00_01                           0x0037   /* Customer Defined Config ID 1 */
  #define SYNA_F34_FLASH_CTRL00_02                           0x0038   /* Customer Defined Config ID 2 */
  #define SYNA_F34_FLASH_CTRL00_03                           0x0039   /* Customer Defined Config ID 3 */
  #define SYNA_F01_RMI_CTRL00                                0x003A   /* Device Control */
  #define SYNA_F01_RMI_CTRL01_00                             0x003B   /* Interrupt Enable 0 */
  #define SYNA_F01_RMI_CTRL02                                0x003C   /* Doze Period */
  #define SYNA_F01_RMI_CTRL03                                0x003D   /* Doze Wakeup Threshold */
  #define SYNA_F01_RMI_CTRL05                                0x003E   /* Extended Wait Before Doze */
  #define SYNA_F01_RMI_CTRL06                                0x003F   /* I2C Control */
  #define SYNA_F01_RMI_CTRL08                                0x0040   /* Attention Control */
  #define SYNA_F11_2D_CTRL00                                 0x0041   /* 2D Report Mode */
  #define SYNA_F11_2D_CTRL01                                 0x0042   /* 2D Palm Detect */
  #define SYNA_F11_2D_CTRL02                                 0x0043   /* 2D Delta-X Thresh */
  #define SYNA_F11_2D_CTRL03                                 0x0044   /* 2D Delta-Y Thresh */
  #define SYNA_F11_2D_CTRL04                                 0x0045   /* 2D Velocity */
  #define SYNA_F11_2D_CTRL05                                 0x0046   /* 2D Acceleration */
  #define SYNA_F11_2D_CTRL06                                 0x0047   /* 2D Max X Position (7:0) */
  #define SYNA_F11_2D_CTRL07                                 0x0048   /* 2D Max X Position (11:8) */
  #define SYNA_F11_2D_CTRL08                                 0x0049   /* 2D Max Y Position (7:0) */
  #define SYNA_F11_2D_CTRL09                                 0x004A   /* 2D Max Y Position (11:8) */
  #define SYNA_F11_2D_CTRL29                                 0x004B   /* Z Touch Threshold */
  #define SYNA_F11_2D_CTRL30                                 0x004C   /* Z Touch Hysteresis */
  #define SYNA_F11_2D_CTRL31                                 0x004D   /* Small Z Threshold */
  #define SYNA_F11_2D_CTRL32_00                              0x004E   /* Small Z Scale Factor */
  #define SYNA_F11_2D_CTRL32_01                              0x004F   /* Small Z Scale Factor */
  #define SYNA_F11_2D_CTRL33_00                              0x0050   /* Large Z Scale Factor */
  #define SYNA_F11_2D_CTRL33_01                              0x0051   /* Large Z Scale Factor */
  #define SYNA_F11_2D_CTRL34                                 0x0052   /* Position Calculation & Post Correction */
  #define SYNA_F11_2D_CTRL36                                 0x0053   /* Wx Scale Factor */
  #define SYNA_F11_2D_CTRL37                                 0x0054   /* Wx Offset */
  #define SYNA_F11_2D_CTRL38                                 0x0055   /* Wy Scale Factor */
  #define SYNA_F11_2D_CTRL39                                 0x0056   /* Wy Offset */
  #define SYNA_F11_2D_CTRL40_00                              0x0057   /* X Pitch */
  #define SYNA_F11_2D_CTRL40_01                              0x0058   /* X Pitch */
  #define SYNA_F11_2D_CTRL41_00                              0x0059   /* Y Pitch */
  #define SYNA_F11_2D_CTRL41_01                              0x005A   /* Y Pitch */
  #define SYNA_F11_2D_CTRL42_00                              0x005B   /* Default Finger Width Tx */
  #define SYNA_F11_2D_CTRL42_01                              0x005C   /* Default Finger Width Tx */
  #define SYNA_F11_2D_CTRL43_00                              0x005D   /* Default Finger Width Ty */
  #define SYNA_F11_2D_CTRL43_01                              0x005E   /* Default Finger Width Ty */
  #define SYNA_F11_2D_CTRL44                                 0x005F   /* Report Finger Width */
  #define SYNA_F11_2D_CTRL45                                 0x0060   /* Segmentation Aggressiveness */
  #define SYNA_F11_2D_CTRL46                                 0x0061   /* Rx Clip Low */
  #define SYNA_F11_2D_CTRL47                                 0x0062   /* Rx Clip High */
  #define SYNA_F11_2D_CTRL48                                 0x0063   /* Tx Clip Low */
  #define SYNA_F11_2D_CTRL49                                 0x0064   /* Tx Clip High */
  #define SYNA_F11_2D_CTRL50                                 0x0065   /* Minimum Finger Separation */
  #define SYNA_F11_2D_CTRL51                                 0x0066   /* Maximum Finger Movement */
  #define SYNA_F11_2D_CTRL58                                 0x0067   /* Large Object Suppression Parameters */
  #define SYNA_F11_2D_CTRL73                                 0x0068   /* Jitter Control 1 */
  #define SYNA_F11_2D_CTRL74                                 0x0069   /* Jitter Control 2 */
  #define SYNA_F11_2D_CTRL75_00                              0x006A   /* Jitter Control 3 */
  #define SYNA_F11_2D_CTRL75_01                              0x006B   /* Jitter Control 3 */
  #define SYNA_F11_2D_CTRL75_02                              0x006C   /* Jitter Control 3 */
  #define SYNA_F11_2D_CTRL76_00                              0x006D   /* Jitter Control 4 */
  #define SYNA_F11_2D_CTRL76_01                              0x006E   /* Jitter Control 4 */
  #define SYNA_F11_2D_CTRL76_02                              0x006F   /* Jitter Control 4 */
  #define SYNA_F11_2D_CTRL77                                 0x0070   /* 2D RX Number */
  #define SYNA_F11_2D_CTRL78                                 0x0071   /* 2D TX Number */
  #define SYNA_F01_RMI_CMD00                                 0x0072   /* Device Command */
  #define SYNA_F11_2D_CMD00                                  0x0073   /* 2D Command */
  #define SYNA_F34_FLASH_QUERY00                             0x0074   /* Bootloader ID 0 */
  #define SYNA_F34_FLASH_QUERY01                             0x0075   /* Bootloader ID 1 */
  #define SYNA_F34_FLASH_QUERY02                             0x0076   /* Flash Properties */
  #define SYNA_F34_FLASH_QUERY03                             0x0077   /* Block Size 0 */
  #define SYNA_F34_FLASH_QUERY04                             0x0078   /* Block Size 1 */
  #define SYNA_F34_FLASH_QUERY05                             0x0079   /* Firmware Block Count 0 */
  #define SYNA_F34_FLASH_QUERY06                             0x007A   /* Firmware Block Count 1 */
  #define SYNA_F34_FLASH_QUERY07                             0x007B   /* Configuration Block Count 0 */
  #define SYNA_F34_FLASH_QUERY08                             0x007C   /* Configuration Block Count 1 */
  #define SYNA_F01_RMI_QUERY00                               0x007D   /* Manufacturer ID Query */
  #define SYNA_F01_RMI_QUERY01                               0x007E   /* Product Properties Query */
  #define SYNA_F01_RMI_QUERY02                               0x007F   /* Customer Family Query */
  #define SYNA_F01_RMI_QUERY03                               0x0080   /* Firmware Revision Query */
  #define SYNA_F01_RMI_QUERY04                               0x0081   /* Device Serialization Query 0 */
  #define SYNA_F01_RMI_QUERY05                               0x0082   /* Device Serialization Query 1 */
  #define SYNA_F01_RMI_QUERY06                               0x0083   /* Device Serialization Query 2 */
  #define SYNA_F01_RMI_QUERY07                               0x0084   /* Device Serialization Query 3 */
  #define SYNA_F01_RMI_QUERY08                               0x0085   /* Device Serialization Query 4 */
  #define SYNA_F01_RMI_QUERY09                               0x0086   /* Device Serialization Query 5 */
  #define SYNA_F01_RMI_QUERY10                               0x0087   /* Device Serialization Query 6 */
  #define SYNA_F01_RMI_QUERY11                               0x0088   /* Product ID Query 0 */
  #define SYNA_F01_RMI_QUERY12                               0x0089   /* Product ID Query 1 */
  #define SYNA_F01_RMI_QUERY13                               0x008A   /* Product ID Query 2 */
  #define SYNA_F01_RMI_QUERY14                               0x008B   /* Product ID Query 3 */
  #define SYNA_F01_RMI_QUERY15                               0x008C   /* Product ID Query 4 */
  #define SYNA_F01_RMI_QUERY16                               0x008D   /* Product ID Query 5 */
  #define SYNA_F01_RMI_QUERY17                               0x008E   /* Product ID Query 6 */
  #define SYNA_F01_RMI_QUERY18                               0x008F   /* Product ID Query 7 */
  #define SYNA_F01_RMI_QUERY19                               0x0090   /* Product ID Query 8 */
  #define SYNA_F01_RMI_QUERY20                               0x0091   /* Product ID Query 9 */
  #define SYNA_F01_RMI_QUERY42                               0x0092   /* Product Properties 2 */
  #define SYNA_F01_RMI_QUERY43_00                            0x0093   /* DS4 Queries0 */
  #define SYNA_F01_RMI_QUERY43_01                            0x0094   /* DS4 Queries1 */
  #define SYNA_F01_RMI_QUERY43_02                            0x0095   /* DS4 Queries2 */
  #define SYNA_F01_RMI_QUERY44                               0x0096   /* Reset Query */
  #define SYNA_F11_2D_QUERY00                                0x0097   /* Per-device Query */
  #define SYNA_F11_2D_QUERY01                                0x0098   /* 2D Reporting Mode */
  #define SYNA_F11_2D_QUERY02                                0x0099   /* 2D Number of X Electrodes */
  #define SYNA_F11_2D_QUERY03                                0x009A   /* 2D Number of Y Electrodes */
  #define SYNA_F11_2D_QUERY04                                0x009B   /* 2D Maximum Electrodes */
  #define SYNA_F11_2D_QUERY05                                0x009C   /* 2D Absolute Query */
  #define SYNA_F11_2D_QUERY11                                0x009D   /* Per-device Query */
  #define SYNA_F11_2D_QUERY12                                0x009E   /* Advanced Sensing Features 2 */

  /* Start of Page Description Table (PDT) */

  #define SYNA_PDT_P00_F11_2D_QUERY_BASE                     0x00DD   /* Query Base */
  #define SYNA_PDT_P00_F11_2D_COMMAND_BASE                   0x00DE   /* Command Base */
  #define SYNA_PDT_P00_F11_2D_CONTROL_BASE                   0x00DF   /* Control Base */
  #define SYNA_PDT_P00_F11_2D_DATA_BASE                      0x00E0   /* Data Base */
  #define SYNA_PDT_P00_F11_2D_INTERRUPTS                     0x00E1   /* Version & Interrupt Count */
  #define SYNA_PDT_P00_F11_2D_EXISTS                         0x00E2   /* Function Exists */
  #define SYNA_PDT_P00_F01_RMI_QUERY_BASE                    0x00E3   /* Query Base */
  #define SYNA_PDT_P00_F01_RMI_COMMAND_BASE                  0x00E4   /* Command Base */
  #define SYNA_PDT_P00_F01_RMI_CONTROL_BASE                  0x00E5   /* Control Base */
  #define SYNA_PDT_P00_F01_RMI_DATA_BASE                     0x00E6   /* Data Base */
  #define SYNA_PDT_P00_F01_RMI_INTERRUPTS                    0x00E7   /* Version & Interrupt Count */
  #define SYNA_PDT_P00_F01_RMI_EXISTS                        0x00E8   /* Function Exists */
  #define SYNA_PDT_P00_F34_FLASH_QUERY_BASE                  0x00E9   /* Query Base */
  #define SYNA_PDT_P00_F34_FLASH_COMMAND_BASE                0x00EA   /* Command Base */
  #define SYNA_PDT_P00_F34_FLASH_CONTROL_BASE                0x00EB   /* Control Base */
  #define SYNA_PDT_P00_F34_FLASH_DATA_BASE                   0x00EC   /* Data Base */
  #define SYNA_PDT_P00_F34_FLASH_INTERRUPTS                  0x00ED   /* Version & Interrupt Count */
  #define SYNA_PDT_P00_F34_FLASH_EXISTS                      0x00EE   /* Function Exists */
  #define SYNA_P00_PDT_PROPERTIES                            0x00EF   /* P00_PDT Properties */
  #define SYNA_ADDR_00F9                                     0x00F9   /* Ballistics Control */
  #define SYNA_ADDR_00FA                                     0x00FA   /* Lift/Land Filter Control 0 */
  #define SYNA_ADDR_00FB                                     0x00FB   /* Lift/Land Filter Control 1 */
  #define SYNA_ADDR_00FC                                     0x00FC   /* Auto Fast Relax Control 0 */
  #define SYNA_ADDR_00FD                                     0x00FD   /* Auto Fast Relax Control 1 */
  #define SYNA_ADDR_00FE                                     0x00FE   /* Auto Fast Relax Control 2 */
  #define SYNA_P00_PAGESELECT                                0x00FF   /* Page Select register */

  /* Registers on Page 0x01 */

  /*      Register Name                                      Address     Register Description */
  /*      -------------                                      -------     -------------------- */
  #define SYNA_F54_ANALOG_DATA00                             0x0100   /* Report Type */
  #define SYNA_F54_ANALOG_DATA01                             0x0101   /* FIFO Index Lo */
  #define SYNA_F54_ANALOG_DATA02                             0x0102   /* FIFO Index Hi */
  #define SYNA_F54_ANALOG_DATA03                             0x0103   /* FIFO */
  #define SYNA_F54_ANALOG_DATA04                             0x0104   /* Current Sense Freq Selection */
  #define SYNA_F54_ANALOG_DATA06_00                          0x0105   /* Interference Metric Lo */
  #define SYNA_F54_ANALOG_DATA06_01                          0x0106   /* Interference Metric Hi */
  #define SYNA_F54_ANALOG_DATA07_00                          0x0107   /* TwoByte Report Rate Low */
  #define SYNA_F54_ANALOG_DATA07_01                          0x0108   /* TwoByte Report Rate Hi */
  #define SYNA_F54_ANALOG_DATA08_00                          0x0109   /* Variance Metric Low */
  #define SYNA_F54_ANALOG_DATA08_01                          0x010A   /* Variance Metric High */
  #define SYNA_F54_ANALOG_CTRL00                             0x010B   /* General Control */
  #define SYNA_F54_ANALOG_CTRL01                             0x010C   /* General Control 1 */
  #define SYNA_F54_ANALOG_CTRL02_00                          0x010D   /* Saturation Capacitance Low */
  #define SYNA_F54_ANALOG_CTRL02_01                          0x010E   /* Saturation Capacitance High */
  #define SYNA_F54_ANALOG_CTRL03                             0x010F   /* Pixel Touch Threshold */
  #define SYNA_F54_ANALOG_CTRL04                             0x0110   /* Misc Analog Control */
  #define SYNA_F54_ANALOG_CTRL05                             0x0111   /* RefCap RefLo Settings */
  #define SYNA_F54_ANALOG_CTRL06                             0x0112   /* RefCap RefHi Settings */
  #define SYNA_F54_ANALOG_CTRL07                             0x0113   /* CBC Cap Settings */
  #define SYNA_F54_ANALOG_CTRL08_00                          0x0114   /* Integration Duration Low */
  #define SYNA_F54_ANALOG_CTRL08_01                          0x0115   /* Integration Duration High */
  #define SYNA_F54_ANALOG_CTRL09                             0x0116   /* Reset Duration */
  #define SYNA_F54_ANALOG_CTRL10                             0x0117   /* Noise Measurement Control */
  #define SYNA_F54_ANALOG_CTRL11_00                          0x0118   /* Reserved */
  #define SYNA_F54_ANALOG_CTRL11_01                          0x0119   /* Reserved */
  #define SYNA_F54_ANALOG_CTRL12                             0x011A   /* Slow Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL13                             0x011B   /* Fast Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL14                             0x011C   /* Sensor Physical Properties */
  #define SYNA_F54_ANALOG_CTRL15_00                          0x011D   /* Sensor Rx Mapping 0 */
  #define SYNA_F54_ANALOG_CTRL15_01                          0x011E   /* Sensor Rx Mapping 1 */
  #define SYNA_F54_ANALOG_CTRL15_02                          0x011F   /* Sensor Rx Mapping 2 */
  #define SYNA_F54_ANALOG_CTRL15_03                          0x0120   /* Sensor Rx Mapping 3 */
  #define SYNA_F54_ANALOG_CTRL15_04                          0x0121   /* Sensor Rx Mapping 4 */
  #define SYNA_F54_ANALOG_CTRL15_05                          0x0122   /* Sensor Rx Mapping 5 */
  #define SYNA_F54_ANALOG_CTRL15_06                          0x0123   /* Sensor Rx Mapping 6 */
  #define SYNA_F54_ANALOG_CTRL15_07                          0x0124   /* Sensor Rx Mapping 7 */
  #define SYNA_F54_ANALOG_CTRL15_08                          0x0125   /* Sensor Rx Mapping 8 */
  #define SYNA_F54_ANALOG_CTRL15_09                          0x0126   /* Sensor Rx Mapping 9 */
  #define SYNA_F54_ANALOG_CTRL15_10                          0x0127   /* Sensor Rx Mapping 10 */
  #define SYNA_F54_ANALOG_CTRL15_11                          0x0128   /* Sensor Rx Mapping 11 */
  #define SYNA_F54_ANALOG_CTRL15_12                          0x0129   /* Sensor Rx Mapping 12 */
  #define SYNA_F54_ANALOG_CTRL15_13                          0x012A   /* Sensor Rx Mapping 13 */
  #define SYNA_F54_ANALOG_CTRL15_14                          0x012B   /* Sensor Rx Mapping 14 */
  #define SYNA_F54_ANALOG_CTRL15_15                          0x012C   /* Sensor Rx Mapping 15 */
  #define SYNA_F54_ANALOG_CTRL15_16                          0x012D   /* Sensor Rx Mapping 16 */
  #define SYNA_F54_ANALOG_CTRL15_17                          0x012E   /* Sensor Rx Mapping 17 */
  #define SYNA_F54_ANALOG_CTRL15_18                          0x012F   /* Sensor Rx Mapping 18 */
  #define SYNA_F54_ANALOG_CTRL15_19                          0x0130   /* Sensor Rx Mapping 19 */
  #define SYNA_F54_ANALOG_CTRL16_00                          0x0131   /* Sensor Tx Mapping 0 */
  #define SYNA_F54_ANALOG_CTRL16_01                          0x0132   /* Sensor Tx Mapping 1 */
  #define SYNA_F54_ANALOG_CTRL16_02                          0x0133   /* Sensor Tx Mapping 2 */
  #define SYNA_F54_ANALOG_CTRL16_03                          0x0134   /* Sensor Tx Mapping 3 */
  #define SYNA_F54_ANALOG_CTRL16_04                          0x0135   /* Sensor Tx Mapping 4 */
  #define SYNA_F54_ANALOG_CTRL16_05                          0x0136   /* Sensor Tx Mapping 5 */
  #define SYNA_F54_ANALOG_CTRL16_06                          0x0137   /* Sensor Tx Mapping 6 */
  #define SYNA_F54_ANALOG_CTRL16_07                          0x0138   /* Sensor Tx Mapping 7 */
  #define SYNA_F54_ANALOG_CTRL16_08                          0x0139   /* Sensor Tx Mapping 8 */
  #define SYNA_F54_ANALOG_CTRL16_09                          0x013A   /* Sensor Tx Mapping 9 */
  #define SYNA_F54_ANALOG_CTRL16_10                          0x013B   /* Sensor Tx Mapping 10 */
  #define SYNA_F54_ANALOG_CTRL16_11                          0x013C   /* Sensor Tx Mapping 11 */
  #define SYNA_F54_ANALOG_CTRL16_12                          0x013D   /* Sensor Tx Mapping 12 */
  #define SYNA_F54_ANALOG_CTRL17_00                          0x013E   /* Sense Frequency Control0 0 */
  #define SYNA_F54_ANALOG_CTRL17_01                          0x013F   /* Sense Frequency Control0 1 */
  #define SYNA_F54_ANALOG_CTRL17_02                          0x0140   /* Sense Frequency Control0 2 */
  #define SYNA_F54_ANALOG_CTRL17_03                          0x0141   /* Sense Frequency Control0 3 */
  #define SYNA_F54_ANALOG_CTRL17_04                          0x0142   /* Sense Frequency Control0 4 */
  #define SYNA_F54_ANALOG_CTRL17_05                          0x0143   /* Sense Frequency Control0 5 */
  #define SYNA_F54_ANALOG_CTRL17_06                          0x0144   /* Sense Frequency Control0 6 */
  #define SYNA_F54_ANALOG_CTRL17_07                          0x0145   /* Sense Frequency Control0 7 */
  #define SYNA_F54_ANALOG_CTRL18_00                          0x0146   /* Sense Frequency Control Low 0 */
  #define SYNA_F54_ANALOG_CTRL18_01                          0x0147   /* Sense Frequency Control Low 1 */
  #define SYNA_F54_ANALOG_CTRL18_02                          0x0148   /* Sense Frequency Control Low 2 */
  #define SYNA_F54_ANALOG_CTRL18_03                          0x0149   /* Sense Frequency Control Low 3 */
  #define SYNA_F54_ANALOG_CTRL18_04                          0x014A   /* Sense Frequency Control Low 4 */
  #define SYNA_F54_ANALOG_CTRL18_05                          0x014B   /* Sense Frequency Control Low 5 */
  #define SYNA_F54_ANALOG_CTRL18_06                          0x014C   /* Sense Frequency Control Low 6 */
  #define SYNA_F54_ANALOG_CTRL18_07                          0x014D   /* Sense Frequency Control Low 7 */
  #define SYNA_F54_ANALOG_CTRL19_00                          0x014E   /* Sense Frequency Control2 0 */
  #define SYNA_F54_ANALOG_CTRL19_01                          0x014F   /* Sense Frequency Control2 1 */
  #define SYNA_F54_ANALOG_CTRL19_02                          0x0150   /* Sense Frequency Control2 2 */
  #define SYNA_F54_ANALOG_CTRL19_03                          0x0151   /* Sense Frequency Control2 3 */
  #define SYNA_F54_ANALOG_CTRL19_04                          0x0152   /* Sense Frequency Control2 4 */
  #define SYNA_F54_ANALOG_CTRL19_05                          0x0153   /* Sense Frequency Control2 5 */
  #define SYNA_F54_ANALOG_CTRL19_06                          0x0154   /* Sense Frequency Control2 6 */
  #define SYNA_F54_ANALOG_CTRL19_07                          0x0155   /* Sense Frequency Control2 7 */
  #define SYNA_F54_ANALOG_CTRL20                             0x0156   /* Noise Mitigation General Control */
  #define SYNA_F54_ANALOG_CTRL21_00                          0x0157   /* HNM Frequency Shift Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL21_01                          0x0158   /* HNM Frequency Shift Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL22                             0x0159   /* HNM Exit Density */
  #define SYNA_F54_ANALOG_CTRL23_00                          0x015A   /* Medium Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL23_01                          0x015B   /* Medium Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL24_00                          0x015C   /* High Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL24_01                          0x015D   /* High Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL25                             0x015E   /* FNM Frequency Shift Density */
  #define SYNA_F54_ANALOG_CTRL26                             0x015F   /* FNM Exit Threshold */
  #define SYNA_F54_ANALOG_CTRL27                             0x0160   /* IIR Filter Coefficient */
  #define SYNA_F54_ANALOG_CTRL28_00                          0x0161   /* FNM Frequency Shift Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL28_01                          0x0162   /* FNM Frequency Shift Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL29                             0x0163   /* Common-Mode Noise Control */
  #define SYNA_F54_ANALOG_CTRL30                             0x0164   /* CMN Cap Scale Factor */
  #define SYNA_F54_ANALOG_CTRL31                             0x0165   /* Pixel Threshold Hysteresis */
  #define SYNA_F54_ANALOG_CTRL32_00                          0x0166   /* Rx LowEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL32_01                          0x0167   /* Rx LowEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL33_00                          0x0168   /* Rx HighEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL33_01                          0x0169   /* Rx HighEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL34_00                          0x016A   /* Tx LowEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL34_01                          0x016B   /* Tx LowEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL35_00                          0x016C   /* Tx HighEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL35_01                          0x016D   /* Tx HighEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL36_00                          0x016E   /* Axis 1 Compensation 0 */
  #define SYNA_F54_ANALOG_CTRL36_01                          0x016F   /* Axis 1 Compensation 1 */
  #define SYNA_F54_ANALOG_CTRL36_02                          0x0170   /* Axis 1 Compensation 2 */
  #define SYNA_F54_ANALOG_CTRL36_03                          0x0171   /* Axis 1 Compensation 3 */
  #define SYNA_F54_ANALOG_CTRL36_04                          0x0172   /* Axis 1 Compensation 4 */
  #define SYNA_F54_ANALOG_CTRL36_05                          0x0173   /* Axis 1 Compensation 5 */
  #define SYNA_F54_ANALOG_CTRL36_06                          0x0174   /* Axis 1 Compensation 6 */
  #define SYNA_F54_ANALOG_CTRL36_07                          0x0175   /* Axis 1 Compensation 7 */
  #define SYNA_F54_ANALOG_CTRL36_08                          0x0176   /* Axis 1 Compensation 8 */
  #define SYNA_F54_ANALOG_CTRL36_09                          0x0177   /* Axis 1 Compensation 9 */
  #define SYNA_F54_ANALOG_CTRL36_10                          0x0178   /* Axis 1 Compensation 10 */
  #define SYNA_F54_ANALOG_CTRL36_11                          0x0179   /* Axis 1 Compensation 11 */
  #define SYNA_F54_ANALOG_CTRL36_12                          0x017A   /* Axis 1 Compensation 12 */
  #define SYNA_F54_ANALOG_CTRL36_13                          0x017B   /* Axis 1 Compensation 13 */
  #define SYNA_F54_ANALOG_CTRL36_14                          0x017C   /* Axis 1 Compensation 14 */
  #define SYNA_F54_ANALOG_CTRL36_15                          0x017D   /* Axis 1 Compensation 15 */
  #define SYNA_F54_ANALOG_CTRL36_16                          0x017E   /* Axis 1 Compensation 16 */
  #define SYNA_F54_ANALOG_CTRL36_17                          0x017F   /* Axis 1 Compensation 17 */
  #define SYNA_F54_ANALOG_CTRL36_18                          0x0180   /* Axis 1 Compensation 18 */
  #define SYNA_F54_ANALOG_CTRL36_19                          0x0181   /* Axis 1 Compensation 19 */
  #define SYNA_F54_ANALOG_CTRL38_00                          0x0182   /* Noise Control 1 0 */
  #define SYNA_F54_ANALOG_CTRL38_01                          0x0183   /* Noise Control 1 1 */
  #define SYNA_F54_ANALOG_CTRL38_02                          0x0184   /* Noise Control 1 2 */
  #define SYNA_F54_ANALOG_CTRL38_03                          0x0185   /* Noise Control 1 3 */
  #define SYNA_F54_ANALOG_CTRL38_04                          0x0186   /* Noise Control 1 4 */
  #define SYNA_F54_ANALOG_CTRL38_05                          0x0187   /* Noise Control 1 5 */
  #define SYNA_F54_ANALOG_CTRL38_06                          0x0188   /* Noise Control 1 6 */
  #define SYNA_F54_ANALOG_CTRL38_07                          0x0189   /* Noise Control 1 7 */
  #define SYNA_F54_ANALOG_CTRL39_00                          0x018A   /* Noise Control 2 0 */
  #define SYNA_F54_ANALOG_CTRL39_01                          0x018B   /* Noise Control 2 1 */
  #define SYNA_F54_ANALOG_CTRL39_02                          0x018C   /* Noise Control 2 2 */
  #define SYNA_F54_ANALOG_CTRL39_03                          0x018D   /* Noise Control 2 3 */
  #define SYNA_F54_ANALOG_CTRL39_04                          0x018E   /* Noise Control 2 4 */
  #define SYNA_F54_ANALOG_CTRL39_05                          0x018F   /* Noise Control 2 5 */
  #define SYNA_F54_ANALOG_CTRL39_06                          0x0190   /* Noise Control 2 6 */
  #define SYNA_F54_ANALOG_CTRL39_07                          0x0191   /* Noise Control 2 7 */
  #define SYNA_F54_ANALOG_CTRL40_00                          0x0192   /* Noise Control 3 0 */
  #define SYNA_F54_ANALOG_CTRL40_01                          0x0193   /* Noise Control 3 1 */
  #define SYNA_F54_ANALOG_CTRL40_02                          0x0194   /* Noise Control 3 2 */
  #define SYNA_F54_ANALOG_CTRL40_03                          0x0195   /* Noise Control 3 3 */
  #define SYNA_F54_ANALOG_CTRL40_04                          0x0196   /* Noise Control 3 4 */
  #define SYNA_F54_ANALOG_CTRL40_05                          0x0197   /* Noise Control 3 5 */
  #define SYNA_F54_ANALOG_CTRL40_06                          0x0198   /* Noise Control 3 6 */
  #define SYNA_F54_ANALOG_CTRL40_07                          0x0199   /* Noise Control 3 7 */
  #define SYNA_F54_ANALOG_CTRL41                             0x019A   /* Multi Metric Noise Mitigation Control */
  #define SYNA_F54_ANALOG_CTRL42_00                          0x019B   /* Variance Metric Threshold Low */
  #define SYNA_F54_ANALOG_CTRL42_01                          0x019C   /* Variance Metric Threshold High */
  #define SYNA_F54_ANALOG_CTRL55                             0x019D   /* 0D Slow Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL56                             0x019E   /* 0D Fast Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL57                             0x019F   /* 0D CBC Settings */
  #define SYNA_F54_ANALOG_CTRL58                             0x01A0   /* 0D Acquisition Scheme */
  #define SYNA_F54_ANALOG_CMD00                              0x01A1   /* Analog Command 0 */
  #define SYNA_F54_ANALOG_QUERY00                            0x01A2   /* Device Capability Information */
  #define SYNA_F54_ANALOG_QUERY01                            0x01A3   /* Device Capability Information */
  #define SYNA_F54_ANALOG_QUERY02                            0x01A4   /* Image Reporting Modes */
  #define SYNA_F54_ANALOG_QUERY03_00                         0x01A5   /* Clock Rate Low */
  #define SYNA_F54_ANALOG_QUERY03_01                         0x01A6   /* Clock Rate High */
  #define SYNA_F54_ANALOG_QUERY04                            0x01A7   /* Analog Hardware Family */
  #define SYNA_F54_ANALOG_QUERY05                            0x01A8   /* Reserved */
  #define SYNA_F54_ANALOG_QUERY06                            0x01A9   /* Data Acquisition Controls */
  #define SYNA_F54_ANALOG_QUERY07                            0x01AA   /* Reserved  */
  #define SYNA_F54_ANALOG_QUERY08                            0x01AB   /* Data Acquisition Post-Processing Controls */
  #define SYNA_F54_ANALOG_QUERY09                            0x01AC   /* Reserved  */
  #define SYNA_F54_ANALOG_QUERY10                            0x01AD   /* Reserved  */
  #define SYNA_F54_ANALOG_QUERY11                            0x01AE   /* Reserved  */
  #define SYNA_F54_ANALOG_QUERY12                            0x01AF   /* Sense Frequency Control */
  #define SYNA_F54_ANALOG_QUERY13                            0x01B0   /* Analog Information */

  /* Start of Page Description Table (PDT) for Page 0x01 */

  #define SYNA_PDT_P01_F54_ANALOG_QUERY_BASE                 0x01E9   /* Query Base */
  #define SYNA_PDT_P01_F54_ANALOG_COMMAND_BASE               0x01EA   /* Command Base */
  #define SYNA_PDT_P01_F54_ANALOG_CONTROL_BASE               0x01EB   /* Control Base */
  #define SYNA_PDT_P01_F54_ANALOG_DATA_BASE                  0x01EC   /* Data Base */
  #define SYNA_PDT_P01_F54_ANALOG_INTERRUPTS                 0x01ED   /* Version & Interrupt Count */
  #define SYNA_PDT_P01_F54_ANALOG_EXISTS                     0x01EE   /* Function Exists */
  #define SYNA_P01_PDT_PROPERTIES                            0x01EF   /* P01_PDT Properties */
  #define SYNA_P01_PAGESELECT                                0x01FF   /* Page Select register */

  /* Registers on Page 0x02 */

  /*      Register Name                                      Address     Register Description */
  /*      -------------                                      -------     -------------------- */
  #define SYNA_F1A_0D_DATA00                                 0x0200   /* Button Data */
  #define SYNA_F31_LED_CTRL00_00                             0x0201   /* LED Brightness 0 */
  #define SYNA_F31_LED_CTRL00_01                             0x0202   /* LED Brightness 1 */
  #define SYNA_F31_LED_CTRL00_02                             0x0203   /* LED Brightness 2 */
  #define SYNA_F31_LED_CTRL00_03                             0x0204   /* LED Brightness 3 */
  #define SYNA_F31_LED_CTRL00_04                             0x0205   /* LED Brightness 4 */
  #define SYNA_F31_LED_CTRL00_05                             0x0206   /* LED Brightness 5 */
  #define SYNA_F31_LED_CTRL00_06                             0x0207   /* LED Brightness 6 */
  #define SYNA_F31_LED_CTRL00_07                             0x0208   /* LED Brightness 7 */
  #define SYNA_F1A_0D_CTRL00                                 0x0209   /* General Control */
  #define SYNA_F1A_0D_CTRL01                                 0x020A   /* Button Interrupt Enable */
  #define SYNA_F1A_0D_CTRL02                                 0x020B   /* MultiButton Group Selection */
  #define SYNA_F1A_0D_CTRL03_00                              0x020C   /* Tx Button 0 */
  #define SYNA_F1A_0D_CTRL04_00                              0x020D   /* Rx Button 0 */
  #define SYNA_F1A_0D_CTRL03_01                              0x020E   /* Tx Button 1 */
  #define SYNA_F1A_0D_CTRL04_01                              0x020F   /* Rx Button 1 */
  #define SYNA_F1A_0D_CTRL03_02                              0x0210   /* Tx Button 2 */
  #define SYNA_F1A_0D_CTRL04_02                              0x0211   /* Rx Button 2 */
  #define SYNA_F1A_0D_CTRL03_03                              0x0212   /* Tx Button 3 */
  #define SYNA_F1A_0D_CTRL04_03                              0x0213   /* Rx Button 3 */
  #define SYNA_F1A_0D_CTRL05_00                              0x0214   /* Touch Threshold Button 0 */
  #define SYNA_F1A_0D_CTRL05_01                              0x0215   /* Touch Threshold Button 1 */
  #define SYNA_F1A_0D_CTRL05_02                              0x0216   /* Touch Threshold Button 2 */
  #define SYNA_F1A_0D_CTRL05_03                              0x0217   /* Touch Threshold Button 3 */
  #define SYNA_F1A_0D_CTRL06                                 0x0218   /* Release Threshold */
  #define SYNA_F1A_0D_CTRL07                                 0x0219   /* Strongest Button Hysteresis */
  #define SYNA_F1A_0D_CTRL08                                 0x021A   /* Filter Strength */
  #define SYNA_F31_LED_QUERY00                               0x021B   /* General LED Controls  */
  #define SYNA_F31_LED_QUERY01                               0x021C   /* Number of Leds */
  #define SYNA_F1A_0D_QUERY00                                0x021D   /* Maximum Button Count */
  #define SYNA_F1A_0D_QUERY01                                0x021E   /* Button Features */

  /* Start of Page Description Table (PDT) for Page 0x02 */

  #define SYNA_PDT_P02_F1A_0D_QUERY_BASE                     0x02E3   /* Query Base */
  #define SYNA_PDT_P02_F1A_0D_COMMAND_BASE                   0x02E4   /* Command Base */
  #define SYNA_PDT_P02_F1A_0D_CONTROL_BASE                   0x02E5   /* Control Base */
  #define SYNA_PDT_P02_F1A_0D_DATA_BASE                      0x02E6   /* Data Base */
  #define SYNA_PDT_P02_F1A_0D_INTERRUPTS                     0x02E7   /* Version & Interrupt Count */
  #define SYNA_PDT_P02_F1A_0D_EXISTS                         0x02E8   /* Function Exists */
  #define SYNA_PDT_P02_F31_LED_QUERY_BASE                    0x02E9   /* Query Base */
  #define SYNA_PDT_P02_F31_LED_COMMAND_BASE                  0x02EA   /* Command Base */
  #define SYNA_PDT_P02_F31_LED_CONTROL_BASE                  0x02EB   /* Control Base */
  #define SYNA_PDT_P02_F31_LED_DATA_BASE                     0x02EC   /* Data Base */
  #define SYNA_PDT_P02_F31_LED_INTERRUPTS                    0x02ED   /* Version & Interrupt Count */
  #define SYNA_PDT_P02_F31_LED_EXISTS                        0x02EE   /* Function Exists */
  #define SYNA_P02_PDT_PROPERTIES                            0x02EF   /* P02_PDT Properties */
  #define SYNA_P02_PAGESELECT                                0x02FF   /* Page Select register */

  /* Offsets within the configuration block */

  /*      Register Name                                      Offset      Register Description */
  /*      -------------                                      ------      -------------------- */
  #define SYNA_F34_FLASH_CTRL00_00_CFGBLK_OFS                0x0000   /* Customer Defined Config ID 0 */
  #define SYNA_F34_FLASH_CTRL00_01_CFGBLK_OFS                0x0001   /* Customer Defined Config ID 1 */
  #define SYNA_F34_FLASH_CTRL00_02_CFGBLK_OFS                0x0002   /* Customer Defined Config ID 2 */
  #define SYNA_F34_FLASH_CTRL00_03_CFGBLK_OFS                0x0003   /* Customer Defined Config ID 3 */
  #define SYNA_F01_RMI_CTRL00_CFGBLK_OFS                     0x0004   /* Device Control */
  #define SYNA_F01_RMI_CTRL01_00_CFGBLK_OFS                  0x0005   /* Interrupt Enable 0 */
  #define SYNA_F01_RMI_CTRL02_CFGBLK_OFS                     0x0006   /* Doze Period */
  #define SYNA_F01_RMI_CTRL03_CFGBLK_OFS                     0x0007   /* Doze Wakeup Threshold */
  #define SYNA_F01_RMI_CTRL05_CFGBLK_OFS                     0x0008   /* Extended Wait Before Doze */
  #define SYNA_F01_RMI_CTRL06_CFGBLK_OFS                     0x0009   /* I2C Control */
  #define SYNA_F01_RMI_CTRL08_CFGBLK_OFS                     0x000A   /* Attention Control */
  #define SYNA_F11_2D_CTRL00_CFGBLK_OFS                      0x000B   /* 2D Report Mode */
  #define SYNA_F11_2D_CTRL01_CFGBLK_OFS                      0x000C   /* 2D Palm Detect */
  #define SYNA_F11_2D_CTRL02_CFGBLK_OFS                      0x000D   /* 2D Delta-X Thresh */
  #define SYNA_F11_2D_CTRL03_CFGBLK_OFS                      0x000E   /* 2D Delta-Y Thresh */
  #define SYNA_F11_2D_CTRL04_CFGBLK_OFS                      0x000F   /* 2D Velocity */
  #define SYNA_F11_2D_CTRL05_CFGBLK_OFS                      0x0010   /* 2D Acceleration */
  #define SYNA_F11_2D_CTRL06_CFGBLK_OFS                      0x0011   /* 2D Max X Position (7:0) */
  #define SYNA_F11_2D_CTRL07_CFGBLK_OFS                      0x0012   /* 2D Max X Position (11:8) */
  #define SYNA_F11_2D_CTRL08_CFGBLK_OFS                      0x0013   /* 2D Max Y Position (7:0) */
  #define SYNA_F11_2D_CTRL09_CFGBLK_OFS                      0x0014   /* 2D Max Y Position (11:8) */
  #define SYNA_F11_2D_CTRL29_CFGBLK_OFS                      0x0015   /* Z Touch Threshold */
  #define SYNA_F11_2D_CTRL30_CFGBLK_OFS                      0x0016   /* Z Touch Hysteresis */
  #define SYNA_F11_2D_CTRL31_CFGBLK_OFS                      0x0017   /* Small Z Threshold */
  #define SYNA_F11_2D_CTRL32_00_CFGBLK_OFS                   0x0018   /* Small Z Scale Factor */
  #define SYNA_F11_2D_CTRL32_01_CFGBLK_OFS                   0x0019   /* Small Z Scale Factor */
  #define SYNA_F11_2D_CTRL33_00_CFGBLK_OFS                   0x001A   /* Large Z Scale Factor */
  #define SYNA_F11_2D_CTRL33_01_CFGBLK_OFS                   0x001B   /* Large Z Scale Factor */
  #define SYNA_F11_2D_CTRL34_CFGBLK_OFS                      0x001C   /* Position Calculation & Post Correction */
  #define SYNA_F11_2D_CTRL36_CFGBLK_OFS                      0x001D   /* Wx Scale Factor */
  #define SYNA_F11_2D_CTRL37_CFGBLK_OFS                      0x001E   /* Wx Offset */
  #define SYNA_F11_2D_CTRL38_CFGBLK_OFS                      0x001F   /* Wy Scale Factor */
  #define SYNA_F11_2D_CTRL39_CFGBLK_OFS                      0x0020   /* Wy Offset */
  #define SYNA_F11_2D_CTRL40_00_CFGBLK_OFS                   0x0021   /* X Pitch */
  #define SYNA_F11_2D_CTRL40_01_CFGBLK_OFS                   0x0022   /* X Pitch */
  #define SYNA_F11_2D_CTRL41_00_CFGBLK_OFS                   0x0023   /* Y Pitch */
  #define SYNA_F11_2D_CTRL41_01_CFGBLK_OFS                   0x0024   /* Y Pitch */
  #define SYNA_F11_2D_CTRL42_00_CFGBLK_OFS                   0x0025   /* Default Finger Width Tx */
  #define SYNA_F11_2D_CTRL42_01_CFGBLK_OFS                   0x0026   /* Default Finger Width Tx */
  #define SYNA_F11_2D_CTRL43_00_CFGBLK_OFS                   0x0027   /* Default Finger Width Ty */
  #define SYNA_F11_2D_CTRL43_01_CFGBLK_OFS                   0x0028   /* Default Finger Width Ty */
  #define SYNA_F11_2D_CTRL44_CFGBLK_OFS                      0x0029   /* Report Finger Width */
  #define SYNA_F11_2D_CTRL45_CFGBLK_OFS                      0x002A   /* Segmentation Aggressiveness */
  #define SYNA_F11_2D_CTRL46_CFGBLK_OFS                      0x002B   /* Rx Clip Low */
  #define SYNA_F11_2D_CTRL47_CFGBLK_OFS                      0x002C   /* Rx Clip High */
  #define SYNA_F11_2D_CTRL48_CFGBLK_OFS                      0x002D   /* Tx Clip Low */
  #define SYNA_F11_2D_CTRL49_CFGBLK_OFS                      0x002E   /* Tx Clip High */
  #define SYNA_F11_2D_CTRL50_CFGBLK_OFS                      0x002F   /* Minimum Finger Separation */
  #define SYNA_F11_2D_CTRL51_CFGBLK_OFS                      0x0030   /* Maximum Finger Movement */
  #define SYNA_F11_2D_CTRL58_CFGBLK_OFS                      0x0031   /* Large Object Suppression Parameters */
  #define SYNA_F11_2D_CTRL73_CFGBLK_OFS                      0x0032   /* Jitter Control 1 */
  #define SYNA_F11_2D_CTRL74_CFGBLK_OFS                      0x0033   /* Jitter Control 2 */
  #define SYNA_F11_2D_CTRL75_00_CFGBLK_OFS                   0x0034   /* Jitter Control 3 */
  #define SYNA_F11_2D_CTRL75_01_CFGBLK_OFS                   0x0035   /* Jitter Control 3 */
  #define SYNA_F11_2D_CTRL75_02_CFGBLK_OFS                   0x0036   /* Jitter Control 3 */
  #define SYNA_F11_2D_CTRL76_00_CFGBLK_OFS                   0x0037   /* Jitter Control 4 */
  #define SYNA_F11_2D_CTRL76_01_CFGBLK_OFS                   0x0038   /* Jitter Control 4 */
  #define SYNA_F11_2D_CTRL76_02_CFGBLK_OFS                   0x0039   /* Jitter Control 4 */
  #define SYNA_F11_2D_CTRL77_CFGBLK_OFS                      0x003A   /* 2D RX Number */
  #define SYNA_F11_2D_CTRL78_CFGBLK_OFS                      0x003B   /* 2D TX Number */
  #define SYNA_F54_ANALOG_CTRL00_CFGBLK_OFS                  0x003C   /* General Control */
  #define SYNA_F54_ANALOG_CTRL01_CFGBLK_OFS                  0x003D   /* General Control 1 */
  #define SYNA_F54_ANALOG_CTRL02_00_CFGBLK_OFS               0x003E   /* Saturation Capacitance Low */
  #define SYNA_F54_ANALOG_CTRL02_01_CFGBLK_OFS               0x003F   /* Saturation Capacitance High */
  #define SYNA_F54_ANALOG_CTRL03_CFGBLK_OFS                  0x0040   /* Pixel Touch Threshold */
  #define SYNA_F54_ANALOG_CTRL04_CFGBLK_OFS                  0x0041   /* Misc Analog Control */
  #define SYNA_F54_ANALOG_CTRL05_CFGBLK_OFS                  0x0042   /* RefCap RefLo Settings */
  #define SYNA_F54_ANALOG_CTRL06_CFGBLK_OFS                  0x0043   /* RefCap RefHi Settings */
  #define SYNA_F54_ANALOG_CTRL07_CFGBLK_OFS                  0x0044   /* CBC Cap Settings */
  #define SYNA_F54_ANALOG_CTRL08_00_CFGBLK_OFS               0x0045   /* Integration Duration Low */
  #define SYNA_F54_ANALOG_CTRL08_01_CFGBLK_OFS               0x0046   /* Integration Duration High */
  #define SYNA_F54_ANALOG_CTRL09_CFGBLK_OFS                  0x0047   /* Reset Duration */
  #define SYNA_F54_ANALOG_CTRL10_CFGBLK_OFS                  0x0048   /* Noise Measurement Control */
  #define SYNA_F54_ANALOG_CTRL11_00_CFGBLK_OFS               0x0049   /* Reserved */
  #define SYNA_F54_ANALOG_CTRL11_01_CFGBLK_OFS               0x004A   /* Reserved */
  #define SYNA_F54_ANALOG_CTRL12_CFGBLK_OFS                  0x004B   /* Slow Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL13_CFGBLK_OFS                  0x004C   /* Fast Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL14_CFGBLK_OFS                  0x004D   /* Sensor Physical Properties */
  #define SYNA_F54_ANALOG_CTRL15_00_CFGBLK_OFS               0x004E   /* Sensor Rx Mapping 0 */
  #define SYNA_F54_ANALOG_CTRL15_01_CFGBLK_OFS               0x004F   /* Sensor Rx Mapping 1 */
  #define SYNA_F54_ANALOG_CTRL15_02_CFGBLK_OFS               0x0050   /* Sensor Rx Mapping 2 */
  #define SYNA_F54_ANALOG_CTRL15_03_CFGBLK_OFS               0x0051   /* Sensor Rx Mapping 3 */
  #define SYNA_F54_ANALOG_CTRL15_04_CFGBLK_OFS               0x0052   /* Sensor Rx Mapping 4 */
  #define SYNA_F54_ANALOG_CTRL15_05_CFGBLK_OFS               0x0053   /* Sensor Rx Mapping 5 */
  #define SYNA_F54_ANALOG_CTRL15_06_CFGBLK_OFS               0x0054   /* Sensor Rx Mapping 6 */
  #define SYNA_F54_ANALOG_CTRL15_07_CFGBLK_OFS               0x0055   /* Sensor Rx Mapping 7 */
  #define SYNA_F54_ANALOG_CTRL15_08_CFGBLK_OFS               0x0056   /* Sensor Rx Mapping 8 */
  #define SYNA_F54_ANALOG_CTRL15_09_CFGBLK_OFS               0x0057   /* Sensor Rx Mapping 9 */
  #define SYNA_F54_ANALOG_CTRL15_10_CFGBLK_OFS               0x0058   /* Sensor Rx Mapping 10 */
  #define SYNA_F54_ANALOG_CTRL15_11_CFGBLK_OFS               0x0059   /* Sensor Rx Mapping 11 */
  #define SYNA_F54_ANALOG_CTRL15_12_CFGBLK_OFS               0x005A   /* Sensor Rx Mapping 12 */
  #define SYNA_F54_ANALOG_CTRL15_13_CFGBLK_OFS               0x005B   /* Sensor Rx Mapping 13 */
  #define SYNA_F54_ANALOG_CTRL15_14_CFGBLK_OFS               0x005C   /* Sensor Rx Mapping 14 */
  #define SYNA_F54_ANALOG_CTRL15_15_CFGBLK_OFS               0x005D   /* Sensor Rx Mapping 15 */
  #define SYNA_F54_ANALOG_CTRL15_16_CFGBLK_OFS               0x005E   /* Sensor Rx Mapping 16 */
  #define SYNA_F54_ANALOG_CTRL15_17_CFGBLK_OFS               0x005F   /* Sensor Rx Mapping 17 */
  #define SYNA_F54_ANALOG_CTRL15_18_CFGBLK_OFS               0x0060   /* Sensor Rx Mapping 18 */
  #define SYNA_F54_ANALOG_CTRL15_19_CFGBLK_OFS               0x0061   /* Sensor Rx Mapping 19 */
  #define SYNA_F54_ANALOG_CTRL16_00_CFGBLK_OFS               0x0062   /* Sensor Tx Mapping 0 */
  #define SYNA_F54_ANALOG_CTRL16_01_CFGBLK_OFS               0x0063   /* Sensor Tx Mapping 1 */
  #define SYNA_F54_ANALOG_CTRL16_02_CFGBLK_OFS               0x0064   /* Sensor Tx Mapping 2 */
  #define SYNA_F54_ANALOG_CTRL16_03_CFGBLK_OFS               0x0065   /* Sensor Tx Mapping 3 */
  #define SYNA_F54_ANALOG_CTRL16_04_CFGBLK_OFS               0x0066   /* Sensor Tx Mapping 4 */
  #define SYNA_F54_ANALOG_CTRL16_05_CFGBLK_OFS               0x0067   /* Sensor Tx Mapping 5 */
  #define SYNA_F54_ANALOG_CTRL16_06_CFGBLK_OFS               0x0068   /* Sensor Tx Mapping 6 */
  #define SYNA_F54_ANALOG_CTRL16_07_CFGBLK_OFS               0x0069   /* Sensor Tx Mapping 7 */
  #define SYNA_F54_ANALOG_CTRL16_08_CFGBLK_OFS               0x006A   /* Sensor Tx Mapping 8 */
  #define SYNA_F54_ANALOG_CTRL16_09_CFGBLK_OFS               0x006B   /* Sensor Tx Mapping 9 */
  #define SYNA_F54_ANALOG_CTRL16_10_CFGBLK_OFS               0x006C   /* Sensor Tx Mapping 10 */
  #define SYNA_F54_ANALOG_CTRL16_11_CFGBLK_OFS               0x006D   /* Sensor Tx Mapping 11 */
  #define SYNA_F54_ANALOG_CTRL16_12_CFGBLK_OFS               0x006E   /* Sensor Tx Mapping 12 */
  #define SYNA_F54_ANALOG_CTRL17_00_CFGBLK_OFS               0x006F   /* Sense Frequency Control0 0 */
  #define SYNA_F54_ANALOG_CTRL17_01_CFGBLK_OFS               0x0070   /* Sense Frequency Control0 1 */
  #define SYNA_F54_ANALOG_CTRL17_02_CFGBLK_OFS               0x0071   /* Sense Frequency Control0 2 */
  #define SYNA_F54_ANALOG_CTRL17_03_CFGBLK_OFS               0x0072   /* Sense Frequency Control0 3 */
  #define SYNA_F54_ANALOG_CTRL17_04_CFGBLK_OFS               0x0073   /* Sense Frequency Control0 4 */
  #define SYNA_F54_ANALOG_CTRL17_05_CFGBLK_OFS               0x0074   /* Sense Frequency Control0 5 */
  #define SYNA_F54_ANALOG_CTRL17_06_CFGBLK_OFS               0x0075   /* Sense Frequency Control0 6 */
  #define SYNA_F54_ANALOG_CTRL17_07_CFGBLK_OFS               0x0076   /* Sense Frequency Control0 7 */
  #define SYNA_F54_ANALOG_CTRL18_00_CFGBLK_OFS               0x0077   /* Sense Frequency Control Low 0 */
  #define SYNA_F54_ANALOG_CTRL18_01_CFGBLK_OFS               0x0078   /* Sense Frequency Control Low 1 */
  #define SYNA_F54_ANALOG_CTRL18_02_CFGBLK_OFS               0x0079   /* Sense Frequency Control Low 2 */
  #define SYNA_F54_ANALOG_CTRL18_03_CFGBLK_OFS               0x007A   /* Sense Frequency Control Low 3 */
  #define SYNA_F54_ANALOG_CTRL18_04_CFGBLK_OFS               0x007B   /* Sense Frequency Control Low 4 */
  #define SYNA_F54_ANALOG_CTRL18_05_CFGBLK_OFS               0x007C   /* Sense Frequency Control Low 5 */
  #define SYNA_F54_ANALOG_CTRL18_06_CFGBLK_OFS               0x007D   /* Sense Frequency Control Low 6 */
  #define SYNA_F54_ANALOG_CTRL18_07_CFGBLK_OFS               0x007E   /* Sense Frequency Control Low 7 */
  #define SYNA_F54_ANALOG_CTRL19_00_CFGBLK_OFS               0x007F   /* Sense Frequency Control2 0 */
  #define SYNA_F54_ANALOG_CTRL19_01_CFGBLK_OFS               0x0080   /* Sense Frequency Control2 1 */
  #define SYNA_F54_ANALOG_CTRL19_02_CFGBLK_OFS               0x0081   /* Sense Frequency Control2 2 */
  #define SYNA_F54_ANALOG_CTRL19_03_CFGBLK_OFS               0x0082   /* Sense Frequency Control2 3 */
  #define SYNA_F54_ANALOG_CTRL19_04_CFGBLK_OFS               0x0083   /* Sense Frequency Control2 4 */
  #define SYNA_F54_ANALOG_CTRL19_05_CFGBLK_OFS               0x0084   /* Sense Frequency Control2 5 */
  #define SYNA_F54_ANALOG_CTRL19_06_CFGBLK_OFS               0x0085   /* Sense Frequency Control2 6 */
  #define SYNA_F54_ANALOG_CTRL19_07_CFGBLK_OFS               0x0086   /* Sense Frequency Control2 7 */
  #define SYNA_F54_ANALOG_CTRL20_CFGBLK_OFS                  0x0087   /* Noise Mitigation General Control */
  #define SYNA_F54_ANALOG_CTRL21_00_CFGBLK_OFS               0x0088   /* HNM Frequency Shift Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL21_01_CFGBLK_OFS               0x0089   /* HNM Frequency Shift Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL22_CFGBLK_OFS                  0x008A   /* HNM Exit Density */
  #define SYNA_F54_ANALOG_CTRL23_00_CFGBLK_OFS               0x008B   /* Medium Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL23_01_CFGBLK_OFS               0x008C   /* Medium Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL24_00_CFGBLK_OFS               0x008D   /* High Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL24_01_CFGBLK_OFS               0x008E   /* High Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL25_CFGBLK_OFS                  0x008F   /* FNM Frequency Shift Density */
  #define SYNA_F54_ANALOG_CTRL26_CFGBLK_OFS                  0x0090   /* FNM Exit Threshold */
  #define SYNA_F54_ANALOG_CTRL27_CFGBLK_OFS                  0x0091   /* IIR Filter Coefficient */
  #define SYNA_F54_ANALOG_CTRL28_00_CFGBLK_OFS               0x0092   /* FNM Frequency Shift Noise Threshold Low */
  #define SYNA_F54_ANALOG_CTRL28_01_CFGBLK_OFS               0x0093   /* FNM Frequency Shift Noise Threshold High */
  #define SYNA_F54_ANALOG_CTRL29_CFGBLK_OFS                  0x0094   /* Common-Mode Noise Control */
  #define SYNA_F54_ANALOG_CTRL30_CFGBLK_OFS                  0x0095   /* CMN Cap Scale Factor */
  #define SYNA_F54_ANALOG_CTRL31_CFGBLK_OFS                  0x0096   /* Pixel Threshold Hysteresis */
  #define SYNA_F54_ANALOG_CTRL32_00_CFGBLK_OFS               0x0097   /* Rx LowEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL32_01_CFGBLK_OFS               0x0098   /* Rx LowEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL33_00_CFGBLK_OFS               0x0099   /* Rx HighEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL33_01_CFGBLK_OFS               0x009A   /* Rx HighEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL34_00_CFGBLK_OFS               0x009B   /* Tx LowEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL34_01_CFGBLK_OFS               0x009C   /* Tx LowEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL35_00_CFGBLK_OFS               0x009D   /* Tx HighEdge Compensation Low */
  #define SYNA_F54_ANALOG_CTRL35_01_CFGBLK_OFS               0x009E   /* Tx HighEdge Compensation High */
  #define SYNA_F54_ANALOG_CTRL36_00_CFGBLK_OFS               0x009F   /* Axis 1 Compensation 0 */
  #define SYNA_F54_ANALOG_CTRL36_01_CFGBLK_OFS               0x00A0   /* Axis 1 Compensation 1 */
  #define SYNA_F54_ANALOG_CTRL36_02_CFGBLK_OFS               0x00A1   /* Axis 1 Compensation 2 */
  #define SYNA_F54_ANALOG_CTRL36_03_CFGBLK_OFS               0x00A2   /* Axis 1 Compensation 3 */
  #define SYNA_F54_ANALOG_CTRL36_04_CFGBLK_OFS               0x00A3   /* Axis 1 Compensation 4 */
  #define SYNA_F54_ANALOG_CTRL36_05_CFGBLK_OFS               0x00A4   /* Axis 1 Compensation 5 */
  #define SYNA_F54_ANALOG_CTRL36_06_CFGBLK_OFS               0x00A5   /* Axis 1 Compensation 6 */
  #define SYNA_F54_ANALOG_CTRL36_07_CFGBLK_OFS               0x00A6   /* Axis 1 Compensation 7 */
  #define SYNA_F54_ANALOG_CTRL36_08_CFGBLK_OFS               0x00A7   /* Axis 1 Compensation 8 */
  #define SYNA_F54_ANALOG_CTRL36_09_CFGBLK_OFS               0x00A8   /* Axis 1 Compensation 9 */
  #define SYNA_F54_ANALOG_CTRL36_10_CFGBLK_OFS               0x00A9   /* Axis 1 Compensation 10 */
  #define SYNA_F54_ANALOG_CTRL36_11_CFGBLK_OFS               0x00AA   /* Axis 1 Compensation 11 */
  #define SYNA_F54_ANALOG_CTRL36_12_CFGBLK_OFS               0x00AB   /* Axis 1 Compensation 12 */
  #define SYNA_F54_ANALOG_CTRL36_13_CFGBLK_OFS               0x00AC   /* Axis 1 Compensation 13 */
  #define SYNA_F54_ANALOG_CTRL36_14_CFGBLK_OFS               0x00AD   /* Axis 1 Compensation 14 */
  #define SYNA_F54_ANALOG_CTRL36_15_CFGBLK_OFS               0x00AE   /* Axis 1 Compensation 15 */
  #define SYNA_F54_ANALOG_CTRL36_16_CFGBLK_OFS               0x00AF   /* Axis 1 Compensation 16 */
  #define SYNA_F54_ANALOG_CTRL36_17_CFGBLK_OFS               0x00B0   /* Axis 1 Compensation 17 */
  #define SYNA_F54_ANALOG_CTRL36_18_CFGBLK_OFS               0x00B1   /* Axis 1 Compensation 18 */
  #define SYNA_F54_ANALOG_CTRL36_19_CFGBLK_OFS               0x00B2   /* Axis 1 Compensation 19 */
  #define SYNA_F54_ANALOG_CTRL38_00_CFGBLK_OFS               0x00B3   /* Noise Control 1 0 */
  #define SYNA_F54_ANALOG_CTRL38_01_CFGBLK_OFS               0x00B4   /* Noise Control 1 1 */
  #define SYNA_F54_ANALOG_CTRL38_02_CFGBLK_OFS               0x00B5   /* Noise Control 1 2 */
  #define SYNA_F54_ANALOG_CTRL38_03_CFGBLK_OFS               0x00B6   /* Noise Control 1 3 */
  #define SYNA_F54_ANALOG_CTRL38_04_CFGBLK_OFS               0x00B7   /* Noise Control 1 4 */
  #define SYNA_F54_ANALOG_CTRL38_05_CFGBLK_OFS               0x00B8   /* Noise Control 1 5 */
  #define SYNA_F54_ANALOG_CTRL38_06_CFGBLK_OFS               0x00B9   /* Noise Control 1 6 */
  #define SYNA_F54_ANALOG_CTRL38_07_CFGBLK_OFS               0x00BA   /* Noise Control 1 7 */
  #define SYNA_F54_ANALOG_CTRL39_00_CFGBLK_OFS               0x00BB   /* Noise Control 2 0 */
  #define SYNA_F54_ANALOG_CTRL39_01_CFGBLK_OFS               0x00BC   /* Noise Control 2 1 */
  #define SYNA_F54_ANALOG_CTRL39_02_CFGBLK_OFS               0x00BD   /* Noise Control 2 2 */
  #define SYNA_F54_ANALOG_CTRL39_03_CFGBLK_OFS               0x00BE   /* Noise Control 2 3 */
  #define SYNA_F54_ANALOG_CTRL39_04_CFGBLK_OFS               0x00BF   /* Noise Control 2 4 */
  #define SYNA_F54_ANALOG_CTRL39_05_CFGBLK_OFS               0x00C0   /* Noise Control 2 5 */
  #define SYNA_F54_ANALOG_CTRL39_06_CFGBLK_OFS               0x00C1   /* Noise Control 2 6 */
  #define SYNA_F54_ANALOG_CTRL39_07_CFGBLK_OFS               0x00C2   /* Noise Control 2 7 */
  #define SYNA_F54_ANALOG_CTRL40_00_CFGBLK_OFS               0x00C3   /* Noise Control 3 0 */
  #define SYNA_F54_ANALOG_CTRL40_01_CFGBLK_OFS               0x00C4   /* Noise Control 3 1 */
  #define SYNA_F54_ANALOG_CTRL40_02_CFGBLK_OFS               0x00C5   /* Noise Control 3 2 */
  #define SYNA_F54_ANALOG_CTRL40_03_CFGBLK_OFS               0x00C6   /* Noise Control 3 3 */
  #define SYNA_F54_ANALOG_CTRL40_04_CFGBLK_OFS               0x00C7   /* Noise Control 3 4 */
  #define SYNA_F54_ANALOG_CTRL40_05_CFGBLK_OFS               0x00C8   /* Noise Control 3 5 */
  #define SYNA_F54_ANALOG_CTRL40_06_CFGBLK_OFS               0x00C9   /* Noise Control 3 6 */
  #define SYNA_F54_ANALOG_CTRL40_07_CFGBLK_OFS               0x00CA   /* Noise Control 3 7 */
  #define SYNA_F54_ANALOG_CTRL41_CFGBLK_OFS                  0x00CB   /* Multi Metric Noise Mitigation Control */
  #define SYNA_F54_ANALOG_CTRL42_00_CFGBLK_OFS               0x00CC   /* Variance Metric Threshold Low */
  #define SYNA_F54_ANALOG_CTRL42_01_CFGBLK_OFS               0x00CD   /* Variance Metric Threshold High */
  #define SYNA_F54_ANALOG_CTRL55_CFGBLK_OFS                  0x00CE   /* 0D Slow Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL56_CFGBLK_OFS                  0x00CF   /* 0D Fast Relaxation Rate */
  #define SYNA_F54_ANALOG_CTRL57_CFGBLK_OFS                  0x00D0   /* 0D CBC Settings */
  #define SYNA_F54_ANALOG_CTRL58_CFGBLK_OFS                  0x00D1   /* 0D Acquisition Scheme */
  #define SYNA_F31_LED_CTRL00_00_CFGBLK_OFS                  0x00D2   /* LED Brightness 0 */
  #define SYNA_F31_LED_CTRL00_01_CFGBLK_OFS                  0x00D3   /* LED Brightness 1 */
  #define SYNA_F31_LED_CTRL00_02_CFGBLK_OFS                  0x00D4   /* LED Brightness 2 */
  #define SYNA_F31_LED_CTRL00_03_CFGBLK_OFS                  0x00D5   /* LED Brightness 3 */
  #define SYNA_F31_LED_CTRL00_04_CFGBLK_OFS                  0x00D6   /* LED Brightness 4 */
  #define SYNA_F31_LED_CTRL00_05_CFGBLK_OFS                  0x00D7   /* LED Brightness 5 */
  #define SYNA_F31_LED_CTRL00_06_CFGBLK_OFS                  0x00D8   /* LED Brightness 6 */
  #define SYNA_F31_LED_CTRL00_07_CFGBLK_OFS                  0x00D9   /* LED Brightness 7 */
  #define SYNA_F1A_0D_CTRL00_CFGBLK_OFS                      0x00DA   /* General Control */
  #define SYNA_F1A_0D_CTRL01_CFGBLK_OFS                      0x00DB   /* Button Interrupt Enable */
  #define SYNA_F1A_0D_CTRL02_CFGBLK_OFS                      0x00DC   /* MultiButton Group Selection */
  #define SYNA_F1A_0D_CTRL03_00_CFGBLK_OFS                   0x00DD   /* Tx Button 0 */
  #define SYNA_F1A_0D_CTRL04_00_CFGBLK_OFS                   0x00DE   /* Rx Button 0 */
  #define SYNA_F1A_0D_CTRL03_01_CFGBLK_OFS                   0x00DF   /* Tx Button 1 */
  #define SYNA_F1A_0D_CTRL04_01_CFGBLK_OFS                   0x00E0   /* Rx Button 1 */
  #define SYNA_F1A_0D_CTRL03_02_CFGBLK_OFS                   0x00E1   /* Tx Button 2 */
  #define SYNA_F1A_0D_CTRL04_02_CFGBLK_OFS                   0x00E2   /* Rx Button 2 */
  #define SYNA_F1A_0D_CTRL03_03_CFGBLK_OFS                   0x00E3   /* Tx Button 3 */
  #define SYNA_F1A_0D_CTRL04_03_CFGBLK_OFS                   0x00E4   /* Rx Button 3 */
  #define SYNA_F1A_0D_CTRL05_00_CFGBLK_OFS                   0x00E5   /* Touch Threshold Button 0 */
  #define SYNA_F1A_0D_CTRL05_01_CFGBLK_OFS                   0x00E6   /* Touch Threshold Button 1 */
  #define SYNA_F1A_0D_CTRL05_02_CFGBLK_OFS                   0x00E7   /* Touch Threshold Button 2 */
  #define SYNA_F1A_0D_CTRL05_03_CFGBLK_OFS                   0x00E8   /* Touch Threshold Button 3 */
  #define SYNA_F1A_0D_CTRL06_CFGBLK_OFS                      0x00E9   /* Release Threshold */
  #define SYNA_F1A_0D_CTRL07_CFGBLK_OFS                      0x00EA   /* Strongest Button Hysteresis */
  #define SYNA_F1A_0D_CTRL08_CFGBLK_OFS                      0x00EB   /* Filter Strength */
  #define SYNA_CFGBLK_CRC1_CFGBLK_OFS                        0x01FC   /* Configuration CRC [7:0] */
  #define SYNA_CFGBLK_CRC2_CFGBLK_OFS                        0x01FD   /* Configuration CRC [15:8] */
  #define SYNA_CFGBLK_CRC3_CFGBLK_OFS                        0x01FE   /* Configuration CRC [23:16] */
  #define SYNA_CFGBLK_CRC4_CFGBLK_OFS                        0x01FF   /* Configuration CRC [31:24] */

  /* Masks for interrupt sources */

  /*      Symbol Name                                        Mask        Description */
  /*      -----------                                        ----        ----------- */
  #define SYNA_F01_RMI_INT_SOURCE_MASK_ALL                   0x0002   /* Mask of all Func $01 (RMI) interrupts */
  #define SYNA_F01_RMI_INT_SOURCE_MASK_STATUS                0x0002   /* Mask of Func $01 (RMI) 'STATUS' interrupt */
  #define SYNA_F11_2D_INT_SOURCE_MASK_ABS0                   0x0004   /* Mask of Func $11 (2D) 'ABS0' interrupt */
  #define SYNA_F11_2D_INT_SOURCE_MASK_ALL                    0x0004   /* Mask of all Func $11 (2D) interrupts */
  #define SYNA_F1A_0D_INT_SOURCE_MASK_ALL                    0x0020   /* Mask of all Func $1A (0D) interrupts */
  #define SYNA_F1A_0D_INT_SOURCE_MASK_BUTTON                 0x0020   /* Mask of Func $1A (0D) 'BUTTON' interrupt */
  #define SYNA_F31_LED_INT_SOURCE_MASK_ALL                   0x0010   /* Mask of all Func $31 (LED) interrupts */
  #define SYNA_F31_LED_INT_SOURCE_MASK_LED                   0x0010   /* Mask of Func $31 (LED) 'LED' interrupt */
  #define SYNA_F34_FLASH_INT_SOURCE_MASK_ALL                 0x0001   /* Mask of all Func $34 (FLASH) interrupts */
  #define SYNA_F34_FLASH_INT_SOURCE_MASK_FLASH               0x0001   /* Mask of Func $34 (FLASH) 'FLASH' interrupt */
  #define SYNA_F54_ANALOG_INT_SOURCE_MASK_ALL                0x0008   /* Mask of all Func $54 (ANALOG) interrupts */
  #define SYNA_F54_ANALOG_INT_SOURCE_MASK_ANALOG             0x0008   /* Mask of Func $54 (ANALOG) 'ANALOG' interrupt */

#endif  /* SYNA_REGISTER_MAP_H */

