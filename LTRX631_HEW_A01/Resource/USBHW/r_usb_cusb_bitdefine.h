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
* File Name    : r_usb_cusb_bitdefine.h
* Version      : 2.00
* Device(s)    : Renesas SH-Series, RX-Series
* Tool-Chain   : Renesas SuperH RISC engine Standard Toolchain
*              : Renesas RX Standard Toolchain
* OS           : Common to None and uITRON 4.0 Spec
* H/W Platform : Independent
* Description  : USB BIT definition
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 29.07.2011 0.50    First Release
******************************************************************************/

/* $Id: r_usb_cusb_bitdefine.h 143 2012-05-07 09:16:46Z tmura $ */

#ifndef __R_USB_CUSBDEFBITDEFINE_H__
#define __R_USB_CUSBDEFBITDEFINE_H__


/*****************************************************************************
Structure Types
******************************************************************************/

/* USB_597IP_PP Register definition */

/* System Configuration Control Register */
/*#define   USB_SYSCFG0     (*((REGP*)(USB_BASE+0x00))) */
/*#define   USB_SYSCFG1     (*((REGP*)(USB_BASE+0x02))) */
#define USB_XTAL                (0xC000u)   /* b15-14: Crystal selection */
#define   USB_XTAL48             0x8000u        /* 48MHz */
#define   USB_XTAL24             0x4000u        /* 24MHz */
#define   USB_XTAL12             0x0000u        /* 12MHz */
#define USB_XCKE                (0x2000u)   /* b13: External clock enable */
#define USB_CNTFLG              (0x1000u)   /* b12: Auto clock monitor */
#define USB_PLLC                (0x0800u)   /* b11: PLL control */
#define USB_SCKE                (0x0400u)   /* b10: USB clock enable */
#define USB_PCSDIS              (0x0200u)   /* b9: not CS wakeup */
#define USB_LPSME               (0x0100u)   /* b8: Low power sleep mode */
#define USB_HSE             (0x0080u)   /* b7: Hi-speed enable */
#define USB_DCFM                (0x0040u)   /* b6: Function select */
#define USB_DRPD                (0x0020u)   /* b5: D+/D- pull down control */
#define USB_DPRPU               (0x0010u)   /* b4: D+ pull up control */
#define USB_USBE                (0x0001u)   /* b0: USB module enable */

/* System Configuration Status Register */
/*#define   USB_SYSSTS0     (*((REGP*)(USB_BASE+0x04))) */
/*#define   USB_SYSSTS1     (*((REGP*)(USB_BASE+0x06))) */
#define USB_OVCBIT              (0x8000u)   /* b15-14: Over-current bit */
#define USB_OVCMON              (0xC000u)   /* b15-14: Over-current monitor */
#define USB_SOFEA               (0x0020u)   /* b5: SOF monitor */
#define USB_IDMON               (0x0004u)   /* b2: ID-pin monitor */
#define USB_LNST                (0x0003u)   /* b1-0: D+, D- line status */
#define   USB_SE1                0x0003u        /* SE1 */
#define   USB_FS_KSTS            0x0002u        /* Full-Speed K State */
#define   USB_FS_JSTS            0x0001u        /* Full-Speed J State */
#define   USB_LS_JSTS            0x0002u        /* Low-Speed J State */
#define   USB_LS_KSTS            0x0001u        /* Low-Speed K State */
#define   USB_SE0                0x0000u        /* SE0 */

/* Device State Control Register */
/*#define   USB_DVSTCTR0        (*((REGP*)(USB_BASE+0x08))) */
/*#define   USB_DVSTCTR1        (*((REGP*)(USB_BASE+0x0A))) */
#define USB_HNPBTOA         (0x0800u)   /* b11: Host negotiation protocol
                                             (BtoA) */
