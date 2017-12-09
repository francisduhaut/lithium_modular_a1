/* $Id: r_usb_ansi.h 143 2012-05-07 09:16:46Z tmura $ */


#define USB_CLASS_PERI_MODE 0x100

typedef enum
{
    USB_CLASS_HCDCC =   0x0,
    USB_CLASS_HCDCD,
    USB_CLASS_HHID,
    USB_CLASS_HMSC,
    USB_CLASS_HSTD_BULK,
    USB_CLASS_HSTD_INT,
    USB_CLASS_PCDCC = (USB_CLASS_PERI_MODE + USB_CLASS_HSTD_INT + 1),
    USB_CLASS_PCDCD,
    USB_CLASS_PHID,
    USB_CLASS_PMSC,
    USB_CLASS_PSTD_BULK,
    USB_CLASS_PSTD_INT, 
    USB_CLASS_MAX = ((USB_CLASS_PSTD_INT + 1) & (~USB_CLASS_PERI_MODE))
} USB_CLASS_ENUM_t;

#define     USB_RSVED_FN        3
#define     USB_NOPIPE          ((uint8_t)0xFF)

int16_t     usb_open( USB_CLASS_ENUM_t );
int16_t     usb_close( int16_t );
int32_t     usb_read( int16_t, void *, uint32_t );
int32_t     usb_write( int16_t , void *, uint32_t );
uint16_t    usb_open_peri( USB_CLASS_ENUM_t class, int16_t fn );
uint16_t    usb_open_host( USB_CLASS_ENUM_t class, int16_t fn );
uint16_t    usb_close_peri( int16_t fn );
uint16_t    usb_close_host( int16_t fn );

int16_t usb_open_hcdc( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );
int16_t usb_open_hhid( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );
int16_t usb_open_hmsc( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );
int16_t usb_open_pcdc( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );
int16_t usb_open_phid( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );
int16_t usb_open_pmsc( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );

int16_t usb_open_hvendor( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );
int16_t usb_open_pvendor( USB_UTR_t *, struct usb_fninfo *, uint8_t, uint8_t );

extern  int32_t pcdc_read_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t pcdc_write_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hcdc_read_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hcdc_write_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hcdc_read_control( int16_t fn, void *buf, uint32_t length );
extern  int32_t pvendor_read_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t pvendor_write_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hvendor_read_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hvendor_write_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hhid_read_data( int16_t fn, void *buf, uint32_t length );
extern  int32_t hhid_write_data( int16_t fn, void *buf, uint32_t length );

extern  USB_ER_t usb_control_class_request(void *pram);
extern  struct  usb_fninfo  usb_fnInfo[];
extern  struct  usb_devinfo usb_devInfo[];

/******************************************************************************
End  Of File
******************************************************************************/
