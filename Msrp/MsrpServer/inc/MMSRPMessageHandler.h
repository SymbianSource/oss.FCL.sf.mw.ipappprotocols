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

#ifndef MMSRPMESSAGEHANDLER_H_
#define MMSRPMESSAGEHANDLER_H_

// INCLUDES


// CLASS DECLARATIONS

/* MSRP Message request interface
 * 
 */
class MMSRPMessageHandler
    {
public:
    
    enum TMsrpHeaderType
        {
        EHeaderUnknown = -1,
        EToPath,
        EFromPath,
        EMessageId,
        ESuccessReport,
        EFailureReport,
        EByteRange,
        EContentType,
        EStatus,
        EExtenstion
        };
    
    enum TMsrpMsgEndStatus
        {
        EMessageEnd, // $
        EMessageContinues, // +
        EMessageTerminated // #
        };
    
    inline virtual ~MMSRPMessageHandler(){}
   
    virtual void AddHeaderL( TMsrpHeaderType aHeaderType, TPtrC8& aHeaderValue, TPtrC8& aHeaderTypeAndValue ) = 0;
    
    virtual void AddContentL( TPtrC8& aContent ) = 0;
    
    virtual void SetTransactionId( TPtrC8& aTransactionId ) = 0;
    
    virtual void SetStatusOfResponseL( TPtrC8& aStatusCode, TPtrC8& aStatusMessage ) = 0;
    
    virtual void EndOfMessageL( TMsrpMsgEndStatus aStatus ) = 0;
    
    };

#endif /* MMSRPMESSAGEHANDLER_H_ */
