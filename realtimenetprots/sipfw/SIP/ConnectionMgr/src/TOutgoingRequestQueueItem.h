/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : TOutgoingRequestQueueItem.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef TOUTGOINGREQUESTQUEUEITEM_H
#define TOUTGOINGREQUESTQUEUEITEM_H
  
// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TSIPTransportParams;
class CSIPRequest;
class TInetAddr;
class RSocket;
class CUri8;

// CLASS DEFINITION
//
class TOutgoingRequestQueueItem
    {
    public: // Constuctor
    
        TOutgoingRequestQueueItem( const TSIPTransportParams& aParams,
                                   CSIPRequest& aRequest,
                                   TBool aForceUDP,
                                   const TInetAddr& aAddr,
                                   TUint aLocalPort,
                                   const CUri8* aOutboundProxy,
                                   TUint aOrigTransport,
                                   TRequestStatus& aStatus,
                                   RSocket& aSocket,
                                   TBool aPermissionToUse );
                           
        TBool Match( TUint32 aRequestId ) const;
        
        TBool Match( RSocket& aSocket ) const;
        
        TBool Match( TRequestStatus& aStatus ) const;
        
        TBool IsActiveRequest() const;
        
        const TSIPTransportParams& Params();
        
        CSIPRequest& Request();
        
        TBool ForceUDP();
        
        const TInetAddr& Addr();
        
        TUint LocalPort() const;
        
        const CUri8* OutboundProxy();
        
        TUint OrigTransport() const;
        
        TRequestStatus& Status();
        
        RSocket& Socket();
        
        void SetPermissionToUse( TBool aPermissionToUse );
        
        TBool PermissionToUse() const;
        
        void SetRequestId( TUint32 aRequestId );
        
        TUint32 RequestId() const;

        void SetFailed( TInt aError );
        
        TBool IsFailed() const;
        
        TInt Error() const;
        
    private: // Data
    
        const TSIPTransportParams& iParams;
        CSIPRequest& iRequest; 
        TBool iForceUDP;
        const TInetAddr& iAddr;
        TUint iLocalPort;
        const CUri8* iOutboundProxy;
        TUint iOrigTransport;
        TRequestStatus& iStatus;
        RSocket& iSocket;
        TBool iPermissionToUse;
        TUint32 iRequestId;
        TInt iError;
        
    };
    
#endif // TOUTGOINGREQUESTQUEUEITEM_H
