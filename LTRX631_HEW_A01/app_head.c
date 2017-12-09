/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "include.h"


void main(USB_VP_INT stacd);

//#pragma section C app_head
	/* START ADDRESS of user application header data - Appheader address + 0x00 */
//	const uint32_t app_entry_addr = (uint32_t) PowerON_Reset_PC;
												/* 	- Appheader address + 0x04. */													
//	const uint32_t * const app_hi_addr =	__sectop("app_head"); //or simply APP_HEAD_ADR
    											/*	- Appheader address + 0x08. */
//	const uint32_t * const app_lo_addr =	__sectop("C");
    
	/* USER APP SW ID   							- Appheader address + 0x0C */
//	const uint8_t   app_id[0x10] = "APPLIC_ID_12345\0"; // 16 bytes reserved
	
	/* USER APP CHECKSUM REFERENCE
        These members are used for checksum calculation of the UserApp area. 
        We want the checksum to cover as much as possible of the area that was reflashed.
        Since this header itself can't be included in the checksum (because it contains 
        the checksum result) all the addresses from the start of the UserApp area up to 
        the start of this header section will be covered. The size of the checksum area
        is then calculated as the differnce between the start address of the app_head 
        section and the start address of the UserApp section (at ResetPRG). 
            Don't use a checksum while developing since the checksum will change all 
        the time, just a fixed check value. When completely done, change this value
        to the value achieved when pressing S2. Also change the CANloader checksum 
        routine to return a checksum instead of this same fixed test value.
                                                    - Appheader address + 0x1C */
    #if CRC_AUTOGEN
	 	/* Actual value is entered by the linker. Leave area as unwritten here. */
//	 	const uint16_t  app_checksum_ref = 0xFFFF;
	#else
//	 	const uint16_t  app_checksum_ref = DEBUG_CHECKSUM; //0x55AA is testing value.
    #endif

	/* Total header area size is 32 bytes */	
