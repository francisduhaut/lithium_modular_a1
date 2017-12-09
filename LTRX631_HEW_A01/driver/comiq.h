/*
 * comiq.h
 *
 *  Created on: 05/09/2013
 *      Author: duhautf
 */

#ifndef COMIQ_H_
#define COMIQ_H_

#include "type.h"

#define in16(var,l,h)  		    var = ((uint)(l)) | (((uint)(h)) << 8)
#define out16(l,h,val) 		    { l = (uchar)val; h = (uchar)(val >> 8); }
#define in24(var,l,m,h)  	    var = ((ulong)(l)) | (((ulong)(m)) << 8 | ((ulong)(h)) << 16)
#define out24(l,m,h,val) 	    { l = (uchar)val; m = (uchar)(val >> 8); h = (uchar)(val >> 16); }

#define XON                     17    // Xon : CTRL + Q
#define XOFF                    19    // Xoff : CTRL + S

#define NO_OK		            0
#define OK			            1

#define	NB_SCAN_ADDR			40	  // Number max of address scanned
#define	MAX_LEN				    128   // MAx length of frame
#define	TIME_WIIQMEAS           300L  // Time IQ Measure (s)



/* Technology types */
// MODIF R2.7
//#define TEC_POS     22  // Position of techno label
//#define TEC_POS     23  // Position of techno label
#define TEC_POS     24  // Position of techno label
#define TEC_NB      19
#define TEC_GEL     1
#define TEC_PBO     2
#define TEC_PNEU    3
#define TEC_WF      4
#define TEC_PREM    5
#define TEC_FASTEU  6
#define TEC_FASTUS  7
#define TEC_PZM     8
#define TEC_XFC     9
#define TEC_HGB     10 
#define TEC_LOWMAIN 11
#define TEC_PZM2    12
#define TEC_VRLA    13
#define TEC_OPP    	14
#define TEC_TPPL    15
#define TEC_PZQ    	16
#define TEC_COLD   	17
#define TEC_LOWCHG	18

// State Link
//-----------
// Level 1
#define StateIQOff			  0x00
#define StateIQRFTrAddr       0x01
#define StateIQRFTrModAddr    0x02
#define StateIQRFTrLoadWIIQ   0x03
#define StateIQRFTrEraseFlash 0x04
#define StateIQScan	  		  0x05
#define StateIQLink	    	  0x06
#define StateIQMeas           0x07
#define StateIQCgInfo         0x08
#define StateIQScanOff 		  0x09
#define StateIQResetFlagEqual 0x0A
#define StateIQReadTPPL       0x0B
#define StateIQReadEasyKit    0x0C
#define StateIQSendInChargeData     0x0D
#define StateIQSendEndChargeData    0x0E
#define StateIQWriteJ1939Adr  0x0F


// Level 2
#define StateIQPh1	          0x00
#define StateIQPh2 	  	      0x01
#define StateIQPh3            0x02
#define StateIQPh4            0x03
#define StateIQPh5 	  	      0x04
#define StateIQPh6 	  	      0x05
#define StateIQPh7 	  	      0x06
#define StateIQPh8 	  	      0x08
// State Internal
//---------------
// Level 1
#define State1Off			    0x00
#define State1RFTrAddr	        0x01
#define State1RFTrVers	        0x02
#define State1RFTrModAddr       0x03
#define State1RFTrCom   	    0x04
#define State1RFTrSaveWIIQ      0x05
#define State1RFTrLoadWIIQ      0x06
#define State1RFTrEraseFlash    0x07
#define State1OnScan		    0x08
#define State1ReqAddrScan	    0x09
#define State1AddrOffScan 	    0x0A
#define State1Measure		    0x0B
#define State1CalCurOffset      0x0C
#define State1BatInfo		    0x0D
#define State1CgInfo		    0x0E
#define State1StartAddrOffScan  0x0F
#define State1ResetFlagEqual    0x10
#define State1TPPLRead          0x11
#define State1EasyKitRead       0x12
#define State1IQSendInChargeData    0x13
#define State1IQSendEndChargeData   0x14
#define State1WriteJ1939Adr   0x15