#define USB_EXTLP0              (0x0400u)   /* b10: External port */
#define USB_VBOUT               (0x0200u)   /* b9: VBUS output */
#define USB_WKUP                (0x0100u)   /* b8: Remote wakeup */
#define USB_RWUPE               (0x0080u)   /* b7: Remote wakeup sense */
#define USB_USBRST              (0x0040u)   /* b6: USB reset enable */
#define USB_RESUME              (0x0020u)   /* b5: Resume enable */
#define USB_UACT                (0x0010u)   /* b4: USB bus enable */
#define USB_RHST                (0x0007u)   /* b2-0: Reset handshake status */
#define   USB_HSPROC             0x0004u        /* HS handshake processing */
#define   USB_HSMODE             0x0003u        /* Hi-Speed mode */
#define   USB_FSMODE             0x0002u        /* Full-Speed mode */
#define   USB_LSMODE             0x0001u        /* Low-Speed mode */
#define   USB_UNDECID            0x0000u        /* Undecided */

/* Test Mode Register */
/*#define   USB_TESTMODE        (*((REGP*)(USB_BASE+0x0C))) */
#define USB_UTST                (0x000Fu)   /* b3-0: Test select */
#define   USB_H_TST_PACKET       0x000Cu        /* HOST TEST Packet */
#define   USB_H_TST_SE0_NAK  0x000Bu        /* HOST TEST SE0 NAK */
#define   USB_H_TST_K            0x000Au        /* HOST TEST K */
#define   USB_H_TST_J            0x0009u        /* HOST TEST J */
#define   USB_H_TST_NORMAL       0x0000u        /* HOST Normal Mode */
#define   USB_P_TST_PACKET       0x0004u        /* PERI TEST Packet */
#define   USB_P_TST_SE0_NAK  0x0003u        /* PERI TEST SE0 NAK */
#define   USB_P_TST_K            0x0002u        /* PERI TEST K */
#define   USB_P_TST_J            0x0001u        /* PERI TEST J */
#define   USB_P_TST_NORMAL       0x0000u        /* PERI Normal Mode */

/* Data Pin Configuration Register */
/*#define   USB_PINCFG          (*((REGP*)(USB_BASE+0x0E))) */
#define USB_LDRV                (0x8000u)   /* b15: Drive Current Adjust */
#define   USB_VIF1               0x0000u        /* VIF = 1.8V */
#define   USB_VIF3               0x8000u        /* VIF = 3.3V */
#define USB_INTA                (0x0001u)   /* b1: USB INT-pin active */


/* DMAx Pin Configuration Register */
/*#define   USB_DMA0CFG     (*((REGP*)(USB_BASE+0x10))) */
/*#define   USB_DMA1CFG     (*((REGP*)(USB_BASE+0x12))) */
#define USB_DREQA               (0x4000u)   /* b14: Dreq active select */
#define USB_BURST               (0x2000u)   /* b13: Burst mode */
#define USB_DACKA               (0x0400u)   /* b10: Dack active select */
#define USB_DFORM               (0x0380u)   /* b9-7: DMA mode select */
#define   USB_CPU_ADR_RD_WR  0x0000u        /* Address + RD/WR mode
                                                 (CPU bus) */
#define   USB_CPU_DACK_RD_WR     0x0100u        /* DACK + RD/WR (CPU bus) */
#define   USB_CPU_DACK_ONLY  0x0180u        /* DACK only (CPU bus) */
#define   USB_SPLIT_DACK_ONLY    0x0200u        /* DACK only (SPLIT bus) */
#define USB_DENDA               (0x0040u)   /* b6: Dend active select */
#define USB_PKTM                (0x0020u)   /* b5: Packet mode */
#define USB_DENDE               (0x0010u)   /* b4: Dend enable */
#define USB_OBUS                (0x0004u)   /* b2: OUTbus mode */


