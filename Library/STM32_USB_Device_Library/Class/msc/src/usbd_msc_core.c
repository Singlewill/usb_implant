/**
  ******************************************************************************
  * @file    usbd_msc_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the MSC core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                MSC Class  Description
  *          =================================================================== 
  *           This module manages the MSC class V1.0 following the "Universal 
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *      
  *  @endverbatim
  *
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
#include "usbd_msc_mem.h"
#include "usbd_msc_core.h"
#include "usbd_msc_bot.h"
#include "usbd_req.h"
__ALIGN_BEGIN static uint32_t  USBD_HID_Protocol  __ALIGN_END = 0;
__ALIGN_BEGIN static uint32_t  USBD_HID_IdleState __ALIGN_END = 0;
// USB报告描述符 ()
// 键盘
//__ALIGN_BEGIN static uint8_t HID_ReportDescriptor[HID_ReportDescSize] __ALIGN_END =
//{
//	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
//	0x09, 0x06, // USAGE (Keyboard)
//	0xa1, 0x01, // COLLECTION (Application)
//	0x05, 0x07, //     USAGE_PAGE (Keyboard/Keypad)
//	0x19, 0xe0, //     USAGE_MINIMUM (Keyboard LeftControl)
//	0x29, 0xe7, //     USAGE_MAXIMUM (Keyboard Right GUI)
//	0x15, 0x00, //     LOGICAL_MINIMUM (0)
//	0x25, 0x01, //     LOGICAL_MAXIMUM (1)
//	0x95, 0x08, //     REPORT_COUNT (8)
//	0x75, 0x01, //     REPORT_SIZE (1)
//	0x81, 0x02, //     INPUT (Data,Var,Abs)
//	0x95, 0x01, //     REPORT_COUNT (1)
//	0x75, 0x08, //     REPORT_SIZE (8)
//	0x81, 0x03, //     INPUT (Cnst,Var,Abs)
//	0x95, 0x06, //   REPORT_COUNT (6)
//	0x75, 0x08, //   REPORT_SIZE (8)
//	0x25, 0xFF, //   LOGICAL_MAXIMUM (255)
//	0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
//	0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
//	0x81, 0x00, //     INPUT (Data,Ary,Abs)
//	0x25, 0x01, //     LOGICAL_MAXIMUM (1)
//	0x95, 0x02, //   REPORT_COUNT (2)
//	0x75, 0x01, //   REPORT_SIZE (1)
//	0x05, 0x08, //   USAGE_PAGE (LEDs)
//	0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
//	0x29, 0x02, //   USAGE_MAXIMUM (Caps Lock)
//	0x91, 0x02, //   OUTPUT (Data,Var,Abs)
//	0x95, 0x01, //   REPORT_COUNT (1)
//	0x75, 0x06, //   REPORT_SIZE (6)
//	0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)
//	0xc0        // END_COLLECTION
//};

// 键盘
__ALIGN_BEGIN static uint8_t HID_ReportDescriptor[HID_ReportDescSize] __ALIGN_END =
{
0x06,0xA0,0xFF,//用法页(FFA0h, vendor defined)
0x09, 0x01,//用法(vendor defined)
0xA1, 0x01,//集合(Application)
0x09, 0x02 ,//用法(vendor defined)
0xA1, 0x00,//集合(Physical)
0x06,0xA1,0xFF,//用法页(vendor defined)
//输入报告
0x09, 0x03 ,//用法(vendor defined)
0x09, 0x04,//用法(vendor defined)
0x15, 0x80,//逻辑最小值(0x80 or -128)
0x25, 0x7F,//逻辑最大值(0x7F or 127)
0x35, 0x00,//物理最小值(0)
0x45,0xFF,//物理最大值(255)
0x75, 0x08,//报告长度Report size (8位)
0x95, 0x40,//报告数值(64 fields)
0x81, 0x02,//输入(data, variable, absolute)
//输出报告
0x09, 0x05,//用法(vendor defined)
0x09, 0x06,//用法(vendor defined)
0x15, 0x80,//逻辑最小值(0x80 or -128)
0x25, 0x7F,//逻辑最大值(0x7F or 127)
0x35, 0x00,//物理最小值(0)
0x45,0xFF,//物理最大值(255)
0x75,0x08,//报告长度(8位)
0x95, 0x40,//报告数值(64 fields)
0x91, 0x02,//输出(data, variable, absolute)
0xC0,//集合结束(Physical)
0xC0//集合结束(Application)
};

__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END=
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x10,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x21,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_ReportDescSize,/*wItemLength: Total length of Report descriptor*/
  0x00
};

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_CORE 
  * @brief Mass storage core module
  * @{
  */ 

/** @defgroup MSC_CORE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_FunctionPrototypes
  * @{
  */ 
uint8_t  USBD_MSC_Init (void  *pdev, 
                            uint8_t cfgidx);

uint8_t  USBD_MSC_DeInit (void  *pdev, 
                              uint8_t cfgidx);

uint8_t  USBD_MSC_Setup (void  *pdev, 
                             USB_SETUP_REQ *req);

uint8_t  USBD_MSC_DataIn (void  *pdev, 
                              uint8_t epnum);


uint8_t  USBD_MSC_DataOut (void  *pdev, 
                               uint8_t epnum);

uint8_t  *USBD_MSC_GetCfgDesc (uint8_t speed, 
                                      uint16_t *length);

#ifdef USB_OTG_HS_CORE  
uint8_t  *USBD_MSC_GetOtherCfgDesc (uint8_t speed, 
                                      uint16_t *length);
#endif


uint8_t USBD_MSC_CfgDesc[USB_MSC_CONFIG_DESC_SIZ];




/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Variables
  * @{
  */ 


USBD_Class_cb_TypeDef  USBD_MSC_cb = 
{
  USBD_MSC_Init,
  USBD_MSC_DeInit,
  USBD_MSC_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_MSC_DataIn,
  USBD_MSC_DataOut,
  NULL, /*SOF */ 
  NULL,  
  NULL,     
  USBD_MSC_GetCfgDesc,
#ifdef USB_OTG_HS_CORE  
  USBD_MSC_GetOtherCfgDesc,
#endif
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN uint8_t USBD_MSC_CfgDesc[USB_MSC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_MSC_CONFIG_DESC_SIZ, 0x00,
	
  0x02,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x00,   /* iConfiguration: */
  0x80,   /* bmAttributes: */
  250,   /* MaxPower 500 mA */
  
	/********************first interface : MassStorage ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x0,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_IN_EP,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x00,   /*Polling interval in milliseconds */
  
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_OUT_EP,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x00,     /*Polling interval in milliseconds*/
  
  /********************Second interface : HID ********************/
  
	// <bLength> : 接口描述符的长度为9字节。
	// [0x09]
	0x09,

	// <bDescriptorType> : 接口描述符的编号为0x04。
	// [0x04]
	0x04,

	// <bInterfaceNumber> 
	0x01,

	// <bAlternateSetting> : 该接口的备用编号，为0。
	0x00,

	// <bNumEndpoints> : 非0端点的数目。该接口有2个批量端点
	0x02,

	// <bInterfaceClass> : 该接口的类型
	// [0x03]
	0x03,

	// <bInterfaceSubClass> : 该接口所使用的子类。在HID1.1协议中，

		//只规定了一种子类：支持BIOS引导启动的子类。
		//USB键盘、鼠标属于该子类，子类代码为0x01。
		//但这里我们是自定义的HID设备，所以不使用子类。
	0x01,

	// <bInterfaceProtocol> : 如果子类为支持引导启动的子类，

		//则协议可选择鼠标和键盘。键盘代码为0x01，鼠标代码为0x02。
		//自定义的HID设备，也不使用协议。
	0x01,

	// <iConfiguration> ：该接口的字符串索引值。这里没有，为0。
	0x00,

	/*********************HID接口描述符*************************/
	// <bLength> : 本HID描述符下只有一个下级描述符。所以长度为9字节。
	0x09,

	// <bDescriptorType> : HID描述符的编号为0x21。
	0x21,

	// <bcdHID> : 本协议使用的HID1.1协议。注意低字节在先。
	0x10,
	0x01,

	// <bCountyCode> : 设备适用的国家代码，这里选择为美国，代码0x21。
	0x21,

	// <bNumDescriptors> : 下级描述符的数目。我们只有一个报告描述符。
	0x01,

	// <bDescriptorType> : 下级描述符的类型，为报告描述符，编号为0x22。
	0x22,

	// <bDescriptorLength> : 下级描述符的长度。下级描述符为报告描述符。
	HID_ReportDescSize,
	0x00,

	/*********************端点描述符**********************************/

	// <bLength> : 端点描述符长度为7字节。
	// [0x07]
	0x07,

	// <bDescriptorType> : 端点描述符编号为0x05。
	// [0x05]
	0x05,

	// <bEndpointAddress> : 端点的地址。我们使用D12的输入端点1。
	HID_IN_EP,

	// <bmAttributes> : D1~D0为端点传输类型选择。
	// [0x03]
	0x03,

	// <wMaxPacketSize> : 该端点的最大包长。最大包长为64字节。
	HID_MAX_PACKET,0x00,

	// <bInterval> : 端点查询的时间，端点查询的时间，此处无意义。
	0x01,

	/***********************端点描述符*******************************************/
	// <bLength> : 端点描述符长度为7字节。
	// [0x07]
	0x07,

	// <bDescriptorType> : 端点描述符编号为0x05。
	// [0x05]
	0x05,

	// <bEndpointAddress> : 端点的地址。我们使用D12的输入端点1。
	HID_OUT_EP,

	// <bmAttributes> : D1~D0为端点传输类型选择。
	// [0x03]
	0x03,

	// <wMaxPacketSize> : 该端点的最大包长。最大包长为64字节。
	HID_MAX_PACKET,0x00,

	// <bInterval> : 端点查询的时间，端点查询的时间，此处无意义。
	0x01
	
};
#ifdef USB_OTG_HS_CORE 
 #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
   #if defined ( __ICCARM__ ) /*!< IAR Compiler */
     #pragma data_alignment=4   
   #endif
 #endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t USBD_MSC_OtherCfgDesc[USB_MSC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,   
  USB_MSC_CONFIG_DESC_SIZ,
  
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */
  
  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent command set*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_IN_EP,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00,   /*Polling interval in milliseconds */
  
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_OUT_EP,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00     /*Polling interval in milliseconds*/
};
#endif 

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint8_t  USBD_MSC_MaxLun  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint8_t  USBD_MSC_AltSet  __ALIGN_END = 0;

/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Functions
  * @{
  */ 

/**
* @brief  USBD_MSC_Init
*         Initialize  the mass storage configuration
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/
uint8_t  USBD_MSC_Init (void  *pdev, 
                            uint8_t cfgidx)
{
  USBD_MSC_DeInit(pdev , cfgidx );
  
  /* Open EP IN */
  DCD_EP_Open(pdev,
              MSC_IN_EP,
              MSC_EPIN_SIZE,
              USB_OTG_EP_BULK);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              MSC_OUT_EP,
              MSC_EPOUT_SIZE,
              USB_OTG_EP_BULK);
	
	
	  /* Open EP IN */
  DCD_EP_Open(pdev,
              HID_IN_EP,
              HID_IN_PACKET,
              USB_OTG_EP_INT);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              HID_OUT_EP,
              HID_OUT_PACKET,
              USB_OTG_EP_INT);
 
  /* Init the BOT  layer */
  MSC_BOT_Init(pdev); 
  
  return USBD_OK;
}

