#include "keyboard_fun.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"

#include "delay.h"
__ALIGN_BEGIN static uint8_t  buffer[8]  __ALIGN_END;

extern USB_OTG_CORE_HANDLE     USB_OTG_dev;

void keyboard_send_char(unsigned int flag, unsigned int  key)
{
	switch(key)
	{
		//	tab 
		case 0x09:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x2B;

			break;
			
		// CAPS_LOCK
		case 0xE5 :
		
			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x39;

			break;
			
		// 0
		case 0x30:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x27;

			break;

		// 回车
		case 0x0D:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x28;

			break;

		// 退格
		case 0x08:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x2A;

			break;

		// 空格
		case 0x20:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x2C;

			break;

		// 减号
		case 0x2D:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x2D;

			break;

		// 等号
		case 0x3D:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x2E;
			
			break;

		// [
		case 0x5B:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x2F;
			
			break;

		// ] 
		case 0x5D:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x30;
			
			break;

		/* \ */
		case 0x5C:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x31;
			
			break;

		// ; 
		case 0x3B:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x33;
			
			break;

		// '(单引号)
		case 0x27:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x34;
			
			break;

		// ,(逗号)
		case 0x2C:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x36;
			
			break;

		// .(句号)
		case 0x2E:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x37;
			
			break;

		// /
		case 0x2F:

			buffer[0] = 0x00;
			buffer[1] = 0x00;
			buffer[2] = 0x38;
			
			break;

		// ! 
		case 0x21:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x1E;
			
			break;

		// "(双引号)
		case 0x22:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x24;
			
			break;

		// #
		case 0x23:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x20;
			
			break;

		// $
		case 0x24:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x21;
			
			break;

		// %
		case 0x25:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x22;
			
			break;

		// &
		case 0x26:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x24;
			
			break;

		// (
		case 0x28:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x26;
			
			break;

		// )
		case 0x29:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x27;
			
			break;

		// *
		case 0x2A:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x25;
			
			break;

		// +
		case 0x2B:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x2E;
			
			break;

		// :
		case 0x3A:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x33;
			
			break;

		// <
		case 0x3C:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x3C;
			
			break;

		// >
		case 0x3E:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x37;
			
			break;

		// ?
		case 0x3F:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x38;
			
			break;

		// @
		case 0x40:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x1F;
			
			break;

		// ^
		case 0x5E:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x23;
			
			break;

		// _(下划线)
		case 0x5F:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x2D;
			
			break;

		// ~
		case 0x7E:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x3F;
			
			break;

		// {
		case 0x7B:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x2F;
			
			break;

		// |
		case 0x7C:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x31;
			
			break;

		// }
		case 0x7D:

			buffer[0] = 0x02;
			buffer[1] = 0x00;
			buffer[2] = 0x30;
			
			break;

		default:
			
			// 1 ~ 9
			if ( (key >= 0x31) && (key <= 0x39))
			{
				buffer[0] = 0x00;
				buffer[1] = 0x00;
				buffer[2] = key - 0x13;
			}
			// A ~ Z
			else if ( (key >= 0x41) && (key <= 0x5A))
			{
				buffer[0] = 0x02;
				buffer[1] = 0x00;
				buffer[2] = key - 0x3D;
			}
			// a ~ z
			else if ( (key >= 0x61) && (key <= 0x7A))
			{
				buffer[0] = 0x00;
				buffer[1] = 0x00;
				buffer[2] = key - 0x5D;
			}
			else if ( (key >= 0xE1) && (key <= 0xE4))
			{
				buffer[0] = 0x00;
				buffer[1] = 0x00;
				buffer[2] = key - 0x92;
			}
			else
			{
				buffer[0] = 0x00;
				buffer[1] = 0x00;
				buffer[2] = 0x00;
			}

			break;
	}

	if (flag)
		buffer[0] = flag;

	buffer[0] = 0x08;
	buffer[1] = 0x00;
	buffer[2] = 0x15;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0x00;

	USBD_HID_SendReport(&USB_OTG_dev, buffer, 8);
	delay_ms(100);
	buffer[0] = 0x00;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	USBD_HID_SendReport(&USB_OTG_dev, buffer, 8);

}

