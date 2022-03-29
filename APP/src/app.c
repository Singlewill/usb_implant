/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
 
 #include "ff.h"
 #include "usart.h"	
 #include "delay.h"
 #include "keyboard_fun.h"

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;

// Mass_is_ok = 1， 标识接收到SCSI_TEST_UNIT_READY 命令， 此时离PC端驱动执行完毕已经很近了
__IO uint8_t Mass_is_ok = 0;
 


void file_test(void)
{
	unsigned int a;
	FRESULT res;
	FILINFO finfo;
	FIL fsrc, fdst;      // file objects
	UINT br, bw;         // File R/W count	
	DIR dirs;
	char path[16]={""}; 
	BYTE buffer[512]; // file copy buffer
	
	printf("%s\r\n", __FUNCTION__);
	if (f_opendir(&dirs, path) == FR_OK) 
	{
    	while (f_readdir(&dirs, &finfo) == FR_OK)  
    	{
			
      		if (finfo.fattrib & AM_ARC) 
			{
				if(!finfo.fname[0]) break;   
        		printf("%s is file name\r\n",finfo.fname);
        		res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
        		br =1;
				a=0;
				printf("the first 256 byte :");
					for(a=0; a<512; a++) buffer[a]=0; 
		    	    res = f_read(&fsrc, buffer, sizeof(buffer), &br);
					printf("%s\r\n",buffer);	
		    	    if (res || br == 0) break;   // error or eof
				f_close(&fsrc);                 
			} else {
				printf("NOTAM_ARC \r\n");
			}
		} 
	}
	else{
			printf("f_opendir failed..\r\n");
	}
}
int main(void)
{
 FATFS fs;
int ret;


	delay_init(168);
	uart_init(115200);
	printf("system start ...\r\n");

  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif            
            &USR_desc,
            &USBD_MSC_cb, 
            &USR_cb);

	
  while (Mass_is_ok == 0)
  {
	  delay_ms(1000);
  }
  printf("Mass_is_ok = %d\r\n", Mass_is_ok);
  delay_ms(20000);
  // win + r
  while(1)
  {
	keyboard_send_char (0x0, 'r');
	  delay_ms(1000);
  }
 // disk_initialize(0);
//  ret = f_mount(0, &fs);
//  printf("f_mount returns %d\r\n", ret);
//  if (ret == 0)
//  {
//		file_test();
//  }else {
//		printf("f_mount failed \r\n");
//  }
  
  
  
  while(1);
	
} 

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
