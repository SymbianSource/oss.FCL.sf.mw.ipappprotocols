/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef __MSRPSERVERCOMMON_H
#define __MSRPSERVERCOMMON_H

#include <flogger.h>
#include <e32svr.h>	// For RDebug

// CONSTANTS

// ENUMS
enum TMSRPPanicCodes
	{
	EMSRPServerStartError,
	EMSRPServerBadRequestError,
	EMSRPServerBadDescriptorError,
	EMSRPServerNotSupportedError,
	EMSRPServerSchedulerError,
	EMSRPServerNullPointer
	};

// used in CMSRPListeners/CMSRPConnection
enum TMSRPChunkProcessingStatus
	{
	EMSRPMessageNotConsumed,
	EMSRPMessageConsumed,
	EMSRPMessageConsumedAndCompleted,
	EMSRPFinalMessageProcessed,
	EMSRPMessageError
	};

// FUNCTION PROTOTYPES
void PanicServer( TMSRPPanicCodes aPanicCode );

// Terminology for now.
enum TConnectionDirection	{
		
		EDownstream, // downstream is implies "connect" and the othe end listens.
		EUpstream	// upstream implies "listen" and the other end connects.

	};

// Connection state for users of the connection. If the connection usable or not.
enum TConnectionState
	{
		Usable,
		NoUsable
	};

// Sub Session FSM Events
enum TMSRPFSMEvent
{
	// Events generated from as a result of client requests.	
	ELocalMSRPPathEvent=0,     // 0
	EMSRPConnectEvent,     	   // 1 
	EMSRPListenEvent,	       // 2 Maps to EMSRPListenConnections.
	EMSRPListenMessagesEvent,  // 3
	EMSRPListenSendResultEvent,  // 4 Client Setup event to setup listner for incomming results.
	EMSRPDataSendMessageEvent,	// 5 Client Event to send data.	
	EMSRPDataSendMessageCancelEvent, // 6

	// Should be CancelClientSetup1, 2 and 3 respectively.
	EMSRPCancelReceivingEvent, 			// 7  Maps to EMSRPCancelReceiving. What is EMSRPCancelReceiving for? !!
	EMSRPCancelSendRespListeningEvent,  // 8 Maps to EMSRPCancelSendRespListening. What is EMSRPCancelSendRespListening for? !!
	
	EMSRPProcessQueuedRequestsEvent,    // 9
	EMSRPProgressReportsEvent,          // 10
	
	// Events generated from the Message Layer.
	EMSRPDataSendCompleteEvent,         // 11
	EMSRPResponseSendCompleteEvent,		// 12
	EMSRPSendProgressEvent,             // 13
    EMSRPReceiveProgressEvent,          // 14
	EMSRPDataCancelledEvent,            // 15
	
	// From Connection Layer
	EMSRPIncomingMessageReceivedEvent, // 16
	EConnectionStateChangedEvent,	   // 17
	EUncalimedMessageReceivedEvent,    // 18
    EMSRPReportSendCompleteEvent,     // 19
};
#endif      // __MSRPSERVERCOMMON_H

// End of File
