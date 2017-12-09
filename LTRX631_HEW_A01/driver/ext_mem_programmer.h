/************************************
 type defs
 **************************************/

typedef struct
{
	uint8_t *start;
	uint8_t len;
	uint8_t status;
}T_REC;

typedef struct
{
	uint8_t RecDataLen;
	UINT32_VAL Address;
	uint8_t RecType;
	uint8_t* Data;
	uint8_t CheckSum;
	UINT32_VAL ExtSegAddress;
	UINT32_VAL ExtLinAddress;
}T_HEX_RECORD;


/******************************************************************************
Macros used in this file
 *******************************************************************************/
#define DEV_CONFIG_REG_BASE_ADDRESS 0x9FC02FF0
#define DEV_CONFIG_REG_END_ADDRESS   0x9FC02FFF

#define FLASH_PAGE_SIZE 0x1000

#define DATA_RECORD 		0
#define END_OF_FILE_RECORD 	1
#define EXT_SEG_ADRS_RECORD 2
#define EXT_LIN_ADRS_RECORD 4

#define REC_FLASHED 0
#define REC_NOT_FOUND 1
#define REC_FOUND_BUT_NOT_FLASHED 2

/****************************************************************************
Function prototypes
 *****************************************************************************/
void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec);
int WriteHexRecord2Flash(uint8_t* HexRecord);
uint8_t ProgramExternalImages(void);
