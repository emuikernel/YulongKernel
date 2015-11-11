/*
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2011 Synaptics, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
   Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.


   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

// SynaFirmwareImage.h contains the data for both the entire image and the config block
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/rmi.h>
#include <linux/delay.h>
#include "touchscreen.h"
//#include "registerMap.h"

// add by ck //firmware&config file
/*xiongdajun update TW firmware to V0103*/
#if defined(CONFIG_BOARD_CP8870U)
static unsigned char CTPM_FW_OFILM[] = {
	#include "Coolpad_8870U_Ofilm_V0103_20130513"
};

static unsigned char CTPM_FW_YEJI[] = {
	#include "Coolpad_8870U_Eachopto_V0103_20130513"
};
#elif defined(CONFIG_BOARD_CP8860U) || defined(CONFIG_BOARD_CP8861U)
static unsigned char CTPM_FW_OFILM[] = {
	#include "Coolpad_8860U_COB_V0103_20121207"
};

static unsigned char CTPM_FW_YEJI[] = {
	#include "Coolpad_8860U_COB_V0103_20121207"
};
#else
static unsigned char CTPM_FW_OFILM[] = {
	//#include "SynaImage_5216_ofilm_0x02_0x01"//randy add for COB first update  20121008
	//#include "Coolpad_MTR3_COB_V0103_20121026"//randy add for COB first update  20121008
	#include "Coolpad_QC1_COB_V0204_20130221"//yuhao add
};

static unsigned char CTPM_FW_YEJI[] = {//randy add for name of fw that truly to yeji 20121212
	//#include "SynaImage_5216_yeji_0x02_0x01"//randy add for COB first update  20121008
	//#include "Coolpad_MTR3_COB_V0103_20121026"//randy add for COB first update  20121008
	#include "Coolpad_QC1_COB_V0204_20130221"//yuhao add
};
#endif

/************************* randy change for  update when power on  *************************/
//#define			ENABLE_POWERON_UPDATE
//#define		ENABLE_POWERON_FORCE_UPDATE
/************************* randy change for  update when power on  *************************/

unsigned char *SynaFirmware = NULL;
unsigned char SynaProductor[10]="unknown";
//?end add by ck?

/* Variables for F34 functionality */
unsigned short SynaF34DataBase;
unsigned short SynaF34QueryBase;
unsigned short SynaF01DataBase;
unsigned short SynaF01CommandBase;
unsigned short SynaF01QueryBase;
unsigned short SynaF34ControlBase;

unsigned short SynaF34Reflash_BlockNum;
unsigned short SynaF34Reflash_BlockData;
unsigned short SynaF34ReflashQuery_BootID;
unsigned short SynaF34ReflashQuery_FlashPropertyQuery;
unsigned short SynaF34ReflashQuery_FirmwareBlockSize;
unsigned short SynaF34ReflashQuery_FirmwareBlockCount;
unsigned short SynaF34ReflashQuery_ConfigBlockSize;
unsigned short SynaF34ReflashQuery_ConfigBlockCount;

unsigned short SynaFirmwareBlockSize;
unsigned short SynaFirmwareBlockCount;
unsigned long SynaImageSize;

unsigned short SynaConfigBlockSize;
unsigned short SynaConfigBlockCount;
unsigned long SynaConfigImageSize;

unsigned short SynaBootloadID;

unsigned short SynaF34_FlashControl;

unsigned char *SynafirmwareImgData;
unsigned char *SynaconfigImgData;
unsigned char *SynalockImgData;
unsigned int SynafirmwareImgVersion;

unsigned char ConfigBlock[520];

/* End: Variables for F34 functionality */

unsigned char debug_flag = 0; 	//0;  Ben change fotr debug refresh  //randy  change for update

struct rmi_device * upflash_rmi_device;


int readRMI(unsigned short addr, unsigned char *buf, int len)
{
        int ret = 0;
        if(!upflash_rmi_device)
        {
                printk("upflash_rmi_device = NULL!!\n");
                return (-1);
        }

        ret = rmi_read_block( upflash_rmi_device, addr, buf, len);
        if(ret != len)
        {
                printk("readRMI fail: ret = %d..\n", ret);
                return (-1);
        }

        return 0;
}

