/*******************************************************************************
*
* Copyright (C) 2017 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
*******************************************************************************/
/******************************************************************************/
/**
 *
 * @file xv_sditxss_log.c
 *
 *
 * @note	None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -----------------------------------------------
 * 1.0   jsr  07/17/17 Initial release.
 * </pre>
 *
*******************************************************************************/

/******************************* Include Files ********************************/

#include "xv_sditxss.h"

/**************************** Function Prototypes *****************************/

/**************************** Function Definitions ****************************/

/*****************************************************************************/
/**
* This function will reset the driver's logging mechanism.
*
* @param	InstancePtr is a pointer to the xv_sditxss core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_SdiTxSs_LogReset(XV_SdiTxSs *InstancePtr)
{
	/* Verify arguments. */
	Xil_AssertVoid(InstancePtr != NULL);

	InstancePtr->Log.HeadIndex = 0;
	InstancePtr->Log.TailIndex = 0;
}

/*****************************************************************************/
/**
* This function will insert an event in the driver's logginc mechanism.
*
* @param	InstancePtr is a pointer to the XV_SdiTxSs core instance.
* @param	Evt is the event type to log.
* @param	Data is the associated data for the event.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_SdiTxSs_LogWrite(XV_SdiTxSs *InstancePtr, XV_SdiTxSs_LogEvent Evt,
	u8 Data)
{
	/* Verify arguments. */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(Evt <= (XV_SDITXSS_LOG_EVT_DUMMY));
	Xil_AssertVoid(Data < 0xFF);

	/* Write data and event into log buffer */
	InstancePtr->Log.DataBuffer[InstancePtr->Log.HeadIndex] =
			(Data << 8) | Evt;

	/* Update head pointer if reached to end of the buffer */
	if (InstancePtr->Log.HeadIndex ==
			(u8)((sizeof(InstancePtr->Log.DataBuffer) / 2) - 1)) {
		/* Clear pointer */
		InstancePtr->Log.HeadIndex = 0;
	} else {
		/* Increment pointer */
		InstancePtr->Log.HeadIndex++;
	}

	/* Check tail pointer. When the two pointer are equal, then the buffer
	 * is full. In this case then increment the tail pointer as well to
	 * remove the oldest entry from the buffer. */
	if (InstancePtr->Log.TailIndex == InstancePtr->Log.HeadIndex) {
		if (InstancePtr->Log.TailIndex ==
			(u8)((sizeof(InstancePtr->Log.DataBuffer) / 2) - 1)) {
			InstancePtr->Log.TailIndex = 0;
		} else {
			InstancePtr->Log.TailIndex++;
		}
	}
}

/*****************************************************************************/
/**
* This function will read the last event from the log.
*
* @param	InstancePtr is a pointer to the XV_SdiTxSs core instance.
*
* @return	The log data.
*
* @note		None.
*
******************************************************************************/
u16 XV_SdiTxSs_LogRead(XV_SdiTxSs *InstancePtr)
{
	u16 Log;

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Check if there is any data in the log */
	if (InstancePtr->Log.TailIndex == InstancePtr->Log.HeadIndex) {
		Log = 0;
	} else {
		Log = InstancePtr->Log.DataBuffer[InstancePtr->Log.TailIndex];

		/* Increment tail pointer */
		if (InstancePtr->Log.TailIndex ==
			(u8)((sizeof(InstancePtr->Log.DataBuffer) / 2) - 1)) {
			InstancePtr->Log.TailIndex = 0;
		} else {
			InstancePtr->Log.TailIndex++;
		}
	}

	return Log;
}

/*****************************************************************************/
/**
* This function will print the entire log.
*
* @param	InstancePtr is a pointer to the XV_SdiTxSs core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_SdiTxSs_LogDisplay(XV_SdiTxSs *InstancePtr)
{
	u16 Log;
	u8 Evt;
	u8 Data;

	/* Verify argument. */
	Xil_AssertVoid(InstancePtr != NULL);

	xil_printf("\r\n\n\nSDI TX log\r\n");
	xil_printf("------\r\n");

	/* Read log data */
	Log = XV_SdiTxSs_LogRead(InstancePtr);

	while (Log != 0) {
		/* Event */
		Evt = Log & 0xff;

		/* Data */
		Data = (Log >> 8) & 0xFF;
		Data = Data;

		switch (Evt) {
		case (XV_SDITXSS_LOG_EVT_NONE):
			xil_printf("SDI TXSS log end\r\n-------\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_SDITX_INIT):
		    xil_printf("Initializing SDI TX core....\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_VTC_INIT):
		    xil_printf("Initializing VTC core....\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_START):
		    xil_printf("Start SDI TX Subsystem....\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_STOP):
		    xil_printf("Stop SDI TX Subsystem....\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_RESET):
		    xil_printf("Reset SDI TX Subsystem....\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_STREAMCFG):
		    xil_printf("Configure SDI TX Core....\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_STREAMUP):
		    xil_printf("TX Stream is Up\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_STREAMDOWN):
		    xil_printf("TX Stream is Down\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_OVERFLOW):
		    xil_printf("TX Stream is Overflowed\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_UNDERFLOW):
		    xil_printf("TX Stream is Underflowed\r\n");
			break;
	    case (XV_SDITXSS_LOG_EVT_STREAMSTART):
		    xil_printf("TX Stream Start\r\n");
			break;
		default:
			xil_printf("Unknown event\r\n");
			break;
		}

		/* Read log data */
		Log = XV_SdiTxSs_LogRead(InstancePtr);
	}
}
