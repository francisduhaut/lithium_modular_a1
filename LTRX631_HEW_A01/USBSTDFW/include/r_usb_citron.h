/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR  A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE  EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2010(2011,2012) Renesas Electronics Corpration
* and Renesas Solutions Corp. All rights reserved.
******************************************************************************
* File Name    : r_usb_cItron.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB common uItron header
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_citron.h 143 2012-05-07 09:16:46Z tmura $ */

#ifndef __R_USB_CITRON_H__
#define __R_USB_CITRON_H__


/* Condition compilation by the difference of quality control */
    #ifndef NULL
      #define   NULL    0u
    #endif  /* NULL */

/*****************************************************************************
Typedef definitions
******************************************************************************/
    typedef void*           VP;             /* Pointer to variable      */
    typedef void            (*FP)(void);    /* Program address          */
    typedef unsigned long   UINT;           /* unsigned integer         */

    typedef long            ER;             /* Error code               */
    typedef short           ID;             /* Object ID (xxxid)        */
    typedef long            TMO;            /* Time out                 */
    typedef unsigned long   RELTIM;         /* System call define       */
    typedef void*           MH;             /* Message Header typedef   */
    typedef unsigned long   ATR;            /* Attribute                */
    typedef unsigned long   STAT;           /* Object status            */
    typedef short           PRI;            /* Task priority            */
    typedef unsigned long   SIZE;           /* Memory area size         */
    typedef long            VP_INT;         /* Integer data             */

    /*----------- msghead -----------*/
    typedef struct
    {
        VP              msghead;        /* Message header               */
    } T_MSG;
    /*----------- cre_tsk -----------*/
    typedef struct                      /* cre_tsk packet               */
    {
        ATR             tskatr;         /* Task attribute               */
        VP_INT          exinf;          /* Extended information         */
        FP              task;           /* Task start address           */
        PRI             itskpri;        /* Initial priority             */
        SIZE            stksz;          /* Stack size                   */
        VP              stk;            /* Stack address                */
    } T_CTSK;
    /*----------- cre_sem -----------*/
    typedef struct                      /* cre_sem packet               */
    {
        ATR             sematr;         /* Semaphore attribute          */
        UINT            isemcnt;        /* Initial semaphore count      */
        UINT            maxsem;         /* Maximum semaphore count      */
    } T_CSEM;
    /*----------- cre_mbx -----------*/
    typedef struct                      /* cre_mbx packet               */
    {
        ATR             mbxatr;         /* Mailbox attribute            */
        PRI             maxmpri;        /* Maximum message priority     */
        VP              mprihd;         /* Message queue header address */
    } T_CMBX;
    /*----------- cre_mpf -----------*/
    typedef struct                      /* cre_mpf packet               */
    {
        ATR             mpfatr;         /* Memorypool attribute         */
        UINT            blkcnt;         /* Total memory block count     */
        UINT            blksz;          /* Memory block size            */
        VP              mpf;            /* Memorypool area address      */
    } T_CMPL;
    /*----------- ref_tst -----------*/
    typedef struct                      /* ref_tst packet               */
    {
        STAT            tskstat;        /* Task status                  */
        STAT            tskwait;        /* Factor of WAIT               */
    } T_RTST;
    /*----------- cre_alm -----------*/
    typedef struct                      /* cre_alm packet               */
    {
        ATR             almatr;         /* Alarm handler attribute      */
        VP_INT          exinf;          /* Extended information         */
        FP              almhdr;         /* Alarm handler address        */
    } T_CALM;

/******************************************************************************
Constant macro definitions
******************************************************************************/

    #define E_OK        0L                  /* Normal end               */
    #define E_TMOUT     (-50L)              /* Time out                 */
    #define E_QOVR      (-43L)              /* Queuing over flow        */


/******************************************************************************
Constant macro definitions
******************************************************************************/

    /*-------------------- Task/Handler attribute (***atr) -------------*/
    #define TA_HLNG     0x00000000u     /* High-level language program  */
    /*----------------------- Object attribute (***atr) ----------------*/
    #define TA_TFIFO    0x00000000u     /* FIFO wait queue              */
    #define TA_MFIFO    0x00000000u     /* FIFO message queue           */
    #define TA_ACT      0x00000002u     /* Create task with activation  */

    /*-------------------------- Object status -------------------------*/
    #define TTS_RUN     0x00000001UL    /* RUNNING                      */
    #define TTS_RDY     0x00000002UL    /* READY                        */
    #define TTS_WAI     0x00000004UL    /* WAITING                      */
    #define TTS_SUS     0x00000008UL    /* SUSPENDED                    */
    #define TTS_WAS     0x0000000cUL    /* WAITING-SUSPENDED            */
    #define TTS_DMT     0x00000010UL    /* DORMANT                      */
    #define TTS_STK     0x40000000UL    /* STACK WAITING                */

    /* <system call> */
    #define USB_NO_SYSTEM_PP

/*****************************************************************************
Typedef definitions
******************************************************************************/
    typedef T_MSG           USB_MSG_t;      /* ITRON message            */
    typedef T_RTST          USB_RTST_t; /* ITRON task status        */

    typedef ER              USB_ER_t;       /* ITRON system call err    */
    typedef ID              USB_ID_t;       /* ITRON system call define */
    typedef TMO             USB_TM_t;       /* ITRON time out           */
    typedef RELTIM          USB_RT_t;       /* ITRON system call define */
    typedef VP              USB_MH_t;       /* ITRON Message Header     */
    typedef VP_INT          USB_VI_t;       /* ITRON system call define */
    typedef ATR             USB_ATR_t;      /* ITRON attribute          */
    typedef FP              USB_FP_t;       /* ITRON task address       */
    typedef PRI             USB_PRI_t;      /* ITRON priority           */
    typedef SIZE            USB_SIZ_t;      /* ITRON size               */
    typedef VP              USB_VP_t;       /* ITRON address            */
    typedef UINT            USB_UINT_t; /* ITRON unsigned integer   */
    typedef VP_INT          USB_VP_INT_t;


/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_NULL        NULL
#define USB_VP_INT      VP_INT
#define USB_TA_HLNG TA_HLNG
#define USB_TA_TFIFO    TA_TFIFO
#define USB_TA_MFIFO    TA_MFIFO

#define USB_E_TMOUT E_TMOUT     /* TRCV_MSG time out */
#define USB_E_QOVR      E_QOVR      /* Submit overlap error */
#define USB_E_ERROR (-1L)
#define USB_E_OK        E_OK
#define USB_TMPOL       TMO_POL     /* TRCV_MSG poling */
#define USB_TMFEVR      TMO_FEVR    /* TRCV_MSG no time */
#define USB_TAJLNG      TA_HLNG     /* High-level language program */
#define USB_TATFIFO TA_TFIFO    /* FIFO wait queue */
#define USB_TAMFIFO TA_MFIFO    /* FIFO message queue */
#define USB_TAACT       TA_ACT      /* Create task with activation */
#define USB_TTSRUN      TTS_RUN     /* RUNNING */

#define USB_TTS_RUN TTS_RUN
#define USB_TTS_RDY TTS_RDY
#define USB_TTS_WAI TTS_WAI
#define USB_TTS_SUS TTS_SUS
#define USB_TTS_WAS TTS_WAS
#define USB_TTS_DMT TTS_DMT
#define USB_TTS_STK TTS_STK

#endif  /* __R_USB_CITRON_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