int writeRMI(unsigned short addr, unsigned char *buf, int len)
{
        int ret = 0;
        if(!upflash_rmi_device)
        {
                printk("upflash_rmi_device = NULL!!\n");
                return (-1);
        }

        ret = rmi_write_block( upflash_rmi_device, addr, buf, len);
        if(ret != len)
        {
                printk("writeRMI fail: ret = %d..\n", ret);
                return (-1);
        }

        return 0;
}


/* SynaWaitForATTN waits for ATTN to be asserted within a certain time threshold.
 */
void SynaWaitForATTN(void)
{
//    unsigned int error;
//	error = waitATTN(300);
        msleep(10);
}

/* SynaWaitATTN waits for ATTN to be asserted within a certain time threshold.
 * The function also checks for the F34 "Program Enabled" bit and clear ATTN accordingly.
 */
int SynaWaitATTN(void)
{
        unsigned char uData;
        unsigned char uStatus;
        unsigned int timeout = 1000;

        for(; timeout > 0; timeout--)
        {
                readRMI(SynaF34_FlashControl, &uData, 1);
                readRMI((SynaF01DataBase + 1), &uStatus, 1);

                if(uData==0x80)
                        return 0;

                msleep(1);
        }

        return (-1);
}

int waitRegComplete(unsigned short addr, unsigned char mask, unsigned char value, unsigned int timeout)
{
        unsigned char uData = 0;

        for(; timeout > 0; timeout--)
        {
                readRMI(addr, &uData, 1);
                if( (uData & mask) == value)
                        return 0;

                msleep(1);
        }

        return (-1);
}

/* SynaSetup scans the Page Description Table (PDT) and sets up the necessary variables
 * for the reflash process. This function is a "slim" version of the PDT scan function in
 * in PDT.c, since only F34 and F01 are needed for reflash.
 */
int SynaSetup(void)
{
        unsigned char address;
        unsigned char buffer[6];

        for (address = 0xe9; address > 0xc0; address = address - 6)
        {
                readRMI(address, buffer, 6);

                printk("F34: %x,%x,%x,%x,%x,%x..\n", buffer[0],buffer[1],buffer[2],
                			buffer[3],buffer[4],buffer[5]);

                switch (buffer[5])
                {
                case 0x34:
                	SynaF34DataBase = buffer[3];
                	SynaF34QueryBase = buffer[0];
                	SynaF34ControlBase = buffer[2];
                	break;
                case 0x01:
                	SynaF01DataBase = buffer[3];
                	SynaF01CommandBase = buffer[1];
                	SynaF01QueryBase = buffer[0];
                	break;
                }
        }

        SynaF34Reflash_BlockNum = SynaF34DataBase;
        SynaF34Reflash_BlockData = SynaF34DataBase + 2;
        SynaF34ReflashQuery_BootID = SynaF34QueryBase;
        SynaF34ReflashQuery_FlashPropertyQuery = SynaF34QueryBase + 2;
        SynaF34ReflashQuery_FirmwareBlockSize = SynaF34QueryBase + 3;
        SynaF34ReflashQuery_FirmwareBlockCount = SynaF34QueryBase +5;
        SynaF34ReflashQuery_ConfigBlockSize = SynaF34QueryBase + 3;
        SynaF34ReflashQuery_ConfigBlockCount = SynaF34QueryBase + 7;

        return 0;
}

/* SynaInitialize sets up the reflahs process
 */
int SynaInitialize(void)
{
        unsigned char uData[2];
        unsigned char uStatus = 0;

        printk("\nInitializing Reflash Process...\n");

        writeRMI(0xff, &uStatus, 1); // switch to page0

        waitRegComplete(0, 0x80, 0, 1000);

        SynaSetup();

        readRMI(SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);

        SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);

        return 0;
}

/* SynaReadFirmwareInfo reads the F34 query registers and retrieves the block size and count
 * of the firmware section of the image to be reflashed
 */