// Level 2
#define State2TxInit		    0x00
#define State2TxWait		    0x01
#define State2RxInit  		    0x02
#define State2RxWait	  	    0x03
// IQ transceiver
//---------------
#define	JBUS_ANS			    0x01
#define	INT_REQ				    0x02
#define	EXT_REQ_WITHOUT_ANSWER	0x03
#define	EXT_REQ_WITH_ANSWER	    0x04
// Wi-IQ code
#define	CODE_CURRENT_CALIB	    0x03
#define	CODE_SCAN_ADDRESS	    0x0A
#define	CODE_MEASURE_REQUEST    0x08
#define	CODE_BAT_INFO		    0x0D
#define	CODE_CHARGER_INFO	    0x0E
#define CODE_DUAL_CHARGE_INFO   0x14
#define CODE_RESET_EQUAL        0xF8
#define CODE_TPPL_READ          0xF9
// Wi-IQ cycle phase
#define START_OF_DISCHARGE	    0x01
#define DISCHARGE			    0x02
#define START_OF_CHARGE		    0x03
#define IN_CHARGE				0x04
#define END_OF_CHARGE			0x05
#define AFTER_CHARGE			0x06
#define END_OF_CYCLE			0x07
// Timeout - Nb retry
#define TIMEOUT_IQ				500L    // (ms) Timeout request Rx/Tx
#define NB_TIMEOUT_IQ			20	    // Number of timeout
#define NB_INT_REQ				5	    // Number of retry for internal request
#define NB_REQ_WITH_ANSWER		10 	    // Number of retry for request with answer
#define NB_REQ_WITHOUT_ANSWER	5	    // Number of request without answer
#define TIMEOUT_WIIQSCAN        120L    // (s) Timeout Wi-IQ Scan
#define NB_CUR_STEP		        10	    // Number of current step for link
// Tolerance
#define OPEN_VCEL_TOL	        10	    // Open voltage tolerance : 100mV/elt
#define VCEL_TOL_POS	        40	    // Positive voltage tolerance : 100mV/elt
#define VCEL_TOL_NEG            50	    // Negative voltage tolerance : 500mV/elt
#define I_TOL                   60      // Current tolerance : 6A

/* ********** DataW.RW (ulong) MOD V3.4 ********** */
#define BitEasyKitRead          (ulong)0x00000001  /* 1 = EasyKit can be read */
#define BitEasyKitLink          (ulong)0x00000002  /* 1 = EasyKit is linked */
#define BitSendInChargeData     (ulong)0x00000004  /* 1 = Send In charge Data charger */
#define BitSendEndChargeData    (ulong)0x00000008  /* 1 = Send End charge Data charger */
#define BitWriteJ1939Adr        (ulong)0x00000010  /* 1 = Write J1939 adr into IQ transceiver */

#define EasyKitRead             (DataW.RW & BitEasyKitRead)
#define EasyKitReadOn           (DataW.RW |= BitEasyKitRead)
#define EasyKitReadOff          (DataW.RW &= ~BitEasyKitRead)
#define EasyKitLink             (DataW.RW & BitEasyKitLink)
#define EasyKitLinkOn           (DataW.RW |= BitEasyKitLink)
#define EasyKitLinkOff          (DataW.RW &= ~BitEasyKitLink)
#define SendInChargeData        (DataW.RW & BitSendInChargeData)
#define SendInChargeDataOn      (DataW.RW |= BitSendInChargeData)
#define SendInChargeDataOff     (DataW.RW &= ~BitSendInChargeData)
#define SendEndChargeData       (DataW.RW & BitSendEndChargeData)
#define SendEndChargeDataOn     (DataW.RW |= BitSendEndChargeData)
#define SendEndChargeDataOff    (DataW.RW &= ~BitSendEndChargeData)
#define WriteJ1939Adr           (DataW.RW & BitWriteJ1939Adr)
#define WriteJ1939AdrOn         (DataW.RW |= BitWriteJ1939Adr)
#define WriteJ1939AdrOff        (DataW.RW &= ~BitWriteJ1939Adr)

