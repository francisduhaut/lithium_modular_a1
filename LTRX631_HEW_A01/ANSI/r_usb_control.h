/* $Id: r_usb_control.h 143 2012-05-07 09:16:46Z tmura $ */

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum
{
    USB_CTL_USBIP_NUM = 0,
} USB_CTRLCODE_t;

/* Control function command type */
typedef enum _CTLCOD
{
       USB_CTL_USB_IP_NUM = 0,
       USB_CTL_RD_NOTIFY_SET,
       USB_CTL_WR_NOTIFY_SET,
       USB_CTL_RD_LENGTH_GET,
       USB_CTL_WR_LENGTH_GET,
       USB_CTL_GET_OPEN_STATE,
       USB_CTL_GET_RD_STATE,
       USB_CTL_GET_WR_STATE,
       USB_CTL_H_RD_TRANSFER_END,
       USB_CTL_H_WR_TRANSFER_END,
       USB_CTL_H_CHG_DEVICE_STATE,
       USB_CTL_H_GET_DEVICE_INFO,
       USB_CTL_P_RD_TRANSFER_END,
       USB_CTL_P_WR_TRANSFER_END,
       USB_CTL_P_CHG_DEVICE_STATE,
       USB_CTL_P_GET_DEVICE_INFO,
       USB_CTL_P_RD_SET_STALL,
       USB_CTL_P_WR_SET_STALL,
       USB_CTL_CLASS_REQUEST,
} CTLCODE;

/* Control function error type */
typedef enum _CTLERRCOD
{
       USB_CTL_ERR_NONE = 0,
       USB_CTL_ERR_PROCESS_COMPLETE,
       USB_CTL_ERR_PROCESS_WAIT = -2,
       USB_CTL_ERR_PROCESS_ERR = -3,
       USB_CTL_ERR_PAR = -17,
} CTLERRCODE;

/* USB_CTL_HOST_TRANSFER_END/USB_CTL_PERI_TRANSFER_END Arguments */
typedef struct usb_control_transfer_end
{
    uint16_t        status;             /* End Status */
}
USB_CTL_TRANSFER_END_t;

/* USB_CTL_H_GET_DEVICE_INFO/USB_CTL_P_GET_DEVICE_INFO Arguments */
typedef struct usb_control_device_information
{
    uint16_t        *tbl;               /* Table Pointer */
}
USB_CTL_DEVICE_INFORMATION_t;

/* USB_CTL_H_CHG_DEVICE_STATE/USB_CTL_P_CHG_DEVICE_STATE Arguments */
typedef struct usb_control_change_device_state
{
    USB_CB_t        complete;           /* Callback function */
    uint16_t        msginfo;            /* Request type */
    uint16_t        devaddr;            /* Rootport/Pipe number */
}
USB_CTL_CHANGE_DEVICE_STATE_t;


/* USB_CTL_P_RD_SET_STALL/USB_CTL_P_WR_SET_STALL Arguments */
typedef struct usb_control_set_pipe_stall
{
    USB_CB_t        complete;           /* Callback function */
}
USB_CTL_SET_PIPE_STALL_t;

/* Input parameter for control() function */
typedef union {
    USB_CTL_CHANGE_DEVICE_STATE_t   device_state;
    USB_CTL_DEVICE_INFORMATION_t    device_information;
    USB_CTL_TRANSFER_END_t          transfer_end;
    USB_CTL_SET_PIPE_STALL_t        setstall;
} USB_CTL_PARAMETER_t;

/******************************************************************************
Constant macro definitions
******************************************************************************/
/* Class Request File No. for control function */
#define     CLASS_REQUEST_FN    0x10

/******************************************************************************
Private global variables and functions
******************************************************************************/
int16_t     usb_control( int16_t, USB_CTRLCODE_t, void * );

extern  USB_ER_t usb_control_class_request(void *pram);