int SynaReadFirmwareInfo(void)
{
        unsigned char uData[2];

        printk("Read Firmware Info\n");

        readRMI(SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);
        SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);

        readRMI(SynaF34ReflashQuery_FirmwareBlockCount, &uData[0], 2);
        SynaFirmwareBlockCount = uData[0] | (uData[1] << 8);
        SynaImageSize = SynaFirmwareBlockCount * SynaFirmwareBlockSize;
        SynaconfigImgData   = (unsigned char *)(SynafirmwareImgData+SynaImageSize);

        printk("Firmware: blocksize = %x, blockcount=%x..\n", SynaFirmwareBlockSize, SynaFirmwareBlockCount);

        return 0;
}

/* SynaReadConfigInfo reads the F34 query registers and retrieves the block size and count
 * of the configuration section of the image to be reflashed
 */
int SynaReadConfigInfo(void)
{
        unsigned char uData[2];

        printk("Read Config Info\n");

        readRMI(SynaF34ReflashQuery_ConfigBlockSize, &uData[0], 2);
        SynaConfigBlockSize = uData[0] | (uData[1] << 8);

        readRMI(SynaF34ReflashQuery_ConfigBlockCount, &uData[0], 2);
        SynaConfigBlockCount = uData[0] | (uData[1] << 8);
        SynaConfigImageSize = SynaConfigBlockCount * SynaConfigBlockSize;

        printk("Config: blocksize = %x, blockcount=%x..\n", SynaConfigBlockCount, SynaConfigBlockSize);
        return 0;
}


/* SynaReadBootloadID reads the F34 query registers and retrieves the bootloader ID of the firmware
 */
int SynaReadBootloadID(void)
{
        unsigned char uData[2];

        readRMI(SynaF34ReflashQuery_BootID, &uData[0], 2);
        SynaBootloadID = uData[0] + uData[1] * 0x100;
        return 0;
}

/* SynaWriteBootloadID writes the bootloader ID to the F34 data register to unlock the reflash process
 */
int SynaWriteBootloadID(void)
{
        unsigned char uData[2];

        uData[0] = SynaBootloadID % 0x100;
        uData[1] = SynaBootloadID / 0x100;

        writeRMI(SynaF34Reflash_BlockData, &uData[0], 2);
        return 0;
}

/* SynaEnableFlashing kicks off the reflash process
 */
int SynaEnableFlashing(void)
{
        unsigned char uData;
        unsigned char uStatus;

        printk("Enable Reflash...\n");

        // Reflash is enabled by first reading the bootloader ID from the firmware and write it back
        SynaReadBootloadID();
        SynaWriteBootloadID();

        // Make sure Reflash is not already enabled
        waitRegComplete(SynaF34_FlashControl, 0x0f, 0, 1000);

        // Clear ATTN
        readRMI (SynaF01DataBase, &uStatus, 1);

        if ((uStatus &0x40) == 0)
        {
                // Write the "Enable Flash Programming command to F34 Control register
                // Wait for ATTN and then clear the ATTN.
                uData = 0x0f;
                writeRMI(SynaF34_FlashControl, &uData, 1);
                SynaWaitForATTN();
                readRMI((SynaF01DataBase + 1), &uStatus, 1);

                // Scan the PDT again to ensure all register offsets are correct
                SynaSetup();

                // Read the "Program Enabled" bit of the F34 Control register, and proceed only if the
                // bit is set.
                readRMI(SynaF34_FlashControl, &uData, 1);

                while (uData != 0x80)
                {
			printk("in enable state error \n");
                        // In practice, if uData!=0x80 happens for multiple counts, it indicates reflash
                        // is failed to be enabled, and program should quit
                        return 0;
                }
        }


        return 0;
}


/* SynaProgramConfiguration writes the configuration section of the image block by block
 */
