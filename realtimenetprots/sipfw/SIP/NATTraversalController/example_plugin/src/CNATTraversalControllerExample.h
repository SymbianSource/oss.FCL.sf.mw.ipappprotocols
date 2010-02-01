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
* Name          : CNATTraversalControllerExample.h
* Part of       : SIP NAT Traversal Controller
* Version       : SIP/5.0 
*
*/



#ifndef CNATTRAVERSALCONTROLLEREXAMPLE_H
#define CNATTRAVERSALCONTROLLEREXAMPLE_H

// INCLUDES
#include "sipnattraversalcontroller.h"
#include "MNATTraversalPendingRequestContainer.h"
#include "TNATTraversalPendingRequest.h"

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
* Example implementation of SIP NAT Traversal Controller ECOM plug-in.
*/
class CNATTraversalControllerExample : 
    public CSIPNATTraversalController,
    public MNATTraversalPendingRequestContainer
    {
	public: // Constructors and destructor
	    
    	static CNATTraversalControllerExample* NewL(TAny* aInitParams);
    	
    	~CNATTraversalControllerExample ();
    		
    public: // From CSIPNATTraversalController

        TUint32 ResolvePublicAddrL(
            TUint32 aIapId,
            RConnection& aConnection,
            const TInetAddr& aLocalAddr,
            const TDesC8& aDomain,
            RSocket& aSocket,
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

        void FreeResources( TUint32 aIapId );

        void FreeResources( MSIPNATBindingObserver& aBindingObserver );

    public: // From MNATTraversalPendingRequestContainer
    
        void RequestCompleted( TUint32 aRequestId );
                                    
	private: // Constructors
	
	    CNATTraversalControllerExample( RSocketServ& aSocketServ );
		void ConstructL();
		
    private: // New functions
				
		TUint32 NextRequestId();
        
        TUint Protocol( RSocket& aSocket ) const;
        
        TBool BindingExists(const MSIPNATBindingObserver* aObserver) const;	
		
	private: // Data
	
        RSocketServ& iSocketServ;
        CDeltaTimer* iDeltaTimer;
        RArray<TNATTraversalPendingRequest> iPendingRequests;
        TLinearOrder<TNATTraversalPendingRequest> iRequestOrder;     
        TUint32 iRequestIdCounter;
    };

#endif // CNATTRAVERSALCONTROLLEREXAMPLE_H
