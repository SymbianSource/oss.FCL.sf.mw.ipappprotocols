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

#ifndef __MMMSRPCONNECTIONMANAGER_H
#define __MMMSRPCONNECTIONMANAGER_H

#include <in_sock.h>
#include <e32base.h>

/* 
 * subsession to ensure no connect call after listen
 * or multiple connect/listen calls
 * make call to connmngr only if iConn not null
 * stop association of multiple connections with single session
 * stop association of single session multiple times with varying conns
 * stopping assocation of same session id twice or more with the same connection needs to be taken care by connmgr
 * done automatically by subsession, 506 (session already in use) response scenario doesnot arise
 */
// FORWARD DECLARATIONS
class MMSRPConnection;

// CLASS DECLARATIONS
class MMSRPConnectionManager
	{
	public:
	    inline virtual ~MMSRPConnectionManager(){}
        
	    /**
        * Get local ip address to form msrp path
        * @out param aLocalAddress 
        */	    
	    virtual void ResolveLocalIPAddressL( TInetAddr& aLocalAddr ) = 0;

        /**
        * Connection request to a specified address
        * @param aHost host address, direct ip address or host name
		* @param 
        */
	    
	    virtual MMSRPConnection& getConnectionL( TDesC8& aHost, TUint aPort ) = 0;

	    //take care of case where listen completes before connect
	    //and connect completes before listen to same host
	    //virtual MMSRPConnection& ListenL( TDesC8& aHost, TUint aPort, MMSRPConnectionObserver& aSubsession ) = 0;
	    
	    /**
	     * Start listener if not running
	     * else call listen to increment listen count
	     */	    
	    virtual void ListenL(MMSRPConnection* /*aConnection*/) = 0;
	    
       /**
         * issue listener cancel and remove connection
         */     
        virtual void ListenCancel(MMSRPConnection* aConnection) = 0;    
	    
	    /**
	     * should we move to CMSRPConnectionManager, 
	     * and keep a ref to the actual object in conn rather than access through interface class
	     */
	    virtual void Remove(MMSRPConnection* aConnection) = 0;
	    
	    
	    /*to be moved to C class?*/
	    
	    virtual RSocketServ& SocketServer() = 0;               
	            
	    virtual RConnection& SocketServerConn() = 0 ;
	    
	    //virtual TBool MatchIapId(TUint32 aIapID);
	    
	};

#endif // __MMMSRPCONNECTIONMANAGER_H

// End of file
