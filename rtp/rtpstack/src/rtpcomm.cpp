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




// INCLUDE FILES
#include "rtpcomm.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpComm::CRtpComm()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CRtpComm::ConstructL( TUint& aLocalPort,
                           RSocketServ& aSocketServ,
                           RConnection& aRConn, 
                           const TCreateSessionParams& aParams,
                           MRtpErrNotify& aErrNotify,
                           TBool aEnableRtcp )
    {
    
    
    RTP_DEBUG_DETAIL_DVALUE( "CRtpComm::ConstructL(), local port = ", aLocalPort );
    
    TInt  err( 0 );
    TBool bRequiredPortsOpen( EFalse );
    TUint dynLocalPort( aLocalPort );
    
    if ( ( dynLocalPort % 2 ) != 0 )
        {
        // If an odd number is supplied then the application should replace
        // that number with the next lower (even) number to use as the base
        // of the port pair. (RFC3550)
        dynLocalPort--;
        }

    if ( dynLocalPort == 0 ) // use default port 
        {
        dynLocalPort = KDefaultLocalPort;
        }

    TInetAddr localRtpAddress( dynLocalPort );
    TInetAddr localRtcpAddress( dynLocalPort + 1 );
    
    // Open RTP socket
    CleanupClosePushL( iSocket[ERTPPort] );
    err = iSocket[ERTPPort].Open( aSocketServ, 
                                  KAfInet,
                                  KSockDatagram, 
                                  KProtocolInetUdp, 
                                  aRConn 
                                );
    User::LeaveIfError( err );

    if ( aEnableRtcp )
        {
        // Open RTCP socket
        CleanupClosePushL( iSocket[ERTCPPort] );
        err = iSocket[ERTCPPort].Open( aSocketServ, 
                                       KAfInet,
                                       KSockDatagram, 
                                       KProtocolInetUdp, 
                                       aRConn             
                                     );
        User::LeaveIfError( err );
        }
    
   	// Find consecutive ports for RTP and RTCP
    while ( !bRequiredPortsOpen )
        {
        err = iSocket[ERTPPort].SetLocalPort(dynLocalPort);
        if ( err == KErrNone && aEnableRtcp )
            {
         err = iSocket[ERTCPPort].SetLocalPort(dynLocalPort + 1 );
            }
        if ( err == KErrNone )
            {
            bRequiredPortsOpen = ETrue;
            }
        else
            {
            // RTP always uses even ports, so increment by 2 and retry.
            dynLocalPort += 2;
            if ( dynLocalPort >= KMaxLocalPort )
                {
                // Reached maximum port number
                User::LeaveIfError( KErrTooBig );
                }
            }
        }            

    err = iSocket[ERTPPort].SetOpt( KSORecvBuf, KSOLSocket,
                                    aParams.iSocketBufSize );
    User::LeaveIfError( err );

    err = iSocket[ERTPPort].SetOpt( KSOSendBuf, KSOLSocket, aParams.iSocketBufSize );
    User::LeaveIfError( err );  

    err = iSocket[ERTPPort].SetOpt( KSoUdpSynchronousSend, KSolInetUdp,
                                    ETrue );
    User::LeaveIfError( err );          

    if ( aEnableRtcp )
        {
        err = iSocket[ERTCPPort].SetOpt( KSORecvBuf, KSOLSocket,
                                         KSocketBufSize );
        User::LeaveIfError( err );

        err = iSocket[ERTCPPort].SetOpt( KSOSendBuf, KSOLSocket,
                                         KSocketBufSize );
        User::LeaveIfError( err );          

        err = iSocket[ERTCPPort].SetOpt( KSoUdpSynchronousSend, KSolInetUdp,
                                         ETrue );
        User::LeaveIfError( err );          
        }

    // Let the application know what port we ended up using
    aLocalPort = dynLocalPort;

    RTP_DEBUG_DETAIL_DVALUE( "CRtpComm::ConstructL(), local port chosen = ", aLocalPort );
    
    iPriority = aParams.iPriority;
    iErrNotify = &aErrNotify;
    iRtcpEnabled = aEnableRtcp;
    iSocketSize = aParams.iSocketBufSize;

    // The sender and receiver objects are created separately
    // in ConstructSenderL and ConstructReceiverL

    CleanupStack::Pop(); // iSocket[ERTPPort]
    if ( aEnableRtcp )
        {
        CleanupStack::Pop(); // iSocket[ERTCPPort]
        }
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpComm* CRtpComm::NewL( TUint& aLocalPort,
                          RSocketServ& aSocketServ,
                          RConnection& aRConn, 
                          const TCreateSessionParams& aParams,
                          MRtpErrNotify& aErrNotify,
                          TBool aEnableRtcp )
    {
    CRtpComm* self = new ( ELeave ) CRtpComm();
    CleanupStack::PushL( self );
    self->ConstructL( aLocalPort, aSocketServ, aRConn, aParams, aErrNotify,
                      aEnableRtcp );
    CleanupStack::Pop(); //self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CRtpComm::~CRtpComm()
     {
     for ( TInt k = 0; k < KMaxPorts; k++ ) 
         {
         if ( iReceiver[ k ] )
         	{
         	delete iReceiver[ k ];
         	}
         if ( iSender[ k ] )
         	{
         	delete iSender[ k ];
         	}
         }
 
     Close();
     }

// ---------------------------------------------------------------------------
// CRtpComm::ConstructSenderL()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::ConstructSenderL(TInetAddr& aRtpAddr, TInetAddr& aRtcpAddr)
    {
    if ( !iSender[ERTPPort] )
        {
        iSender[ERTPPort] = CRtpCommSend::NewL( iSocket[ERTPPort], *iErrNotify, 
                                                /*KMaxPackets * 1024,*/ aRtpAddr );
        }
    if ( iRtcpEnabled && !iSender[ERTCPPort] )
        {
        iSender[ERTCPPort] = CRtpCommSend::NewL( iSocket[ERTCPPort],
                                                 *iErrNotify,
                                                 /*KMaxPackets * 256,*/ aRtcpAddr );
        // Boost the priority of the RTCP transmission, to make sure those
        // few packets reach their destination.
        iSender[ERTCPPort]->SetPriority( TCreateSessionParams::EPriorityHigh );
        }
    }

// ---------------------------------------------------------------------------
// CRtpComm::ConstructReceiverL()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::ConstructReceiverL(TBool aNonRtpObserverSet)
    {
    if ( !iReceiver[ERTPPort] )
        {
        iReceiver[ERTPPort] = CRtpCommRecv::NewL( ERTPPort, iSocket[ERTPPort], 
                                                  iPriority, *iErrNotify, aNonRtpObserverSet );
        iReceiver[ERTPPort]->RegisterReceivedNotify( iReceivedNotify );
        }
    if ( iRtcpEnabled && !iReceiver[ERTCPPort] )
        {
        iReceiver[ERTCPPort] = CRtpCommRecv::NewL( ERTCPPort,
                                                   iSocket[ERTCPPort], 
                                                   TCreateSessionParams::EPriorityHigh,
                                                   *iErrNotify, aNonRtpObserverSet );
        iReceiver[ERTCPPort]->RegisterReceivedNotify( iReceivedNotify );
        }
    }

// ---------------------------------------------------------------------------
// CRtpComm::Close()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::Close()
    {
    
    for ( TInt k = 0; k < KMaxPorts; k++ )
        {
        iSocket[k].Close();
        }
    }

// ---------------------------------------------------------------------------
// CRtpComm::SetToAddress()
// Return TRequestStatus.Int()
// ---------------------------------------------------------------------------
//  
TInt CRtpComm::SetToAddress( TPortType aPortType, TInetAddr& aAddr )
    {
    TInt ret = KErrNone;
    
    CRtpCommSend* aCommSend = iSender[aPortType];
        
    if (aCommSend)    
        {
        aCommSend->SetToAddress(aAddr);       
        }
        
    CRtpCommRecv* aCommRecv = iReceiver[aPortType];
        
    if (aCommRecv)    
        {
        aCommRecv->SetRemoteAddress(aAddr);       
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CRtpComm::SetAcceptedFromAddress()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::SetAcceptedFromAddress(TInetAddr& aAddr)
    {
     iReceiver[ERTPPort]->SetRemoteAddress(aAddr);                                          
    
    if ( iRtcpEnabled && iReceiver[ERTCPPort] )
        {
        iReceiver[ERTCPPort]->SetRemoteAddress(aAddr);                                          
        }
    }

// ---------------------------------------------------------------------------
// CRtpComm::SetNonRtpObserverFlag()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::SetNonRtpObserverFlag(TBool aValue)
    {
    if (iReceiver[ERTPPort] )
        {
        iReceiver[ERTPPort]->SetNonRtpObserverFlag(aValue);        
        }
    if (iReceiver[ERTCPPort] )
        {
        iReceiver[ERTCPPort]->SetNonRtpObserverFlag(aValue);        
        }    
    }

// ---------------------------------------------------------------------------
// CRtpComm::MaxSocketSize()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpComm::MaxSocketSize()
    {
    return iSocketSize;
    }
    
// ---------------------------------------------------------------------------
// CRtpComm::Send()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpComm::Send( TPortType aPortType, const TDesC8& aSendBuf )
    {
    TInt result( KErrNotReady );
    if ( iSender[aPortType] )
        {
        result = iSender[aPortType]->Send( aSendBuf );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CRtpComm::Send()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::Send( TPortType aPortType, const TDesC8& aSendBuf, TRequestStatus& aStatus )
    {
    if ( iSender[aPortType] )
        {
        iSender[aPortType]->Send( aSendBuf, aStatus );
        }
    else
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// CRtpComm::CancelSend()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::CancelSend( TPortType aPortType )
    {
    if ( iSender[aPortType] )
        {
        iSender[aPortType]->CancelAsyncSend();
        }
    }

// ---------------------------------------------------------------------------
// CRtpComm::Receive()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpComm::Receive( TPortType aPortType, TDes8& aRecvBuf )
    {
    TInt result( KErrNotReady );
    if ( iReceiver[aPortType] )
        {
        result = iReceiver[aPortType]->Recv( aRecvBuf );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CRtpComm::RegisterReceivedNotify()
// 
// ---------------------------------------------------------------------------
//
void CRtpComm::RegisterReceivedNotify( MReceivedNotify* aNotify )
    {
    iReceivedNotify = aNotify;
    if ( iReceiver[ERTPPort] )
        {
        iReceiver[ERTPPort]->RegisterReceivedNotify( aNotify );
        }
    if ( iReceiver[ERTCPPort] )
        {
        iReceiver[ERTCPPort]->RegisterReceivedNotify( aNotify );
        }
    }

// ---------------------------------------------------------------------------
// RSocket* CRtpComm::GetSocketObject()
// 
// ---------------------------------------------------------------------------
//
RSocket* CRtpComm::GetSocket( TPortType aPortType )
    {
    return &iSocket[aPortType];
    }

//  End of File
