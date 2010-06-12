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
         * An MSRP SEND request has been received from the network.
         * The content of the request is given to the client, along with the 
         * status of receiving the request. Ownership of the content is transferred.
         * @param aMessageContent the received message content
         * @param aStatus KErrNone with complete message, KErrCancel if terminated
         */
        virtual void IncomingMessage( HBufC8* aMessageContent, TInt aStatus ) = 0;

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
         * @param aStatus error code as defined in MsrpCommon::TErrorCode
         * @param aMessageId message identifier of the sent message that
         * was returned by the SendBuffer call
         */
        virtual void SendResult( TInt aStatus, const TDesC8& aMessageid ) = 0;

        /**
         * Indicates a failure in connection establishment
         * or in case of any error in an established connection
         * @param aStatus a system wide error code
         */
        virtual void ListenPortFailure( TInt aStatus ) = 0;

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
         * Notifies that the entire file has been sent successfully 
         */
        virtual void SendFileNotification(TBool aStatus) = 0;
        
        /**
         * Notifies that the entire file has been received successfully 
         */
        virtual void ReceiveFileNotification(TBool status) = 0;
        
        /**
        * File data transfer progress. returns the progress of data transfer
        * @param aTransferred number of bytes transferred
        * @param aTotal Total amount of bytes to be transferred
        */
        virtual void FileSendProgress( TInt aTransferred, TInt aTotal ) = 0;

        /**
        * File receive progress, number of bytes received from incoming data
        * @param aReceived number of bytes received
        * @param aTotal Total amount of bytes to be received
        */
        virtual void FileReceiveProgress( TInt aReceived, TInt aTotal ) = 0;
        
    };

#endif /* MMSRPSESSIONOBSERVER_H_ */