/* CFIFO/DxFIFO Port Register */
/*#define   USB_CFIFO           (*((REGP*)(USB_BASE+0x14))) */
/*#define   USB_CFIFO_8     (*((REGP8*)(USB_BASE+0x14))) */
/*#define   USB_D0FIFO          (*((REGP*)(USB_BASE+0x18))) */
/*#define   USB_D0FIFO_8        (*((REGP8*)(USB_BASE+0x18))) */
/*#define   USB_D1FIFO          (*((REGP*)(USB_BASE+0x1C))) */
/*#define   USB_D1FIFO_8        (*((REGP8*)(USB_BASE+0x1C))) */

/* CFIFO/DxFIFO Port Select Register */
/*#define   USB_CFIFOSEL        (*((REGP*)(USB_BASE+0x20))) */
/*#define   USB_D0FIFOSEL       (*((REGP*)(USB_BASE+0x28))) */
/*#define   USB_D1FIFOSEL       (*((REGP*)(USB_BASE+0x2C))) */
#define USB_RCNT                (0x8000u)   /* b15: Read count mode */
#define USB_REW             (0x4000u)   /* b14: Buffer rewind */
#define USB_DCLRM               (0x2000u)   /* b13: DMA buffer clear mode */
#define USB_DREQE               (0x1000u)   /* b12: DREQ output enable */
#define USB_MBW             (0x0400u)   /* b10: Maximum bit width for
                                                     FIFO access */
#define   USB_MBW_8          0x0000u        /*  8bit */
#define   USB_MBW_16             0x0400u        /* 16bit */
#define USB_BIGEND              (0x0100u)   /* b8: Big endian mode */
#define   USB_FIFO_LITTLE        0x0000u        /* Little endian */
#define   USB_FIFO_BIG           0x0100u        /* Big endian */
#define USB_ISEL                (0x0020u)   /* b5: DCP FIFO port direction
                                             select */
#define USB_CURPIPE         (0x000Fu)   /* b2-0: PIPE select */

/* CFIFO/DxFIFO Port Control Register */
/*#define   USB_CFIFOCTR        (*((REGP*)(USB_BASE+0x22))) */
/*#define   USB_D0FIFOCTR       (*((REGP*)(USB_BASE+0x2A))) */
/*#define   USB_D1FIFOCTR       (*((REGP*)(USB_BASE+0x2E))) */
#define USB_BVAL                (0x8000u)   /* b15: Buffer valid flag */
#define USB_BCLR                (0x4000u)   /* b14: Buffer clear */
#define USB_FRDY                (0x2000u)   /* b13: FIFO ready */
#define USB_DTLN                (0x0FFFu)   /* b11-0: FIFO data length */


/* Interrupt Enable Register 0 */
/*#define   USB_INTENB0     (*((REGP*)(USB_BASE+0x30))) */
#define USB_VBSE                (0x8000u)   /* b15: VBUS interrupt */
#define USB_RSME                (0x4000u)   /* b14: Resume interrupt */
#define USB_SOFE                (0x2000u)   /* b13: Frame update interrupt */
#define USB_DVSE                (0x1000u)   /* b12: Device state transition
                                             interrupt */
#define USB_CTRE                (0x0800u)   /* b11: Control transfer stage
                                             transition interrupt */
#define USB_BEMPE               (0x0400u)   /* b10: Buffer empty interrupt */
#define USB_NRDYE               (0x0200u)   /* b9: Buffer notready interrupt */
#define USB_BRDYE               (0x0100u)   /* b8: Buffer ready interrupt */

/* Interrupt Enable Register 1 */
/*#define   USB_INTENB1     (*((REGP*)(USB_BASE+0x32))) */
/*#define   USB_INTENB2     (*((REGP*)(USB_BASE+0x34))) */
#define USB_OVRCRE              (0x8000u)   /* b15: Over-current interrupt */
#define USB_BCHGE               (0x4000u)   /* b14: USB bus change interrupt */
#define USB_DTCHE               (0x1000u)   /* b12: Detach sense interrupt */
#define USB_ATTCHE              (0x0800u)   /* b11: Attach sense interrupt */
#define USB_EOFERRE         (0x0040u)   /* b6: EOF error interrupt */
#define USB_SIGNE               (0x0020u)   /* b5: SETUP IGNORE interrupt */
#define USB_SACKE               (0x0010u)   /* b4: SETUP ACK interrupt */

