/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#include "rtpcommrecv.h"

/////////////////////Class CRtpCommRecv///////////////////////////////////////
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// SetRemoteAddress
// ---------------------------------------------------------------------------
//
void CRtpCommRecv::SetRemoteAddress(TInetAddr& aAddr)
    {
    iRemoteAddr.SetAddress( aAddr.Address() ); 
    iRemoteAddr.SetPort( aAddr.Port() );     
    if (aAddr.Family() == KAfInet6 )
    	{
     	iRemoteAddr.SetAddress( aAddr.Ip6Address() ); 
     	}    
    iRemoteAddrSet = ETrue;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpCommRecv::CRtpCommRecv( TPortType aPortType, RSocket& aSocket, 
                            TInt aPriority, MRtpErrNotify& aErrNotify,
                            TBool aNonRtpObserverSet ) 
    : CActive( aPriority ), 
      iPortType( aPortType ),
      iRecvNotify( NULL ),
      iSocket( aSocket ),
      iRtpErrNotify( aErrNotify ),
      iNonRtpObserverSet(aNonRtpObserverSet)
    {
    // Note: high priority needed to prevent socket server buffers filling up
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CRtpCommRecv::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpCommRecv* CRtpCommRecv::NewL( TPortType aPortType, RSocket& aSocket, 
                                  TInt aPriority, MRtpErrNotify& aErrNotify,
                                  TBool aNonRtpObserverSet )
    {
    CRtpCommRecv* self = new ( ELeave ) CRtpCommRecv( aPortType, aSocket, 
                                                      aPriority, aErrNotify,
                                                      aNonRtpObserverSet );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpCommRecv::~CRtpCommRecv()
    {
    Cancel(); // -> in base class -> goes on to call DoCancel in this class...
    }

// ---------------------------------------------------------------------------
// CRtpCommRecv::DoCancel()
// Cancel ongoing requests.
// ---------------------------------------------------------------------------
//
void CRtpCommRecv::DoCancel()
    {
    // Cancel asychronous read request
    iSocket.CancelRead();
    }

// ---------------------------------------------------------------------------
// CRtpCommRecv::RegisterReceivedNotify()
// 
// ---------------------------------------------------------------------------
//
void CRtpCommRecv::RegisterReceivedNotify( MReceivedNotify* aNotify )
    {
    iRecvNotify = aNotify;
    }

// ---------------------------------------------------------------------------
// CRtpCommRecv::SetNonRtpObserverFlag()
// 
// ---------------------------------------------------------------------------
//
void CRtpCommRecv::SetNonRtpObserverFlag(TBool aValue)
    {
    iNonRtpObserverSet = aValue;
    }

// ---------------------------------------------------------------------------
// CRtpCommRecv::Recv()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpCommRecv::Recv( TDes8& aPacket )
    {
    if ( !IsActive() )
        {
        iSocket.RecvFrom( aPacket, iFromAddr, 0, iStatus );
      
        SetActive();
        return KErrNone;
        }
    else
        {
        return KErrInUse;
        }
    }

// ---------------------------------------------------------------------------
// CRtpCommRecv::RunL()
// 
// ---------------------------------------------------------------------------
//
void CRtpCommRecv::RunL()
    {
    RTP_DEBUG_DETAIL( "CRtpCommRecv::RunL entry" );
    if ( !iRecvNotify )
		{
     	RTP_DEBUG_PACKET( "CRtpCommRecv::RunL -- No receiver registered" );
     	iRtpErrNotify.ErrorNotify( KErrNotReady );
   		}
    else
    	{
	  	if ( iStatus == KErrNone )
	  	  	{
	       	if (!iNonRtpObserverSet  &&
                 iRemoteAddrSet && !iFromAddr.Match(iRemoteAddr))
	            {
	            // reject
	            RTP_DEBUG_DETAIL( "Reject Rtp packet received" );
    
	            iRecvNotify->OnPacketRejected(iPortType);
	            }
			else                    
	       		{
	       		//Note that Floor Control Msgs comes from the same remote addr with 
	       		// rtp packets.
	   		 	if ( iPortType == ERTPPort )
	          		{
		          	RTP_DEBUG_DETAIL( "CRtpCommRecv::RunL -- Rtp Packet is received");
			      	iRecvNotify->OnRtpReceivedL();
	    	    	}
	            else if ( iPortType == ERTCPPort )
	      	  	    {
	        	    RTP_DEBUG_DETAIL( "CRtpCommRecv::RunL -- Rtcp Packet is received" );
	        	    iRecvNotify->OnRtcpReceivedL();
	        	    }
	    	    else
	      	  	    {
	        	    // Do nothing
	        	    RTP_DEBUG_DETAIL( "CRtpCommRecv::RunL -- Unknown port type" );
	        	    }
	            }
  	  	    }
  		else
      		{
      	    RTP_DEBUG_DETAIL_DVALUE( "CRtpCommRecv::RunL -- ERROR: iStatus: ", iStatus.Int() );
        	// Also let the client application know that this happened
        	iRtpErrNotify.ErrorNotify( iStatus.Int() );
        	// Let the session decide what to do with the error
         	iRecvNotify->OnReceptionError( iPortType, iStatus.Int() );
        	}
    	}
    RTP_DEBUG_DETAIL( "CRtpCommRecv::RunL exit" );
    }	

// ---------------------------------------------------------------------------
// CRtpCommRecv::IsSetRemoteAdress()
// 
// ---------------------------------------------------------------------------
//
TBool CRtpCommRecv::IsSetRemoteAdress()
	{
	TBool same = ETrue;
	//This will be called by RtpSession classe to verify if the packet come from
	// setting remote address. This is important when NonRtpObserver has been set
	// in RunL and when NonRtpObs has been set, then it does not check remote address
	// since some response might come from STUN server, but if "from" address is not
	//checked then there could be some intrusion RTP valid packet and RTP stack is
	// volunerable for this attack. 
	if ( iRemoteAddrSet && !iFromAddr.Match(iRemoteAddr))
		{
		same = EFalse;
        return same;
        }
    return same;    
	}

//  End of File



