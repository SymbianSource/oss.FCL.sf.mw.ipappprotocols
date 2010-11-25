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

// CLASS HEADER
#include "CMSRPConnection.h"
#include "MSRPCommon.h"
// INTERNAL INCLUDES
#include "CMSRPConnector.h"
#include "CMSRPReader.h"
#include "CMSRPParser.h"
#include "CMSRPWriter.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPConnection::CMSRPConnection( TInetAddr& aRemoteAddr, MMSRPConnectionManager& aConnMngr ): iHostAddress(aRemoteAddr), iConnMngr(aConnMngr)
    {
    MSRPLOG( "CMSRPConnection::CMSRPConnection enter" )
    MSRPLOG( "CMSRPConnection::CMSRPConnection exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::~CMSRPConnection
// -----------------------------------------------------------------------------
//
CMSRPConnection::~CMSRPConnection()
	{
	MSRPLOG( "CMSRPConnection::~CMSRPConnection enter" )
	iSubsessions.Reset();
	iSubsessions.Close();
	delete iListenTimer;
	delete iConnector;
	delete iWriter;
	delete iParser;
	delete iReader;  
	//delete iBufPool;
	if(iSocket)
	    {
	    iSocket->Close();
	    delete iSocket;
	    }
	
	MSRPLOG( "CMSRPConnection::~CMSRPConnection exit" )
	}

// -----------------------------------------------------------------------------
// CMSRPConnection* CMSRPConnection::NewL
// -----------------------------------------------------------------------------
//
/*static*/ MMSRPConnection* CMSRPConnection::NewL( TInetAddr& aRemoteAddr, MMSRPConnectionManager& aConnMngr )
    {
    MSRPLOG( "CMSRPConnection::NewL enter" )

	// Perform construction.
    CMSRPConnection* self = new ( ELeave ) CMSRPConnection(aRemoteAddr, aConnMngr);
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    MSRPLOG( "CMSRPConnection::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPConnection::ConstructL( )
	{
	MSRPLOG( "CMSRPConnection::ConstructL enter" )
	iConnectionState = ENotConnected;
	iListenTimer = CMSRPTimeOutTimer::NewL( *this );
	MSRPLOG( "CMSRPConnection::ConstructL exit" )	
	}

// -----------------------------------------------------------------------------
// CMSRPConnection::CheckIfConnected
// listening state check for validating listener connections 
// -----------------------------------------------------------------------------
//
TBool CMSRPConnection::CheckConnection(TInetAddr& aRemoteAddr, TBool aOnlyListen)
	{
	MSRPLOG( "CMSRPConnection::CheckConnection enter" )
	TUint port_stored = iHostAddress.Port();
	TUint port_connect = aRemoteAddr.Port();
	
	//if (iHostAddress.CmpAddr(aRemoteAddr))//v4, v6 issue	
	if (iHostAddress.Match(aRemoteAddr) && port_stored == port_connect)
	    {
	    if (aOnlyListen && iConnectionState == EListening) //check if listening
	        return ETrue;	    
	    else if (!aOnlyListen /*&& iConnectionState <= EConnected */) //not connected, connecting, listening, connected
	        return ETrue;	   
	    /*else
	        return EFalse;*/ //timed out, disconnecting, disconnected, create a new conn object
	    }
	MSRPLOG( "CMSRPConnection::CheckConnection exit" )	
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CMSRPConnection::ConnectL
// -----------------------------------------------------------------------------
//
TInt CMSRPConnection::ConnectL(MMSRPConnectionObserver& aSubsession)
	{
	MSRPLOG("CMSRPConnection::ConnectL() enter")
	//subsession to ensure it does not reissue
	//connection does not check for pre-existent observers
	
	iSubsessions.AppendL(&aSubsession);	
	
	switch(iConnectionState)
	    {
	    case ENotConnected:
	        //create connector and issue connect
	        //delete iConnector;
	        iConnector = CMSRPConnector::NewL( iConnMngr, *this );
	        iConnector->Connect(iHostAddress);
	        iConnectionState = EConnecting;
	        
	        break;
	        
	    case EConnecting:        
	        break;
	        
	    case EListening: //as of now, dont try to issue connect,complete when other party connects
	        //TBD
	        //later change state to connecting and issue connect
	        //if connect completes before listen, then change state to connected
	        // if at all listen completes on same conn object, then check if state == connected and drop listen conn
	        // do vice versa if listen completes before connect	        	        
	        break;
	        
	    case EConnected:
            break;
            
	    case EListenTimedOut:
	    case EConnectTimedOut:
	    case EError:
	        //shudn't reach here, if connect issued immediately after/within getConn
	        //User::Leave(MMSRPConnectionObserver::EConnectionStateUnexpected);
	        break;                	        
	    
	    }
	MSRPLOG("CMSRPConnection::ConnectL() exit")
	return iConnectionState;
	}
		

// -----------------------------------------------------------------------------
// CMSRPConnection::ListenL
// -----------------------------------------------------------------------------
//
TInt CMSRPConnection::ListenL(MMSRPConnectionObserver& aSubsession)
    {
    MSRPLOG("CMSRPConnection::ListenL() enter")
        
    iSubsessions.AppendL(&aSubsession);
        
    
    switch(iConnectionState)
        {
        case ENotConnected:
            iConnectionState = EListening;
            iConnMngr.ListenL(this);
            //iListenTimer->After( KListenTimeoutInSeconds * KSecondinMicroseconds );
            
            break;
            
        case EConnecting: //dont issue listen, drop conn if other party connects, change later          
            
            //added check if listening in check connection to prevent accepting conn, 
            //simply allowing will corrupt listen count, TBD: introduce another EListeningandConnecting state                        
            break;
            
        case EListening:                      
            break;
            
        case EConnected:
            break;
                
        case EListenTimedOut:
        case EConnectTimedOut:
        case EError:
            //shudn't reach here, if connect issued immediately after/within getConn
            //User::Leave(MMSRPConnectionObserver::EConnectionStateUnexpected);
            break;                         
        
        }
    MSRPLOG("CMSRPConnection::ListenL() exit")
    return iConnectionState;
    
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::ReleaseConnection
// -----------------------------------------------------------------------------
//
void CMSRPConnection::ReleaseConnection(MMSRPConnectionObserver& aSubsession)
    {
    MSRPLOG("CMSRPConnection::ReleaseConnection() enter")
    
    Remove(&aSubsession);
        
    if(iSubsessions.Count())
        {
        MSRPLOG("CMSRPConnection::ReleaseConnection() exit")
        return;
        }
    
    //if no subsessions (TBD : start timeout timer and disconnect on timeout)
    /*if(iConnectionState == EConnecting)
        {
            //iConnector->CancelConnect();
            delete iConnector;
            iConnector = NULL;
        }
    else if(iConnectionState == EConnected)*/
                
    CloseConnection();
        
    
    //iConnectionState = ENotConnected;
    MSRPLOG("CMSRPConnection::ReleaseConnection() exit")
    return;
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::ConnectionEstablishedL
// -----------------------------------------------------------------------------
//
void CMSRPConnection::ConnectionEstablishedL( TInt aNewState, RSocket* aSocket, TInt aStatus )
    {
    MSRPLOG("CMSRPConnection::ConnectionEstablishedL() enter")
    iListenTimer->Cancel();
    //if not error
    if(aNewState == EConnected)
        {
        iConnectionState = aNewState;
        iSocket = aSocket;
        if(iConnector)
           {
           iConnector->ConnectComplete();
           /*dont delete now,as in case of leave, connector needs 2 b available*/
           //delete iConnector;
           //iConnector = NULL;
           }
        iWriter = CMSRPWriter::NewL(*iSocket , *this);               
        iParser = CMSRPParser::NewL(*this);
        iReader = CMSRPReader::NewL(*iSocket , *this);           
             
        iReader->StartReceivingL(iBufPool.ProvideBufferL());
        }

    /*not deleting connector as yet, notify all could leave in subsession  and also delete conn on conn error*/
    //intimate all subsessions
    NotifyAllL(aNewState, aStatus );
    
    //on error  or connect timeout close connection, i.e. intimate all subsessions and delete connection obj
    if(aStatus != KErrNone)
        {
        //listenaccepted does not comeback with error intimations, hence safe to decrement listen count
        //currently no risk of double decrementing listener count
        //CloseConnection();
        }
    

      
    MSRPLOG("CMSRPConnection::ConnectionEstablishedL() exit")   
    }

TInt CMSRPConnection::getConnectionState()
    {
    return iConnectionState;
    // iConnection state should not be Tint, it should be MMSRPConnection::TMSRPConnectionState.
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::ReadStatus
// -----------------------------------------------------------------------------
//
void CMSRPConnection::ReadStatusL(RMsrpBuf& aBuf, TInt aStatus)
    {
    MSRPLOG("CMSRPConnection::ReadStatusL() enter")
    if(aStatus == KErrNone)
        {
        //post to parser and return immediate
        iParser->ParseL(aBuf);
        
        RMsrpBuf copy(aBuf);
        if(copy.MsrpRightExtract() > KThreshold)
            {
            iReader->StartReceivingL(copy);
            }   
        else
            {
            iReader->StartReceivingL(iBufPool.ProvideBufferL());
            }
        return;
        }
    //read error or connection error
    //intimate all subsessions
    NotifyAllL(EError, aStatus );
    //CloseConnection();
    
    MSRPLOG("CMSRPConnection::ReadStatusL() exit")     
    }

TInt CMSRPConnection::ParseStatusL (CMSRPMessageHandler* aMsg, TInt aStatus)
    {
    MSRPLOG("CMSRPConnection::ParseStatus enter")
    if (aStatus != KErrNone)
        {
        NotifyAllL(EError, aStatus );
        return MMSRPParserObserver::EParseStatusError;
        }
    
    /*if error, count is zero*/
    //while msg not consumed try to handover    
    TBool consumed = FALSE;
    for ( TInt i = 0; (!consumed) && i < iSubsessions.Count(); i++ )
         {
         TRAPD( err, consumed = iSubsessions[ i ]->MessageReceivedL( aMsg ) );
         if ( err )
             {
             // invalid message
             NotifyAllL( EError, err );
             return MMSRPParserObserver::EParseStatusError;
             }
         }
    
    //if unclaimed by any subsession, callback to first subsession
    if(!consumed && iSubsessions.Count())
        {
        iSubsessions[0]->UnclaimedMessageL( aMsg );
        return MMSRPParserObserver::EParseStatusMessageUnclaimed;
        }
          
    MSRPLOG("CMSRPConnection::ParseStatus exit")
    return MMSRPParserObserver::EParseStatusMessageHandled;
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::ReportReceiveprogressL
// -----------------------------------------------------------------------------
//
void CMSRPConnection::ReportReceiveprogressL( CMSRPMessageHandler* aMsg )
    {
    MSRPLOG("-> CMSRPConnection::ReportReceiveprogressL")
    
    for ( TInt i = 0; i < iSubsessions.Count(); i++ )
         {
         TRAPD( err, iSubsessions[ i ]->MessageReceiveProgressL( aMsg ) );
         if ( err )
             {
             // invalid message
             NotifyAllL( EError, err );
             return;
             }
         }
          
    MSRPLOG("<- CMSRPConnection::ReportReceiveprogressL")
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::WriteSocketError
// -----------------------------------------------------------------------------
//
void CMSRPConnection::WriteSocketError(TInt aStatus)
    {
    MSRPLOG("CMSRPConnection::WriteSocketError() enter")
    TInt err =0;
    TRAP(err, NotifyAllL(EError, aStatus));
    //CloseConnection();
    MSRPLOG("CMSRPConnection::WriteSocketError() exit")
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::Send
// -----------------------------------------------------------------------------
//
void CMSRPConnection::SendL(MMSRPWriterObserver& aMsg)
    {
    MSRPLOG("CMSRPConnection::SendL() enter")
    if(iConnectionState == EConnected)
        iWriter->RequestSendL(aMsg);
    MSRPLOG("CMSRPConnection::SendL() exit") 
    }
    
// -----------------------------------------------------------------------------
// CMSRPConnection::ContinueSendingL
// -----------------------------------------------------------------------------
//
void CMSRPConnection::ContinueSendingL( MMSRPWriterObserver& aMsg )
    {
    MSRPLOG("-> CMSRPConnection::ContinueSendingL")
    if( iConnectionState == EConnected )
        {
        iWriter->RequestSendL( aMsg );
        }
    MSRPLOG("<- CMSRPConnection::ContinueSendingL") 
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::CancelSendingL
// -----------------------------------------------------------------------------
//
void CMSRPConnection::CancelSendingL( const MMSRPWriterObserver* aMsg )
    {
    MSRPLOG("-> CMSRPConnection::CancelSendingL")
    if( iConnectionState == EConnected )
        {
        iWriter->CancelSendingL( aMsg );
        }
    MSRPLOG("<- CMSRPConnection::CancelSendingL") 
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::TimerExpired
// -----------------------------------------------------------------------------
//
void CMSRPConnection::TimerExpiredL()   
    {
    MSRPLOG("CMSRPConnection::TimerExpired() enter")
    if(iConnectionState == EListening)
        NotifyAllL(EListenTimedOut, KErrTimedOut);
    else 
        NotifyAllL(EConnectTimedOut, KErrTimedOut);
    //iConnMngr.ListenCancel(this);
    MSRPLOG("CMSRPConnection::TimerExpired() exit")
    }


// -----------------------------------------------------------------------------
// CMSRPConnection::Remove
// -----------------------------------------------------------------------------
//
void CMSRPConnection::Remove(MMSRPConnectionObserver* aSubsession)
    {
    MSRPLOG("CMSRPConnection::Remove() enter")
    TInt index = iSubsessions.Find(aSubsession);    
    //iSubsessions[index].DisconnectIssued;
    if(index != KErrNotFound)
        iSubsessions.Remove(index);
    MSRPLOG("CMSRPConnection::Remove() exit")
    }

// -----------------------------------------------------------------------------
// CMSRPConnection::CloseConnection
// -----------------------------------------------------------------------------
//
void CMSRPConnection::CloseConnection()
    {
    MSRPLOG("CMSRPConnection::CloseConnection() enter")
    //for connected no listen cancel needed
    //for listener error listener deleted
    if(/*iConnectionState == EListenConnected ||*/ iConnectionState == EListening || iConnectionState == EListenTimedOut)
        iConnMngr.ListenCancel(this);
    else
        //deleting connection, takes care of issuing cancel connect on connector also (if connecting)
        iConnMngr.Remove(this);
    MSRPLOG("CMSRPConnection::CloseConnection() exit")
    }



// -----------------------------------------------------------------------------
// CMSRPConnection::NotifyAll
// -----------------------------------------------------------------------------
//
void CMSRPConnection::NotifyAllL(TInt aNewState, TInt aStatus ) 
    {
    MSRPLOG("CMSRPConnection::NotifyAll() enter")
    iConnectionState = aNewState;
    for ( TInt i = 0; i < iSubsessions.Count(); i++ )
         {
         //make sure connectionstate does not release in notify
         //else returns a value to reflect the same
         iSubsessions[ i ]->ConnectionStateL( aNewState, aStatus );
         }
    MSRPLOG("CMSRPConnection::NotifyAll() exit")
    }


// End of File