/* BRDY Interrupt Enable/Status Register */
/*#define   USB_BRDYENB     (*((REGP*)(USB_BASE+0x36))) */
/*#define   USB_BRDYSTS     (*((REGP*)(USB_BASE+0x46))) */
#define USB_BRDY9               (0x0200u)   /* b9: PIPE9 */
#define USB_BRDY8               (0x0100u)   /* b8: PIPE8 */
#define USB_BRDY7               (0x0080u)   /* b7: PIPE7 */
#define USB_BRDY6               (0x0040u)   /* b6: PIPE6 */
#define USB_BRDY5               (0x0020u)   /* b5: PIPE5 */
#define USB_BRDY4               (0x0010u)   /* b4: PIPE4 */
#define USB_BRDY3               (0x0008u)   /* b3: PIPE3 */
#define USB_BRDY2               (0x0004u)   /* b2: PIPE2 */
#define USB_BRDY1               (0x0002u)   /* b1: PIPE1 */
#define USB_BRDY0               (0x0001u)   /* b1: PIPE0 */

/* NRDY Interrupt Enable/Status Register */
/*#define   USB_NRDYENB     (*((REGP*)(USB_BASE+0x38))) */
/*#define   USB_NRDYSTS     (*((REGP*)(USB_BASE+0x48))) */
#define USB_NRDY9               (0x0200u)   /* b9: PIPE9 */
#define USB_NRDY8               (0x0100u)   /* b8: PIPE8 */
#define USB_NRDY7               (0x0080u)   /* b7: PIPE7 */
#define USB_NRDY6               (0x0040u)   /* b6: PIPE6 */
#define USB_NRDY5               (0x0020u)   /* b5: PIPE5 */
#define USB_NRDY4               (0x0010u)   /* b4: PIPE4 */
#define USB_NRDY3               (0x0008u)   /* b3: PIPE3 */
#define USB_NRDY2               (0x0004u)   /* b2: PIPE2 */
#define USB_NRDY1               (0x0002u)   /* b1: PIPE1 */
#define USB_NRDY0               (0x0001u)   /* b1: PIPE0 */

/* BEMP Interrupt Enable/Status Register */
/*#define   USB_BEMPENB     (*((REGP*)(USB_BASE+0x3A))) */
/*#define   USB_BEMPSTS     (*((REGP*)(USB_BASE+0x4A))) */
#define USB_BEMP9               (0x0200u)   /* b9: PIPE9 */
#define USB_BEMP8               (0x0100u)   /* b8: PIPE8 */
#define USB_BEMP7               (0x0080u)   /* b7: PIPE7 */
#define USB_BEMP6               (0x0040u)   /* b6: PIPE6 */
#define USB_BEMP5               (0x0020u)   /* b5: PIPE5 */
#define USB_BEMP4               (0x0010u)   /* b4: PIPE4 */
#define USB_BEMP3               (0x0008u)   /* b3: PIPE3 */
#define USB_BEMP2               (0x0004u)   /* b2: PIPE2 */
#define USB_BEMP1               (0x0002u)   /* b1: PIPE1 */
#define USB_BEMP0               (0x0001u)   /* b0: PIPE0 */

/* SOF Pin Configuration Register */
/*#define   USB_SOFCFG          (*((REGP*)(USB_BASE+0x3C))) */
#define USB_TRNENSEL            (0x0100u)   /* b8: Select transaction enable
                                             period */