/* ********** IQData.Status (ulong) ********** */
#define BitTxTimeout  	        (ulong)0x00000001  /* Transmitter timeout bit */
#define BitRxRecv			    (ulong)0x00000002  /* Receive character bit */
#define BitRxTimeout	        (ulong)0x00000004  /* Receiver timeout bit */
#define BitInFrameTx		    (ulong)0x00000008  /* 1 = Frame Tx ok */
#define BitInFrameRx	        (ulong)0x00000010  /* 1 = Frame Rx ok */
#define BitReqOk		        (ulong)0x00000020  /* 1 = Request ok */
#define BitInRFTrLink	        (ulong)0x00000040  /* 1 = RF Transceiver link in process */
#define BitRFTrLink	  	        (ulong)0x00000080  /* 1 = RF Transceiver link ok */
#define BitInWIIQScan 	        (ulong)0x00000100  /* 1 = Scan Wi-IQ in process */
#define BitWIIQScan		        (ulong)0x00000200  /* 1 = Scan Wi-IQ ok */
#define BitInWIIQLink 	        (ulong)0x00000400  /* 1 = Link Wi-IQ in proceed */
#define BitWIIQLink		        (ulong)0x00000800  /* 1 = Link Wi-IQ ok */
#define BitInWIIQMeas	        (ulong)0x00001000  /* 1 = Measure Wi-IQ in process */
#define BitWIIQMeas		        (ulong)0x00002000  /* 1 = Measure Wi-IQ ok */
#define BitInWIIQCgInfo         (ulong)0x00004000  /* 1 = Send charger info to Wi-IQ in process */
#define BitWIIQCgInfo	        (ulong)0x00008000  /* 1 = Send charger info to Wi-IQ ok */
#define BitFrameTxJb	        (ulong)0x10000000  /* 1 = Frame Tx Jbus */
#define BitFrameRxJb            (ulong)0x20000000  /* 1 = Frame Rx Jbus */
#define BitInRFTrModAddr        (ulong)0x40000000  /* 1 = Modify key address */

