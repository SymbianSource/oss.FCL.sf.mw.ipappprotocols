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

#ifndef MMSRPOUTGOINGMESSAGE_H_
#define MMSRPOUTGOINGMESSAGE_H_


// INCLUDES

// FORWARD DECLARATIONS

class MMSRPIncomingMessage;

// CLASS DECLARATIONS

/* MSRP Outgoing message request interface with the subsession
 * 
 */
class MMSRPOutgoingMessage
    {
public:
    
    virtual void SendMessageL( MMSRPConnection& aConnection ) = 0;
    
    virtual TBool IsOwnerOfResponse( MMSRPIncomingMessage& aIncomingMessage ) = 0;
    
    virtual TUint ConsumeResponseL( MMSRPIncomingMessage& aIncomingMessage ) = 0;
    
    virtual TBool IsMessageComplete() = 0;
    
    /**
    * Returns the responses error code
    * @param aMessageId Buffer for message id, allocates buffer, ownership is transferred
    * @return response error code, defined in TMSRPErrorCodes
    */
    virtual TUint GetSendResultL( HBufC8* &aMessageId ) = 0;
    
    /**
    * Used to check whether the Failure-Report header is set as "partial"
    * @return true if failure-report header is set to partial
    */
    virtual TBool IsFailureHeaderPartial() = 0; 
    };
#endif /* MMSRPOUTGOINGMESSAGE_H_ */