int SynaFlashConfigWrite(void)
{
        unsigned char uData[2];
        unsigned char *puData = SynaconfigImgData;  //(unsigned char *)(SynaFirmware+0xb100);
        unsigned short blockNum;

        printk("Program Configuration Section...\n");

        for (blockNum = 0; blockNum < SynaConfigBlockCount; blockNum++)
        {
                uData[0] = blockNum & 0xff;
                uData[1] = (blockNum & 0xff00) >> 8;

                //Block by blcok, write the block number and data to the corresponding F34 data registers
                writeRMI(SynaF34Reflash_BlockNum, &uData[0], 2);
                writeRMI(SynaF34Reflash_BlockData, puData, SynaConfigBlockSize);
                puData += SynaConfigBlockSize;

                // Issue the "Write Configuration Block" command
                uData[0] = 0x06;
                writeRMI(SynaF34_FlashControl, &uData[0], 1);

                SynaWaitATTN();
                //printk("blockNum = %d...\n", blockNum);
        }
        return 0;
}

/* SynaFlashFirmwareWrite writes the firmware section of the image block by block
 */
void SynaFlashFirmwareWrite(void)
{
        unsigned char *puFirmwareData = SynafirmwareImgData;
        unsigned char uData[2];
        unsigned short blockNum;

        printk("SynaFlashFirmwareWrite...\n");
        for (blockNum = 0; blockNum < SynaFirmwareBlockCount; ++blockNum)
        {
                //Block by blcok, write the block number and data to the corresponding F34 data registers
                uData[0] = blockNum & 0xff;
                uData[1] = (blockNum & 0xff00) >> 8;
                writeRMI(SynaF34Reflash_BlockNum, &uData[0], 2);

                writeRMI(SynaF34Reflash_BlockData, puFirmwareData, SynaFirmwareBlockSize);
                puFirmwareData += SynaFirmwareBlockSize;

                // Issue the "Write Firmware Block" command
                uData[0] = 2;
                writeRMI(SynaF34_FlashControl, &uData[0], 1);

                SynaWaitATTN();
                //printk("blockNum = %d...\n", blockNum);
        }
}

void eraseFirmwareConfigBlock(char mode)
{
        unsigned char uData;

        // Erase of firmware and config block is done by first entering into bootloader mode
        SynaReadBootloadID();
        SynaWriteBootloadID();

        if(mode == 0)
        {
                // Command 3 to erase firmware and config block
                uData = 3;
                writeRMI(SynaF34_FlashControl, &uData, 1);
                msleep(3000);
        }
        else
        {
                // Command 7 to erase config block
                uData = 7;
                writeRMI(SynaF34_FlashControl, &uData, 1);
                msleep(200);
        }

        SynaWaitATTN();
}


/* SynaProgramFirmware prepares the firmware writing process
 */
void SynaProgramFirmwareConfig(int mode)
{
 	unsigned char uData[10] = {0};

	unsigned char * revision = SynaconfigImgData;

        printk("SynaProgramFirmwareConfig...\n");

        readRMI(SynaF34ControlBase, &uData[0], 4);
	printk("Read Data : D1 = %4d, R1 = %4d, D3 = %4d, R3 = %4d -->(%4x, %4x)\n", uData[1], revision[1], uData[3], revision[3], uData[0], uData[2]);

//randy pan change for update 20121012
        if(mode == 0)
        {
#ifdef	ENABLE_POWERON_FORCE_UPDATE	//randy change for force update when power on
        	printk("Mode-0 : First  update firmware && config\n");
		eraseFirmwareConfigBlock(0);	//erase firmware && config
		SynaFlashFirmwareWrite();
		SynaFlashConfigWrite();
		return;
#else
	if( uData[1] < revision[1] )		//check firmware_id change? update firmware
	{
		printk("Mode-0 : Update firmware && config\n");
		eraseFirmwareConfigBlock(0);	//erase firmware && config
		SynaFlashFirmwareWrite();
	        SynaFlashConfigWrite();
		return;
	}
	else if( uData[1] == revision[1] )
	{
		if(uData[3] < revision[3] )	//update config
		{
			printk("Mode-0 : Update config\n");
			eraseFirmwareConfigBlock(1);	//erase config
			SynaFlashConfigWrite();
			return;
		}
		else
		{
			printk("Version01 : %s:s2202:0x%02x:0x%02x is New !!!",SynaProductor, uData[1], uData[3]);
			return;
		}
	}
	else
	{
		printk("Version02 : %s:s2202:0x%02x:0x%02x is New !!!",SynaProductor, uData[1], uData[3]);
		return;
	}
#endif
        }
        else
        {
              if( uData[1] < revision[1] )		//check firmware_id change? update firmware
		{
			printk("Mode-1 : Update firmware && config\n");
			eraseFirmwareConfigBlock(0);	//erase firmware && config
			SynaFlashFirmwareWrite();
		        SynaFlashConfigWrite();
			return;
		}
		else if( uData[1] == revision[1] )
		{
			if(uData[3] < revision[3] )	//update config
			{
				printk("Mode-1 : Update config\n");
				eraseFirmwareConfigBlock(1);	//erase config
				SynaFlashConfigWrite();
				return;
			}
			else
			{
				printk("Version11 : %s:s2202:0x%02x:0x%02x is New !!!",SynaProductor, uData[1], uData[3]);
				return;
			}
		}
		else
		{
			printk("Version12 : %s:s2202:0x%02x:0x%02x is New !!!",SynaProductor, uData[1], uData[3]);
			return;
		}
        }
//randy pan change for update 20121012
}


