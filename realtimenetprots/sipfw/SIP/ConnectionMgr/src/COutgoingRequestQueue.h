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
* Name        : COutgoingRequestQueue.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef COUTGOINGREQUESTQUEUE_H
#define COUTGOINGREQUESTQUEUE_H
  
// INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include <sipnattraversalrequestobserver.h>

#include "MSocketUsagePermissionObserver.h"
#include "TOutgoingRequestQueueItem.h"

// FORWARD DECLARATIONS
class MOutgoingRequestQueueContext;
class CSIPNATTraversalController;
class TSIPTransportParams;
class CSIPRequest;
class CSIPMessage;
class CSIPHeaderBase;
class MSIPNATBindingObserver;
class TInetAddr;
class RConnection;
class RSocket;
class CUri8;

// CLASS DEFINITION
// 
class COutgoingRequestQueue : public CBase, 
                              public MSIPNATTraversalRequestObserver,
                              public MSocketUsagePermissionObserver
    {
    public: // Constructor and destructor
        
        static COutgoingRequestQueue* NewL( 
                    MOutgoingRequestQueueContext& aContext,
                    CSIPNATTraversalController& aNATTraversal,
                    RConnection& aConnection,
                    TUint32 aIapId );
      
        ~COutgoingRequestQueue();
        
    public: // From MSIPNATTraversalRequestObserver
    
        void RequestComplete( TUint32 aRequestId, TInt aCompletionCode );
		    
		void PublicAddrResolved( TUint32 aRequestId, const TInetAddr& aPublicAddr );		
      
    public: // From MSocketUsagePermissionObserver
    
        void UsagePermissionChanged( RSocket& aSocket, TBool aPermissionToUse );  
    
    public: // New methods
    
        void NATTraverseL( const TSIPTransportParams& aParams,
                           CSIPRequest& aRequest,
                           TBool aForceUDP,
                           const TInetAddr& aAddr,
                           const CUri8* aOutboundProxy,
                           TUint aOrigTransport,
                           TRequestStatus& aStatus,
                           const TInetAddr& aLocalAddr );
                           
        void FreeResources( MSIPNATBindingObserver& aSIPNATBindingObserver );
        
        TBool CancelSend( TRequestStatus& aStatus );
                           
    private: // Private construction
        
        void ConstructL();
        
        COutgoingRequestQueue( MOutgoingRequestQueueContext& aContext,
                               CSIPNATTraversalController& aNATTraversal,
                               RConnection& aConnection,
                               TUint32 aIapId );
    
    private:
    
        void Update( TOutgoingRequestQueueItem& aItem, const TInetAddr& aPublicAddr );
        
        void Complete( TOutgoingRequestQueueItem& aItem, 
                       TInt aCompletionCode, 
                       TBool aPublicAddrResolved );
        
        void UpdateHeadersL( TOutgoingRequestQueueItem& aItem, 
                             const TInetAddr& aLocalAddr, 
                             const TInetAddr& aPublicAddr );
                             
        void UpdateContactHeaderL( CSIPMessage& aMessage,
                                   const TDesC8& aLocalAddr, 
                                   const TDesC8& aPublicAddr,
                                   TUint aPublicPort );

        void UpdateViaHeaderL( CSIPMessage& aMessage,
                               const TDesC8& aPublicAddr,
                               TUint aPublicPort );
                                   
        void HandleUsagePermissionChange( TOutgoingRequestQueueItem& aItem, 
                                          TBool aPermissionToUse );
                                          
        TBool HasRequestForSocket( RSocket& aSocket, TBool aFindPendingRequest );
        
        void CleanupFailedRequests();
        
        void ActivatePendingRequests();
        
        void IssueRequestL( TOutgoingRequestQueueItem& aItem );
        
        TInt FindRequestQueueItemIndex( TUint32 aRequestId ) const;
        
        void RemoveNATTraversalParameter( CSIPRequest& aRequest );
        
        TUint ContactPortL( CSIPRequest& aRequest, const TDesC8& aLocalAddr );
         
        static void ConvertToSIPURIRollBack(TAny* aDummy);
         
    private: // Data
    
        MOutgoingRequestQueueContext& iContext;
        
        CSIPNATTraversalController& iNATTraversal;
        
        RConnection& iConnection;
        
        TUint32 iIapId;
        
        TInetAddr iLocalAddr;

        RArray< TOutgoingRequestQueueItem > iRequestQueue;
        
        RStringF iStrNATTraversalRequired;
        
        
#ifdef CPPUNIT_TEST
        friend class COutgoingRequestQueueTest;
#endif
    
    };
    
#endif // COUTGOINGREQUESTQUEUE_H
