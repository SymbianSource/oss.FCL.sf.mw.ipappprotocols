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

#ifndef MMMSRPCONNECTION_H
#define MMMSRPCONNECTION_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MMSRPConnectionManager.h"
#include "MMSRPConnectionObserver.h"
#include "MMSRPWriterObserver.h"

// FORWARD DECLARATIONS
class MMSRPConnectionObserver;
class MMSRPWriterObserver;
class MMSRPConnectionManager;

// CLASS DECLARATIONS



class MMSRPConnection
    {
public:
    //move to CMSRPConn
    enum TMSRPConnectionState
        {
        EError = -1,
        ENotConnected = 0,
        EConnecting,        
        EListening,
        //EListenConnected,
        EConnected,
        EListenTimedOut,
        EConnectTimedOut,
        EDisconnecting, //cancel issued on connecting socket
        EDisconnected//EConnectCancelling/ed
        //EListenConnected (as opposed to connect connected)//validation to be taken care by session 
        //EListenValidated (received bodiless msg) //per session basis, hence these 2 states not needed 
        };
//
    inline virtual ~MMSRPConnection(){}
    
    /**
    * Connect to remote address
    */   
    virtual TInt ConnectL(MMSRPConnectionObserver& aSubsession) = 0;
    
    /**
    * Listen to remote address
    * add observer and start listener, if not started
    * if new conn object i.e. single observer conn then 
    * update listen count
    */   
    virtual TInt ListenL(MMSRPConnectionObserver& aSubsession) = 0;


	virtual TInt getConnectionState()=0;

    
    /**
    * Cancel the connect request if pending, else disconnect subsession
    */   
    //virtual void ConnectCancel(MMSRPConnectionObserver& aSubsession) = 0; //state == not connected means error
                                      //connecting : issue cancel connect on socket if only subsession pending
                                       //if connected: disconnect if only subsession pending
                                      //else listening, panic client
    /**
     * Cancel the listen request if pending, else disconnect subsession
     */     
    //virtual void ListenCancel(MMSRPConnectionObserver& aSubsession) = 0;
    
    /**
     * The particular subsession wishes to cease using the connection/ disconnect
     */     
    virtual void ReleaseConnection(MMSRPConnectionObserver& aSubsession) = 0;
                
    
    /**
    * Sends the buffer content
    * the observer cud be implemented by msg or subsession
    * connection observer deemed unnecessary in send call
    */
    virtual void SendL( MMSRPWriterObserver& aMsg ) = 0;
            
    
    /**
     * use case unknown : connection failure detected, parse error on the connection
     * Send failed , goes directly to subsession.
     * In that scenario, subession could ask mngr to close connection itself as part of error recovery
     */    
    /**
     * mngr/server decides to close all subsessions on connection      
     */     
    //virtual void CloseConnection() = 0;
    
    
    /*move to CMSrpConn*/
    virtual TBool CheckConnection(TInetAddr& aRemoteAddr, TBool aOnlyListen) = 0;
    
    /**
     * connection observer is available with every request
     * NewL used to get a reference to connection mgr, 
     * call delete connection entry in conn mngr on disconnect
     */
    //virtual MMSRPConnection* NewL( TInetAddr& aRemoteAddr, MMSRPConnectionManager& aConnMngr) = 0;
    
    /**
     * called on listen complete by connection manager
     */
    virtual void ConnectionEstablishedL( TInt aNewState, RSocket* aSocket, TInt aStatus ) = 0;
    
    
    /**
    * Cancel the send     
    */
    //Send called on subsession with message
    //Send cancel should be called on message (subsession with message id)
    //msg state modified to cancel next fetch of message by writer
    //but if the writer is sending same msg currently and send abort is issued then socket will need to be
    //shutdown with stopoutput and then restarted
    //Things cud get complicated , support not really necessary
    //virtual void SendCancel( MMSRPConnectionObserver& aSubsession, MsgPTr, TDesC8& aBuf ) = 0;
        
    };

#endif // MMMSRPCONNECTION_H

// End of file