/* SynaFinalizeReflash finalizes the reflash process
 */
void SynaFinalizeReflash(void)
{
        unsigned char uData;
        unsigned char uStatus;

        printk("Finalizing Reflash...\n");

        // Issue the "Reset" command to F01 command register to reset the chip
        // This command will also test the new firmware image and check if its is valid
        uData = 1;
        writeRMI(SynaF01CommandBase, &uData, 1);
	mdelay(10);
        SynaWaitForATTN();
        readRMI(SynaF01DataBase, &uData, 1);

        // Sanity check that the reflash process is still enabled
        waitRegComplete(SynaF34_FlashControl, 0x0f, 0, 1000);

        readRMI((SynaF01DataBase + 1), &uStatus, 1);

        SynaSetup();

        // Check if the "Program Enabled" bit in F01 data register is cleared
        // Reflash is completed, and the image passes testing when the bit is cleared
        waitRegComplete(SynaF01DataBase, 0x40, 0, 1000);

        // Rescan PDT the update any changed register offsets
        SynaSetup();

        printk("Reflash Completed. Please reboot.\n");

}


/* SynaBootloaderLock locks down the bootloader
*/
void SynaBootloaderLock(void)
{
        unsigned short lockBlockCount;
        unsigned char *puFirmwareData = SynalockImgData;
        unsigned char uData[2];
        unsigned short uBlockNum;

        // Check if device is in unlocked state
        readRMI((SynaF34QueryBase+ 2), &uData[0], 1);

        //Device is unlocked
        if (uData[0] & 0x02)
        {
                printk("Device unlocked. Lock it first...\n");
                // Different bootloader version has different block count for the lockdown data
                // Need to check the bootloader version from the image file being reflashed
                switch (SynafirmwareImgVersion)
                {
                case 2:
                        lockBlockCount = 3;
                        break;
                case 3:
                case 4:
                        lockBlockCount = 4;
                        break;
                case 5:
                        lockBlockCount = 5;
                        break;
                default:
                        lockBlockCount = 0;
                        break;
                }

                // Write the lockdown info block by block
                // This reference code of lockdown process does not check for bootloader version
                // currently programmed on the ASIC against the bootloader version of the image to
                // be reflashed. Such case should not happen in practice. Reflashing cross different
                // bootloader versions is not supported.
                for (uBlockNum = 0; uBlockNum < lockBlockCount; ++uBlockNum)
                {
                        uData[0] = uBlockNum & 0xff;
                        uData[1] = (uBlockNum & 0xff00) >> 8;

                        /* Write Block Number */
                        writeRMI(SynaF34Reflash_BlockNum, &uData[0], 2);

                        /* Write Data Block */
                        writeRMI(SynaF34Reflash_BlockData, puFirmwareData, SynaFirmwareBlockSize);

                        /* Move to next data block */
                        puFirmwareData += SynaFirmwareBlockSize;

                        /* Issue Write Lockdown Block command */
                        uData[0] = 4;
                        writeRMI(SynaF34_FlashControl, &uData[0], 1);

                        /* Wait ATTN until device is done writing the block and is ready for the next. */
                        SynaWaitATTN();
                }
                printk("Device locking done.\n");

                // Enable reflash again to finish the lockdown process.
                // Since this lockdown process is part of the reflash process, we are enabling
                // reflash instead, rather than resetting the device to finish the unlock procedure.
 		// SynaEnableFlashing();
        }
        else
                printk("Device already locked.\n");
}