#define USB_BRDYM               (0x0040u)   /* b6: BRDY clear timing */
#define USB_INTL                (0x0020u)   /* b5: Interrupt sense select */
#define USB_EDGESTS         (0x0010u)   /* b4:  */
#define USB_SOFMODE         (0x000Cu)   /* b3-2: SOF pin select */
#define   USB_SOF_125US      0x0008u        /* SOF 125us Frame Signal */
#define   USB_SOF_1MS            0x0004u        /* SOF 1ms Frame Signal */
#define   USB_SOF_DISABLE        0x0000u        /* SOF Disable */


/* Interrupt Status Register 0 */
/*#define   USB_INTSTS0     (*((REGP*)(USB_BASE+0x40))) */
#define USB_VBINT               (0x8000u)   /* b15: VBUS interrupt */
#define USB_RESM                (0x4000u)   /* b14: Resume interrupt */
#define USB_SOFR                (0x2000u)   /* b13: SOF update interrupt */
#define USB_DVST                (0x1000u)   /* b12: Device state transition
                                             interrupt */
#define USB_CTRT                (0x0800u)   /* b11: Control transfer stage
                                             transition interrupt */
#define USB_BEMP                (0x0400u)   /* b10: Buffer empty interrupt */
#define USB_NRDY                (0x0200u)   /* b9: Buffer notready interrupt */
#define USB_BRDY                (0x0100u)   /* b8: Buffer ready interrupt */
#define USB_VBSTS               (0x0080u)   /* b7: VBUS input port */
#define USB_DVSQ                (0x0070u)   /* b6-4: Device state */
#define   USB_DS_SPD_CNFG        0x0070u        /* Suspend Configured */
#define   USB_DS_SPD_ADDR        0x0060u        /* Suspend Address */
#define   USB_DS_SPD_DFLT        0x0050u        /* Suspend Default */
#define   USB_DS_SPD_POWR        0x0040u        /* Suspend Powered */
#define   USB_DS_SUSP            0x0040u        /* Suspend */
#define   USB_DS_CNFG            0x0030u        /* Configured */
#define   USB_DS_ADDS            0x0020u        /* Address */
#define   USB_DS_DFLT            0x0010u        /* Default */
#define   USB_DS_POWR            0x0000u        /* Powered */
#define USB_DVSQS               (0x0030u)   /* b5-4: Device state */
#define USB_VALID               (0x0008u)   /* b3: Setup packet detect flag */
#define USB_CTSQ                (0x0007u)   /* b2-0: Control transfer stage */
#define   USB_CS_SQER            0x0006u        /* Sequence error */
#define   USB_CS_WRND            0x0005u        /* Ctrl write nodata status
                                                 stage */
#define   USB_CS_WRSS            0x0004u        /* Ctrl write status stage */
#define   USB_CS_WRDS            0x0003u        /* Ctrl write data stage */
#define   USB_CS_RDSS            0x0002u        /* Ctrl read status stage */
#define   USB_CS_RDDS            0x0001u        /* Ctrl read data stage */
#define   USB_CS_IDST            0x0000u        /* Idle or setup stage */

/* Interrupt Status Register 1 */
/*#define   USB_INTSTS1     (*((REGP*)(USB_BASE+0x42))) */
/*#define   USB_INTSTS2     (*((REGP*)(USB_BASE+0x44))) */
#define USB_OVRCR               (0x8000u)   /* b15: Over-current interrupt */
#define USB_BCHG                (0x4000u)   /* b14: USB bus change interrupt */
#define USB_DTCH                (0x1000u)   /* b12: Detach sense interrupt */
#define USB_ATTCH               (0x0800u)   /* b11: Attach sense interrupt */
#define USB_EOFERR              (0x0040u)   /* b6: EOF-error interrupt */
#define USB_SIGN                (0x0020u)   /* b5: Setup ignore interrupt */
#define USB_SACK                (0x0010u)   /* b4: Setup ack interrupt */

