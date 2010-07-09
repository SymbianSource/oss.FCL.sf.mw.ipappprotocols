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
#include "rtpsession.h"


// CONSTANTS
const TInt TStream::iOffset = _FOFF( TStream, iMagicKey );

// Used to calculate the average size of an RTCP packet, this represents
// the fraction by which the most recent RTCP packet influences the average.
const TReal KRtcpSizeGain = ( 1.0 / 16.0 );
const TReal KRtcpSizeGainRemainder = 1 - KRtcpSizeGain;
const TInt  KRtpLimitNumberOfRecvStream=100;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpSession::CRtpSession( const TRtpId aSessionId,
                          const TUint32* aProfileRTPTimeRates,
                          const TBool aStandardRtp,
                          MRtpErrNotify& aErrNotify,
                          const CRtpSDES* aSdesInfo , 
                          MRtpAsignUniqueID& aAssignUniqueID) 
    :
    iStandardRtp( aStandardRtp ),
    iSessionId( aSessionId ),
    iLocalSdes( aSdesInfo ),
    iNewSdes( NULL ),
    // Send one SDES packet after every KDES_SEND_PACKET SR or RR
    iSendSdesCounter( KSDES_SEND_PACKET ),
    iRtpRecvBuf( NULL, 0 ),
    iRtcpRecvBuf( NULL, 0 ),
    iRtcpEnabled( EFalse ),
    iFirstRTCPSent( EFalse ),
    iFPortsInit( EFalse ),
    iNumOfTxStreams( 0 ),
    iNumOfRxStreams( 0 ),
    iProfileRTPTimeRates( aProfileRTPTimeRates ),
    iAverageRtcpSize( static_cast<TReal>( KAverageRtcpPacketLength ) ),
    iRtcpTimeInterval( KMinRtcpTimeInterval ),
    // default: the session has one participant
    iTotalParticipantsSession( 1 ),
    iRtpObserver( NULL ),
    iRtcpObserver( NULL ),
    iNonRTPDataObserver( NULL ),
    iErrNotify( aErrNotify ),
    iAssignUniqueID( aAssignUniqueID),
    iSessionStarted( EFalse ),
    iSSRCJumps( 0 ),
    iRtcpErrors( 0 ),
    iRtcpSendingSuspended( EFalse ),
    iSeed( 0 )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpSession::ConstructL( const TCreateSessionParams& aParams,
                              TUint& aPort,
                              TBool aEnableRtcp,
                              const TRtcpParams* aRtcpParams,
                              RSocketServ& aSocketServ,
                              RConnection& aRConn,
                              const RLibrary& aLibrary,
                              MRtpErrNotify& aErrNotify )
    {
    RTP_DEBUG_DETAIL_DVALUE( "CRtpSession::ConstructL, socket size:", 
                             aParams.iSocketBufSize  );
    
    // Create at least packets of size KMaxRtpPacketSize. If client
    // wants to use bigger socket than that, also packets need to
    // have the same size.                         
    TInt packetSize = ( aParams.iSocketBufSize > KMaxRtpPacketSize ) ? 
            aParams.iSocketBufSize : KMaxRtpPacketSize;
            
    if ( iStandardRtp )
        {
        // allocate 1 packet to RTP sending 
        iPktSnd = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );
        iPktSnd->SetType( ERTP );

        // allocate 1 packet to RTP receiving
        iPktRcv = CRtpPacket::NewL( packetSize, iProfileRTPTimeRates );   
        iPktRcv->SetType( ERTP );
        TPtr8 aux1( iPktRcv->GetHBuf()->Des() );
        iRtpRecvBuf.Set( aux1 );
        }
    else
        {
        // Function at ordinal 1 creates new CRtpPacketExt
        TLibraryFunction entry = aLibrary.Lookup( 1 );

        iPktExtRcv = ( MRtpPacketExt * ) entry(); //NewL()
        iPktExtRcv->ConstructL( packetSize, iSessionId );
        TPtr8 aux1( iPktExtRcv->GetHBuf()->Des() );
        iRtpRecvBuf.Set( aux1 );

        iPktExtSnd = ( MRtpPacketExt * ) entry();  //NewL()
        iPktExtSnd->ConstructL( packetSize, iSessionId );
        }
    
    if ( aEnableRtcp )
        {
        iRtcpEnabled = ETrue;
        
        // allocate 1 packet for RTCP sending
        iPktRtcpSnd = CRtpPacket::NewL( KMaxRtcpPacketSize,
                                        iProfileRTPTimeRates );   

        // allocate 1 packet for RTCP receiving
        iPktRtcpRcv = CRtpPacket::NewL( KMaxRtcpPacketSize,
                                        iProfileRTPTimeRates );  
        TPtr8 aux2( iPktRtcpRcv->GetHBuf()->Des() );
        iRtcpRecvBuf.Set( aux2 );

        iSndRtcpTimer = CRtpTimer::NewL( CActive::EPriorityStandard, *this );
        
        InitialiseRtcp( aRtcpParams );
        }

    iCommNet = CRtpComm::NewL( aPort, aSocketServ, aRConn, aParams, aErrNotify,
                               aEnableRtcp );
    
    iCommNet->RegisterReceivedNotify( this );

    // instantiate RTP stream arrays
    iStreamTxArray = new ( ELeave ) CArrayFixFlat<TStream>( 2 );
    iStreamRxArray = new ( ELeave ) CArrayFixFlat<TStream>( 2 );
    iSdesArray = new ( ELeave ) CArrayPtrFlat<CRtpSDES>( 2 );

    // Assigned default SSRC     
    iDefaultSSRC = GenerateSSRC();
    iIsSrtp=EFalse;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpSession* CRtpSession::NewL( const TCreateSessionParams& aParams,
                                TUint& aPort,
                                TBool aEnableRtcp,
                                const TRtcpParams* aRtcpParams,
                                RSocketServ& aSocketServ,
                                RConnection& aRConn,
                                const TRtpId aSessionId,
                                const CRtpSDES* aSdes, // application's SDES
                                const TUint32* aProfileRTPTimeRates,
                                const TBool aStandardRtp,
                                const RLibrary& aLibrary,
                                MRtpErrNotify& aErrNotify, 
                                MRtpAsignUniqueID& aAssignUniqueID )
    {
    CRtpSession* self = new ( ELeave ) CRtpSession( aSessionId, 
                                                    aProfileRTPTimeRates,
                                                    aStandardRtp, aErrNotify,
                                                    aSdes,aAssignUniqueID);
    CleanupStack::PushL( self );
    self->ConstructL( aParams, aPort, aEnableRtcp, aRtcpParams, aSocketServ, 
                      aRConn, aLibrary, aErrNotify );
    CleanupStack::Pop(); // self 
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpSession::~CRtpSession()
    {
    RemoveAllStreams();
    delete iStreamTxArray;
    delete iStreamRxArray;
    if (iSdesArray)
		{
	    iSdesArray->ResetAndDestroy();
	    }
    delete iSdesArray;
    delete iCommNet;
    if ( iRtcpEnabled )
        {
        delete iSndRtcpTimer;
        }
    if ( iNewSdes )
        {
        delete iNewSdes;
        }
    if ( iStandardRtp )
        {
        delete iPktSnd;
        delete iPktRcv;
        if ( iRtcpEnabled )
            {
            delete iPktRtcpSnd;
            delete iPktRtcpRcv;
            }
        }
    else
        {
        if (iPktExtSnd)
			{
        	iPktExtSnd->Close(); // delete iPktExtSnd;
        	}
        if (iPktExtRcv)
        	{
        	iPktExtRcv->Close(); // delete iPktExtRcv;
        	}
        }
    iRxSSRCArray.Close();
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SetRemoteAddress()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SetRemoteAddress( TInetAddr& aRemoteAddr )
    {
    if ( !iCommNet )
        {
        return KErrGeneral;
        }
    
    TInt error( KErrNone );
    
    if ( iSessionStarted )
        {
        ResetTxStreamStats();
        ResetRxStreamStats();
        }
        
    iRemoteAddr[ERTPPort].SetAddress( aRemoteAddr.Address() ); 
    iRemoteAddr[ERTPPort].SetPort( aRemoteAddr.Port() ); 

    error = iCommNet->SetToAddress( ERTPPort, aRemoteAddr );   
    if (aRemoteAddr.Family() == KAfInet6 )
    	{
    	iRemoteAddr[ERTPPort].SetAddress( aRemoteAddr.Ip6Address() ); 
    	}
    
    if ( error == KErrNone && iRtcpEnabled )
        {
        TInetAddr remoteAddr( aRemoteAddr );
        
         // Set RTCP port
        remoteAddr.SetPort( remoteAddr.Port() + 1 );
        
        iRemoteAddr[ERTCPPort].SetAddress( remoteAddr.Address() ); 
        iRemoteAddr[ERTCPPort].SetPort( remoteAddr.Port() ); 
        if (aRemoteAddr.Family() == KAfInet6 )
    		{
    		iRemoteAddr[ERTCPPort].SetAddress( aRemoteAddr.Ip6Address() ); 
    		}
        error = iCommNet->SetToAddress( ERTCPPort, remoteAddr );                                        
        }
    
    if ( error == KErrNone && !iSessionStarted )
        {
        iCommNet->RegisterReceivedNotify( this );
        }
        
    iRemoteAddrSet = ETrue;
    return error;
    } 


// ---------------------------------------------------------------------------
// TInt CRtpSession::SetRemoteRtcpAddress()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SetRemoteRtcpAddress( TInetAddr& aRemoteAddr )
    {
    TInt error( KErrNone );
    
    if ( !iRtcpEnabled )
        {
        return KErrNotSupported;
        }
    else
        {

        iRemoteAddr[ERTCPPort].SetAddress( aRemoteAddr.Address() ); 
        iRemoteAddr[ERTCPPort].SetPort( aRemoteAddr.Port() ); 
		if (aRemoteAddr.Family() == KAfInet6 )
    		{
    		iRemoteAddr[ERTCPPort].SetAddress( aRemoteAddr.Ip6Address() ); 
    		}
        error = iCommNet->SetToAddress( ERTCPPort, aRemoteAddr );    
        }
        
    return error;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::CreateReceiveStreamL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::CreateReceiveStreamL( TRtpId aRcvStreamId,
                                        const TRtpPayloadType aPayloadType )
    {
    if ( iNumOfRxStreams == 0 )
        {
        // This is the first Rx stream
        iCommNet->ConstructReceiverL(iNonRTPDataObserver!=NULL);
        if (iRemoteAddrSet)
            {
            iCommNet->SetAcceptedFromAddress(iRemoteAddr[ERTPPort]);
            }
        }

    CRtpRecvStream* newReceiveStream =
        CRtpRecvStream::NewL( aPayloadType, iSessionId, aRcvStreamId,
                              &iRtpObserver, iRtcpObserver,
                              iProfileRTPTimeRates, this );


    // store new receive stream address in stream array 
    TStream arrayData( aRcvStreamId, ( TUint ) newReceiveStream );

    TInt err( AddStream( arrayData, ERxStream ) );

    if ( err == KErrNone )
        {
        iNumOfRxStreams++;
        }
    else
        {
        delete newReceiveStream;
        }

    return err;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::GetUniqueSSRC()
// 
// ---------------------------------------------------------------------------
//
TRtpSSRC CRtpSession::GetUniqueSSRC()
    {
    TRtpSSRC ssrc( 0 );
    if ( iNumOfTxStreams != 0 )
        {
        ssrc = GenerateSSRC();

        // find if SSRC is already in use
        while ( FindStreamForSSRC( ssrc ) )
            {
            // if SSRC is already in use, create a new one    
            ssrc = GenerateSSRC();
            }
        }
    else // first TX stream, set transmit stream SSRC equal default SSRC
        {
        ssrc = iDefaultSSRC;
        }
    return ssrc;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::CreateTransmitStreamL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::CreateTransmitStreamL( TRtpId aTranStreamId, 
                                         const TRtpPayloadType aPayloadType,
                                         TRtpSSRC& aSSRC )
    {
    if ( iNumOfTxStreams == 0 )
        {
        // This is the first Tx stream
        iCommNet->ConstructSenderL(iRemoteAddr[ERTPPort], iRemoteAddr[ERTCPPort]);
        }

    aSSRC = GetUniqueSSRC();

    CRtpTranStream* newTranStream =
        CRtpTranStream::NewL( aPayloadType, iSessionId, aTranStreamId, aSSRC,
                              iRtcpObserver, iProfileRTPTimeRates );

    // store new transmit stream address in stream array 
    TStream arrayData( aTranStreamId, ( TUint ) newTranStream );

    TInt ret( AddStream( arrayData, ETxStream ) );

    if ( ret == KErrNone )
        {
        iNumOfTxStreams++;
        RedistributeBandwidth();
        }
    else
        {
        delete newTranStream;
        }
    
    RTP_DEBUG_DETAIL_DVALUE( "Created TX Stream with Session ID = ", iSessionId );
    RTP_DEBUG_DETAIL_DVALUE( "Created TX Stream ID = ", aTranStreamId );
    RTP_DEBUG_DETAIL_DVALUE( "Created TX Stream SSRC = ", aSSRC );
   
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::CreateTransmitStreamExtL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::CreateTransmitStreamExtL( TRtpId aTranStreamId, 
                                            const TRtpPayloadType aPayloadType, 
                                            const TRtpSSRC aSSRC )
    {
    if ( iNumOfTxStreams == 0 )
        {
        // This is the first Tx stream
        iCommNet->ConstructSenderL(iRemoteAddr[ERTPPort], iRemoteAddr[ERTCPPort]);
        }

    iDefaultSSRC = aSSRC;  // asigned this value to the default just in case

    CRtpTranStream* newTranStream =
        CRtpTranStream::NewL( aPayloadType, iSessionId, aTranStreamId, aSSRC,
                              iRtcpObserver, iProfileRTPTimeRates );

    // store new receive stream address in stream array 
    TStream arrayData( aTranStreamId, ( TUint ) newTranStream );

    TInt ret( AddStream( arrayData, ETxStream ) );

    if ( ret == KErrNone )
        {        
        iNumOfTxStreams++;
        RedistributeBandwidth();
        }
    else
        {
        delete newTranStream;
        }
    
    RTP_DEBUG_DETAIL_DVALUE( "Created TX Stream with Session ID = ", iSessionId );
    RTP_DEBUG_DETAIL_DVALUE( "Created TX Stream ID = ", aTranStreamId );
    RTP_DEBUG_DETAIL_DVALUE( "Created TX Stream SSRC = ", aSSRC );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CRtpSession::CloseStream()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::CloseStream( TRtpId aRcvStreamID )
    {
    TStreamType streamType;

    // remove stream
    TInt error( RemoveStream( aRcvStreamID, streamType ) );

    if ( error == KErrNone )
        {
        if ( streamType == ERxStream )
            {
            iNumOfRxStreams--;
            }
        else //ETxStream
            {
            iNumOfTxStreams--;
            RedistributeBandwidth();
            }
        }
    }


// ---------------------------------------------------------------------------
// CRtpSession::RedistributeBandwidths()
// 
// Calculate a new bandwidth for the TX streams.
//
// iFraction is the procent reserved for RTCP.
//
// Note, the bandwith is just the theoretical bandwidth which is used 
// for RTCP and has nothing to do with the real bandwith. 
// So if RTCP is not enabled, it will return (no calculation needed). 
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::RedistributeBandwidth()
    {
    if ( iRtcpEnabled && ( iStreamTxArray != NULL ) )
       {
        
        CRtpTranStream* tempTxStream;
        
        TInt count = iStreamTxArray->Count();
        if ( count == 0 )
            {
            return;
            }
        TUint32 bandwidth = static_cast<TUint32>(
                                ( iBandWidth * ( 1 - iFraction )  ) / count );
        
        for( int index = 0; index < count; index++ )
            {
            tempTxStream = reinterpret_cast<CRtpTranStream*>(
                           ( iStreamTxArray->At( index ).GetStreamAddress() )
                           );
            tempTxStream->SetBandwidth( bandwidth );
            }
       }
    }
    
// ---------------------------------------------------------------------------
// CRtpSession::ResetRxStreamStats()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::ResetRxStreamStats()
    {
    CRtpRecvStream* tempStream;
    
    TInt count = iStreamRxArray->Count();
    
    for(int index = 0; index < count; index++ )
        {
        tempStream = ( CRtpRecvStream* )
                     ( iStreamRxArray->At( index ).GetStreamAddress() );
        tempStream->ResetStreamStat();    
        }
    } 

// ---------------------------------------------------------------------------
// CRtpSession::ResetTxStreamStats()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::ResetTxStreamStats()
    {
    CRtpTranStream* tempStream;
    
    TInt count = iStreamTxArray->Count();    
    
    for ( TInt index( 0 ); index < count; index++ )
            {
            tempStream = ( CRtpTranStream* )
                         ( iStreamTxArray->At( index ).GetStreamAddress() );
            tempStream->ResetStreamStat();
            }
    }


// ---------------------------------------------------------------------------
// TInt CRtpSession::ReadyToSendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::ReadyToSendRtpPacket(TRtpId aTranStreamId)
    {
    if ( iRtpPacketObserver )
        {
        RTP_DEBUG_DETAIL("Giving ReadyToSendRtpPacket() Call Back " );
        TPtr8 pktPtr(iPktSnd->Data(), iPktSnd->Size(), iPktSnd->Size());
        iRtpPacketObserver->ReadyToSendRtpPacket( aTranStreamId , pktPtr );
        }   
    }


// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtpPacket( TRtpId aTranStreamId,
                                 const TRtpSendHeader& aHeaderInfo, 
                                 const TDesC8& aPayloadData,
                                 const TArray<TRtpCSRC> *aCsrcList )
    {
    if ( static_cast<TUint> (aPayloadData.Size()) > iCommNet->MaxSocketSize() )
    	{
    	return KErrOverflow;
    	}

    // find stream
    TUint streamAddress = 0;
    TInt ret( FindStream( aTranStreamId, streamAddress ) );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream = ( CRtpTranStream* ) streamAddress;
        tempStream->BuildRtpPacket( aHeaderInfo, aPayloadData, 0, EFalse,
                                    iPktSnd, aCsrcList );
        
        ReadyToSendRtpPacket(aTranStreamId);
        
        ret = iCommNet->Send( ERTPPort, iPktSnd->Des() );
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtpPacket( TRtpId aTranStreamId,
                                 const TRtpSendHeader& aHeaderInfo,
                                 const TDesC8& aPayloadData,
                                 TRequestStatus& aStatus,
                                 const TArray<TRtpCSRC> *aCsrcList )
    {
    if ( static_cast<TUint>( aPayloadData.Size() ) > iCommNet->MaxSocketSize() )
    	{
    	return KErrOverflow;
    	}

    // find stream
    TUint streamAddress = 0;
    TInt ret( FindStream( aTranStreamId, streamAddress ) );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream =
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        tempStream->BuildRtpPacket( aHeaderInfo, aPayloadData, 0, EFalse,
                                    iPktSnd, aCsrcList );
									
        ReadyToSendRtpPacket(aTranStreamId);
			
        iCommNet->Send( ERTPPort, iPktSnd->Des(), aStatus );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtpPacket( const TRtpSendHeader& aHeaderInfo,
                                 const TDesC8& aPayloadData )
    {
    if ( static_cast<TUint>( aPayloadData.Size() ) > iCommNet->MaxSocketSize() )
    	{
   		return KErrOverflow;
    	}
       
    TInt ret( iPktExtSnd->RtpPacketBuild( aHeaderInfo, aPayloadData ) );

    if( ret == KErrNone )
        {
        ret = iCommNet->Send( ERTPPort, iPktExtSnd->Des() );
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtpPacket( const TRtpSendHeader& aHeaderInfo, 
                                 const TDesC8& aPayloadData, 
                                 TRequestStatus& aStatus )
    {
    if ( static_cast<TUint>( aPayloadData.Size() ) > iCommNet->MaxSocketSize() )
    	{
    	return KErrOverflow;
    	}
    	
    TInt ret( iPktExtSnd->RtpPacketBuild( aHeaderInfo, aPayloadData ) );
    
    if( ret == KErrNone )
        {
        iCommNet->Send( ERTPPort, iPktExtSnd->Des(), aStatus );    
        }
    

    return ret;
    }

// ---------------------------------------------------------------------------
// void CRtpSession::SendData()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::SendData( TBool aUseRTPSocket,
                            const TDesC8& aData, 
                            TRequestStatus& aStatus )
    {        
    if (aUseRTPSocket)
        {
        iCommNet->Send( ERTPPort, aData, aStatus );    
        }
    else
        {
        iCommNet->Send( ERTCPPort, aData, aStatus );    
        }                
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtpPacket( TRtpId aTranStreamId,
                                 TRtpSequence aSequenceNum,
                                 const TRtpSendHeader& aHeaderInfo,
                                 const TDesC8& aPayloadData,
                                 TRequestStatus& aStatus,
                                 const TArray<TRtpCSRC> *aCsrcList )
    {
    if ( static_cast<TUint>( aPayloadData.Size() ) > iCommNet->MaxSocketSize() )
    	{
    	return KErrOverflow;
    	}

    // find stream
    TUint streamAddress = 0;
    TInt ret( FindStream( aTranStreamId, streamAddress ) );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream =
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        tempStream->BuildRtpPacket( aHeaderInfo, aPayloadData, aSequenceNum,
                                    ETrue, iPktSnd, aCsrcList );
        
        ReadyToSendRtpPacket(aTranStreamId);								
									
        iCommNet->Send( ERTPPort, iPktSnd->Des(), aStatus );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CRtpSession::CancelSend()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::CancelSend()
    {
    iCommNet->CancelSend( ERTPPort );
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtcpByePacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtcpByePacketL( TRtpId aTranStreamId,
                                     const TDesC8& aReason )
    {
    if ( !iRtcpEnabled )
        {
        return KErrNotSupported;
        }
    if ( !iSessionStarted )
        {
        RTP_DEBUG_DETAIL("CRtpSession::SendRtcpByePacket, Session not started" );
        
        return KErrNotReady;
        }
    if ( aReason.Size() >= KMaxRtcpReason )
        {
        return KErrTooBig;
        }

    // find stream
    TUint streamAddress( 0 );
    TInt ret( FindStream( aTranStreamId, streamAddress ) );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream =
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        ret = BuildRTCPReport( tempStream, NULL );
        if ( ret == KErrNone )
            {
            ret = tempStream->BuildRtcpBYEPacket( aReason, iPktRtcpSnd );
            }
        if ( ret == KErrNone )
            {
            if(IsSrtp())
            	{
            	SendSRTCPReportL(tempStream->GetLocalSSRC());
            	}
            else	
            	{
            	SendRTCPReport();	
            	}            
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtcpAppPacketL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtcpAppPacketL( TRtpId aTranStreamId,
                                     const TRtcpApp& aApp )
    {
    if ( !iRtcpEnabled )
        {
        return KErrNotSupported;
        }
    if ( !iSessionStarted )
        {
        RTP_DEBUG_DETAIL("CRtpSession::SendRtcpAppPacket, Session not started" );
      
        return KErrNotReady;
        }
    if ( aApp.iAppDataLen >= KMaxRtcpAppData )
        {
        return KErrTooBig;
        }

    // find stream
    TUint streamAddress = 0;
    TInt ret( FindStream( aTranStreamId, streamAddress ) );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream =
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        ret = BuildRTCPReport( tempStream, NULL );
        if ( ret == KErrNone )
            {
            ret = tempStream->BuildRtcpAPPPacket( aApp, iPktRtcpSnd );
            }
        if ( ret == KErrNone )
            {
            if(IsSrtp())
            	{
            	SendSRTCPReportL(tempStream->GetLocalSSRC());
            	}
            else	
            	{
            	SendRTCPReport();	
            	}            
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtcpSrPacketL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtcpSrPacketL( TRtpId aTranStreamId )
    {
    if ( !iRtcpEnabled )
        {
        return KErrNotSupported;
        }
    if ( !iSessionStarted )
        {
       	RTP_DEBUG_DETAIL( "CRtpSession::SendRtcpSrPacket, Session not started" );
        return KErrNotReady;
        }

    TInt ret( KErrNone );

    // find stream
    TUint streamAddress = 0;
    ret = FindStream( aTranStreamId, streamAddress );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream =
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        ret = BuildRTCPReport( tempStream, NULL );
        if ( ret == KErrNone )
            {
            if(IsSrtp())
            	{
            	SendSRTCPReportL(tempStream->GetLocalSSRC());
            	}
            else	
            	{
            	SendRTCPReport();	
            	}            
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SendRtcpRrPacketL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SendRtcpRrPacketL( TRtpId aRecvStreamId )
    {
    RTP_DEBUG_DETAIL( "CRtpSession::SendRtcpRrPacket entry" );
   
    if ( !iRtcpEnabled )
        {
        return KErrNotSupported;
        }
    if ( !iSessionStarted )
        {
        RTP_DEBUG_DETAIL( "Error: Session not started");
        
        return KErrNotReady;
        }

    TInt ret( KErrNone );
    CRtpRecvStream* tempStream( NULL );
    // find stream
    TUint streamAddress = 0;
    ret = FindStream( aRecvStreamId, streamAddress );

    if ( ret == KErrNone )
        {
        // Make sure the stream has received packets
        tempStream = 
            reinterpret_cast<CRtpRecvStream*>( streamAddress );
        if ( tempStream->FirstPkg() )
            {
           	RTP_DEBUG_DETAIL( "Error: Must send RTP packets before RTCP" );
            
            ret = KErrNotReady;
            }
        }

    if ( ret == KErrNone )
        {
        // Build the RR version of the RTCP header
        TRtpPacketStreamParam streamParam;
        TRtpPacketIOParam initParam;

        initParam.TRTCP_HEADER.pt = ERTCP_RR;
        initParam.TRTCP_HEADER.sourceCount = 1;

        // The length of one RR block in 32-bit units is 6
        // Added to this the source SSRC makes the total length 7
        initParam.TRTCP_HEADER.length = 7;

        streamParam.TRTCP_HEADER.SSRC = iDefaultSSRC;

        iPktRtcpSnd->SetType( ERTCP_HEADER );
        ret = iPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam );
        }

    if ( ret == KErrNone )
        {
        // Build the RR and SDES sections, then send the packet
        tempStream->RtpStreamCreateRtcpReportSection( iPktRtcpSnd );
        BuildSdesSection( NULL );
        if(IsSrtp())
           	{
           	SendSRTCPReportL(tempStream->GetLocalSSRC());
           	}
        else	
        	{
        	SendRTCPReport();	
        	}            
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// RSocket* CRtpSession::GetRtpSocket()
// 
// ---------------------------------------------------------------------------
//
RSocket* CRtpSession::GetRtpSocket()
    {
    if ( iCommNet )
        return iCommNet->GetSocket( ERTPPort );
    else
        return NULL;
    }

// ---------------------------------------------------------------------------
// RSocket* CRtpSession::GetRtcpSocket()
// 
// ---------------------------------------------------------------------------
//
RSocket* CRtpSession::GetRtcpSocket()
    {
    if ( iCommNet && iRtcpEnabled )
        return iCommNet->GetSocket( ERTCPPort );
    else
        return NULL;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::GetStreamStatistics()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::GetStreamStatistics( TRtpId aStreamId, TRtpPeerStat& aStat )
    {
    TInt ret;

    // find stream
    TUint streamAddress = 0;
    TStreamType aStreamType;

    ret = FindStream( aStreamId, streamAddress, aStreamType );

    if ( ret != KErrNone )
        {
        return ret;
        }

    if ( aStreamType == ERxStream )
        {
        CRtpRecvStream* tempStream = ( CRtpRecvStream* ) streamAddress;
        ret = tempStream->GetStreamStat( aStat );
        }
    else //ETxStream
        {
        CRtpTranStream* tempStream = ( CRtpTranStream* ) streamAddress;
        ret = tempStream->GetStreamStat( aStat );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::InitialiseRtcp()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::InitialiseRtcp( const TRtcpParams* aParams )
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }

    if ( aParams == NULL )
        {
        TRtcpParams params;
        params.iSessionBWidth = KRtpDefaultBandWidth;
        params.iRtcpFraction = KRtpDefaultRtcpFrac;
        SetRtcpParameters( params );
        }
    else
        {
        SetRtcpParameters( *aParams );
        }

    // 1000 - milliseconds per second
    // 8 - bits in one byte
    iRtcpTimeInterval = ( TUint ) ( 1000 * KAverageRtcpPacketLength * 8 /
                                  ( iBandWidth * iFraction ) ); 

    // The first RTCP interval should be half the usual amount. Hence, we
    // divide by two here.
     if ( iRtcpTimeInterval < KMinRtcpTimeInterval / 2 )
        {
        iRtcpTimeInterval = KMinRtcpTimeInterval / 2;
        }

    // Randomise
    TReal rtcpTimeInterval64 = iRtcpTimeInterval * ( Random64() + 0.5 );
    iRtcpTimeInterval = static_cast<TUint>( rtcpTimeInterval64 );
    
    RTCP_DEBUG_DETAIL_DVALUE( "CRtpSession::InitialiseRtcp - RTCP Interval(ms) = ", iRtcpTimeInterval );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SetRtcpParameters()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SetRtcpParameters( const TRtcpParams& aRtcpParams )
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }

    if ( aRtcpParams.iSessionBWidth < KRtpMinimumBandWidth )
        {
        iBandWidth = KRtpDefaultBandWidth;
        }
    else
        {
        iBandWidth = aRtcpParams.iSessionBWidth;
        }
    if ( aRtcpParams.iRtcpFraction == 0 || aRtcpParams.iRtcpFraction > 1 )
        {
        iFraction = KRtpDefaultRtcpFrac;
        }
    else
        {
        iFraction = aRtcpParams.iRtcpFraction;
        }
    
    RedistributeBandwidth();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::RegisterRtpObserver()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::RegisterRtpObserver( MRtpObserver& aObserver )
    {
    if ( iPktExtRcv )
        iPktExtRcv->RegisterRtpObserver( aObserver );

    iRtpObserver = &aObserver;        
       
    // now update observers in receiver streams
    CRtpRecvStream* tempStream;
    TInt count = iStreamRxArray->Count();
    for(int index = 0; index < count; index++ )
        {
        tempStream = ( CRtpRecvStream* )
                     ( iStreamRxArray->At( index ).GetStreamAddress() );
        tempStream->RegisterRtpObserver(&aObserver);    
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpSession::UnregisterRtpObserver()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::UnregisterRtpObserver()
    {
    if ( iPktExtRcv )
        iPktExtRcv->UnregisterRtpObserver();

    iRtpObserver = NULL;
    
    // now unregister receiver stream RTP observer
    CRtpRecvStream* tempStream;    
    TInt count = iStreamRxArray->Count();
    
    for(int index = 0; index < count; index++ )
        {
        tempStream = ( CRtpRecvStream* )
                     ( iStreamRxArray->At( index ).GetStreamAddress() );
        tempStream->UnRegisterRtpObserver();    
        }                   
    }


// ---------------------------------------------------------------------------
// TInt CRtpSession::RegisterRtpPostProcessingObserver()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::RegisterRtpPostProcessingObserver( MRtpPostProcessingObserver& aRTPPacketObserver )
    {
    iRtpPacketObserver = &aRTPPacketObserver;  
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpSession::UnregisterRtpPostProcessingObserver()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::UnregisterRtpPostProcessingObserver()
    {
    iRtpPacketObserver = NULL;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::RegisterRtcpObserver()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::RegisterRtcpObserver( MRtcpObserver& aObserver )
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }
        
    iRtcpObserver = &aObserver;
    
    // now register receiver stream RTCP observers
    CRtpRecvStream* tempRecvStream;
    TInt count = iStreamRxArray->Count();    
    for(int index = 0; index < count; index++ )
        {
        tempRecvStream = ( CRtpRecvStream* )
                     ( iStreamRxArray->At( index ).GetStreamAddress() );
        tempRecvStream->RegisterRtcpObserver(aObserver);    
        }               

    // now register transmit stream RTCP observers
    CRtpTranStream* tempTxStream;
    count = iStreamTxArray->Count();    
    for(int index = 0; index < count; index++ )
        {
        tempTxStream = ( CRtpTranStream* )
                     ( iStreamTxArray->At( index ).GetStreamAddress() );
        tempTxStream->RegisterRtcpObserver(aObserver);    
        }               
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpSession::UnregisterRtcpObserver()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::UnregisterRtcpObserver()
    {
    iRtcpObserver = NULL;

    // now unregister receiver stream RTCP observers
    CRtpRecvStream* tempRecvStream;   
    TInt count = iStreamRxArray->Count();
    
    for(int index = 0; index < count; index++ )
        {
        tempRecvStream = ( CRtpRecvStream* )
                     ( iStreamRxArray->At( index ).GetStreamAddress() );
        tempRecvStream->UnRegisterRtcpObserver();    
        }               

    // now unregister transmit stream RTCP observers
    CRtpTranStream* tempTxStream;    
    count = iStreamTxArray->Count();
    
    for(int index = 0; index < count; index++ )
        {
        tempTxStream = ( CRtpTranStream* )
                     ( iStreamTxArray->At( index ).GetStreamAddress() );
        tempTxStream->UnRegisterRtcpObserver();    
        }               

    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::SetNonRTPDataObserver()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::SetNonRTPDataObserver( MNonRTPDataObserver* aNonRTPDataObserver )
    {
    iNonRTPDataObserver = aNonRTPDataObserver;    
    if (iNonRTPDataObserver)
        {
        iCommNet->SetNonRtpObserverFlag(ETrue);        
        }
    else
        {
        iCommNet->SetNonRtpObserverFlag(EFalse);                
        }
    return KErrNone;
    }



// ---------------------------------------------------------------------------
// CRtpSession::StartSession()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::StartSession()
    {
    TInt result( KErrNone );
    if ( iNumOfRxStreams == 0 && iNumOfTxStreams == 0 )
        {
        return KErrNotReady;
        }
    if ( iNumOfRxStreams != 0 )
        {
        // Start receiving RTP streams
        result = IssueRtpRecv();
        // IssueRtpRecv() returns KErrNone the first time, KErrInUse if it
        // has been called before and KErrNotReady if the receive resources
        // are not constructed.
        if ( iRtcpEnabled && ( result == KErrNone ) )
            {
            result = IssueRtcpRecv();
            }
        // If the sockets are already active listening for packets, this is OK
        if ( result == KErrInUse )
            {
            result = KErrNone;
            }
        }
    if ( iRtcpEnabled && iNumOfTxStreams != 0 && !iSndRtcpTimer->IsActive() )
        {
        // Start sending RTCP packets
        iSndRtcpTimer->After( iRtcpTimeInterval * KMicrosecondPerMillSecond );
        }
    if ( result == KErrNone )
        {
        iSessionStarted = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CRtpSession::IssueRtpRecv()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::IssueRtpRecv()
    {
    return iCommNet->Receive( ERTPPort, iRtpRecvBuf );
    }

// ---------------------------------------------------------------------------
// CRtpSession::IssueRtcpRecv()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::IssueRtcpRecv()
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }
    return iCommNet->Receive( ERTCPPort, iRtcpRecvBuf );
    }

// ---------------------------------------------------------------------------
// CRtpSession::DoBuildSendRTCPReportIfNotSentL()
// 
// ---------------------------------------------------------------------------
//
TBool CRtpSession::DoBuildSendRTCPReportIfNotSentL()
    {
    if ( !iRtcpEnabled )
        {
        return EFalse;
        }
    TInt index( 0 );
    TBool reportSent( EFalse );
    TInt totalTxStream( iStreamTxArray->Count() );
    
    for ( index = 0; index < totalTxStream; index++ )
        {
        CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>
            ( iStreamTxArray->At( index ).GetStreamAddress() );
        if ( !tempStream->SentRtcpReport() && !reportSent )
            {
            // if no report has been sent, send it now
            // totalTxStream - index - 1 makes sure method knows when this is
            // the last RTCP report to be sent
            if ( BuildRTCPReport( tempStream, totalTxStream - index - 1 )
                 == KErrNone )
                {
                RTP_DEBUG_DETAIL_DVALUE( "BuildSendRTCPReport: Sending SR RTCP REPORT TX stream ID = ",
                    iStreamTxArray->At( index ).GetMagicKey() );
                
                if(IsSrtp())
	            	{
	            	SendSRTCPReportL(tempStream->GetLocalSSRC());
	            	}
	            else	
	            	{
	            	SendRTCPReport();	
	            	}            
                }
            else
                {
                RTP_DEBUG_DETAIL_DVALUE( "BuildSendRTCPReport: No report sent for TX stream ID = ",
                    iStreamTxArray->At( index ).GetMagicKey() );
                }
    
            tempStream->SetRtcpReportFlag();
            reportSent = ETrue;
            
            }
        }
    return reportSent;
    }

// ---------------------------------------------------------------------------
// CRtpSession::DoBuildSendRTCPReportL()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::DoBuildSendRTCPReportL()
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TInt index( 0 );
    TInt totalTxStream( iStreamTxArray->Count() );

    for ( index = 0; index < totalTxStream; index++ )
        {
        CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>(
            ( iStreamTxArray->At( index ).GetStreamAddress() ) );
        tempStream->ResetRtcpReportFlag();

        // totalTxStream - index - 1 makes sure method knows when this is
        // the last RTCP report to be send
        if ( BuildRTCPReport( tempStream, totalTxStream - index - 1 )
             == KErrNone )
            {
            
            RTP_DEBUG_DETAIL_DVALUE( "BuildSendRTCPReport: Sending (AFTER) SR RTCP REPORT TX stream ID = ",
                iStreamTxArray->At( index ).GetMagicKey() );
            
            if(IsSrtp())
            	{
            	SendSRTCPReportL(tempStream->GetLocalSSRC());
            	}
            else	
            	{
            	SendRTCPReport();	
            	}            
            }
        else
            {
            RTP_DEBUG_DETAIL_DVALUE( "BuildSendRTCPReport: No report sent for TX stream ID = ",
                iStreamTxArray->At( index ).GetMagicKey() );
            }
        tempStream->SetRtcpReportFlag();
        }
    }

// ---------------------------------------------------------------------------
// CRtpSession::BuildSendRTCPReportL()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::BuildSendRTCPReportL()
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TInt totalRxStream( iStreamRxArray->Count() );
    TInt totalTxStream( iStreamTxArray->Count() );
    TBool reportSent( ETrue );

    if ( totalTxStream > 0 )
        {
        reportSent = DoBuildSendRTCPReportIfNotSentL();
        // if no reports were sent for any of the TX streams, reset flags and
        // send an RTCP report this assures that if 2 or more Tx streams will
        // not send at the same time
        if ( !reportSent )
            {
            DoBuildSendRTCPReportL();
            }
        }
    else if ( totalRxStream > 0 )
        {
        
        RTCP_DEBUG_DETAIL_DVALUE( "BuildSendRTCPReport::Sending RR RTCP REPORT sender SSRC = ",
                      iDefaultSSRC );
       
        if ( BuildRTCPReport( NULL, NULL ) == KErrNone )
            {
            if(IsSrtp())
            	{
            	SendSRTCPReportL(iDefaultSSRC);
            	}
            else	
            	{
            	SendRTCPReport();	
            	}            
            }
        }
    else
        {
        // Do Nothing
        }

    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::ShouldBuildEmptyRR()
// 
// ---------------------------------------------------------------------------
//
TBool CRtpSession::ShouldBuildEmptyRR( CRtpTranStream* aTempTranStream )
    {
    if ( !iRtcpEnabled )
        {
        return EFalse;
        }

    TBool fEmptyRR( ETrue );
    if ( aTempTranStream )
        {
        // check if no data has been sent lately
        if ( aTempTranStream->SentRTPPackets() )
            {
            fEmptyRR = EFalse;
            }
        }

    // find if any receive stream has received an RTP packet
    if ( AnyRcvStreamReceivedRtpPacket() )
        {
        fEmptyRR = EFalse;
        }
    return fEmptyRR;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::DetermineSourceCountAndLength()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::DetermineSourceCountAndLength(
                      TInt& aSourceCount,
                      TInt& aLength,
                      TBool aRxActive[KMaxNumActiveRcvStreams] )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TInt k( 0 );
    TInt numRcvStreams( iStreamRxArray->Count() );
    for ( k = 0; k < numRcvStreams; k++ )
        {
        if ( aRxActive[k] )
            {
            if ( RcvStreamReceivedRtpPacket( k ) )
                {
                aSourceCount++;
                aLength += 6; // Length of one RR block in 32-bit units
                
                
                RTCP_DEBUG_DETAIL_DVALUE( "SEND: RX RR ID = ",
                          iStreamRxArray->At( k ).GetMagicKey() );
                RTCP_DEBUG_DETAIL_DVALUE( "SEND: RX RR SSRC = ", 
                   ( ( CRtpRecvStream* )iStreamRxArray->At( k ).
                      GetStreamAddress() )->GetLocalSSRC() );
               
                }
            }
        }
    }

   
// ---------------------------------------------------------------------------
// TInt CRtpSession::SetParamsForFullRTCPReport()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::SetParamsForFullRTCPReport(
                       CRtpTranStream* aTranStream,
                       TRtpPacketStreamParam* aStreamParam,
                       TRtpPacketIOParam* aInitParam,
                       TBool aRxActive[KMaxNumActiveRcvStreams] )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TInt sourceCount = 0;
    TInt length = 1;

    if ( aTranStream != NULL )
        {
        if ( aTranStream->SentRTPPackets() )
            {
            
            RTCP_DEBUG_DETAIL_DVALUE( "SEND: TX SR ID = ",
                      aTranStream->GetStreamID() );
            RTCP_DEBUG_DETAIL_DVALUE( "SEND: TX SR SSRC = ",
                      aTranStream->GetLocalSSRC() );
        
            
            length += 5;
            aInitParam->TRTCP_HEADER.pt = ERTCP_SR;
            }
        else
            {
            aInitParam->TRTCP_HEADER.pt = ERTCP_RR;
            }
        aStreamParam->TRTCP_HEADER.SSRC = aTranStream->GetLocalSSRC();
        }   
    else
        {
        aInitParam->TRTCP_HEADER.pt = ERTCP_RR;
        aStreamParam->TRTCP_HEADER.SSRC = iDefaultSSRC;
        }

    DetermineSourceCountAndLength( sourceCount, length, aRxActive );

    aInitParam->TRTCP_HEADER.sourceCount = sourceCount;
    aInitParam->TRTCP_HEADER.length = length;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::BuildRTCPReport()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::BuildRTCPReport( CRtpTranStream* aTempTranStream,
                                   TInt aLastReport )
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }
    
    RTCP_DEBUG_DETAIL( "CRtpSession::BuildRTCPReport" );
    

    TBool activeStreams( EFalse );
    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;
    TBool rxActive[KMaxNumActiveRcvStreams];
    TInt numRcvStreams( iStreamRxArray->Count() );
    TInt k( 0 );
    TBool streamExists( EFalse );

    iPktRtcpSnd->RtpPacketReset();

    // send an SDES packet if no Tx or Rx streams are available
    if ( aTempTranStream != NULL || numRcvStreams != NULL )
        {
        streamExists = ETrue;
        }
    else
        {
        
        RTCP_DEBUG_DETAIL( "Error: no suitable stream exists" );
        
        return KErrGeneral;
        }

    Mem::FillZ( rxActive, KMaxNumActiveRcvStreams );

    // check which streams are active
    for ( k = 0; k < numRcvStreams; k++ )
        {
        if ( RcvStreamActive( k ) )
            {
            rxActive[k] = ETrue;
            activeStreams = ETrue;
            }
        else
            {
            rxActive[k] = EFalse;
            }
        }

    if ( aTempTranStream )
        {
        if ( aTempTranStream->FirstPkg() )
            {
            RTCP_DEBUG_DETAIL( "Error: must send RTP packets before RTCP" );
            
            return KErrGeneral;
            }
        }
    else
        {
        if ( !activeStreams )
            {
            RTCP_DEBUG_DETAIL( "Error: no active receive streams found" );
            
            return KErrGeneral;
            }
        }

    if ( streamExists )
        {
        TBool fEmptyRR( ShouldBuildEmptyRR( aTempTranStream ) );
        
        if ( fEmptyRR == EFalse )
            {
            // find which stream are active and send report accordingly
            SetParamsForFullRTCPReport( aTempTranStream, &streamParam,
                                        &initParam, rxActive );
            // first build the standard RTCP header
            iPktRtcpSnd->SetType( ERTCP_HEADER );
            iPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam );

            // always send report about itself (stream) and all the other
            // active receiving streams
            BuildSrSection( aTempTranStream );
            BuildRrSection( numRcvStreams, rxActive, aLastReport );
            }
        else
            {
            // build empty RR packet if we have inactive Tx and/or Rx streams
            BuildEmptyRr( aTempTranStream );
            }
        }

    BuildSdesSection( aTempTranStream );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpSession::BuildSrSection()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::BuildSrSection( CRtpTranStream* aTempTranStream )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    if ( aTempTranStream )
        {
        if ( aTempTranStream->SentRTPPackets() )
            {
            RTCP_DEBUG_DETAIL( "SEND: Sent SR packet" );
                    
            aTempTranStream->RtpStreamCreateRtcpReportSection( iPktRtcpSnd );
            aTempTranStream->SetSentRTPPackets( EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::BuildRrSection()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::BuildRrSection( TInt aNumRcvStreams,
                                  TBool aRxActive[KMaxNumActiveRcvStreams],
                                  TInt aLastReport )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    for ( TInt k = 0; k < aNumRcvStreams; k++ )
        {
        if ( aRxActive[k] && RcvStreamReceivedRtpPacket( k ) )
            {
            // get stream with the indicated index value
            CRtpRecvStream* tempRcvStream = GetRcvStreamByIndex( k );
            
            RTCP_DEBUG_DETAIL( "SEND: Sent RR packet" );
            
            
            tempRcvStream->RtpStreamCreateRtcpReportSection( iPktRtcpSnd );
            // do not set the receive stream until this is the last RTCP
            // report to be sent
            if ( !aLastReport )
                {
                tempRcvStream->SetReceivedRTPPackets( EFalse );
                }
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CRtpSession::BuildEmptyRr()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::BuildEmptyRr( CRtpTranStream* aTempTranStream )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    initParam.TRTCP_HEADER.pt = ERTCP_RR;
    initParam.TRTCP_HEADER.sourceCount = 0;
    initParam.TRTCP_HEADER.length = 1;

    if ( aTempTranStream == NULL ) //No Tx stream
        {
        streamParam.TRTCP_HEADER.SSRC = iDefaultSSRC;
        }
    else
        {
        streamParam.TRTCP_HEADER.SSRC = aTempTranStream->GetLocalSSRC();
        }

    iPktRtcpSnd->SetType( ERTCP_HEADER );
    iPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam );
    
    
    RTCP_DEBUG_DETAIL_DVALUE( "SEND: Sent EMPTY RR report sender SSRC = ", 
                  streamParam.TRTCP_HEADER.SSRC );
    
    }

// ---------------------------------------------------------------------------
// CRtpSession::BuildSdesSection()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::BuildSdesSection( CRtpTranStream* aTempTranStream )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TBool fCNameOnly( EFalse );

    // start counter, when it equals 5, send full SDES information,
    // other wise send CNAME only
    iSendSdesCounter++;

    // create SDES packet
    if ( iSendSdesCounter >= KSDES_SEND_PACKET )
        {
        fCNameOnly = EFalse;
        
        
        RTCP_DEBUG_DETAIL( "SEND: Sent FULL SDES packet" );
        
        
        iSendSdesCounter = 0;

        if ( !aTempTranStream ) //No Tx stream
            {
            CreateSDES( fCNameOnly, iDefaultSSRC );
            }
        else
            {
            CreateSDES( fCNameOnly, aTempTranStream->GetLocalSSRC() );
            }
        }
    else
        {
        fCNameOnly = ETrue;
        
        
        RTCP_DEBUG_DETAIL( "SEND: Sent only CNAME SDES packet" );
        
        
        if ( !aTempTranStream ) //No Tx stream
            {
            CreateSDES( fCNameOnly, iDefaultSSRC );
            }
        else
            {
            CreateSDES( fCNameOnly, aTempTranStream->GetLocalSSRC() );
            }
        }
    

    if ( !aTempTranStream ) //No Tx stream
        {
        RTCP_DEBUG_DETAIL( "SEND: SENDING NEW RTCP REPORT ONLY RR" );
        
        }
    else
        {
        RTCP_DEBUG_DETAIL_DVALUE( "SEND: SENDING NEW RTCP REPORT SR and RR(?) for TX Stream ID = ", 
                      aTempTranStream->GetStreamID() );
        
        }
    }

// ---------------------------------------------------------------------------
// CRtpSession::SendRTCPReport()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::SendRTCPReport()
    {
    if ( !iRtcpEnabled )
        {
        return;
        }
    if ( iCommNet->Send( ERTCPPort, iPktRtcpSnd->Des() ) == KErrDisconnected )
        {
        RTCP_DEBUG_DETAIL( "Fail to send RTCP packet due to network disconnected" );
       
        ( void ) StopRtcpSending();
        }

    // update RTCP average packet size
    AverageRtcpSize( iPktRtcpSnd->Size() );
    iPktRtcpSnd->RtpPacketReset();
    }

// ---------------------------------------------------------------------------
// CRtpSession::CreateSDES()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::CreateSDES( TInt aFCNameOnly, TRtpSSRC aSSRC )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TInt paddingSize = 0;

    // build RTCP SDES packet header

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam initParam;

    TInt theIndex = 0;
    TInt sourceCount = 1;
    TInt length = 0;
    TInt sdesCount = 0;

    initParam.TRTCP_HEADER.pt = ERTCP_SDES;

    // get number of chunks in this SDES report
    if ( aFCNameOnly )
        {
        // send only the CNAME information
        for ( theIndex = 0; theIndex <= ERTCP_SDES_CNAME; theIndex++ )
            {
            if ( iLocalSdes->iSDESItemsSize[theIndex] != 0 )
                {
                sdesCount++;
                }
            // calculate size of all SDES items in bytes
            length += iLocalSdes->iSDESItemsSize[theIndex];
            }
        }
    else
        {
        // send FULL SDES information
        for ( theIndex = 0; theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
            {
            if ( iLocalSdes->iSDESItemsSize[theIndex] != 0 )
                {
                sdesCount++;
                }
            // calculate size of all SDES items in bytes
            length += iLocalSdes->iSDESItemsSize[theIndex];
            }
        }

    // sdesCount*2 is the bytes for 8-bit type field and octet count
    if ( ( sdesCount * 2 + length + 1 ) % 4 == 0 )
    {
        // multiples of 4 bytes, no padding is needed
        // Calcultate total number of 32 bit words in SDES packet,
        // including end of list null octet
        length = ( sdesCount * 2 + length + 1 ) / 4 + sourceCount;
        }
    else
        {
        // not multiples of 4 bytes, padding is needed
        paddingSize = 4 - ( sdesCount * 2 + length + 1 ) % 4;
        length = ( sdesCount * 2 + length + 1 ) / 4 + 1 + sourceCount;
        }

    initParam.TRTCP_HEADER.sourceCount = sourceCount;
    initParam.TRTCP_HEADER.length = length;

    iPktRtcpSnd->SetType( ERTCP_HEADER );
    iPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam );

    streamParam.TRTCP_SDES.SSRC = aSSRC;

    if ( aFCNameOnly )
        {
        for ( theIndex = 0; theIndex <= ERTCP_SDES_CNAME; theIndex++ )
            {
            initParam.TRTCP_SDES.sdesItems[theIndex] =
                iLocalSdes->iSDESItems[theIndex];
            initParam.TRTCP_SDES.sdesItemsSize[theIndex] =
                iLocalSdes->iSDESItemsSize[theIndex];
            }
        
        // set other sdes item sizes to zero
        for ( theIndex = ERTCP_SDES_CNAME + 1;
              theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
            {
            initParam.TRTCP_SDES.sdesItemsSize[theIndex] = 0;
            }
        }
    else
        {
        for ( theIndex = 0; theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
            {
            initParam.TRTCP_SDES.sdesItems[theIndex] =
                iLocalSdes->iSDESItems[theIndex];
            initParam.TRTCP_SDES.sdesItemsSize[theIndex] =
                iLocalSdes->iSDESItemsSize[theIndex];
            }
        }

    initParam.TRTCP_SDES.paddingSize = paddingSize; 
    iPktRtcpSnd->SetType( ERTCP_SDES );
    iPktRtcpSnd->RtpPacketBuild( &streamParam, &initParam ); 
    
    RTCP_DEBUG_DETAIL( "SEND: Sending SDES message as" );
    for ( TInt k = 0; k <= ERTCP_SDES_CNAME; k++ )
        {
        RTCP_DEBUG_DETAIL( initParam.TRTCP_SDES.sdesItems[k] );
        }
    
    
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessRTCPReportSection()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessOneRTCPReportSectionL(
                              const TRtpPacketIOParam& aExtractParam,
                              TRtpSSRC aSourceSSRC )
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpRtcpEnum parseResult( ERTCP_NO_ERROR );
    iPktRtcpRcv->SetType( aExtractParam.TRTCP_HEADER.pt );

    // handle compound packets
    switch( iPktRtcpRcv->Type() )
        {
        case ERTCP_SDES:
            parseResult = ProcessSDESSectionL( aSourceSSRC );
            
            
            RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Parsed SDES Info source SSRC ", aSourceSSRC );
            
            
            break;

        case ERTCP_BYE:
            parseResult = ProcessBYESectionL();
            
            RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Parsed BYE Info source SSRC ", aSourceSSRC );
            
            
            if ( parseResult == ERTCP_FOUND_RXSTREAM )
                {
                return ERTCP_FOUND_RXSTREAM;
                }
            break;
        
        case ERTCP_APP:
            parseResult = ProcessAPPSectionL( aExtractParam.TRTCP_HEADER.length,
                                    aExtractParam.TRTCP_HEADER.sourceCount );
            
            RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Parsed APP Info source SSRC ", aSourceSSRC );
            
            
            if ( parseResult == ERTCP_FOUND_RXSTREAM )
                {
                return ERTCP_FOUND_RXSTREAM;
                }
            break;

        case ERTCP_SR:
            
             RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Received RTCP_SR, sender SSRC ", aSourceSSRC );
            
            
            parseResult = ProcessSRSectionL(
                              aExtractParam.TRTCP_HEADER.sourceCount );
            break;

        case ERTCP_RR:
            
             RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Received RTCP_RR, sender SSRC ", aSourceSSRC );
            
            
            // update average RTCP report size
            AverageRtcpSize( iPktRtcpRcv->Size() );

            if ( aExtractParam.TRTCP_HEADER.sourceCount == 0 )
                {
                // this is for the special case where we have an empty RR
                // section (RC=0)
                
                    RTCP_DEBUG_DETAIL( "RECEIVE: Empty RR Report" );
                
                 if ( iRtcpObserver )
            		{
            		iRtcpObserver->RrReceived( iSessionId, iDefaultSSRC );
            		}
                // expect an SDES after the empty RR
                parseResult = ERTCP_PACKET_MORE;
                }
            else
                {
                // parse all RR reports
                parseResult = ProcessRRSectionL(
                                  aExtractParam.TRTCP_HEADER.sourceCount );
                }
            break;

        default:
            parseResult = ERTCP_PACKET_ERROR;
            break;
        } // switch
    return parseResult;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessRTCPReportL()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessRTCPReportL()
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;
    TRtpRtcpEnum parseResult( ERTCP_NO_ERROR );
    TRtpSSRC sourceSSRC( NULL );
    TInt iPacketLength( 0 );
    TBool bHeader( ETrue );
    TBool bContinue = ETrue;
    
    // set packet pointer to the beginning
    iPktRtcpRcv->RtpPacketResetPtr();

    /* All RTCP reports are compound packets, consisting of at least:
     * - an SR section (potentially containing RR sections) or an RR section
     * - an SDES
     * In addition, the packet may contain one APP or BYE section.
     * This loop processes one section at a time.
     */
    while ( bContinue )
        {
        // parse RTCP packet header    
        iPktRtcpRcv->SetType( ERTCP_HEADER );
        // Get SSRC of packet sender for this report
        // (This operation does not move the packet pointer)
        sourceSSRC = iPktRtcpRcv->RtpPacketGetSSRC(); 
        parseResult = iPktRtcpRcv->RtpPacketProcessL(
                          &streamParam, &extractParam );

        if ( parseResult == ERTCP_PACKET_ERROR ||
             parseResult == ERTCP_NO_ERROR )
            {
            return parseResult;
            }
        
        /**
         * The payload type field of the first RTCP packet in a compound packet
         * must be equal to SR or RR
         **/
        if( bHeader && ( extractParam.TRTCP_HEADER.pt != ERTCP_SR && 
                         extractParam.TRTCP_HEADER.pt != ERTCP_RR ) )
            {
            return ERTCP_PACKET_ERROR;
            }
            
        if( bHeader )
            {
            /**
             * The padding bit should be zero for for the first packet of a
             * compound RTCP packet because padding should only be applied,
             * if it is needed, to the last packet [RFC3550]
             **/
            if( extractParam.TRTP.padding != 0 )
                {
                return ERTCP_PACKET_ERROR;
                }
            else
                {
                bHeader = EFalse;
                }
            }
        
        parseResult = ProcessOneRTCPReportSectionL( extractParam, sourceSSRC );

        if ( parseResult == ERTCP_FOUND_RXSTREAM )
            {
            return ERTCP_FOUND_RXSTREAM;
            }
        
        /**
         * The length fields of the individual RTCP packets must add 
         * up to the overall length of the compound RTCP packet as 
         * received. [RFC3550]
         **/
        if ( parseResult != ERTCP_PACKET_MORE )
            {
            TInt iTruePacketLength = iPktRtcpRcv->Size();
            
            iPacketLength +=  ( ( extractParam.TRTCP_HEADER.length + 1 ) * 4 );
            
            if ( iPacketLength != iTruePacketLength )
                {
                return ERTCP_PACKET_ERROR;
                }
            }
        else //if( parseResult != ERTCP_NO_ERROR)
            {
            //Get the packet length and convert the 8 bit value to 32 bit
            iPacketLength +=  ( ( extractParam.TRTCP_HEADER.length + 1 ) * 4 );
            }
          
        
        if ( parseResult == ERTCP_PACKET_ERROR ||
             parseResult == ERTCP_NO_ERROR )
            {
            return parseResult;
            }
        } // for loop
    
    // We should never get here
    return ERTCP_NO_ERROR;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessSRSection()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessSRSectionL( TInt aSourceCount )
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpRtcpEnum parseResult( ERTCP_NO_ERROR );
    CRtpStream* stream;
    TRtpSSRC tempSSRC( NULL );
    TUint streamAddress( 0 );
    TStreamType streamType;
    TInt ret( 0 );

    // update average RTCP report size
    AverageRtcpSize( iPktRtcpRcv->Size() );

    // Get SSRC of SR packet, or SSRC_1 in RR Packet 
    tempSSRC = iPktRtcpRcv->RtpPacketGetSSRC(); 
    ret = MatchSSRCToStream( streamAddress, tempSSRC, streamType );

    if ( ret != KErrNone )
        {
        return ERTCP_PACKET_ERROR;
        }
    if ( streamType == ERxStream )
        {
        stream = ( CRtpRecvStream* ) streamAddress;
        }
    else //ETxStream
        {
        stream = ( CRtpTranStream* ) streamAddress;
        }
    if ( stream == NULL )
        {
        return ERTCP_PACKET_ERROR;
        }
    parseResult = stream->RtpStreamProcessRtcpReportSectionL( iPktRtcpRcv );
    if ( parseResult == ERTCP_PACKET_ERROR ||
         parseResult == ERTCP_NO_ERROR )
        {
        return parseResult;
        }
    
                    
    RTCP_DEBUG_DETAIL( "RECEIVE: Parsing SR Report source SSRC secion" );
    
    
    if ( aSourceCount > 0 )
        {
        iPktRtcpRcv->SetType( ERTCP_RR );
        parseResult = ProcessRRSectionL( aSourceCount );
        }
    return parseResult;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessRRSection()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessRRSectionL( TInt aSourceCount )
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpRtcpEnum parseResult( ERTCP_NO_ERROR );
    CRtpStream* stream;
    TRtpSSRC tempSSRC( NULL );
    TUint streamAddress( 0 );
    TStreamType streamType;
    TBool bExit( EFalse );
    TInt ret( 0 );

    while ( aSourceCount && !bExit )
        {
        // extract SSRC_X of RR packet in SR Report packet
        tempSSRC = iPktRtcpRcv->RtpPacketGetSSRC();
        
        RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Parsing RR Report source SSRC ", tempSSRC );
        
        
        ret = MatchSSRCToStream( streamAddress, tempSSRC, streamType );
        if ( ret != KErrNone )
            {
            return ERTCP_PACKET_ERROR;
            }
    
        if ( streamType == ERxStream )
            {
            stream = reinterpret_cast<CRtpRecvStream*>( streamAddress );
            }
        else //ETxStream
            {
            stream = reinterpret_cast<CRtpTranStream*>( streamAddress );
            }
        // If this is an RR, the sender participant is identified by
        // streamParam.TRTCP_HEADER.SSRC
        // DETAIL: Processing RTCP_RR Packet
        parseResult = stream->RtpStreamProcessRtcpReportSectionL( iPktRtcpRcv );
        if ( parseResult == ERTCP_PACKET_ERROR ||
             parseResult == ERTCP_NO_ERROR )
            // parseResult should not be ERTCP_NO_ERROR, as the packet must
            // include an SDES section
            {
            
            RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Error/unexpected end of RTCP packet, sender SSRC ",
                tempSSRC );
            
            
            return parseResult;
            }
  
        aSourceCount--;
    
        if ( aSourceCount == 0 )
            {
            RTCP_DEBUG_DETAIL( "RECEIVE: Parsed Last RR Report" );
            
                    
            bExit = ETrue;
            }
        }
    return parseResult;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessSDESSection()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::DeallocateMemoryForSdes( TRtpPacketIOParam* aParam )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TInt index( 0 );
    if ( aParam == NULL )
        {
        return;
        }
    // deallocate memory for SDES report packet
    for ( index = 0; index < ERTCP_NUM_OF_SDES_ITEMS; index++ )
        {
        if ( aParam->TRTCP_SDES.sdesItems[index] != NULL )
            {
            User::Free( aParam->TRTCP_SDES.sdesItems[index] );
            aParam->TRTCP_SDES.sdesItems[index] = NULL;
            }
        aParam->TRTCP_SDES.sdesItemsSize[index] = 0;
        }
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::GetSDESFromSDESData()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::GetSDESFromSDESData( TRtpSdesParams* aTargetSdes,
                                       const CRtpRecvStream* aRecvStream,
                                       CRtpSDES* aSourceSdes )
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }

    if ( aRecvStream )
        {
        aRecvStream->iRemoteSDES->GetSDES( *aTargetSdes );
        }
    else
        {
        if ( !aSourceSdes )
            {
            return KErrGeneral;
            }
        aSourceSdes->GetSDES( *aTargetSdes );
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::LogSDESData()
// 
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
void CRtpSession::LogSDESData( const TRtpPacketStreamParam& aStreamParam,
                               const CRtpRecvStream* aRecvStream,
                               const CRtpSDES* aSdes,
                               const TBool aSdesSession )
    {
    
    if ( !iRtcpEnabled )
        {
        return;
        }
    
    RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: NEW SDES item for SSRC = ",
              aStreamParam.TRTCP_SDES.SSRC );
    
    if ( !aSdesSession )
        {
        RTCP_DEBUG_DETAIL( "RECEIVE: Receiving NEW SDES items as" );
        for ( TInt k = 0; k < ERTCP_NUM_OF_SDES_ITEMS; k++ )
            {
            RTCP_DEBUG_DETAIL( aRecvStream->iRemoteSDES->iSDESItems[k] );
            }
        }
    else
        {
        RTCP_DEBUG_DETAIL( "RECEIVE: Receiving NEW SDES items as" );
        for ( TInt k = 0; k < ERTCP_NUM_OF_SDES_ITEMS; k++ )
            {
            RTCP_DEBUG_DETAIL( aSdes->iSDESItems[k] );
            }
        }
    
    }

#endif    
// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessSDESData()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessSDESDataL( TRtpPacketStreamParam& aStreamParam,
                                           TRtpPacketIOParam& aExtractParam,
                                           TRtpSSRC aSourceSSRC )
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    CRtpSDES* tempSdes = NULL;
    // Flag to indicate whether SDES is to be assigned to the session
    TBool fSdesSession( EFalse );
    TInt newSDESSenderItemFlag( EFalse );
    TInt err;

    // For finding the receiving stream that belongs to remote participant SSRC
    TStreamType streamType;
    TUint streamAddress( NULL );
    TInt matchResult( KErrNone );
    CRtpRecvStream* tempStream = NULL;
    matchResult = MatchSSRCToStream( streamAddress,
                                     aStreamParam.TRTCP_SDES.SSRC,
                                     streamType );

    // If there are no RX streams, only TX streams, then assign the remote
    // SDES to the session
    if ( matchResult != KErrNone && iNumOfTxStreams && !iNumOfRxStreams )
        {
        TUint tempSdesAdress = NULL;
        TBool result( EFalse );
        TRAP( err, result = AddSdesToArrayL( tempSdesAdress,
                                             &aExtractParam ) );
        if ( result )
            {
            tempSdes = ( CRtpSDES * ) tempSdesAdress;
            TRAP( err, tempSdes->AddSsrcToSdesL( aSourceSSRC ) );

            if ( err )
                return ERTCP_PACKET_ERROR;

            fSdesSession = ETrue;
            // increase number of members in session
            if ( iTotalParticipantsSession <= KRtcpMaximumParticipants )
                {
                iTotalParticipantsSession++;
                }
            }
        }
    else if ( matchResult != KErrNone )
        {
        // did not find a matching stream
        return ERTCP_PACKET_ERROR;
        }
    else  // check stream type
        {
        if ( streamType == ERxStream ) //Rx stream
            {
            tempStream = reinterpret_cast<CRtpRecvStream*>( streamAddress );
            }
        else // ETxStream
            {
            return ERTCP_PACKET_ERROR;
            }
        // update/add SDES in session SDES array
        TUint tempSdesAdress = NULL;
        TBool result( EFalse );
        TRAP( err, result = AddSdesToArrayL( tempSdesAdress,
                                             &aExtractParam ) );
        if ( result )
            {
            tempSdes = reinterpret_cast<CRtpSDES*>( tempSdesAdress );
            TRAP( err, tempSdes->AddSsrcToSdesL( aSourceSSRC ) );

            if ( err )
                return ERTCP_PACKET_ERROR;

            // increase number of members in session
            if ( iTotalParticipantsSession <= KRtcpMaximumParticipants )
                {
                iTotalParticipantsSession++;
                }
            }
        }

    if ( matchResult == KErrNone && !fSdesSession )
        {
        newSDESSenderItemFlag = CompareAndUpdateSdes( tempStream->iRemoteSDES,
                                                      &aExtractParam,
                                                      ETrue /* Update all */ );
        }

    if ( iNewSdes )
        {
        delete iNewSdes;
        iNewSdes = NULL;
        }
    iNewSdes = new ( ELeave ) TRtpSdesParams() ;
    if ( !iNewSdes ) 
        {
        delete iNewSdes;
        iNewSdes = NULL;
        return ERTCP_PACKET_ERROR;
        }

    if ( newSDESSenderItemFlag || fSdesSession )
        {
        if ( fSdesSession )
            {
            GetSDESFromSDESData( iNewSdes, NULL, tempSdes );
            }
        else
            {
            GetSDESFromSDESData( iNewSdes, tempStream, NULL );
            }
        #ifdef _DEBUG 
        LogSDESData( aStreamParam, tempStream, tempSdes, fSdesSession );
        #endif
        }
    else
        {
        // No changes in SDES
        
        RTCP_DEBUG_DETAIL_DVALUE( "NO CHANGES in SDES item for SSRC = ",
                  aStreamParam.TRTCP_SDES.SSRC );
        
        }

    // Finally, let the observer know what we received
    if ( iRtcpObserver )
        {
        iRtcpObserver->SdesReceived( aStreamParam.TRTCP_SDES.SSRC, *iNewSdes );
        }

    return ERTCP_NO_ERROR;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessSDESSectionL()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessSDESSectionL( TRtpSSRC aSourceSSRC )
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;
    TRtpRtcpEnum parseResult( ERTCP_NO_ERROR );
    TRtpRtcpEnum processDataResult( ERTCP_NO_ERROR );
    TRtpRtcpEnum returnValue( ERTCP_NO_ERROR );

    parseResult = iPktRtcpRcv->RtpPacketProcessL( &streamParam, &extractParam );

    if ( parseResult < ERTCP_NO_ERROR )
        {
        returnValue = parseResult;
        }
    else
        {
        processDataResult = ProcessSDESDataL( streamParam, extractParam,
                                             aSourceSSRC );
        if ( processDataResult != ERTCP_NO_ERROR )
            {
            returnValue = processDataResult;
            }
        else
            {
            // parseResult may be ERTCP_NO_ERROR or ERTCP_PACKET_MORE (BYE/APP)
            returnValue = parseResult;
            }
        }

    DeallocateMemoryForSdes( &extractParam );
    return returnValue;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessBYESection()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessBYESectionL()
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;

    TRtpRtcpEnum parseResult = ERTCP_NO_ERROR;

    parseResult = iPktRtcpRcv->RtpPacketProcessL( &streamParam, &extractParam );

    if ( parseResult < ERTCP_NO_ERROR )
        {
        if ( extractParam.TRTCP_BYE.reason != NULL )
            {
            User::Free( extractParam.TRTCP_BYE.reason );
            }
        return ERTCP_PACKET_ERROR;
        }

    // find the receiving stream that belongs to remote participant SSRC
    TStreamType streamType;
    TUint rcvStreamAddress = NULL;
    TRtpId rcvStreamId = 0;
    TInt ret = KErrNone;
    CRtpRecvStream* rcvStream;
    ret = MatchSSRCToStream( rcvStreamAddress, streamParam.TRTCP_BYE.SSRC,
                             streamType );
    if ( streamType == ERxStream )
        {
        rcvStream = reinterpret_cast<CRtpRecvStream*>( rcvStreamAddress );
        }
    else
        {
        if ( extractParam.TRTCP_BYE.reason != NULL )
            {
            User::Free( extractParam.TRTCP_BYE.reason );
            }
        return ERTCP_PACKET_ERROR;
        }
    if ( ret == KErrNone )
        {
        rcvStreamId = rcvStream->GetStreamID();
        TPtrC8 reason( const_cast<const TUint8*>( extractParam.TRTCP_BYE.reason ),
                       extractParam.TRTCP_BYE.reasonSize );
        
        RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Receiving BYE message RX stream ID ", rcvStreamId );
        RTCP_DEBUG_DETAIL( extractParam.TRTCP_BYE.reason );
        
        
        if ( iRtcpObserver )
            {
            iRtcpObserver->ByeReceived( rcvStreamId,
                                        streamParam.TRTCP_BYE.SSRC, reason );
            }

        // Delete the SDES information in the array for this SSRC and decrease
        // number participants in session
        ret = RemoveSdesFromArray( streamParam.TRTCP_BYE.SSRC );
        if ( ret == KErrNone )
            {
            if ( iTotalParticipantsSession > 1 )
                {
                iTotalParticipantsSession--;
                }
            }

        if ( extractParam.TRTCP_BYE.reason != NULL )
            {
            User::Free( extractParam.TRTCP_BYE.reason );
            }
        parseResult = ERTCP_FOUND_RXSTREAM;
        }

    return parseResult;
    }

// ---------------------------------------------------------------------------
// TRtpRtcpEnum CRtpSession::ProcessAPPSection()
// 
// ---------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpSession::ProcessAPPSectionL( TInt aTotalPacketLen,
                                             TInt aSubType )
    {
    if ( !iRtcpEnabled )
        {
        return ERTCP_PACKET_ERROR;
        }

    TRtpPacketStreamParam streamParam;
    TRtpPacketIOParam extractParam;
    TRtpRtcpEnum parseResult = ERTCP_NO_ERROR;

    streamParam.TRTCP_APP.totalPacketLen = aTotalPacketLen;
    parseResult = iPktRtcpRcv->RtpPacketProcessL( &streamParam, &extractParam );

    if ( parseResult < ERTCP_NO_ERROR )
        {
        return ERTCP_PACKET_ERROR;
        }
    // find the receiving stream that belongs to remote participant SSRC
    TStreamType streamType;
    TUint rcvStreamAddress = NULL;
    TRtpId rcvStreamId = 0;
    TInt ret = KErrNone;
    CRtpRecvStream* rcvStream;
    ret = MatchSSRCToStream( rcvStreamAddress, streamParam.TRTCP_APP.SSRC,
                             streamType );
    if ( streamType == ERxStream )
        {
        rcvStream = reinterpret_cast<CRtpRecvStream*>( rcvStreamAddress );
        }
    else
        {
        return ERTCP_PACKET_ERROR;
        }

    if ( ret == KErrNone )
        {
        rcvStreamId = rcvStream->GetStreamID();

        TRtcpApp app;
        app.iSubType = aSubType;
        Mem::Copy( app.iName, extractParam.TRTCP_APP.name,
                   sizeof( TRtcpAppName ) );
        Mem::Copy( app.iAppData, extractParam.TRTCP_APP.appData,
                   extractParam.TRTCP_APP.appDataLen );
        app.iAppDataLen = extractParam.TRTCP_APP.appDataLen;

        if ( iRtcpObserver )
            {
            iRtcpObserver->AppReceived( rcvStreamId,
                                        streamParam.TRTCP_APP.SSRC, app );
            }
        
        RTCP_DEBUG_DETAIL_DVALUE( "RECEIVE: Receiving APP message RX stream ID ", rcvStreamId );
        RTCP_DEBUG_DETAIL( extractParam.TRTCP_APP.appData );
        
        
        User::Free( extractParam.TRTCP_APP.appData );
        parseResult = ERTCP_FOUND_RXSTREAM;
        }

    return parseResult;
    }

// ---------------------------------------------------------------------------
// void CRtpSession::StopRtcpSending()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::StopRtcpSending()
    {
    TInt result( KErrNotSupported );
    if ( iRtcpEnabled )
        {
        iSndRtcpTimer->Cancel();
        iRtcpSendingSuspended = ETrue;
        result = KErrNone;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// void CRtpSession::IsRtcpSendingSuspended()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::IsRtcpSendingSuspended( TBool& aAutoSending )
    {
    TInt result( KErrNotSupported );
    if ( iRtcpEnabled )
        {
        aAutoSending = !iRtcpSendingSuspended;
        result = KErrNone;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// void CRtpSession::ResumeRtcpSending()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::ResumeRtcpSending()
    {
    TInt result( KErrNotSupported );
    if ( iRtcpEnabled )
        {
        iRtcpSendingSuspended = EFalse;
        result = KErrNone;
        if ( iSessionStarted )
            {
            if ( !iSndRtcpTimer->IsActive() )
                {
                iSndRtcpTimer->After( iRtcpTimeInterval *
                                      KMicrosecondPerMillSecond );
                }
            else
                {
                result = KErrInUse;
                }
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CRtpSession::ScheduleRtcpSend()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::ScheduleRtcpSendL()
    {
    if ( iRtcpEnabled && !iRtcpSendingSuspended )
        {
        BuildSendRTCPReportL();
        NextRtcpInterval();
        iSndRtcpTimer->After( iRtcpTimeInterval * KMicrosecondPerMillSecond );
        }
    }

// ---------------------------------------------------------------------------
// CRtpSession::OnExpiredL()
// Rtcp scheduler time out call back 
// ---------------------------------------------------------------------------
//
void CRtpSession::OnExpiredL( TInt /*aStatus*/ )
    {
    if ( iRtcpEnabled )
        {
        ScheduleRtcpSendL();
        }
    }

// ---------------------------------------------------------------------------
// CRtpSession::OnRtpReceivedL()
// Rtp received callback
// ---------------------------------------------------------------------------
//
void CRtpSession::OnRtpReceivedL()
    {
    RTP_DEBUG_DETAIL( "CRtpSession::OnRtpReceivedL Entry" );
    
    TInt ret;
    if ( iStandardRtp )
        {
        if ( iRtpRecvBuf.Size() < KMinRtpHeaderSize )
            {
            // The packet is too short to be valid (min 96 bits in header).
            // Discard and continue to issue packet receiving.
            
            RTP_DEBUG_DETAIL( "Rtp packet too small, discarded" );
            
            
            // if someone is interested receiving non-RTP control packets
            if (iNonRTPDataObserver != NULL)
                {
                // notify user
                iNonRTPDataObserver->NonRTPDataReceived(GetRtpSocket()->LocalPort(), 
                                                ETrue, iRtpRecvBuf);
                }            
            }
        else
            {
        
        	iPktRcv->SetSize( iRtpRecvBuf.Size() );
            
            RTP_DEBUG_DETAIL_DVALUE( "Rtp Packet size = ", iRtpRecvBuf.Size() );
            
        	// Give the packet to each Rx stream until the stream SSRC matches
            // the SSRC of the RTP packet
            
            RTP_DEBUG_DETAIL( "FindRtpRxStream by SSRC" );
                        
        	ret = FindRtpRxStreamL();

        	if ( ret != KErrNone )
            	{
            	// If no RX streams match remote SSRC, then allow any RX stream
                // with Null SSRC to accept RTP packet
             	
            	RTP_DEBUG_DETAIL( "No RX streams match set remote SSRC" );
            	
            	RTP_DEBUG_DETAIL( "Check if any RX stream with Null SSRC to accept RTP packet" );                           
            	ret = AssignRtpRxStreamL();
            	
            	if ( ret != KErrNone )
                    {
                     //FindAnotherSSRC   
                   	
              		if( ret == KErrNotFound )
             			{
             			
            			RTP_DEBUG_DETAIL( "Find another remote SSRC " );
                
        				RTP_DEBUG_DETAIL( "Either is NonRtp data or a new RTP SSRC stream has to be created" );

               			iPktRcv->RtpPacketResetPtr();

			    		TRtpPacketStreamParam streamParam;
			    		TRtpPacketIOParam extractParam;

			    		// process RTP packet
			    		iPktRcv->SetType( ERTP );

			    		extractParam.TRTP.extension.data = NULL;

				    	if ( iPktRcv->RtpPacketProcessL( &streamParam, &extractParam ) < 0 )
				        	{
					        RTP_DEBUG_DETAIL( "Invalid Rtp packet is received" );
					  
					    	if (iNonRTPDataObserver != NULL)
	                        	{
	                        	// notify user
	                        	iNonRTPDataObserver->NonRTPDataReceived(GetRtpSocket()->LocalPort(), 
	                                                ETrue, iRtpRecvBuf);
	                        	}                                
				       	 	}
				       	 else
				       		{
				       		//Valid RTP packet only
				       		RTP_DEBUG_DETAIL( "Create new Recv Stream for valid RTP packet " );
				       		CreateNewRecvStreamL();
			       			}
             		
             			}
             			
			         else
			         	{
			         	// Maybe invalid packet received, continue to receive
                    	// if someone is interested receiving non-RTP control packets
                    
			         	if (iNonRTPDataObserver != NULL)
                        	{
                        	RTP_DEBUG_DETAIL( "NonRtpDataReceived" );
                        	// notify user
                        	iNonRTPDataObserver->NonRTPDataReceived(GetRtpSocket()->LocalPort(), 
                                                ETrue, iRtpRecvBuf);
                        	}                                
			         	}
			         
                    }

                }	
            }
        }
    else
        {
        RTP_DEBUG_DETAIL( "Not standard RTP packets" );
            
        iPktExtRcv->SetSize( iRtpRecvBuf.Size() );
        TUint8* buf = 0;
        TInt length;
        ret = iPktExtRcv->RtpPacketProcess( buf, &length );
        }
    RTP_DEBUG_DETAIL( "CRtpSession::OnRtpReceivedL Exit" );
    
    IssueRtpRecv();    
    }

// ---------------------------------------------------------------------------
// CRtpSession::OnRtcpReceivedL()
// Rtcp received call back
// ---------------------------------------------------------------------------
//
void CRtpSession::OnRtcpReceivedL()
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TRtpRtcpEnum errPacket;
    iPktRtcpRcv->SetSize( iRtcpRecvBuf.Size() );

    errPacket = ProcessRTCPReportL();
    
    // if someone is interested receiving non-RTP control packets
    if (errPacket == ERTCP_PACKET_ERROR && iNonRTPDataObserver != NULL)
        {
        // notify user
        iNonRTPDataObserver->NonRTPDataReceived(GetRtcpSocket()->LocalPort(), EFalse,
                                                iRtcpRecvBuf);
        }
    
    IssueRtcpRecv();

    if ( errPacket == ERTCP_PACKET_ERROR )
        {
        iRtcpErrors++;
        
        
        RTCP_DEBUG_DETAIL_DVALUE( "ERROR: Packet error Session ID = ", iSessionId );
        
        }
    }

// ---------------------------------------------------------------------------
// CRtpSession::OnReceptionError()
// Rtcp reception error call back
// ---------------------------------------------------------------------------
//
void CRtpSession::OnReceptionError( TPortType aPort, TInt aError )
    {
    
    RTP_DEBUG_DETAIL( "CRtpSession::OnReceptionError" );
    

    switch ( aError )
        {
        case KErrDisconnected:
        case KErrConnectionTerminated:
            
            RTP_DEBUG_DETAIL( "Socket Disconnected" );
            
            break;
            
        default:
            if ( aPort == ERTCPPort )
                {
                
                RTCP_DEBUG_DETAIL_DVALUE( "RTCP reception error",
                          aError );
                
                IssueRtcpRecv();
                }
            else
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "RTP reception error",
                          aError );
                
                IssueRtpRecv();
                }
            
            RTP_DEBUG_DETAIL( "Restarting reception" );
            
            break;
        }
    }

// ---------------------------------------------------------------------------
// TRtpSSRC CRtpSession::OnPacketRejected()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::OnPacketRejected(TPortType aPort)
    {
    
    RTP_DEBUG_DETAIL( "OnPacketRejected" );
        
    
    if ( aPort == ERTPPort )
        {
        IssueRtpRecv();        
        }
    else
        {
        IssueRtcpRecv();        
        }
                            
    }
    
// ---------------------------------------------------------------------------
// TRtpSSRC CRtpSession::GenerateSSRC()
// 
// ---------------------------------------------------------------------------
//
TRtpSSRC CRtpSession::GenerateSSRC()
    {
    return static_cast<TUint32>( Random() );
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::AddStream()
// Add Stream object to array
// ---------------------------------------------------------------------------
//
TInt CRtpSession::AddStream( const TStream aArrayID, TStreamType aStreamType )
    {
    if ( aStreamType == ERxStream )
        {
        TRAPD( ret, iStreamRxArray->AppendL( aArrayID ) );
        return ret;
        }
    else //ETxStream
        {
        TRAPD( ret, iStreamTxArray->AppendL( aArrayID ) );
        return ret;
        }
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::FindStream()
// Find Stream Object in array
// ---------------------------------------------------------------------------
//
TInt CRtpSession::FindStream( const TRtpId aMagicKey, TUint& aStreamAddress )
    {
    TKeyArrayFix magicKey( TStream::iOffset, ECmpTUint );
    TInt index = -1;

    TStream match( aMagicKey, 0 );
    if ( ( iStreamRxArray->Find( match, magicKey, index ) == KErrNone )
         && ( index >= 0 ) )
        {
        aStreamAddress = iStreamRxArray->At( index ).GetStreamAddress();
        }
    else if ( ( iStreamTxArray->Find( match, magicKey, index ) == KErrNone )
              && ( index >= 0 ) )
        {
        aStreamAddress = iStreamTxArray->At( index ).GetStreamAddress();
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::FindStream()
// Find Stream Object in array
// ---------------------------------------------------------------------------
//
TInt CRtpSession::FindStream( const TRtpId aMagicKey, TUint& aStreamAddress, 
                              TStreamType& aStreamType )
    {
    TKeyArrayFix magicKey( TStream::iOffset, ECmpTUint );
    TUint temp = 0;
    TInt index = -1;

    TStream match( aMagicKey, temp );
    if ( ( iStreamRxArray->Find( match, magicKey, index ) == KErrNone )
         && ( index >= 0 ) )
        {
        aStreamAddress = iStreamRxArray->At( index ).GetStreamAddress();
        aStreamType = ERxStream;
        }
    else if ( ( iStreamTxArray->Find( match, magicKey, index ) == KErrNone )
               && ( index >= 0 ) )
        {
        aStreamAddress = iStreamTxArray->At( index ).GetStreamAddress();
        aStreamType = ETxStream;
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::RemoveStream()
// Delete Stream object from array
// ---------------------------------------------------------------------------
//
TInt CRtpSession::RemoveStream( const TRtpId aMagicKey,
                                TStreamType& aStreamType )
    {
    TKeyArrayFix magicKey( TStream::iOffset, ECmpTUint );
    TInt index = -1;

    TStream match( aMagicKey );
    if ( ( iStreamRxArray->Find( match, magicKey, index ) == KErrNone )
         && ( index >= 0 ) )
        {
        
        RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: Remove Stream ID = ", 
                  iStreamRxArray->At( index ).GetMagicKey() );
        
        
        CRtpRecvStream* tempStream = reinterpret_cast<CRtpRecvStream*>(
            ( iStreamRxArray->At( index ).GetStreamAddress() ) );
 
        
        aStreamType = ERxStream;
        delete tempStream;
        iStreamRxArray->Delete( index );
        iStreamRxArray->Compress();
        }
    else if ( ( iStreamTxArray->Find( match, magicKey, index ) == KErrNone )
              && ( index >= 0 ) )
        {
        
        RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: Remove Stream ID = ", 
                  iStreamTxArray->At( index ).GetMagicKey() );
        
        
        CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>(
            ( iStreamTxArray->At( index ).GetStreamAddress() ) );
        aStreamType = ETxStream;
        delete tempStream;
        iStreamTxArray->Delete( index );
        iStreamTxArray->Compress();
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpSession::RemoveAllStreams()
// Delete all stream objects from array
// ---------------------------------------------------------------------------
//
void CRtpSession::RemoveAllStreams()
    {
    if ( iStreamRxArray )
		{
		TInt totalRxStream( iStreamRxArray->Count() );
	   
        //Delete all RX Streams starting with the last one
        while( totalRxStream > 0 )
            {
            //totalRxStream - 1 is the last element in the array
            
            RTP_DEBUG_DETAIL_DVALUE("RX STREAM ARRAY: Remove ALL RX Stream ID = ",
                      iStreamRxArray->At( totalRxStream - 1 ).GetMagicKey() );
            
            
            CRtpRecvStream* tempStream = reinterpret_cast<CRtpRecvStream*>(
                ( iStreamRxArray->At( totalRxStream - 1 ).GetStreamAddress() ) );
            delete tempStream;
            
            iStreamRxArray->Delete( totalRxStream - 1 );
            totalRxStream = iStreamRxArray->Count();
            }
            
        iStreamRxArray->Reset();   
        iStreamRxArray->Compress();
   	  	}

	if ( iStreamTxArray )
		{
		TInt totalTxStream( iStreamTxArray->Count() );
	   
	    //Delete all TX Streams starting with the last one 
        while ( totalTxStream > 0 )
            {
            //totalTxStream - 1 is the last element in the array
            
            RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: Remove ALL TX Stream ID = ", 
                      iStreamTxArray->At( totalTxStream - 1 ).GetMagicKey() );
            
            
            CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>(
                ( iStreamTxArray->At( totalTxStream - 1 ).GetStreamAddress() ) );
            delete tempStream;
            
            iStreamTxArray->Delete( totalTxStream - 1 );
            totalTxStream = iStreamTxArray->Count();
            }
             
        iStreamTxArray->Reset();   
        iStreamTxArray->Compress();
	    }
    }
    
// ---------------------------------------------------------------------------
// TInt CRtpSession::FindRtpRxStreamSSRC()
// Find a RX stream that matches RTP packet SSRC
// ---------------------------------------------------------------------------
//
void CRtpSession::FindRtpRxStreamSSRC(TRtpSSRC& aSSRC)
    {
    aSSRC=0;
    TInt totalStream = iStreamRxArray->Count();

    if ( totalStream > 0 )
        {
        for ( TInt index = 0; index < totalStream; index++ )
            {
            CRtpRecvStream* tempStream = ( CRtpRecvStream* )
                ( iStreamRxArray->At( index ).GetStreamAddress() );
            
            iPktRcv->RtpPacketResetPtr();

        	TRtpSSRC aRemoteSSRC( NULL );
    	    iPktRcv->SetType( ERTP );
        	aRemoteSSRC = iPktRcv->RtpPacketGetSSRC();
        	//first if the stream has been assigned else assign it with this packetSSRC
        	if ( tempStream->GetRemoteSSRC() == aRemoteSSRC )
            	{
            	aSSRC=aRemoteSSRC;
            	
            	}
            }
        if (aSSRC==0)
        	{
        	for ( TInt index = 0; index < totalStream; index++ )
            	{
            	CRtpRecvStream* tempStream = ( CRtpRecvStream* )
                	( iStreamRxArray->At( index ).GetStreamAddress() );
            
        		//find stream with emptly ssrc
        		if ( tempStream->GetRemoteSSRC()==NULL )
        			{
	        		iPktRcv->RtpPacketResetPtr();
	        		aSSRC = iPktRcv->RtpPacketGetSSRC();
	        		}
        		}
        	}
        }
     
    }
// ---------------------------------------------------------------------------
// TInt CRtpSession::FindRtpRxStreamL()
// Find a RX stream that matches RTP packet SSRC
// ---------------------------------------------------------------------------
//
TInt CRtpSession::FindRtpRxStreamL()
    {
    TInt ret = KErrNotFound;
    TBool assignRcvStream = EFalse;
    TInt totalStream = iStreamRxArray->Count();

    if ( totalStream > 0 )
        {
        for ( TInt index = 0; index < totalStream; index++ )
            {
            CRtpRecvStream* tempStream = ( CRtpRecvStream* )
                ( iStreamRxArray->At( index ).GetStreamAddress() );
            ret = tempStream->RtpStreamProcessRtpPacketL( iPktRcv,
                                                         assignRcvStream );
            if ( ret == KErrNone )
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: FOUND RX (RTP) stream ID = ",
                          tempStream->GetStreamID() );
                
                
                return ret;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::AssignRtpRxStreamL()
// Assigns a RX stream to the RTP packet SSRC
// ---------------------------------------------------------------------------
//
TInt CRtpSession::AssignRtpRxStreamL()
    {
    RTP_DEBUG_DETAIL( "CRtpSession::AssignRtpRxStream Entry" );
    TInt index;
    TInt ret = KErrNotFound;
    TBool assignRcvStream = ETrue;
    TInt totalStream = iStreamRxArray->Count();

    if ( totalStream > 0 )
        {
        for ( index = 0; index < totalStream; index++ )
            {
            
            RTP_DEBUG_DETAIL_DVALUE(
                      "RX STREAM ARRAY: Assigning RTP to RX stream ID = ",
                      iStreamRxArray->At( index ).GetMagicKey() );
            
            
            CRtpRecvStream* tempStream = reinterpret_cast<CRtpRecvStream*>(
                ( iStreamRxArray->At( index ).GetStreamAddress() ) );
            ret = tempStream->RtpStreamProcessRtpPacketL( iPktRcv,
                                                         assignRcvStream );
            if ( ret == KErrNone )
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: ASSIGNED RX stream SSRC = ",
                          tempStream->GetLocalSSRC() );
                
				RTP_DEBUG_DETAIL( "CRtpSession::AssignRtpRxStream Exit" );
                return ret;
                }
            }
        }
    RTP_DEBUG_DETAIL( "no stream in RTP Sesssion" );
        
    RTP_DEBUG_DETAIL( "CRtpSession::AssignRtpRxStream Exit" );
        
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpSession::MatchSSRCToStream()
// finds a stream for a given source SSRC
// ---------------------------------------------------------------------------
//
TInt CRtpSession::MatchSSRCToStream( TUint& aStreamAddress, 
                                     TRtpSSRC aSSRC, 
                                     TStreamType& aStreamType )
    {
    TInt index;
    TInt totalRxStream = iStreamRxArray->Count();
    TInt totalTxStream = iStreamTxArray->Count();

    if ( totalRxStream > 0 )
        {
        for ( index = 0; index < totalRxStream; index++ )
            {
            TUint streamAddr( iStreamRxArray->At( index ).GetStreamAddress() );
            CRtpRecvStream* tempStream =
                reinterpret_cast<CRtpRecvStream*>( streamAddr );
            if ( tempStream->GetRemoteSSRC() == aSSRC )
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: MATCHED RX stream ID = ",
                          tempStream->GetStreamID() );
                RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: MATCHED RX stream Key = ",
                          iStreamRxArray->At( index ).GetMagicKey() );
                RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: MATCHED RX stream SSRC = ",
                          tempStream->GetLocalSSRC() );
                
                
                aStreamAddress = streamAddr;
                aStreamType = ERxStream;
                return KErrNone;
                }
            }
        }

    if ( totalTxStream > 0 )
        {
        for ( index = 0; index < totalTxStream; index++ )
            {
            TUint streamAddr( iStreamTxArray->At( index ).GetStreamAddress() );
            CRtpTranStream* tempStream =
                reinterpret_cast<CRtpTranStream*>( streamAddr );
            if ( tempStream->GetLocalSSRC() == aSSRC )
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: MATCHED TX stream ID = ",
                          tempStream->GetStreamID() );
                RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: MATCHED TX stream Key = ",
                          iStreamTxArray->At( index ).GetMagicKey() );
                RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: MATCHED TX stream SSRC = ",
                          tempStream->GetLocalSSRC() );
                
                
                aStreamAddress = streamAddr;
                aStreamType = ETxStream;
                return KErrNone;
                }
            }
        }

    return  KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::FindStreamForSSRC()
// Finds if a send stream is already using the given SSRC
// ---------------------------------------------------------------------------
//
TBool CRtpSession::FindStreamForSSRC( TRtpSSRC aSSRC )
    {
    TInt index;
    TInt totalRxStream = iStreamRxArray->Count();
    TInt totalTxStream = iStreamTxArray->Count();

    if ( totalRxStream > 0 )
        {
        for ( index = 0; index < totalRxStream; index++ )
            {
            CRtpRecvStream* tempStream = reinterpret_cast<CRtpRecvStream*>(
                iStreamRxArray->At( index ).GetStreamAddress() );
            if ( tempStream->GetLocalSSRC() == aSSRC )
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "RX STREAM ARRAY: SSRC Being Used = ", aSSRC );
                
                
                return ETrue;
                }
            }
        }

    if ( totalTxStream > 0 )
        {
        for ( index = 0; index < totalTxStream; index++ )
            {
            CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>(
                iStreamTxArray->At( index ).GetStreamAddress() );
            if ( tempStream->GetLocalSSRC() == aSSRC )
                {
               RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: SSRC Being Used = ", aSSRC );
                
                
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::FindTxStreamForSSRC()
// Finds if a send stream is already using the given SSRC
// ---------------------------------------------------------------------------
//
TBool CRtpSession::FindTxStreamForSSRC( TRtpSSRC aSSRC,
                                        CRtpTranStream** streamPointerPointer )
    {
    TInt index( 0 );
    TInt totalTxStream( iStreamTxArray->Count() );

    if ( totalTxStream > 0 )
        {
        for ( index = 0; index < totalTxStream; index++ )
            {
            CRtpTranStream* tempStream = reinterpret_cast<CRtpTranStream*>(
                iStreamTxArray->At( index ).GetStreamAddress() );
            if ( tempStream->GetLocalSSRC() == aSSRC )
                {
                RTP_DEBUG_DETAIL_DVALUE( "TX STREAM ARRAY: SSRC Being Used = ", aSSRC );
                
                if ( streamPointerPointer )
                    {
                    // put the address of the CRtpTranStream object into the
                    // specified pointer
                    *streamPointerPointer = tempStream;
                    }
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::RcvStreamActive()
// checks if a receive stream is active
// ---------------------------------------------------------------------------
//
TBool CRtpSession::RcvStreamActive( TInt aIndex )
    {
    CRtpRecvStream* tempRcvStream = reinterpret_cast<CRtpRecvStream*>(
        iStreamRxArray->At( aIndex ).GetStreamAddress() );
    if ( !tempRcvStream->FirstPkg() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::AnyRcvStreamReceivedRtpPacket()
// checks if any receive stream has received an RTP packet
// ---------------------------------------------------------------------------
//
TBool CRtpSession::AnyRcvStreamReceivedRtpPacket()
    {
    TInt totalRcvStream = iStreamRxArray->Count();
    for ( TInt index = 0; index < totalRcvStream; index++ )
        {
        CRtpRecvStream* tempRcvStream = reinterpret_cast<CRtpRecvStream*>(
            iStreamRxArray->At( index ).GetStreamAddress() );
        if ( tempRcvStream->ReceivedRTPPackets() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// TBool CRtpSession::RcvStreamReceivedRtpPacket()
// checks if a receive stream has received an RTP packet
// ---------------------------------------------------------------------------
//
TBool CRtpSession::RcvStreamReceivedRtpPacket( TInt aIndex )
    {
    CRtpRecvStream* tempRcvStream = reinterpret_cast<CRtpRecvStream*>(
        iStreamRxArray->At( aIndex ).GetStreamAddress() );
    if ( tempRcvStream->ReceivedRTPPackets() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CRtpRecvStream* CRtpSession::GetRcvStreamByIndex()
// gets a receive stream based on an index value
// ---------------------------------------------------------------------------
//
CRtpRecvStream* CRtpSession::GetRcvStreamByIndex( TInt aIndex )
    {
    CRtpRecvStream* tempRcvStream = reinterpret_cast<CRtpRecvStream*>(
        iStreamRxArray->At( aIndex ).GetStreamAddress() );
    return tempRcvStream;
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::DoAddSdesToEmptyArrayL()
// Adds a new SDES object to the SDES array
// ---------------------------------------------------------------------------
//
CRtpSDES* CRtpSession::DoAddSdesToArrayL( TRtpPacketIOParam* aExtractParam )
    {
    if ( !iRtcpEnabled )
        {
        return NULL;
        }

    CRtpSDES* tempSdes = CRtpSDES::NewL();
    CleanupStack::PushL( tempSdes );    
    // check for empty SDES sections
    for ( TInt k = 0; k < ERTCP_NUM_OF_SDES_ITEMS; k++ )
        {
        if ( ( aExtractParam->TRTCP_SDES.sdesItemsSize[k] ) > 0 )
            {
            TRtpUtil::Strcpy( tempSdes->iSDESItems[k],
                              aExtractParam->TRTCP_SDES.sdesItems[k] );
            tempSdes->iSDESItemsSize[k] =
                aExtractParam->TRTCP_SDES.sdesItemsSize[k];
            }
        }
    iSdesArray->AppendL( tempSdes ); 
    CleanupStack::Pop( tempSdes );
    return tempSdes;
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::CompareAndUpdateSdes()
// Compares the specified SDES values with the ones in the parameters and
// updates the SDES where the values differ, starting from the specified index
// ---------------------------------------------------------------------------
//
TBool CRtpSession::CompareAndUpdateSdes( CRtpSDES* aTempSdes,
                                         TRtpPacketIOParam* aExtractParam,
                                         TBool aUpdateCName )
    {
    TBool updatedSdes( EFalse );
    TInt startValue( 0 );
    if ( !aUpdateCName )
        {
        startValue = 1;
        }
    for ( TInt k = startValue; k < ERTCP_NUM_OF_SDES_ITEMS; k++ )
        {
        if ( ( aExtractParam->TRTCP_SDES.sdesItemsSize[k] ) > 0 )
            {
            TPtrC8 s2( _L8( aTempSdes->iSDESItems[k] ) );
            if ( s2.Compare( _L8( aExtractParam->TRTCP_SDES.sdesItems[k] ) )
                != 0 )
                {
                TRtpUtil::Strcpy( aTempSdes->iSDESItems[k],
                                  aExtractParam->TRTCP_SDES.sdesItems[k] );
                aTempSdes->iSDESItemsSize[k] =
                    aExtractParam->TRTCP_SDES.sdesItemsSize[k];
                updatedSdes = ETrue;
                }
            }
        }
    return updatedSdes;
    }

// ---------------------------------------------------------------------------
// TBool CRtpSession::AddSdesToArrayL()
// adds SDES object to SDES array if it is new SDES or updates SDES information
// if it has changed
// ---------------------------------------------------------------------------
//
TBool CRtpSession::AddSdesToArrayL( TUint& aSdesAddress,
                                    TRtpPacketIOParam* aExtractParam )
    {
    if ( !iRtcpEnabled )
        {
        return EFalse;
        }

    TBool fFoundSDES( EFalse );
    TBool fUpdatedSDES( EFalse );
    CRtpSDES* tempSdes = NULL;
    // find if SDES exists or has been changed
    TInt totalSdes( iSdesArray->Count() );

    if ( totalSdes == 0 )  
        {
        // First SDES item, add it to the array
        aSdesAddress =
            reinterpret_cast<TUint>( DoAddSdesToArrayL( aExtractParam ) );
        return ETrue;
        }

    // SDES items exist, check if this one needs to be added
    TBool bExit = EFalse;

    for ( TInt index = 0; index < totalSdes && !bExit; index++ )
        {
        tempSdes = iSdesArray->At( index );
        // assuming CNAME is always included and does not change, 
        // then a new SDES will have a different CNAME
            
        if ( ( aExtractParam->TRTCP_SDES.
               sdesItemsSize[ERTCP_SDES_CNAME] ) == 0 )
            {
            return EFalse;
            }
 
        TPtrC8 s1( _L8( tempSdes->iSDESItems[ERTCP_SDES_CNAME] ) );
        if ( s1.Compare( _L8( aExtractParam->TRTCP_SDES.
             sdesItems[ERTCP_SDES_CNAME] ) ) == 0 )
            {
            fFoundSDES = ETrue;
            bExit = ETrue;
            // Update SDES, but exclude CName 
            fUpdatedSDES = CompareAndUpdateSdes( tempSdes, aExtractParam,
                                                 EFalse );
            }
        }
        
    if ( !fFoundSDES ) // new SDES, must add it to the array
        {
        aSdesAddress =
            reinterpret_cast<TUint>( DoAddSdesToArrayL( aExtractParam ) );
        return ETrue;
        }
    else if ( fUpdatedSDES )
        {
        aSdesAddress = reinterpret_cast<TUint>( tempSdes );
        return EFalse;
        }
    else
        {
        return EFalse;
        }
    } 

// ---------------------------------------------------------------------------
// TInt CRtpSession::RemoveSdesFromArray()
// Removes SDES object from SDES array 
// ---------------------------------------------------------------------------
//
TInt CRtpSession::RemoveSdesFromArray( TRtpSSRC aSSRC )
    {
    if ( !iRtcpEnabled )
        {
        return KErrGeneral;
        }

    TInt totalSdes( iSdesArray->Count() );

    for ( TInt index = 0; index < totalSdes; index++ )
        {
        CRtpSDES* tempSdes = iSdesArray->At( index ); 
        if ( tempSdes->GetSdesSsrc( aSSRC ) )
            {
            delete tempSdes;
            iSdesArray->Delete( index );
            iSdesArray->Compress();
            
            
            RTCP_DEBUG_DETAIL_DVALUE( "SDES ARRAY: Remove SDES Info for SSRC = ", aSSRC );
            
            
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CRtpSession::NextRtcpInterval()
// calculates new RTCP interval which depends on number of
// sender and receiver in session (see RFC 1889 for description)
// ---------------------------------------------------------------------------
//
void CRtpSession::NextRtcpInterval()
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    TUint senders( 0 );
    TUint rtcpBandwidth( static_cast<TUint>( iBandWidth * iFraction ) );
    TUint members( iTotalParticipantsSession );
    
    
    RTCP_DEBUG_DETAIL_DVALUE( "CRtpSession::NextRtcpInterval - total participants  = ", 
              iTotalParticipantsSession );
    
    
    senders = iNumOfRxStreams + iNumOfTxStreams;

    if ( senders > 0 && senders < members * KRtpDefaultSenderBWFrac )
        {
        if ( iNumOfTxStreams )
            {
            rtcpBandwidth = static_cast<TUint>
                            ( rtcpBandwidth * KRtpDefaultSenderBWFrac );
            members = senders;
            }
        else
            {
            rtcpBandwidth = static_cast<TUint>
                            ( rtcpBandwidth * KRtpDefaultObserverBWFrac );
            members -= senders;
            }
        }

    
    RTCP_DEBUG_DETAIL_DVALUE( "CRtpSession::NextRtcpInterval - members  = ", 
              members );

    RTCP_DEBUG_DETAIL_DVALUE( "CRtpSession::NextRtcpInterval - rtcpBandwidth  = ", 
              rtcpBandwidth );
    
    
    // 1000 - milliseconds in one second
    // 8 - bits in one byte
    iRtcpTimeInterval = static_cast<TUint>( ( 1000 * iAverageRtcpSize * 8 * members )
                        / rtcpBandwidth );
    if ( iRtcpTimeInterval < KMinRtcpTimeInterval )
        {
        iRtcpTimeInterval = KMinRtcpTimeInterval;
        }
    TReal rtcpTimeInterval64 = iRtcpTimeInterval * ( Random64() + 0.5 );
    iRtcpTimeInterval = static_cast<TUint>( rtcpTimeInterval64 );
    iRtcpTimeInterval = static_cast<TUint>( iRtcpTimeInterval / KCompensationRtcp );

    
    RTCP_DEBUG_DETAIL_DVALUE( "CRtpSession::NextRtcpInterval - RTCP Interval (ms) = ",
              iRtcpTimeInterval );
    
    }

// ---------------------------------------------------------------------------
// CRtpSession::AverageRtcpSize()
// updates average RTCP report size
// ---------------------------------------------------------------------------
//
void CRtpSession::AverageRtcpSize( TUint aRtcpReportSize )
    {
    if ( !iRtcpEnabled )
        {
        return;
        }

    iAverageRtcpSize = ( aRtcpReportSize ) * KRtcpSizeGain + 
                       ( iAverageRtcpSize * KRtcpSizeGainRemainder );

    
    RTCP_DEBUG_DETAIL_DVALUE( "RTCP: Actual RTCP report size ", aRtcpReportSize );
    RTCP_DEBUG_DETAIL_DVALUE( "RTCP: Average RTCP report size ", (TInt) iAverageRtcpSize );
    
    }

// ---------------------------------------------------------------------------
// CRtpSession::CheckRemoteSsrcL()
// SSRC collision handling
// ---------------------------------------------------------------------------
//
TInt CRtpSession::CheckRemoteSsrcL( TRtpSSRC& aSSRC )
    {
    CRtpTranStream* tranStream;
    TBuf8<15> buf;
    TRtpSSRC ssrc( 0 );
    
    // In case of collision, we change SSRC only once (RFC3550)
    // So if we already jumped once, this function does nothing.
    if ( iSSRCJumps > 0 )
        {
        return KErrNone;
        }
        
    if ( FindTxStreamForSSRC( aSSRC, &tranStream ) )
        {
        // One of our Tx streams uses an SSRC value that collides with the
        // specified value.
        
        RTCP_DEBUG_DETAIL( "RTCP: Jumping to new SSRC" );
        
                        
        // Send a BYE packet for this SSRC
        buf.Format( _L8( "SSRC collision" ) );
        buf.ZeroTerminate();
        ( void ) SendRtcpByePacketL( tranStream->GetStreamID(), buf );
        
        // Find a new unoccupied SSRC
        ssrc = GetUniqueSSRC();
        
        // Take the new SSRC into use
        tranStream->SetLocalSSRC( ssrc );
        if ( iDefaultSSRC == aSSRC )
            {
            iDefaultSSRC = ssrc;
            }
        // Also let the receive stream know the new SSRC
        aSSRC = ssrc;

        // Increment jump counter
        iSSRCJumps++;
        return 1; // a value larger than 0 indicates that the SSRC changed
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpSession::Random()
// Get a random 32-bit number
// ---------------------------------------------------------------------------
//
TInt CRtpSession::Random()
    {
    if ( iSeed == 0 )
        {
        TTime tmp_time;
        tmp_time.HomeTime();
        iSeed = tmp_time.Int64();
        }
    return TRtpUtil::Random( iSeed );
    }

// ---------------------------------------------------------------------------
// CRtpSession::Random64()
// Get a random 64-bit number
// ---------------------------------------------------------------------------
//
TReal CRtpSession::Random64()
    {
    if ( iSeed == 0 )
        {
        TTime tmp_time;
        tmp_time.HomeTime();
        iSeed = tmp_time.Int64();
        }
    return TRtpUtil::FloatRandom( iSeed );
    }
    
// ---------------------------------------------------------------------------
// CRtpSession::SSRC()
// Get Session default ssrc
// ---------------------------------------------------------------------------
//
TRtpSSRC CRtpSession::SSRC() const
    {
    return iDefaultSSRC;
    };

// ---------------------------------------------------------------------------
// CRtpSession::GetSessionId()
// Get Session ID
// ---------------------------------------------------------------------------
//
TRtpId CRtpSession::GetSessionId()
    {
    return iSessionId;
    }

// ---------------------------------------------------------------------------
// CRtpSession::SendRTCPByewithBanedSSRCL()
// 
// ---------------------------------------------------------------------------
//
void CRtpSession::SendRTCPByewithBanedSSRCL()
	{
	TInt count = iStreamTxArray->Count();    

	for ( TInt index( 0 ); index < count; index++ )
		{
	 
		TBuf8<15> buf;    
		CRtpTranStream* tempStream = ( CRtpTranStream* )
 		( iStreamTxArray->At( index ).GetStreamAddress() );
 		//when sending RTCP report will use the localSSRC 
 		//here we want to sendRTCPBye with specific SSRC which baned
 		//so set back the original value at the end
 		TRtpSSRC originSSRC= tempStream->GetLocalSSRC();
 		tempStream->SetLocalSSRC(iPktRcv->RtpPacketGetSSRC());
		buf.Format( _L8( "SSRC baned" ) );
		buf.ZeroTerminate();
		( void ) SendRtcpByePacketL( tempStream->GetStreamID(), buf );
		tempStream->SetLocalSSRC(originSSRC);
		}	
	}

// ---------------------------------------------------------------------------
// CRtpSession::IsSrtp()
// 
// ---------------------------------------------------------------------------
//
TBool CRtpSession::IsSrtp()
	{
	return iIsSrtp;
	}
// ---------------------------------------------------------------------------
// CRtpSession::SendSRTCPReport(TRtpSSRC aSSRC)
// virtual function
// ---------------------------------------------------------------------------
//	
void CRtpSession::SendSRTCPReportL(TRtpSSRC /*aSSRC*/)
	{
	//should not called base class
	User::Leave(KErrNotSupported);
	}

// ---------------------------------------------------------------------------
// CRtpSession::CheckifBanded(TRtpSSRC aSSRC)
// 
// ---------------------------------------------------------------------------
//	
TBool CRtpSession::CheckifBanded()
	{
	TBool idFound = EFalse;
	RTP_DEBUG_DETAIL( "CRtpSession::CheckifBanded Entry" );
	RTP_DEBUG_DETAIL_DVALUE( "iPktRcv->RtpPacketGetSSRC() =", iPktRcv->RtpPacketGetSSRC() ); 
	for ( TInt i=0; ( i<iRxSSRCArray.Count() && !idFound ); i++)
		{
		if( iRxSSRCArray[i] == iPktRcv->RtpPacketGetSSRC())
			{
			idFound = ETrue;
			}
		}
	//baned the packet send RTCP bye
	// Originally it sent here but now it has to be removed 
    // in case of RTCP Bye packet flooding causing by intrusion
    // also in RFC, it is not required.
	RTP_DEBUG_DETAIL( "CRtpSession::CheckifBanded Exit" );
	return idFound;    
	}

// ---------------------------------------------------------------------------
// CRtpSession::CheckRemoteAddr()
// 
// ---------------------------------------------------------------------------
//	
TBool CRtpSession::CheckRemoteAddr()
	{
	TBool isSetRemoteAddr=iCommNet->iReceiver[ERTPPort]->IsSetRemoteAdress();
	return isSetRemoteAddr;
	}

// ---------------------------------------------------------------------------
// CRtpSession::CreateRecvStream(TRtpSSRC aSSRC)
// 
// ---------------------------------------------------------------------------
//		
void CRtpSession::CreateNewRecvStreamL()
	{
	//create new rcvstream
	//Related to RFC 3550 8.2
     
	RTP_DEBUG_DETAIL( "CRtpSession::CreateNewRecvStream entry" );

	if(iCommNet->iReceiver[ERTPPort]->IsSetRemoteAdress())   
		{
		RTP_DEBUG_DETAIL( "Packet comes from set remote address" );

		if (iNumOfRxStreams<= KRtpLimitNumberOfRecvStream)
			{
			TRtpId newStreamId= iAssignUniqueID.AssignUniqueID();      
			if (!iAssignUniqueID.AddStreamToSession(iSessionId, newStreamId))
				{
				// It is valid RTP packet, not checking if stream created
				TRAPD( err, CreateReceiveStreamL( newStreamId, iPktRcv->RtpPacketGetPayloadType() ));
		   		if ( err != KErrNone )
		   			{
		   			RTP_DEBUG_DETAIL( "error occured when tried to create Recv Stream" );
		   			return;
		   			}
		   		RTP_DEBUG_DETAIL( "Assign new Rx stream" );	
		   		AssignRtpRxStreamL();
		   		}
			}
		//Can not create the Recv once it runs to the limit 
		//This is in case of device running out of memory
		RTP_DEBUG_DETAIL( "Can not create new recv stream!" );
		RTP_DEBUG_DETAIL( "The number of Recv streams is over 100" );
		}	
		
	else
		{
		//baned
		RTP_DEBUG_DETAIL( "Band SSRC" );
		iPktRcv->SetType( ERTP );
        TRtpSSRC remoteSSRC( NULL );
        remoteSSRC = iPktRcv->RtpPacketGetSSRC();
        //Add to baned SSRC list
        if (remoteSSRC )
        	{
        	if (!CheckifBanded())
	        	{
	        	//In the future it might also checked with SDES or CSRC
	        	TRAPD( err, iRxSSRCArray.Append(remoteSSRC) ) ;
	        	if ( err != KErrNone )
	        		{
	        		RTP_DEBUG_DETAIL( "Error ocured where to append banded ssrc" );
	        		return;
	        		}
	        	RTP_DEBUG_DETAIL_DVALUE( "Band ssrc =", remoteSSRC );		
	        	SendRTCPByewithBanedSSRCL();
	        	}
	        //abort packet	
        	}
		//abort packet
		}
	RTP_DEBUG_DETAIL( "CRtpSession::CreateNewRecvStream exit" );	
	}
// End of File
	
