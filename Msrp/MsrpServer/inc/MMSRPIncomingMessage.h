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

#ifndef MMSRPINCOMINGMESSAGE_H_
#define MMSRPINCOMINGMESSAGE_H_


// INCLUDES

// FORWARD DECLARATIONS
class MMSRPMessageObserver;
class CMSRPMessage;
class CMSRPResponse;
class MMSRPConnection;


// CLASS DECLARATIONS

/* MSRP Incoming message request interface with the subsession
 * 
 */
class MMSRPIncomingMessage
    {
public:
    
    enum TMSRPMessageType
        {
        EMSRPNotDefined,
        EMSRPMessage,
        EMSRPReport,
        EMSRPResponse
        };
        
    virtual TMSRPMessageType MessageType( ) = 0;
    
    virtual CMSRPMessage* GetIncomingMessage( ) = 0;
    
	virtual TBool SendResponseL( MMSRPMessageObserver* aMessageObserver, 
												MMSRPConnection& aConnection, TUint aResponseCode )=0;

    
    virtual TDesC8& TransactionId( ) = 0;
    
    virtual CMSRPResponse* GetIncomingResponse( ) = 0;
        
    };

#endif /* MMSRPINCOMINGMESSAGE_H_ */
