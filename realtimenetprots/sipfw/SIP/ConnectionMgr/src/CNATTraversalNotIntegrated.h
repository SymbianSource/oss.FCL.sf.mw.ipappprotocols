/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CNATTraversalNotIntegrated.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef CNATTRAVERSALNOTINTEGRATED_H
#define CNATTRAVERSALNOTINTEGRATED_H

// INCLUDES
#include <sipnattraversalcontroller.h>

// FORWARD DECLARATIONS

// HELPER CLASS DEFINITION

class TPendingAsyncRequest
    {
    public: // Methods
    
        TPendingAsyncRequest( TDeltaTimerEntry aTimerEntry,
                              TUint32 aIapId,
                              MSIPNATTraversalRequestObserver& aObserver,
                              TUint32 aRequestId );
        
        TBool MatchRequestId( TUint32 aRequestId ) const;
        
        TBool MatchIapId( TUint32 aIapId ) const;
              
    public: // Data
    
        TDeltaTimerEntry iTimerEntry;
        
        TUint32 iIapId;
        
        MSIPNATTraversalRequestObserver& iObserver;
        
        TUint32 iRequestId;
    };

// CLASS DEFINITION
/**
* Handling situations where NATTravesalController ecom plugin cannot be used.
*/
class CNATTraversalNotIntegrated : public CSIPNATTraversalController
    {
    public: // Constructor and destructor
    
        static CNATTraversalNotIntegrated* NewL();

        ~CNATTraversalNotIntegrated();

    public: // From CSIPNATTraversalController

        TUint32 ResolvePublicAddrL(
            TUint32 aIapId,
            RConnection& aConnection,
            const TInetAddr& aLocalAddr,
            const TDesC8& aDomain,
            RSocket& aUdpSocket,
            const TInetAddr& aNextHopAddr,
            MSIPNATBindingObserver* aBindingObserver,
            MSIPNATTraversalRequestObserver& aRequestObserver );

        void Cancel( TUint32 aRequestId );        

        TBool RefreshNATBindingL( 
            RSocket& aSocket,
            const MSIPNATBindingObserver* aBindingObserver );
            
        TBool RefreshNATBindingL(
            CSecureSocket& aSecureSocket,
            const MSIPNATBindingObserver* aBindingObserver );
  
        void UpdateNextHop( 
            const TInetAddr& aNextHopAddr,
            const MSIPNATBindingObserver* aBindingObserver );  
  
        void SocketIdle( 
            TBool aIdle,
            RSocket& aSocket );
             
        void SocketIdle( 
            TBool aIdle,
            CSecureSocket& aSecureSocket );
               
        void DataReceivedL( 
            const TDesC8& aData,
            const RSocket& aUdpSocket,
            TBool& aHandled );    

        void FreeResources( 
            TUint32 aIapId );

        void FreeResources( 
            MSIPNATBindingObserver& aBindingObserver );

    
    public: // New methods
    
        static TInt CompletionCallback( TAny* aAny );
        
    private: // Constructors

        void ConstructL();
        
        CNATTraversalNotIntegrated();
        
    private: // New methods
    
        void AddAsyncRequestL( TUint32 aIapId,
                               MSIPNATTraversalRequestObserver& aRequestObserver,
                               TUint32 aRequestId );
        
        void CompletePendingAsyncRequest();
        
        TUint32 NextRequestId();
        
    private: // Data
    
        CDeltaTimer* iAsyncCompletionTimer;
        
        RArray< TPendingAsyncRequest > iAsyncRequests;
        
        TUint32 iRequestIdCounter;
        
        
#ifdef CPPUNIT_TEST
        friend class COutgoingRequestQueueTest;
#endif
        
    };


#endif // CNATTRAVERSALNOTINTEGRATED_H
