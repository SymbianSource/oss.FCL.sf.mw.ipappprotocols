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
class CMSRPReport;
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
    
    /**
    * Returns the message instance belogning to this class 
    * @return MSRP message owned by this class, ownership is not transferred!
    */
    virtual CMSRPMessage* GetIncomingMessage( ) const = 0;
    
	virtual void SendResponseL( MMSRPMessageObserver* aMessageObserver, 
												MMSRPConnection& aConnection, TUint aResponseCode )=0;

    /**
    * Sends a report request to client 
    * @param aMessageObserver incoming message observer entity
    * @param aConnection connection instance
    * @param aStatusCode status code for the response
    * @return true if report was sent
    */
    virtual TBool SendReportL( MMSRPMessageObserver* aMessageObserver, 
                                    MMSRPConnection& aConnection, TUint aStatusCode ) = 0;
  
    virtual TDesC8& TransactionId( ) = 0;
    
    /**
    * Returns the response instance belogning to this class 
    * @return MSRP response owned by this class, ownership is not transferred!
    */
    virtual CMSRPResponse* GetIncomingResponse( ) const = 0;

    /**
    * Returns the report instance belogning to this class 
    * @return MSRP response owned by this class, ownership is not transferred!
    */
    virtual CMSRPReport* GetIncomingReport( ) const = 0;
    };

#endif /* MMSRPINCOMINGMESSAGE_H_ */