/* Frame Number Register */
/*#define   USB_FRMNUM          (*((REGP*)(USB_BASE+0x4C))) */
#define USB_OVRN                (0x8000u)   /* b15: Overrun error */
#define USB_CRCE                (0x4000u)   /* b14: Received data error */
#define USB_FRNM                (0x07FFu)   /* b10-0: Frame number */

/* Micro Frame Number Register */
/*#define   USB_UFRMNUM     (*((REGP*)(USB_BASE+0x4E))) */
#define USB_UFRNM               (0x0007u)   /* b2-0: Micro frame number */


/* USB Address / Low Power Status Recovery Register */
/*#define   USB_USBADDR     (*((REGP*)(USB_BASE+0x50))) */
#define USB_USBADDR_MASK        (0x007Fu)   /* b6-0: USB address */
#define USB_STSRECOV            (0x0F00u)   /* b11-8: Status Recovery */
#define   USB_STSR_SET           0x0800u

/* USB Request Type Register */
/*#define   USB_USBREQ          (*((REGP*)(USB_BASE+0x54))) */
#define USB_BREQUEST            (0xFF00u)   /* b15-8: USB_BREQUEST */
#define USB_BMREQUESTTYPE       (0x00FFu)   /* b7-0: USB_BMREQUESTTYPE */
#define USB_BMREQUESTTYPEDIR    (0x0080u)   /* b7  : Data transfer direction */
#define USB_BMREQUESTTYPETYPE   (0x0060u)   /* b6-5: Type */
#define USB_BMREQUESTTYPERECIP  (0x001Fu)   /* b4-0: Recipient */

/* USB Request Value Register */
/*#define   USB_USBVAL          (*((REGP*)(USB_BASE+0x56))) */
#define USB_WVALUE              (0xFFFFu)   /* b15-0: wValue */
#define USB_DT_TYPE         (0xFF00u)
#define USB_GET_DT_TYPE(v)      (((v) & USB_DT_TYPE) >> 8)
#define USB_DT_INDEX            (0x00FFu)
#define USB_CONF_NUM            (0x00FFu)
#define USB_ALT_SET         (0x00FFu)

/* USB Request Index Register */
/*#define   USB_USBINDX     (*((REGP*)(USB_BASE+0x58))) */
#define USB_WINDEX              (0xFFFFu)   /* b15-0: wIndex */
#define USB_TEST_SELECT     (0xFF00u)   /* b15-b8: Test Mode Selectors */
#define   USB_TEST_J             0x0100u        /* Test_J */
#define   USB_TEST_K             0x0200u        /* Test_K */
#define   USB_TEST_SE0_NAK       0x0300u        /* Test_SE0_NAK */
#define   USB_TEST_PACKET        0x0400u        /* Test_Packet */
#define   USB_TEST_FORCE_ENABLE 0x0500u     /* Test_Force_Enable */
#define   USB_TEST_STSelectors   0x0600u        /* Standard test selectors */
#define   USB_TEST_RESERVED  0x4000u        /* Reserved */
#define   USB_TEST_VSTMODES  0xC000u        /* VendorSpecific test modes */
#define USB_EP_DIR              (0x0080u)   /* b7: Endpoint Direction */
#define   USB_EP_DIR_IN      0x0080u
#define   USB_EP_DIR_OUT         0x0000u

/* USB Request Length Register */
/*#define   USB_USBLENG     (*((REGP*)(USB_BASE+0x5A))) */
#define USB_WLENGTH         (0xFFFFu)   /* b15-0: wLength */


/* Pipe Window Select Register */
/*#define   USB_PIPESEL     (*((REGP*)(USB_BASE+0x64))) */
#define USB_PIPENM              (0x0007u)   /* b2-0: Pipe select */