bool ConfigRevisionCheck(void)
{
        unsigned char uData[10] = {0};

        unsigned char * revision = SynaconfigImgData;


        readRMI(SynaF34ControlBase, &uData[0], 4);

        printk("%s:Config ID: %x %x %x %x \n", __func__,uData[0],uData[1],uData[2],uData[3]);

        if(uData[0] != revision[0] ||uData[1] < revision[1] ||
                uData[2] != revision[2] ||uData[3] < revision[3])
	{
	        printk("%s:  FW Need Update \n", __func__);
	        return 1;       //need update
	}
	else
	{
	        printk("%s:  FW Need Not Update \n", __func__);
	        return 0;       //need  Not update
	}

}

bool DefauleConfigRevisionCheck(void)
{
        unsigned char uData[10] = {0};

        readRMI(SynaF34ControlBase, &uData[0], 4);

        printk("%s:Config ID: %x %x %x %x \n",__func__,uData[0],uData[1],uData[2],uData[3]);

        if(uData[0] == 0x55 && uData[2] == 0xAA)
/*
#ifdef	ENABLE_POWERON_UPDATE	//randy change for  update when power on
                return 1;     //randy pan change 0->1 for power on to force update
#else
		  return 0;
#endif
        else
                return 1;   //default config, need update
                */
	{
	#ifdef	ENABLE_POWERON_UPDATE	//randy change for  update when power on
	        printk("%s:  FW Need Update \n", __func__);
	        return 1;
	#else
	        printk("%s:  FW Need Not Update \n", __func__);
	        return 0;
	#endif
	}
	else
	{
	        printk("%s:  FW Need Update \n", __func__);
	        return 1;
	}

}


bool UImodeCheck(void)
{
        unsigned char uData[10] = {0};

        msleep(50);
        readRMI(SynaF01DataBase, &uData[0], 1);

        printk("IC mode:  %x \n", uData[0]);

        if(uData[0] & 0x40)
        {
                msleep(50);
                readRMI(SynaF01DataBase, &uData[0], 1);
                if(uData[0] & 0x40)
                        return 1;   //bootloader mode, need update
        }

        return 0;
}

//check config need update?
//return 1: need update
int CheckConfigUpdate(int mode)
{
        SynaInitialize();

        SynaReadConfigInfo();

        SynaReadFirmwareInfo();

        SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;

        printk("SynaF34_FlashControl = %x...\n", SynaF34_FlashControl);

        if(UImodeCheck())
        {
                return 1;
        }

        if( debug_flag )
        {
                return 1;
        }

        if(mode == 0)      //power on
        {
                return DefauleConfigRevisionCheck();
        }
        else                    //factory
        {
                return ConfigRevisionCheck();
        }

}

/* CompleteReflash reflashes the entire user image, including the configuration block and firmware
*  mode=0: power on
*  mode=1: factory
*/
int CompleteReflash(int mode)
{
       if(CheckConfigUpdate(mode))
        {
                SynaEnableFlashing();
		//SynaBootloaderLock();
                SynaProgramFirmwareConfig(mode);//randy pan change for update 20121012
                SynaBootloaderLock();// add by ck
                SynaFinalizeReflash();
        }
        return 0;
}

void DefaultReflash(void)
{
      CompleteReflash(0);
}