#define IQTxTimeout		        (IQData.Status & BitTxTimeout)
#define IQTxTimeoutOn	        (IQData.Status |= BitTxTimeout)
#define IQTxTimeoutOff	        (IQData.Status &= ~BitTxTimeout)
#define IQRxRecv		  	    (IQData.Status & BitRxRecv)
#define IQRxRecvOn		  	    (IQData.Status |= BitRxRecv)
#define IQRxRecvOff			    (IQData.Status &= ~BitRxRecv)
#define IQRxTimeout 			(IQData.Status & BitRxTimeout)
#define IQRxTimeoutOn 		    (IQData.Status |= BitRxTimeout)
#define IQRxTimeoutOff  	    (IQData.Status &= ~BitRxTimeout)
#define IQInFrameTx			    (IQData.Status & BitInFrameTx)
#define IQInFrameTxOn 		    (IQData.Status |= BitInFrameTx)
#define IQInFrameTxOff  	    (IQData.Status &= ~BitInFrameTx)
#define IQInFrameRx			    (IQData.Status & BitInFrameRx)
#define IQInFrameRxOn 		    (IQData.Status |= BitInFrameRx)
#define IQInFrameRxOff          (IQData.Status &= ~BitInFrameRx)
#define IQReqOk   			    (IQData.Status & BitReqOk)
#define IQReqOkOn		        (IQData.Status |= BitReqOk)
#define IQReqOkOff		        (IQData.Status &= ~BitReqOk)
#define IQInRFTrLink  		    (IQData.Status & BitInRFTrLink)
#define IQInRFTrLinkOn	        (IQData.Status |= BitInRFTrLink)
#define IQInRFTrLinkOff	        (IQData.Status &= ~BitInRFTrLink)
#define IQRFTrLink  			(IQData.Status & BitRFTrLink)
#define IQRFTrLinkOn	        (IQData.Status |= BitRFTrLink)
#define IQRFTrLinkOff	        (IQData.Status &= ~BitRFTrLink)
#define IQInWIIQScan            (IQData.Status & BitInWIIQScan)
#define IQInWIIQScanOn	        (IQData.Status |= BitInWIIQScan)
#define IQInWIIQScanOff	        (IQData.Status &= ~BitInWIIQScan)
#define IQWIIQScan     	        (IQData.Status & BitWIIQScan)
#define IQWIIQScanOn		    (IQData.Status |= BitWIIQScan)
#define IQWIIQScanOff		    (IQData.Status &= ~BitWIIQScan)
#define IQInWIIQLink            (IQData.Status & BitInWIIQLink)
#define IQInWIIQLinkOn          (IQData.Status |= BitInWIIQLink)
#define IQInWIIQLinkOff	        (IQData.Status &= ~BitInWIIQLink)
#define IQWIIQLink     	        (IQData.Status & BitWIIQLink)
#define IQWIIQLinkOn		    (IQData.Status |= BitWIIQLink)
#define IQWIIQLinkOff	  	    (IQData.Status &= ~BitWIIQLink)
#define IQInWIIQMeas            (IQData.Status & BitInWIIQMeas)
#define IQInWIIQMeasOn  	    (IQData.Status |= BitInWIIQMeas)
#define IQInWIIQMeasOff 	    (IQData.Status &= ~BitInWIIQMeas)
#define IQWIIQMeas              (IQData.Status & BitWIIQMeas)
#define IQWIIQMeasOn  	        (IQData.Status |= BitWIIQMeas)
#define IQWIIQMeasOff	        (IQData.Status &= ~BitWIIQMeas)
#define IQInWIIQCgInfo          (IQData.Status & BitInWIIQCgInfo)
#define IQInWIIQCgInfoOn	    (IQData.Status |= BitInWIIQCgInfo)
#define IQInWIIQCgInfoOff	    (IQData.Status &= ~BitInWIIQCgInfo)
#define IQWIIQCgInfo            (IQData.Status & BitWIIQCgInfo)
#define IQWIIQCgInfoOn	        (IQData.Status |= BitWIIQCgInfo)
#define IQWIIQCgInfoOff	        (IQData.Status &= ~BitWIIQCgInfo)
#define IQFrameTxJb			    (IQData.Status & BitFrameTxJb)
#define IQFrameTxJbOn		    (IQData.Status |= BitFrameTxJb)
#define IQFrameTxJbOff	        (IQData.Status &= ~BitFrameTxJb)
#define IQFrameRxJb			    (IQData.Status & BitFrameRxJb)
#define IQFrameRxJbOn		    (IQData.Status |= BitFrameRxJb)
#define IQFrameRxJbOff	        (IQData.Status &= ~BitFrameRxJb)
#define IQInRFTrModAddr         (IQData.Status & BitInRFTrModAddr)
#define IQInRFTrModAddrOn	    (IQData.Status |= BitInRFTrModAddr)
#define IQInRFTrModAddrOff      (IQData.Status &= ~BitInRFTrModAddr)

#define BatDefLowLevel        0x02
#define BatWarningHighTp      0x04
#define BatWarningEqual       0x10
#define BatWarningLowLevel    0x20
#define BatWarningBal1        0x40
#define BatWarningBal2        0x80


void RFDongle (uchar state);
void Init_COMIQ(void);
void SCIIQ_Chrono(void);
void IQInitRx(void);
void IQInitTx(uchar ReqType);
uchar IQFrameRxRecv(void);
uchar IQScanCode(void);
void IQChrono(void);

#endif 