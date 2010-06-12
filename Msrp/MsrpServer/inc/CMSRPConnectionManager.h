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

#ifndef CMSRPCONNECTIONMANAGER_H
#define CMSRPCONNECTIONMANAGER_H

// INCLUDES
#include "MMSRPConnectionManager.h"
#include "MMSRPConnectionManagerObserver.h"
#include "MMSRPListenerObserver.h"
#include "MMSRPListener.h"
#include "MMSRPConnection.h"


// FORWARD DECLARATIONS
class MMSRPConnectionManagerObserver;
class MMSRPConnection;

// CLASS DECLARATION

/**
*  MSRP Connection Manager
*/
class CMSRPConnectionManager : public CBase, public MMSRPConnectionManager
										, public MMSRPListenerObserver
    {
    
    public:  
        
    /* Constructors and destructor*/
    
        static CMSRPConnectionManager* NewL( const TUint32 aIapID, MMSRPConnectionManagerObserver& aServer );
        
        virtual ~CMSRPConnectionManager();


    /*Functions from base classes*/   
        /* from MMSRPConnectionManager*/
        
        virtual void ResolveLocalIPAddressL( TInetAddr& aLocalAddr );
       
        virtual MMSRPConnection& getConnectionL( TDesC8& aHost, TUint aPort );
          
        
     //called by conn   
        virtual void ListenL(MMSRPConnection* aConnection);
        
        virtual void ListenCancel(MMSRPConnection* aConnection);
        
        virtual void Remove(MMSRPConnection* aConnection);
	    
        virtual RSocketServ& SocketServer();	            
	    
        virtual RConnection& SocketServerConn() ;        
        
        //virtual TBool MatchIapId(TUint32 aIapID);
        
        
        /* from MMSRPListenerObserver  */
        virtual void ListenerStateL( TInt aNewState, RSocket* aDataSocket, TInt aStatus );
                
        

	private: 
	    
    // internal functions
	    /**
	     *  remote address resolution
	     */
	    void ResolveIPAddressL( const TDesC8& aHost, TInetAddr& aAddress );
	    
	    /**
	     * check if we have a connection to the same host and port
	     */
	    MMSRPConnection* CheckConnection( TInetAddr& aHostAddress ,TBool aListenOnly);
	    
	    /**
	     * start up the connection on iap 
	     */
	    TInt StartInterface();
	    

	// constructors

	    CMSRPConnectionManager( const TUint32 aIapID, MMSRPConnectionManagerObserver& aServer );
        
        void ConstructL();

	private: // data

	    RPointerArray<MMSRPConnection> iConnectionArray;
		MMSRPListener* iConnListener;
				
		RSocketServ       iSocketServer;
		RConnection       iConnection;	
		RHostResolver     iHostResolver;
		TUint32           iIapID;
		TInetAddr         iLocalAddr;
		
		MMSRPConnectionManagerObserver& iObserver;
    };

#endif      // CMSRPCONNECTIONMANAGER_H

// End of File