// check sensor productor
static int syna_get_sensorpid(void)
{
	int pid=-1;
	unsigned char uStatus;
	unsigned short pin_status;
	unsigned char uData[2];
	int retval;
	printk("enter in %s \n",__func__);
//step 1. Enable Flashing...
	printk("step 1. enable flashing... \n");
	// switch to page0
 	uStatus = 0;
        writeRMI(0xff, &uStatus, 1);
        waitRegComplete(0, 0x80, 0, 1000);
//add 2012.9.26
        SynaSetup();
        readRMI(SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);
        SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);
	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;
//end 2012.9.26
	//read bootloadID && write it back
        readRMI(SynaF34QueryBase, &uData[0], 2);
        SynaBootloadID = uData[0] + uData[1] * 0x100;
        uData[0] = SynaBootloadID % 0x100;
        uData[1] = SynaBootloadID / 0x100;
        writeRMI(SynaF34Reflash_BlockData, &uData[0], 2);

	waitRegComplete(SynaF34_FlashControl, 0x0f, 0, 1000);
        // Clear ATTN
        readRMI (SynaF01DataBase, &uStatus, 1);
        if ((uStatus & 0x40) == 0)
        {
                // Write the "Enable Flash Programming command to F34 Control register
                // Wait for ATTN and then clear the ATTN.
                uData[0] = 0x0F;
                writeRMI(SynaF34_FlashControl, &uData[0], 1);
   		mdelay(10);
                readRMI(SynaF01DataBase+1, &uStatus, 1);

                // Scan the PDT again to ensure all register offsets are correct
                SynaSetup();

                // Read the "Program Enabled" bit of the F34 Control register, and proceed only if the
                // bit is set.
                readRMI(SynaF34_FlashControl, &uData[0], 1);
		if(uData[0] != 0x80)
                {
			printk("Program enable error!\n");
			return -1;
                }
        }

//step 2. write 16bit gpio PinMask
	printk("step 2. write 16bit gpio PinMask \n");
	uData[0] = 0x0F;
	uData[1] = 0x00;
	writeRMI(SynaF34Reflash_BlockData,&uData[0],2);

//step 3. write 16bit PullupMask
	printk("step 3. write 16bit PullupMask \n");
	uData[0] = 0x0F;
	uData[1] = 0x00;
	writeRMI(SynaF34Reflash_BlockData+2,&uData[0],2);

//step 4. Execute F$34_FLASH_DATA3,writing to the command 0x08
	printk("step 4. Execute F$34_FLASH_DATA3 \n");
	uData[0] = 0x08;
	writeRMI(SynaF34_FlashControl,&uData[0],1);
	mdelay(10);
	readRMI(SynaF34_FlashControl,&uData[0],1);
	if(uData[0] != 0x80)
	{
		printk("error,oh my god !\n");
		goto EXIT_BOOTLOADER;
	}

//step 5. read sensor id pin status
	printk("step 5. read sensor id pin status \n");
	readRMI(SynaF34Reflash_BlockData+4,&uData[0],2);
	pin_status = ((uData[1]<<8) | uData[0]);
	printk("pin_status = 0x%x \n",pin_status);

//step 6. get sensor id vaule
	printk("step 6. get tp sensor_id \n");
	switch(pin_status)
	{
	case 0x00:
		pid = 0;
		break;
	case 0x0c:
		pid = 1;
		break;
	case 0x03:
		pid = 0;
		break;
	case 0x0f:
		pid = 1;
		break;
	default:
		printk("Get Pin_status Error!\n");
		pid =-1;
		break;
	}
	printk("product id = %d \n",pid);