/* Default Control Pipe Configuration Register */
/*#define   USB_DCPCFG          (*((REGP*)(USB_BASE+0x5C))) */
/* Pipe Configuration Register */
/*#define   USB_PIPECFG     (*((REGP*)(USB_BASE+0x68))) */
/* Refer to usbd_DefUSBIP.h */
#define USB_TYPE            (0xC000u)   /* b15-14: Transfer type */
/*#define   USB_BFRE            (0x0400u)   /* b10: Buffer ready interrupt
                                                 mode select */
/*#define   USB_DBLB            (0x0200u)   /* b9: Double buffer mode select */
/*#define   USB_CBTMD           (0x0100u)   /* b8: Continuous transfer mode
                                                 select */
/*#define   USB_SHTNAK          (0x0080u)   /* b7: Transfer end NAK */
/*#define   USB_DIR         (0x0010u)   /* b4: Transfer direction select */
/*#define   USB_EPNUM           (0x000Fu)   /* b3-0: Endpoint number select */

/* Pipe Buffer Configuration Register */
/*#define   USB_PIPEBUF     (*((REGP*)(USB_BASE+0x6A))) */
/* Refer to usbd_DefUSBIP.h */
#define USB_BUFSIZE         (0x7C00u)   /* b14-10: Pipe buffer size */
#define USB_BUFNMB              (0x007Fu)   /* b6-0: Pipe buffer number */
#define USB_PIPE0BUF            (256u)
#define USB_PIPEXBUF            (64u)

/* Default Control Pipe Maxpacket Size Register */
/* Pipe Maxpacket Size Register */
/*#define   USB_DCPMAXP     (*((REGP*)(USB_BASE+0x5E))) */
/* Pipe Maxpacket Size Register */
/*#define   USB_PIPEMAXP        (*((REGP*)(USB_BASE+0x6C))) */
#define USB_DEVSEL              (0xF000u)   /* b15-14: Device address select */
#define USB_MAXP                (0x007Fu)   /* b6-0: Maxpacket size of default
                                             control pipe */
#define USB_MXPS                (0x07FFu)   /* b10-0: Maxpacket size */

/* Pipe Cycle Configuration Register */
/*#define   USB_PIPEPERI        (*((REGP*)(USB_BASE+0x6E))) */
/* Refer to usbd_DefUSBIP.h */
/*#define   USB_IFIS            (0x1000u)   /* b12: Isochronous in-buffer
                                                 flash mode select */
/*#define   USB_IITV            (0x0007u)   /* b2-0: Isochronous interval */

/* Default Control Pipe Control Register */
/*#define   USB_DCPCTR          (*((REGP*)(USB_BASE+0x60))) */
/* Pipex Control Register */
/*#define   USB_PIPE1CTR        (*((REGP*)(USB_BASE+0x70))) */
/*#define   USB_PIPE2CTR        (*((REGP*)(USB_BASE+0x72))) */
/*#define   USB_PIPE3CTR        (*((REGP*)(USB_BASE+0x74))) */
/*#define   USB_PIPE4CTR        (*((REGP*)(USB_BASE+0x76))) */
/*#define   USB_PIPE5CTR        (*((REGP*)(USB_BASE+0x78))) */
/*#define   USB_PIPE6CTR        (*((REGP*)(USB_BASE+0x7A))) */
/*#define   USB_PIPE7CTR        (*((REGP*)(USB_BASE+0x7C))) */
/*#define   USB_PIPE8CTR        (*((REGP*)(USB_BASE+0x7E))) */
/*#define   USB_PIPE9CTR        (*((REGP*)(USB_BASE+0x80))) */
#define USB_BSTS                (0x8000u)   /* b15: Buffer status */
#define USB_SUREQ               (0x4000u)   /* b14: Send USB request  */
#define USB_INBUFM              (0x4000u)   /* b14: IN buffer monitor
                                                (Only for PIPE1 to 5) */
