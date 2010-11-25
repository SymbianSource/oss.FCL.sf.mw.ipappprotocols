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

// forwrd declarations
class CMSRPMessageHandler;


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
        EMessageNotDefined,
        EMessageEnd, // $
        EMessageContinues, // +
        EMessageTerminated // #
        };
    
    inline virtual ~MMSRPMessageHandler(){}
   
    virtual void AddHeaderL( TMsrpHeaderType aHeaderType, TPtrC8& aHeaderValue, TPtrC8& aHeaderTypeAndValue ) = 0;
    
    virtual void AddContentL( TPtrC8& aContent, TBool aByteRangeFound = EFalse ) = 0;
    
    virtual void SetTransactionId( TDesC8& aTransactionId ) = 0;
    
    virtual void SetStatusOfResponseL( TPtrC8& aStatusCode, TPtrC8& aStatusMessage ) = 0;
    
    virtual void EndOfMessageL( TMsrpMsgEndStatus aStatus ) = 0;
    
    /**
    * Check if this chunk belongs to one of messages received 
    * earlier. If so, the messages are combined to one
    * entity
    * @return true if given message belongs to already received chunk
    */
    virtual TBool CheckMessageChunkL( CMSRPMessageHandler& aOtherMessage ) = 0;
    };

#endif /* MMSRPMESSAGEHANDLER_H_ */
