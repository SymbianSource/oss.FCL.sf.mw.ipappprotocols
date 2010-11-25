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

#ifndef CMSRPSESSION_H_
#define CMSRPSESSION_H_

// System Includes
#include <e32base.h>

// Forward Declarations
class CMSRPSessionImplementation;
class MMSRPSessionObserver;
class RMSRP;
class CMSRPMessage;
class CMSRPSessionParams;

// CLASS DECLARATION
/**
* Class provides an interface for MSRP clients to access MSRP functionality
*
*/
class CMSRPSession : public CBase
    {
    // friend class provides the actual implementation
    friend class CMSRPImplementation;
    
    public:    
        
        /** 
         * Object Creation. This is not exported to the client. It is called
         * while attempting to create an msrp session.
         * @param aRMSRP an instance of the client server session
         * @param aObserver the session observer that is created by the client
         * @param aIapId IAP id of the application
         * @param aSessionId session id of this MSRP session. This comes from the
         *        path SDP attribute where session id is used
         * @return session object
         */
        static CMSRPSession* NewL( RMSRP& aRMSRP,
                                            MMSRPSessionObserver& aObserver, 
                                            const TUint aIapId,
                                            const TDesC8& aSessionId );
            
        /** 
         * Object Creation. This is not exported to the client. It is called
         * while attempting to create an msrp session.
         * @param aRMSRP an instance of the client server session
         * @param aObserver the session observer that is created by the client
         * @param aIapId IAP id of the application
         * @param aSessionId session id of this MSRP session. This comes from the
         *        path SDP attribute where session id is used
         * @return session object
         */
        static CMSRPSession* NewLC( RMSRP& aRMSRP,
                                             MMSRPSessionObserver& aObserver,
                                             const TUint aIapId,
                                             const TDesC8& aSessionId );
        
    public:
    
        // === Post Creation; SDP negotiation parameter fetching.
                    
        /**
         * Gives the local MSRP path that is used in the "a" line of SDP
         * @return local msrp path
         */    
        IMPORT_C TDesC8& LocalMSRPPath();
    
        /**
         * Sets the various parameters related to an MSRP session
         * @param aSessionParams the session settings object from which the 
         * settings need to be incorporated into the msrp session.
         * Eg. SR and FR report headers can be set using this
         */    
        IMPORT_C void SetSessionParams( CMSRPSessionParams& aSessionParams );
        
        
        // === Post SDP Negotiation; Connection Setup.
            
        /**
         * Connects to the remote node
         * @param aRemoteMsrpPath the msrp path of the remote node
         * This is obtained through the SDP negotiation
         * @return KErrNone or system wide error codes
         */
        IMPORT_C TInt Connect( const TDesC8& aRemoteMsrpPath );
        
        /**
         * Listens for connections from the remote node
         * @param aRemoteMsrpPath the msrp path of the remote node
         * This is obtained through the SDP negotiation
         * @return KErrNone or system wide error codes
         */
        IMPORT_C TInt Listen ( const TDesC8& aRemoteMsrpPath );
        
        
        // === Post Connection setup; Data Trasfer. 
       
        /**
         * Sends a message to the currently connected client
         * Note: connection must be established before calling this
         * function
         * @param aMessage the data that needs to be sent
         * @param aMimeType the mime type of the data
         * @return the id of the message that was sent by the msrp stack
         *         NULL pointer if sending message failed
         */
        IMPORT_C HBufC8* SendBuffer( const TDesC8& aMessage, const TDesC8& aMimeType );
            
        /**
         * Cancels the send operation initiated by the client
         * @param aMessageId the id of the message that needs to be cancelled
         * @return KErrNone or system wide error codes
         */
        IMPORT_C TInt CancelSending( TDesC8& aMessageId );
    
        /**
         * Sends a message to the currently connected client. The message is given
         * in a file
         * Note: connection must be established before calling this
         * function
         * @param aFilename name of the file to be transmitted, must include file path
         * @param aMimeType the mime type of the data
         * @return the id of the message that was sent by the msrp stack
         *         NULL pointer if sending message failed
         */
        IMPORT_C HBufC8* SendFile( const TFileName& aFileName, const TDesC8& aMimeType );
        
        /**
         * Cancels the receiving of a given message
         * @param aMessageId the id of the message that needs to be cancelled
         * @return System-wide error message
         */
        IMPORT_C TInt CancelReceiving( TDesC8& aMessageId );
        
        /**
         * Client can use this function to indicate whether it wants to receive
         * progress reports or not
         * @param aFlag true if client wants progress reports, false otherwise,
         *        By default the progress reports are off
         */
        IMPORT_C void NotifyProgress( TBool aFlag );
    
    private:
    
        /**
         * Second phase constructor
         * @param aRMSRP RMSRP session class to server
         * @param aObserver SessionObserver interface
         * @param aIapId internet access point ID
         * @param aSessionId session id of this MSRP session. This comes from the
         *        path SDP attribute where session id is used
         */
        void ConstructL( RMSRP& aRMSRP,
                         MMSRPSessionObserver& aObserver,
                         const TUint aIapId,
                         const TDesC8& aSessionId );
        
        /**
         * class destructor
         */
        ~CMSRPSession();
            
    private:
    
        // Session implementation instance, owned
        CMSRPSessionImplementation* iMSRPSessionImpl;
    };

#endif /* CMSRPSESSION_H_ */

// End of File
