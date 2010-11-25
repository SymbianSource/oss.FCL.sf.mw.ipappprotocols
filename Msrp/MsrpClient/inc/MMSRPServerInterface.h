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

#ifndef MMSRPSERVERINTERFACE_H_
#define MMSRPSERVERINTERFACE_H_


// INCLUDES
#include "MsrpCommon.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  An interface to be implemented by RMSRPSession class
*  Interface allows communication to and from the MSRP server
*/
class MMSRPServerInterface
    {
public:
    
    virtual void GetLocalPathL( TDes8 &aLocalHost ) = 0;
    
    virtual void Connect( const TDesC8& aRemoteHost,
                          const TUint aRemotePort,
                          const TDesC8& aRemoteSessionID,
                          TRequestStatus& aStatus ) = 0;
    
    virtual void ListenConnections( const TDesC8& aRemoteHost,
                                    const TUint aRemotePort,
                                    const TDesC8& aRemoteSessionID,
                                    TBool aIsMessage,
                                    TRequestStatus& aStatus ) = 0;
    
    virtual void ListenMessages( TBool aIsMessage,
                                 TRequestStatus& aStatus ) = 0;
        
    virtual void ListenSendResult( const TDesC8& aSessionID,
                                   TRequestStatus& aStatus) = 0;
        
    virtual TInt SendMessage( TDesC8& aMessageBuffer ) = 0;
    
    virtual TInt CancelSending( const TDesC8& aMessageId ) = 0;
    
    virtual TInt CancelReceiving( const TDesC8& aMessageId ) = 0;
    virtual void CancelSendResultListening( ) = 0;
        
    virtual const TDesC8& GetMessageIdOfSentMessage() const = 0 ;
    virtual TInt GetStatusOfSentMessage() const = 0 ;
    virtual TInt GetStatusOfReceivedMessage() const = 0;
    virtual const TDesC8& GetReceivedMessage() const = 0;
    
    virtual TBool GetListenProgress() const = 0;
    virtual TInt GetBytesReceived() const = 0;
    virtual TInt GetTotalBytesReceived() const = 0;
    
    virtual TBool GetSendProgress() const = 0;
    virtual TInt GetBytesSent() const = 0;
    virtual TInt GetTotalBytes() const = 0;
    
    /**
      * returns message id from incoming message
      * @return message id
      */
    virtual TDesC8& ListenMessageId() = 0;
    
    /**
      * returns message id from sent message
      * @return message id
      */
    virtual TDesC8& SendMessageId() = 0;
    };

#endif /* MMSRPSERVERINTERFACE_H_ */
