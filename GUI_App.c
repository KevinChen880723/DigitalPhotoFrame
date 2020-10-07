  /**
  ******************************************************************************
  * @file    GUI_App.c
  * @author  MCD Application Team
  * @brief   Simple demo drawing "Hello world"  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright c 2018 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include "GUI_App.h"
#include "GUI.h"
#include "fatfs.h"
#include "DIALOG.h"
#include "ts.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "fatfs_storage.h"
#include "ff.h"
#include "ft5336.h"
#include "stdbool.h"
void ShowBMP(const char *);
static char _acBuffer[480*4];
GUI_HMEM hMem;
uint32_t counter = 0, transparency = 0;
uint8_t str[30];
uint8_t *uwInternelBuffer;
char SD_Path[4];
char* pDirectoryFiles[MAX_BMP_FILES];
uint8_t  ubNumberOfFiles = 0;
char *sram_buffer;
FIL fil;
UINT *br;
bool display_lock = false;

uint16_t image_x_size;
uint16_t image_y_size;
FRESULT fileResult[5];

void GRAPHICS_MainTask(void) {

/* USER CODE BEGIN GRAPHICS_MainTask */
 /* User can implement his graphic application here */
  /* Hello Word example */
		
	
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_Font32_1);
    GUI_DispStringAt("Hello world!", (LCD_GetXSize()-150)/2, (LCD_GetYSize()-20)/2);
		uwInternelBuffer = (uint8_t *)0xC0260000;
		fileResult[0] = FR_NO_FILE;
		fileResult[1] = FR_NO_FILE;
		fileResult[2] = FR_NO_FILE;
		fileResult[3] = FR_NO_FILE;
		fileResult[4] = FR_NO_FILE;
		while(BSP_SD_IsDetected() != SD_PRESENT)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  Please insert SD Card                  ");
		}
		
		if(FATFS_LinkDriver(&SD_Driver, SD_Path) == 0)	//give the memory address of SD_Driver to SD_Path
		{
			/*##-3- Initialize the Directory Files pointers (heap) ###################*/
			for (counter = 0; counter < MAX_BMP_FILES; counter++)
			{
				pDirectoryFiles[counter] = malloc(MAX_BMP_FILE_NAME);
				if(pDirectoryFiles[counter] == NULL)
				{
					/* Set the Text Color */
					BSP_LCD_SetTextColor(LCD_COLOR_RED);
					
					BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  Cannot allocate memory ");
					
					while(1)
					{
					}       
				}
			}
			/* Get the BMP file names on root directory */
			ubNumberOfFiles = Storage_GetDirectoryBitmapFiles("", pDirectoryFiles);
			
			if (ubNumberOfFiles == 0)
			{
				for (counter = 0; counter < MAX_BMP_FILES; counter++)
				{
					free(pDirectoryFiles[counter]);
				}
				BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"  No Bitmap files...      ");
				while(1)
				{
				}
			}
		}
		else
		{
			/* FatFs Initialization Error */
			Error_Handler();    
		}
		ShowBMP(pDirectoryFiles[0]);
}

void ShowBMP(const char *picture)
{	
		display_lock = true;
		hMem = GUI_ALLOC_AllocZero(2000000);
		sram_buffer = (char *)GUI_ALLOC_h2p(hMem);	
		fileResult[0] = f_mount(&SDFatFS,"",1);	//we need to register our file system first, so we can use that directory	
		fileResult[1] = f_open(&fil , picture , FA_READ);
		fileResult[2] = f_read(&fil , sram_buffer , fil.obj.objsize , br);
		fileResult[3] = f_close(&fil);
		image_x_size = GUI_BMP_GetXSize(sram_buffer);
		image_y_size = GUI_BMP_GetYSize(sram_buffer);
		GUI_Clear();
		GUI_BMP_DrawScaled(sram_buffer,0,(272-(image_y_size*480/image_x_size))/2,480,image_x_size);
		GUI_ALLOC_Free(hMem);
		display_lock = false;
}


/*************************** End of file ****************************/