EXIT_BOOTLOADER:
//step 7. Reset Tp ; bootloader to UI mode
	// Issue the "Reset" command to F01 command register to reset the chip
        // This command will also test the new firmware image and check if its is valid
	printk("step 7. reset tp:mode change \n");
        uData[0] = 1;
        writeRMI(SynaF01CommandBase, &uData[0], 1);
	msleep(10);
        readRMI(SynaF01DataBase, &uData[0], 1);

	// Sanity check that the reflash process is still enabled
	retval = waitRegComplete(SynaF34_FlashControl, 0x0f, 0, 1000);
	if(retval < 0){
		printk("Process is still enabled !\n");
	}

	//***********add 2012.10.10************
        readRMI((SynaF01DataBase + 1), &uStatus, 1);
        SynaSetup();

        // Check if the "Program Enabled" bit in F01 data register is cleared
        // Reflash is completed, and the image passes testing when the bit is cleared
        waitRegComplete(SynaF01DataBase, 0x40, 0, 1000);

        // Rescan PDT the update any changed register offsets
        SynaSetup();

        printk("Reflash Completed. Please reboot.\n");
	//************end 2012.10.10************
	return pid;
}


int syna_firmware_init(void)
{
	int pid = 0;

	pid = syna_get_sensorpid();

	//pid = 0; //randy add for update
	if( pid < 0){
		printk("get sensorpid error!\n");
		sprintf(SynaProductor,"unknown");
		SynaFirmware = CTPM_FW_YEJI;//randy add for name of fw that truly to yeji 20121212
		return -1;	//the structure of yeji is the same as ofilm
	}else if(pid == 0){
		sprintf(SynaProductor,"ofilm");
		SynaFirmware = CTPM_FW_OFILM;
	}else if(pid == 1){
		sprintf(SynaProductor,"yeji");//randy add for name of fw that truly to yeji 20121212
		SynaFirmware = CTPM_FW_YEJI;
	}
	printk("Tp sensor is %s !\n",SynaProductor);

	SynafirmwareImgData = SynaFirmware + 0x100;
	SynafirmwareImgVersion = (unsigned int)*(SynaFirmware + 7);

	switch (SynafirmwareImgVersion)
	{
	case 2:
		SynalockImgData = SynaFirmware + 0xD0;
		break;
	case 3:
	case 4:
		SynalockImgData = SynaFirmware + 0xC0;
		break;
	case 5:
		SynalockImgData = SynaFirmware + 0xB0;

	default:
		break;
	}
	return 0;
}



int syna_tp_debug(int mode)
{
        printk("%s...mode = %d...\n", __FUNCTION__, mode);

        if(mode)
                debug_flag = 1;
        else
                debug_flag = 0;

        return 0;
}

int syna_tp_active(void)
{
        printk("%s...\n", __FUNCTION__);
        return 1;
}

int syna_tp_need_update(void)
{
        printk("%s...\n", __FUNCTION__);
        return CheckConfigUpdate(1);
}

int syna_tp_do_update(void)
{
        printk("%s...\n", __FUNCTION__);
        return CompleteReflash(1);
}

int syna_tp_need_calibrate(void)
{
        printk("%s...\n", __FUNCTION__);
        return 1;
}

int syna_tp_calibrate(void)
{
        printk("%s...\n", __FUNCTION__);
        return 1;
}

int syna_tp_get_version(char* version)
{

        unsigned char uData[10] = {0};

        SynaInitialize();

        SynaReadConfigInfo();

        SynaReadFirmwareInfo();

        readRMI(SynaF34ControlBase, &uData[0], 4);
        printk("Config ID: %x %x %x %x \n", uData[0],uData[1],uData[2],uData[3]);

        return sprintf(version, "%s:s2202:0x%02x:0x%02x",SynaProductor, uData[1], uData[3]);

}

touchscreen_ops_tpye syna_tp_ops = {
        .touch_id = 0,
        .touch_type = 1,
        .active = syna_tp_active,
        .firmware_need_update = syna_tp_need_update,
        .firmware_do_update = syna_tp_do_update,
        .need_calibrate = syna_tp_need_calibrate,
        .calibrate = syna_tp_calibrate,
        .get_firmware_version = syna_tp_get_version,
        .debug = syna_tp_debug,
};


void register_yl_tw_platform_interface(struct rmi_device *  rmi_dev)
{
        upflash_rmi_device = rmi_dev;
        touchscreen_set_ops(&syna_tp_ops);
}


