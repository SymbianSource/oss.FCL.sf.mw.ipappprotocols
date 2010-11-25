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

#ifndef CMSRPSESSIONIMPLEMENTATION_H_
#define CMSRPSESSIONIMPLEMENTATION_H_

// INCLUDES
#include "MsrpCommon.h"

// Forward declarations.
class CMSRP;
class MMSRPSessionObserver;
class CUri8;
class RMSRPSession;
class CMSRPConnectionListener;
class CMSRPIncomingListener;
class CMSRPSendResultListener;
class CMSRPMessage;
class RMSRP;
class CMSRPSessionParams;

// CLASS DECLARATION
/**
* This is an implementation class for CMSRPSession class.
*
*/
class CMSRPSessionImplementation : public CBase
    {
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
         static CMSRPSessionImplementation* NewL( RMSRP& aRMSRP,
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
        static CMSRPSessionImplementation* NewLC( RMSRP& aRMSRP,
                                                  MMSRPSessionObserver& aObserver,
                                                  const TUint aIapId,
                                                  const TDesC8& aSessionId );
        
        ~CMSRPSessionImplementation();
        
    public:
        
        TDesC8& LocalMSRPPath();
        
        void SetSessionParams( CMSRPSessionParams& aSessionParams );
        
        void ConnectL( const TDesC8& aRemoteMsrpPath );
        
        void ListenL( const TDesC8& aRemoteMsrpPath );
            
        HBufC8* SendBufferL( const TDesC8& aMessage, const TDesC8& aMimeType );
            
        void CancelSendingL( TDesC8& aMessageId );
        
        void CancelReceivingL( TDesC8& aMessageId );
    
        void ConnectionEstablishedL( TInt aStatus );
        
        void HandleIncomingMessageL( const TDesC8& aIncomingMessage,
                                     TInt aStatus );
        void ReceiveProgress( const TDesC8& aMessageId, TInt aBytesReceived, TInt aTotalBytes);
        
        void SendProgress(const TDesC8& aMessageId, TInt aBytesSent, TInt aTotalBytes);
        
        void SendStatusL( TInt aStatus, const TDesC8& aMessageid );
        
        void HandleConnectionErrors( TInt aErrorStatus );
        
        HBufC8* SendFileL( const TFileName& aFileName,const TDesC8& aMimeType );
        
        void NotifyProgress(TBool aFlag);
    
    private:
        
        CMSRPMessage* CreateMsrpMessageL( const TDesC8& aMessage, const TDesC8& aToPath, 
                                          const TDesC8& aFromPath, const TDesC8& aMimeType );
        
        void SendMessageL( CMSRPMessage* aMessage );
        
        void GetRemotePathComponentsL( TPtrC8& aRemoteHost, TUint& aRemotePort, TPtrC8& aRemoteSessionID );
        
        CMSRPMessage* SetFileParamsL(const TFileName& aFileName,const TDesC8& aToPath,
                                                                   const TDesC8& aFromPath, 
                                                                   const TDesC8& aMimeType );
    private:
        
        CMSRPSessionImplementation( RMSRP& aRMSRP,
                                    MMSRPSessionObserver& aObserver );
        
        /** 
         * Second phase constructor
         * @param aIapId IAP id of the application
         * @param aSessionId session id of this MSRP session. This comes from the
         *        path SDP attribute where session id is used
         */
        void ConstructL( const TUint aIapId, const TDesC8& aSessionId );
            
    private:
        
        RMSRP& iRMSRP;
        
        MMSRPSessionObserver& iSessionObserver;

        // session to server, owned
        RMSRPSession* iMSRPSession;
        
        RBuf8 iLocalMsrpPath;
        
        RBuf8 iRemoteMsrpPath;
        
        TReportStatus iSuccessReport;
        
        TReportStatus iFailureReport;
                
        TBuf8< KMaxLengthOfIncomingMessageExt > iExtMessageBuffer;

        // connection to other party server listener, owned
        CMSRPConnectionListener* iConnectionListener;
        
        // incoming messages listener instance, owned
        CMSRPIncomingListener* iIncomingListener;
        
        // result of send message request listener, owned
        CMSRPSendResultListener* iSendResultListener;
        
        // array of sent messages, owned
        RPointerArray< CMSRPMessage > iSentMessages;
    };

#endif /* CMSRPSESSIONIMPLEMENTATION_H_ */

// End of file