/**
* @brief  USBD_MSC_DeInit
*         DeInitilaize  the mass storage configuration
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/
uint8_t  USBD_MSC_DeInit (void  *pdev, 
                              uint8_t cfgidx)
{
  /* Close MSC EPs */
  DCD_EP_Close (pdev , MSC_IN_EP);
  DCD_EP_Close (pdev , MSC_OUT_EP);
	
  DCD_EP_Close (pdev , HID_IN_EP);
  DCD_EP_Close (pdev , HID_OUT_EP);
  
  
  /* Un Init the BOT layer */
  MSC_BOT_DeInit(pdev);   
  return USBD_OK;
}
/**
* @brief  USBD_MSC_Setup
*         Handle the MSC specific requests
* @param  pdev: device instance
* @param  req: USB request
* @retval status
*/
uint8_t  USBD_MSC_Setup (void  *pdev, USB_SETUP_REQ *req)
{
	uint16_t len = 0;
	uint8_t  *pbuf = NULL;
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {

  /* Class request */
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    case BOT_GET_MAX_LUN :

      if((req->wValue  == 0) && 
         (req->wLength == 1) &&
         ((req->bmRequest & 0x80) == 0x80))
      {
        USBD_MSC_MaxLun = USBD_STORAGE_fops->GetMaxLun();
        if(USBD_MSC_MaxLun > 0)
        {
           USBD_CtlSendData (pdev,
                             &USBD_MSC_MaxLun,
                              1);
        }
        else
        {
          USBD_CtlError(pdev , req);
          return USBD_FAIL; 
          
        }
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;
      
    case BOT_RESET :
      if((req->wValue  == 0) && 
         (req->wLength == 0) &&
        ((req->bmRequest & 0x80) != 0x80))
      {      
         MSC_BOT_Reset(pdev);
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;
	  
	case HID_REQ_SET_PROTOCOL:
      USBD_HID_Protocol = (uint8_t)(req->wValue);
      break;
      
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_Protocol,
                        1);    
      break;
      
    case HID_REQ_SET_IDLE:
      USBD_HID_IdleState = (uint8_t)(req->wValue >> 8);
      break;
      
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, 
                        (uint8_t *)&USBD_HID_IdleState,
                        1);        
      break;      

    default:
       USBD_CtlError(pdev , req);
       return USBD_FAIL; 
    }
    break;
/////////////////////////////////////////////////////////
  /* Interface & Endpoint request */
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        &USBD_MSC_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      USBD_MSC_AltSet = (uint8_t)(req->wValue);
      break;
    
    case USB_REQ_CLEAR_FEATURE:  
	// only make clear feature for enpoint 1 , bluk point
	if((((uint8_t)req->wIndex) & MSC_ENP_NUM) == 0x01)
	{
		/* Flush the FIFO and Clear the stall status */    
      DCD_EP_Flush(pdev, (uint8_t)req->wIndex);
      
      /* Re-activate the EP */      
      DCD_EP_Close (pdev , (uint8_t)req->wIndex);
		DCD_EP_Open(pdev,
            ((uint8_t)req->wIndex),
            MSC_MAX_PACKET,
            USB_OTG_EP_BULK);
			MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
	}

      break;
	  
	  case USB_REQ_GET_DESCRIPTOR: 
      if( req->wValue >> 8 == HID_REPORT_DESC)
      {
        len = MIN(HID_ReportDescSize , req->wLength);
        pbuf = HID_ReportDescriptor;
      }
      else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
      {
        
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
        pbuf = USBD_HID_Desc;   
#else
        pbuf = USBD_HID_CfgDesc + 0x12;
#endif 
        
		len = MIN(USB_HID_DESC_SIZ , req->wLength); 
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      
      break;
	  
	  
	  
      
    }  
    break;
   
  default:
    break;
  }
  return USBD_OK;
}

