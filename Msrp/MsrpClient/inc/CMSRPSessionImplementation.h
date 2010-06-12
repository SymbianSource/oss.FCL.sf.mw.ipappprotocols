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


class CMSRPSessionImplementation : public CBase
    {
public:       
    static CMSRPSessionImplementation* NewL( RMSRP& aRMSRP,
                                             MMSRPSessionObserver& aObserver,
                                             const TUint aIapId );
    
    static CMSRPSessionImplementation* NewLC( RMSRP& aRMSRP,
                                              MMSRPSessionObserver& aObserver,
                                              const TUint aIapId );
    
    ~CMSRPSessionImplementation();
    
public:
    
    TDesC8& LocalMSRPPath();
    
    void SetSessionParams( CMSRPSessionParams& aSessionParams );
    
    void ConnectL( const TDesC8& aRemoteMsrpPath );
    
    void ListenL( const TDesC8& aRemoteMsrpPath );
        
    HBufC8* SendBufferL( const TDesC8& aMessage, const TDesC8& aMimeType );
        
    void CancelSendingL( TDesC8& aMessageId );
    
        
    void ConnectionEstablishedL( TInt aStatus );
    
    void HandleIncomingMessageL( const TDesC8& aIncomingMessage,
                                 TInt aStatus );
    void ReceiveProgress(TInt aBytesReceived, TInt aTotalBytes);
    
    void SendProgress(TInt aBytesSent, TInt aTotalBytes);
    
    void SendStatusL( TInt aStatus, const TDesC8& aMessageid );
	
    void HandleConnectionErrors( TInt aErrorStatus );
    
	void SendFileL( const TFileName& aFileName,const TDesC8& aMimeType );
	 
	void NotifyProgress(TBool aFlag);

	void ReceiveFileL(const TFileName& aFileName,const TInt aFileSize, const TDesC8& aMimeType);

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
    
    void ConstructL( const TUint aIapId );
        
private:
    
    RMSRP& iRMSRP;
    
    MMSRPSessionObserver& iSessionObserver;
    
    RMSRPSession* iMSRPSession;
    
    RBuf8 iLocalMsrpPath;
    
    RBuf8 iRemoteMsrpPath;
    
    TReportStatus iSuccessReport;
    
    TReportStatus iFailureReport;
            
    TBuf8< KMaxLengthOfIncomingMessageExt > iExtMessageBuffer;
    
    CMSRPConnectionListener* iConnectionListener;
    
    CMSRPIncomingListener* iIncomingListener;
    
    CMSRPSendResultListener* iSendResultListener;
    
    RPointerArray< CMSRPMessage > iSentMessages;
    
    TBool iProgress;
    
    TBool  isSendFile;
    TBool  isReceiveFile;
    };

#endif /* CMSRPSESSIONIMPLEMENTATION_H_ */
