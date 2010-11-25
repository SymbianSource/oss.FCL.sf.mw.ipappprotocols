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

#ifndef MMSRPSESSIONOBSERVER_H_
#define MMSRPSESSIONOBSERVER_H_

// INCLUDES
#include "MsrpCommon.h"

// FORWARD DECLARATIONS
class CMSRPMessage;

class MMSRPSessionObserver
    {
    public:
        
        /**
         * An MSRP message has been received from the network.
         * The content of the request is given to the client, along with the 
         * status of receiving the request. Ownership of the content is transferred.
         * This method is used to give smaller messages to the clients in buffer
         * @param aMessageContent the received message content, ownership is transferred
         * @param aMimeType mimetype of the received data, ownership is transferred
         * @param aStatus KErrNone with complete message, KErrCancel if terminated
         */
        virtual void IncomingMessageInBuffer( HBufC8* aMessageContent, HBufC8* aMimeType, TInt aStatus ) = 0;

        /**
         * An MSRP message has been received from the network.
         * The content of the request is given to the client, along with the 
         * status of receiving the request. 
         * Note: the file is a temporary file which should be moved/renamed to correct 
         * location.
         * @param aFileName file name and path of the received file
         * @param aMimeType mimetype of the received file, ownership is transferred
         * @param aStatus KErrNone with complete message, KErrCancel if terminated
         */
        virtual void IncomingMessageInFile( TFileName& aFileName, HBufC8* aMimeType, TInt aStatus ) = 0;

        /**
         * An MSRP REPORT request has been received from the network.
         * This function is called to notify the client that a recipient wishes
         * to indicate a range of bytes as received, or to indicate an error. 
         * However, it is the client's responsibility to handle this information.
         * @param aStatus status code from the REPORT request
         * @param aRangeStart start of the byte range being reported 
         * @param aRangeEnd end of the byte range being reported
         * @param aTotal total number of bytes in the message
         * NOTE: This callback not issued currently, REPORTs support not present
         */
        virtual void IncomingReport( TInt aStatus, TInt aRangeStart,
            TInt aRangeEnd, TInt aTotal ) = 0;

        /**
         * Result of the send operation initiated by the client
         * @param aStatus error code as defined in MsrpCommon.h, TMSRPRequests
         *        or system-wide error code
         * @param aMessageId message identifier of the sent message that
         * was returned by the SendBuffer call
         */
        virtual void SendResult( TInt aStatus, const TDesC8& aMessageid ) = 0;

        /**
         * Result of the connection attempt made by the client. This callback is
         * issued, both while initiating a connection with ConnectL and while
         * receiving a connection with ListenL, once the call completes.
         * Client must wait for this callback before trying to send out any
         * messages to the remote user.
         * @param aStatus KErrNone or a system wide error code
         */
        virtual void ConnectStatus( TInt aStatus ) = 0;

        /**
        * Data transfer progress. returns the progress of data transfer
        * @param aMessageId message identifier of the data to be sent
        * @param aTransferred number of bytes transferred
        * @param aTotal Total amount of bytes to be transferred
        */
        virtual void SendProgress( const TDesC8& aMessageid, TInt aTransferred, TInt aTotal ) = 0;

        /**
        * Data receive progress, number of bytes received from incoming data
        * @param aMessageId message identifier of the data to be received
        * @param aReceived number of bytes received
        * @param aTotal Total amount of bytes to be received
        */
        virtual void ReceiveProgress( const TDesC8& aMessageid, TInt aReceived, TInt aTotal ) = 0;
    };

#endif /* MMSRPSESSIONOBSERVER_H_ */