/**
* @brief  USBD_MSC_DataIn
*         handle data IN Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t  USBD_MSC_DataIn (void  *pdev, 
                              uint8_t epnum)
{
	if (epnum != 1)
		printf("I --- %d\r\n", epnum);
	if (epnum == MSC_ENP_NUM)
		MSC_BOT_DataIn(pdev , epnum);
	else if (epnum == HID_ENP_NUM)
		 DCD_EP_Flush(pdev, HID_IN_EP);
  return USBD_OK;
}

/**
* @brief  USBD_MSC_DataOut
*         handle data OUT Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t  USBD_MSC_DataOut (void  *pdev, 
                               uint8_t epnum)
{
	uint8_t buffer[8];
	uint16_t len = 8;
	uint8_t i;
	if (epnum != 1)
		printf("O --- %d\r\n", epnum);
	 if (epnum == MSC_ENP_NUM)
		MSC_BOT_DataOut(pdev , epnum);
	 else if (epnum == HID_ENP_NUM)
	 {
		DCD_EP_PrepareRx(pdev, HID_OUT_EP, buffer, len);
		 printf("HID receive : \r\n");
		 for (i = 0; i < 8; i++)
			printf("%02x ", buffer[i]);
		 printf("\r\n");
		 
	 }
  return USBD_OK;
}

/**
* @brief  USBD_MSC_GetCfgDesc 
*         return configuration descriptor
* @param  speed : current device speed
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_MSC_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (USBD_MSC_CfgDesc);
  return USBD_MSC_CfgDesc;
}

/**
* @brief  USBD_MSC_GetOtherCfgDesc 
*         return other speed configuration descriptor
* @param  speed : current device speed
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
#ifdef USB_OTG_HS_CORE  
uint8_t  *USBD_MSC_GetOtherCfgDesc (uint8_t speed, 
                                      uint16_t *length)
{
	*length = sizeof (USBD_MSC_CfgDesc);
  return USBD_MSC_CfgDesc;
//*length = sizeof (USBD_MSC_OtherCfgDesc);
//  return USBD_MSC_OtherCfgDesc;
}
#endif
/**
  * @}
  */ 
/**
  * @brief  USBD_HID_SendReport 
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport     (USB_OTG_CORE_HANDLE  *pdev, 
                                 uint8_t *report,
                                 uint16_t len)
{
   printf("HID_SendReport \r\n");
  if (pdev->dev.device_status == USB_OTG_CONFIGURED )
  {
	  //printf("USB_OTG_CONFIGURED\r\n");
    DCD_EP_Tx (pdev, HID_IN_EP, report, len);
  }
  else{
	printf("NO  USB_OTG_CONFIGURED\r\n");
  }
  return USBD_OK;
}


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