#define USB_CSCLR               (0x2000u)   /* b13: c-split status clear */
#define USB_CSSTS               (0x1000u)   /* b12: c-split status */
#define USB_SUREQCLR            (0x0800u)   /* b11: stop setup request */
#define USB_ATREPM              (0x0400u)   /* b10: Auto repeat mode */
#define USB_ACLRM               (0x0200u)   /* b9: buffer auto clear mode */
#define USB_SQCLR               (0x0100u)   /* b8: Sequence bit clear */
#define USB_SQSET               (0x0080u)   /* b7: Sequence bit set */
#define USB_SQMON               (0x0040u)   /* b6: Sequence bit monitor */
#define USB_PBUSY               (0x0020u)   /* b5: pipe busy */
#define USB_PINGE               (0x0010u)   /* b4: ping enable */
#define USB_CCPL                (0x0004u)   /* b2: Enable control transfer
                                             complete */
#define USB_PID                 (0x0003u)   /* b1-0: Response PID */
#define USB_PID_STALL           0x0002u     /* STALL */
#define USB_PID_BUF             0x0001u     /* BUF */
#define USB_PID_NAK             0x0000u     /* NAK */


/* PIPExTRE */
/*#define   USB_PIPE1TRE        (*((REGP*)(USB_BASE+0x90))) */
/*#define   USB_PIPE2TRE        (*((REGP*)(USB_BASE+0x94))) */
/*#define   USB_PIPE3TRE        (*((REGP*)(USB_BASE+0x98))) */
/*#define   USB_PIPE4TRE        (*((REGP*)(USB_BASE+0x9C))) */
/*#define   USB_PIPE5TRE        (*((REGP*)(USB_BASE+0xA0))) */
#define USB_TRENB               (0x0200u)   /* b9: Transaction count enable */
#define USB_TRCLR               (0x0100u)   /* b8: Transaction count clear */


/* PIPExTRN */
/*#define   USB_PIPE1TRN        (*((REGP*)(USB_BASE+0x92))) */
/*#define   USB_PIPE2TRN        (*((REGP*)(USB_BASE+0x96))) */
/*#define   USB_PIPE3TRN        (*((REGP*)(USB_BASE+0x9A))) */
/*#define   USB_PIPE4TRN        (*((REGP*)(USB_BASE+0x9E))) */
/*#define   USB_PIPE5TRN        (*((REGP*)(USB_BASE+0xA2))) */
#define USB_TRNCNT              (0xFFFFu)   /* b15-0: Transaction counter */


/* DEVADDx */
/*#define   USB_DEVADD0     (*((REGP*)(USB_BASE+0xD0))) */
/*#define   USB_DEVADD1     (*((REGP*)(USB_BASE+0xD2))) */
/*#define   USB_DEVADD2     (*((REGP*)(USB_BASE+0xD4))) */
/*#define   USB_DEVADD3     (*((REGP*)(USB_BASE+0xD6))) */
/*#define   USB_DEVADD4     (*((REGP*)(USB_BASE+0xD8))) */
/*#define   USB_DEVADD5     (*((REGP*)(USB_BASE+0xDA))) */
/*#define   USB_DEVADD6     (*((REGP*)(USB_BASE+0xDC))) */
/*#define   USB_DEVADD7     (*((REGP*)(USB_BASE+0xDE))) */
/*#define   USB_DEVADD8     (*((REGP*)(USB_BASE+0xE0))) */
/*#define   USB_DEVADD9     (*((REGP*)(USB_BASE+0xE2))) */
/*#define   USB_DEVADDA     (*((REGP*)(USB_BASE+0xE4))) */
#define USB_UPPHUB              (0x7800u)
#define USB_HUBPORT         (0x0700u)
#define USB_USBSPD              (0x00C0u)
#define USB_RTPORT              (0x0001u)


/* Invalid Register. for CS-return from PCUT */
/*#pragma   USB_ADDRESS     INVALID_REG (_USBBASE + EEh) */

#endif  /* __R_USB_CUSBDEFBITDEFINE_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
