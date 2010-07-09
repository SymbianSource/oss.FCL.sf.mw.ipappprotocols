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
#include "rtpmanager.h"
#include <utf.h>
// CONSTANTS

// Refer to RFC 1889
// The comparison in rates between NTP timestamp and RTP timestamp.
// A value of n means that each increment of an RTP timestamp
// corresponds to n microseconds.
//
const TUint8 KRtpPayloadClockConversions[KRtpMaxPayloadTypes] =
    {
    125, 125, 125, 125, 125, 125, 63, 125, 125, 63, // 0-9 
    23, 23, 125, 125, 11, 125, 91, 45, 125, 0,      // 10-19
    0, 0, 0, 0, 0, 11, 11, 0, 11, 0,                // 20-29
    0, 11, 11, 11, 11, 0, 0, 0, 0, 0,               // 30-39
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 40-49
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 50-59
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 60-69
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 70-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 80-89
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 110-119
    0, 0, 0, 0, 0, 0, 0, 0                          // 120-127
    };

const TInt TArrayStore::iOffset = _FOFF( TArrayStore, iMagicKey );

const TInt KBufLength = 64;
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpManager::CRtpManager( MRtpErrNotify& aErrNotify ) 
    : iStandardRtp( ETrue ), 
      iEnableRtcp( EFalse ),
      iSocketServPtr( NULL ), 
      iConnPtr( NULL ), 
      iNumOfObjects( 0 ),
      iErrNotify( aErrNotify )
    {
    for ( TUint k = 0; k < KRtpMaxPayloadTypes; k++ )
        {
        iProfileRTPTimeRates[k] = ( TUint32 ) KRtpPayloadClockConversions[k];
        }
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpManager::ConstructL()
    {
    // instantiate RTP session array
    iSessionArray = new ( ELeave ) CArrayFixFlat<TArrayStore>( 2 );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpManager* CRtpManager::NewL( MRtpErrNotify& aErrNotify )
    {
    CRtpManager* self = new ( ELeave ) CRtpManager( aErrNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpManager::~CRtpManager( void )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::~CRtpManager" );
    
    Close();
    delete iSessionArray;
    
    if ( iDefaultSdes )
        {
        delete iDefaultSdes;
        iDefaultSdes = NULL;
        }
    
    iSocketServPtr = NULL;
    iConnPtr = NULL;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::OpenL()
// Return error code
// ---------------------------------------------------------------------------
//
TInt CRtpManager::OpenL( const TRtpSdesParams& aSdesInfo, 
                         const TDesC* aRtpPacketDll,
                         RSocketServ* aSocketServPtr,
                         RConnection* aConnPtr )
    {
    RTP_DEBUG_DETAIL( "***********************************" );
    RTP_DEBUG_DETAIL( "***       OPENING NEW API       ***" );
    RTP_DEBUG_DETAIL( "***********************************" );
    RTP_DEBUG_DETAIL( "CRtpManager::OpenL" );
    
    TInt err( KErrNone );
    iDefaultSdes = CRtpSDES::NewL( aSdesInfo );
    

    if ( aSocketServPtr )
        {
        iSocketServPtr = aSocketServPtr;
        }
    else
        {
        iSocketServPtr = &iSocketServ;
        }

    if ( aConnPtr )
        {
        iConnPtr = aConnPtr;
        }
    else
        {
        iConnPtr = &iConn;
        }
    
    //compare string to default DLL names, if string is "", then load default
    if ( !aRtpPacketDll )
        {
        // Use standard RTP
        iStandardRtp = ETrue;
        }
    else
        {
        // Dynamically load the specified DLL
        err = iLibrary.Load( *aRtpPacketDll );
        if ( err )
            {
            RTP_DEBUG_DETAIL( "CRtpManager::OpenL: error loading DLL" );
            User::Leave( err );
            }
        iStandardRtp = EFalse;
        }

    return err;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::StartConnection()
// Starts the connection synchronously
// ---------------------------------------------------------------------------
//
TInt CRtpManager::StartConnection( TInt aIapId )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::StartConnection (sync)" );

    TInt error( PrepareConnection( iPrefs, aIapId ) );
    if ( error != KErrNone )
        {
        return error;
        }

    error = iConnPtr->Start( iPrefs );
    if ( error != KErrNone )
        {
        iConnPtr->Close();
        iSocketServPtr->Close();

        RTP_DEBUG_DETAIL_DVALUE( "Error starting connection: ", error );
        }
    else
        {
        iIapId = aIapId;
        }
    return error;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::StartConnection()
// Starts the connection asynchronously
// ---------------------------------------------------------------------------
//
TInt CRtpManager::StartConnection( TRequestStatus& aStatus, TInt aIapId )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::StartConnection (async)" );

    TInt error( PrepareConnection( iPrefs, aIapId ) );
    if ( error != KErrNone )
        {
        return error;
        }

    iIapId = aIapId;
    iConnPtr->Start( iPrefs, aStatus );
    
    RTP_DEBUG_DETAIL( "Start request sent" );
 
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::PrepareConnection()
// ---------------------------------------------------------------------------
//
TInt CRtpManager::PrepareConnection( TCommDbConnPref& aPrefs, TInt aIapId )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::PrepareConnection"  );
    
    TInt err( KErrCouldNotConnect );

    if ( aIapId != KUseDefaultIap )
        {
	    aPrefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        aPrefs.SetIapId( aIapId );
        }
    
    aPrefs.SetDirection( ECommDbConnectionDirectionOutgoing );

    err = iSocketServPtr->Connect();

    if ( err != KErrNone )
        {
        iSocketServPtr->Close();
        RTP_DEBUG_DETAIL_DVALUE( "Error opening socket server: " , err );
        return err;
        }

    err = iConnPtr->Open( *iSocketServPtr );

    if ( err != KErrNone )
        {
        iConnPtr->Close(); 
        iSocketServPtr->Close();
        RTP_DEBUG_DETAIL_DVALUE( "Error opening connection: " , err );
        return err;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpManager::CancelStart()
// Cancels the asynchoronous start of the connection. 
// ---------------------------------------------------------------------------
//
void CRtpManager::CancelStart()
    {
    if ( iConnPtr )
        {
        iConnPtr->Close(); 
        }
    if ( iSocketServPtr )
        {
        iSocketServPtr->Close();
        }
    }

// ---------------------------------------------------------------------------
// CRtpManager::Close()
// Close rtp manager 
// ---------------------------------------------------------------------------
//
void CRtpManager::Close( void )
    {
    RemoveRtpAllObjects();
    if ( !iStandardRtp )
        {
        iLibrary.Close();
        }
    
    RTP_DEBUG_DETAIL( "CRtpManager::Close -- Closing RConnection"  );
    RTP_DEBUG_DETAIL( "CRtpManager::Close -- Closing Socket Server" );
    
    if ( iConnPtr )
        {
        iConnPtr->Close();
        }
        
    if ( iSocketServPtr )
        {
        iSocketServPtr->Close();
        }
    
    iConnected = EFalse;
    }

// ---------------------------------------------------------------------------
// CRtpManager::SetLocalSdes()
// Set local sdes
// ---------------------------------------------------------------------------
//
void CRtpManager::SetLocalSdes( const TRtpSdesParams& aSdesInfo )
    {
    iDefaultSdes->SetSDES( aSdesInfo );
    }

// ---------------------------------------------------------------------------
// TInetAddr& CRtpManager::GetLocalIPAddressL()
// return local IP address
// ---------------------------------------------------------------------------
//
TInetAddr& CRtpManager::GetLocalIPAddressL()
    {
    RTP_DEBUG_DETAIL( "CRtpManager::GetLocalIPAddress" );
 	
 	TUint32 iapId;
    if ( iIapId == 0 )
        {
        RTP_DEBUG_DETAIL( "Error: IAP not set (=0)" );

        User::Leave( KErrNotSupported );
        }
    if ( iIapId < 0 )
        {
        User::LeaveIfError( GetIapId( iapId ) );
        }
    else
        {
        iapId = static_cast<TUint32>( iIapId );
        }
    CLocalAddrResolver* localAddressResolver = 
        CLocalAddrResolver::NewLC( *iSocketServPtr );
    localAddressResolver->GetLocalAddrL( iLocalAddr, iapId );
    CleanupStack::PopAndDestroy( localAddressResolver );

    return iLocalAddr;
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpManager::CreateSessionL()
// Return session id 
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::CreateSessionL( const TCreateSessionParams& aSessionParams,
                                    TUint& aPort,
                                    TBool aEnableRtcp,
                                    const TRtcpParams* aRtcpParams )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::CreateSessionL" );
        
    TInt err( KErrNone );

    // dont allow following combination: STP with RTCP
    if ((!iStandardRtp) && aEnableRtcp)
        User::Leave(KErrNotSupported);
    
    const TRtpId sessionId = AssignUniqueID();
    iEnableRtcp = aEnableRtcp;
	CheckSdesCName();
    CRtpSession* newSession = CRtpSession::NewL( aSessionParams,
                                                 aPort,
                                                 aEnableRtcp,
                                                 aRtcpParams,
                                                 *iSocketServPtr,
                                                 *iConnPtr,
                                                 sessionId,
                                                 iDefaultSdes,
                                                 iProfileRTPTimeRates,
                                                 iStandardRtp,
                                                 iLibrary,
                                                 iErrNotify,
                                                 *this );

    // add RTP Session to the server list 
    TArrayStore arrayData( sessionId, ( TUint ) newSession, ESession );
    err = AddRtpObject( arrayData );

    if ( err != KErrNone )
        {
        RTP_DEBUG_DETAIL_DVALUE( "Session creation failed with error = ", err );
        
        delete newSession;
        
        return KNullId;
        }
    RTP_DEBUG_DETAIL_DVALUE( "Session created with ID ", sessionId );
    
    return sessionId;
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpManager::CreateSessionL()
// Return session id 
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::CreateSessionL( const TCreateSessionParams& aSessionParams,
                                    TUint& aPort,
                                    TBool aEnableRtcp,
                                    const TRtcpParams* aRtcpParams,
                                    CSRTPSession& aSRTPSession  )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::CreateSessionL" );
        
    TInt err( KErrNone );

    // dont allow following combination: STP with RTCP
    if ((!iStandardRtp) && aEnableRtcp)
        User::Leave(KErrNotSupported);
    
    const TRtpId sessionId = AssignUniqueID();
    iEnableRtcp = aEnableRtcp;
	CheckSdesCName();
    CRtpSessionSrtp* newSession = CRtpSessionSrtp::NewL( aSessionParams,
                                                 aPort,
                                                 aEnableRtcp,
                                                 aRtcpParams,
                                                 *iSocketServPtr,
                                                 *iConnPtr,
                                                 sessionId,
                                                 iDefaultSdes,
                                                 iProfileRTPTimeRates,
                                                 iStandardRtp,
                                                 iLibrary,
                                                 iErrNotify,
                                                 *this,
												 aSRTPSession);

    // add RTP Session to the server list 
    TArrayStore arrayData( sessionId, ( TUint ) newSession, ESession );
    err = AddRtpObject( arrayData );

    if ( err != KErrNone )
        {
        RTP_DEBUG_DETAIL_DVALUE( "Session creation failed with error = ", err );
        
        delete newSession;
        
        return KNullId;
        }
    RTP_DEBUG_DETAIL_DVALUE( "Session created with ID ", sessionId );
    
    return sessionId;
    }


// ---------------------------------------------------------------------------
// TInt CRtpManager::StartSession()
// Start session, including starting rtp receiving and rtcp sending/receiving
// if RTCP is enabled. 
// Return KErrNone if successful; KErrNotFound otherwise
// ---------------------------------------------------------------------------
//
TInt CRtpManager::StartSession( TRtpId aSessionId )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::StartSession entry"  );
    
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        RTP_DEBUG_DETAIL( "CRtpManager::StartSession exit"  );
        return session->StartSession();
        }
    
    RTP_DEBUG_DETAIL( "Session not found" );
    
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CRtpManager::CloseSession()
// Close session. Delete all streams inside the session. 
// ---------------------------------------------------------------------------
//
void CRtpManager::CloseSession( TRtpId aSessionId )
    {
    RTP_DEBUG_DETAIL( "CRtpManager::CloseSession" );
    
    TInt result( RemoveRtpObject( aSessionId, ESession ) );

    if ( result == KErrNone )
        {
        RTP_DEBUG_DETAIL_DVALUE( "CRtpManager::CloseSession, Removed session with ID = ",
                  aSessionId );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::CloseSession, Session not found"  );
        }
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SetRemoteAddress()
// Set remote IP addresses and port numbers for RTP and RTCP. 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SetRemoteAddress( TRtpId aSessionId, TInetAddr& aRemoteAddr )
    {
    CRtpSession* session = GetSession( aSessionId );
    TInt result( KErrNotFound );

    if ( session )
        {
        result = session->SetRemoteAddress( aRemoteAddr );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SetRemoteAddress, Session not found" );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SetRemoteRtcpAddress()
// Set remote IP addresses and port numbers for RTCP. 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SetRemoteRtcpAddress( TRtpId aSessionId, TInetAddr& aRemoteAddr )
    {
    CRtpSession* session = GetSession( aSessionId );
    TInt result( KErrNotFound );

    if ( session )
        {
        result = session->SetRemoteRtcpAddress( aRemoteAddr );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SetRemoteRtcpAddress, Session not found" );
        }
       
    return result;
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpManager::CreateStreamL()
// Create a stream of the specified type in a session. 
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::CreateStreamL( TRtpId aSessionId,
                                   const TCreateStreamType aStreamType,
                                   const TRtpPayloadType aPayloadType,
                                   TRtpSSRC& aSSRC )
    {
    TInt ret( KErrNone );

    RTP_DEBUG_DETAIL( "CRtpManager::CreateStreamL" );

    CRtpSession* session = GetSession( aSessionId );

    if ( !session )
        {
        RTP_DEBUG_DETAIL( "Error: Session not found" );
        return KNullId;
        }

    const TRtpId streamId( AssignUniqueID() ); 

    switch ( aStreamType )
        {
        case ECreateRecvStream:
            ret = session->CreateReceiveStreamL( streamId, aPayloadType );
            ( void ) aSSRC;
            break;

        case ECreateTranStream:
            ret = session->CreateTransmitStreamL( streamId, aPayloadType,
                                                  aSSRC );
            break;
        
        case ECreateTranStreamExt:
            ret = session->CreateTransmitStreamExtL( streamId, aPayloadType,
                                                     aSSRC );
            break;

        default:
            User::Leave( KErrArgument );
            break;
        }

    if ( ret != KErrNone )
        {
        RTP_DEBUG_DETAIL_DVALUE( "CRtpManager::CreateStreamL, create stream error ", ret );
        return KNullId;
        }

    // add stream to the server array along with the session address
    // to which it belongs.
    TArrayStore arrayData( streamId, ( TUint ) session, EStream );
    ret = AddRtpObject( arrayData );

    if ( ret != KErrNone )
        {
        RTP_DEBUG_DETAIL_DVALUE( "CRtpManager::CreateStreamL, stream array error ", ret );
                          
        return KNullId;
        }
    
    RTP_DEBUG_DETAIL_DVALUE( "CreateStreamL: Created stream in session ", aSessionId );
    RTP_DEBUG_DETAIL_DVALUE( "CreateStreamL: The new stream ID is ", streamId );
    
    return streamId;
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpManager::CreateReceiveStreamL()
// Create a receive stream in a session. 
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::CreateReceiveStreamL( TRtpId aSessionId,
                                          const TRcvStreamParams& aParams )
    {
    
    RTP_DEBUG_DETAIL( "Creating RX stream"  );
    

    TRtpSSRC dummy;
    return CreateStreamL( aSessionId,
                          ECreateRecvStream,
                          aParams.iPayloadType,
                          dummy );
    }

// ---------------------------------------------------------------------------
// CRtpManager::CreateTransmitStreamL()
// Create a transmit stream in a session and bring back the SSRC. 
// Return stream id. 
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::CreateTransmitStreamL( TRtpId aSessionId, 
                                           const TTranStreamParams& aParams, 
                                           TRtpSSRC& aSSRC )
    {
    
    RTP_DEBUG_DETAIL( "Creating TX stream" );
   

    return CreateStreamL( aSessionId, ECreateTranStream,
                          aParams.iPayloadType, aSSRC );
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpManager::CreateTransmitStreamExtL
// Create a transmit stream in a session and bring back SSRC. 
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::CreateTransmitStreamExtL( TRtpId aSessionId, 
                                              const TTranStreamParams& aParams,
                                              const TRtpSSRC aSSRC )
    {
    
    RTP_DEBUG_DETAIL( "Creating TX stream (ext)" );
   

    return CreateStreamL( aSessionId, ECreateTranStreamExt,
                          aParams.iPayloadType,
                          const_cast<TRtpSSRC&>( aSSRC ) );
    }

// ---------------------------------------------------------------------------
// CRtpManager::CloseStream()
//
// ---------------------------------------------------------------------------
//
void CRtpManager::CloseStream( TRtpId aStreamId )
    {
    CRtpSession* session = GetSession( aStreamId );

    if ( !session )
        {
        
        RTP_DEBUG_DETAIL("CRtpManager::CloseStream, Session not found" );
      
        return;
        }

    
    RTP_DEBUG_DETAIL_DVALUE( "Close RX/TX stream in session ", session->GetSessionId() );
    RTP_DEBUG_DETAIL_DVALUE( "Close RX/TX stream ID = ", aStreamId );
    
    
    session->CloseStream( aStreamId );

    // remove RTP stream from server array
    ( void ) RemoveRtpObject( aStreamId, EStream );
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::RegisterRtpObserver()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::RegisterRtpObserver( TRtpId aSessionId,
                                       MRtpObserver& aObserver )
    {
    CRtpSession* session = GetSession( aSessionId ); 
    TInt result( KErrGeneral );
    if ( session )
        {
        result = session->RegisterRtpObserver( aObserver );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::RegisterRtpObserver, Session not found" );
        }
    return result;
    }


// ---------------------------------------------------------------------------
// TInt CRtpManager::RegisterRtpPostProcessingObserver()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::RegisterRtpPostProcessingObserver( TRtpId aSessionId, MRtpPostProcessingObserver& aRtpPacketObserver )
    {
    CRtpSession* session = GetSession( aSessionId ); 
    TInt result( KErrGeneral );
    if ( session )
        {
        result = session->RegisterRtpPostProcessingObserver( aRtpPacketObserver );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::RegisterRtpPostProcessingObserver, Session not found" );
        }
    return result;
    }


// ---------------------------------------------------------------------------
// CRtpManager::UnregisterRtpPostProcessingObserver()
//
// ---------------------------------------------------------------------------
//
void CRtpManager::UnregisterRtpPostProcessingObserver( TRtpId aSessionId )
    {
    CRtpSession* session = GetSession( aSessionId ); 

    if ( session )
        {
        session->UnregisterRtpPostProcessingObserver();
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::UnregisterRtpPostProcessingObserver, Session not found" );
        }
    }
    
// ---------------------------------------------------------------------------
// CRtpManager::UnregisterRtpObserver()
//
// ---------------------------------------------------------------------------
//
void CRtpManager::UnregisterRtpObserver( TRtpId aSessionId )
    {
    CRtpSession* session = GetSession( aSessionId ); 

    if ( session )
        {
        session->UnregisterRtpObserver();
        }
    else
        {
        
        RTP_DEBUG_DETAIL( "CRtpManager::UnregisterRtpObserver, Session not found" );
        
        }
    }


// ---------------------------------------------------------------------------
// TInt CRtpManager::SetNonRTPDataObserver()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SetNonRTPDataObserver( TRtpId aSessionId, 
                                    MNonRTPDataObserver* aNonRTPDataObserver )
    {
    TInt ret = KErrNone;
    CRtpSession* session = GetSession( aSessionId ); 

    if ( session )
        {
        ret = session->SetNonRTPDataObserver(aNonRTPDataObserver);
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SetNonRTPDataObserver, Session not found" );
        }
    return ret;
    }


// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtpPacket()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtpPacket( TRtpId aTranStreamId,
                                 const TRtpSendHeader& aHeaderInfo, 
                                 const TDesC8& aPayloadData,
                                 const TArray<TRtpCSRC> *aCsrcList)
    {
    CRtpSession* rtpSession = GetSession( aTranStreamId );

    if ( rtpSession )
        {
        if ( iStandardRtp )
        	{
            if (rtpSession->IsSrtp())
            	{
            	
 		        return static_cast<CRtpSessionSrtp*>
 		        	(rtpSession)->SendRtpPacket( aTranStreamId, aHeaderInfo,
                                             	 aPayloadData, aCsrcList );
 	
            	}
            else
            	{
            	return rtpSession->SendRtpPacket( aTranStreamId, aHeaderInfo,
             		                                 aPayloadData, aCsrcList );
            	}
            }
        else
            {
            return rtpSession->SendRtpPacket( aHeaderInfo, aPayloadData );
        	}
        }
        
    
    RTP_DEBUG_DETAIL( "CRtpManager::SendRtpPacket, Session not found" );
    
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtpPacket()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtpPacket( TRtpId aTranStreamId,
                                 const TRtpSendHeader& aHeaderInfo,
                                 const TDesC8& aPayloadData,
                                 TRequestStatus& aStatus,
                                 const TArray<TRtpCSRC> *aCsrcList)
    {
    CRtpSession* rtpSession = GetSession( aTranStreamId ); 

    if ( rtpSession )
        {
        if ( iStandardRtp )
            {
            if (rtpSession->IsSrtp())
            	{
            	
 		        return static_cast<CRtpSessionSrtp*>
 		        	(rtpSession)->SendRtpPacket( aTranStreamId, aHeaderInfo,
                                              aPayloadData, aStatus, aCsrcList );
 	
            	}
            else
            	{
            	return rtpSession->SendRtpPacket( aTranStreamId, aHeaderInfo,
                                              aPayloadData, aStatus, aCsrcList );
            	}
           
            }
        else
            {
            return rtpSession->SendRtpPacket( aHeaderInfo, aPayloadData,
                                              aStatus );
        	}	
        }
        
    
    RTP_DEBUG_DETAIL( "CRtpManager::SendRtpPacket, Session not found" );
    
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtpPacket()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtpPacket( TRtpId aTranStreamId,
                                 TRtpSequence aSequenceNum,
                                 const TRtpSendHeader& aHeaderInfo,
                                 const TDesC8& aPayloadData,
                                 TRequestStatus& aStatus,
                                 const TArray<TRtpCSRC> *aCsrcList )
    {
    CRtpSession* rtpSession = GetSession( aTranStreamId ); 
    TInt result( KErrNone );
    if ( rtpSession )
        {
	        if ( iStandardRtp )
	            {
            	if (rtpSession->IsSrtp())
	            	{
	            	
	 		        result = static_cast<CRtpSessionSrtp*>
	 		        	(rtpSession)->SendRtpPacket( aTranStreamId, aSequenceNum,
	                                              aHeaderInfo, aPayloadData,
	                                              aStatus, aCsrcList );
	 	
	            	}
	            else
	            	{
	            	result =  rtpSession->SendRtpPacket( aTranStreamId, aSequenceNum,
	                                              aHeaderInfo, aPayloadData,
	                                              aStatus, aCsrcList );
	            	}	            
	            
	            }
	        else
	            {
	            
	            RTP_DEBUG_DETAIL( "CRtpManager::SendRtpPacket, wrong version of function" );
	            result = KErrNotSupported;
            }
        }
    else
        {
        
        RTP_DEBUG_DETAIL( "CRtpManager::SendRtpPacket, Session not found" );
        
        result = KErrNotFound;
        }
    return result;
    }


// ---------------------------------------------------------------------------
// TInt CRtpManager::SendData()
//
// ---------------------------------------------------------------------------
//
void CRtpManager::SendDataL( TRtpId aSessionId,
                       TBool aUseRTPSocket,
                       const TDesC8& aData,
                       TRequestStatus& aStatus )
    {
    CRtpSession* rtpSession = GetSession( aSessionId ); 

    if ( rtpSession )
        {
        if ( iStandardRtp )
            {
            rtpSession->SendData( aUseRTPSocket, aData, aStatus );
            }  
        else
            {
            User::Leave(KErrNotSupported);            
            }
        }
    else
        {
        
        RTP_DEBUG_DETAIL( "CRtpManager::SendData, Session not found" );
        User::Leave(KErrNotFound);
        }
        
    }


// ---------------------------------------------------------------------------
// CRtpManager::CancelSend()
//
// ---------------------------------------------------------------------------
//
void CRtpManager::CancelSend( TRtpId aSessionId )
    {
    CRtpSession* session = GetSession( aSessionId ); 

    if ( session )
        {
        session->CancelSend();
        }
    else
        {
        
        RTP_DEBUG_DETAIL( "CRtpManager::CancelSend, Session not found" );
        }
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::RegisterRtcpObserver()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::RegisterRtcpObserver( TRtpId aSessionId,
                                        MRtcpObserver& aObserver )
    {
    TInt result( KErrGeneral );
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        result = session->RegisterRtcpObserver( aObserver );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::RegisterRtcpObserver, Session not found" );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::UnregisterRtcpObserver()
//
// ---------------------------------------------------------------------------
//
void CRtpManager::UnregisterRtcpObserver( TRtpId aSessionId )
    {
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        session->UnregisterRtcpObserver();
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::UnregisterRtcpObserver, Session not found" );
        }
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtcpByePacket()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtcpByePacketL( TRtpId aTranStreamId,
                                     const TDesC8& aReason )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aTranStreamId );
    
    if ( session )
        {
        result = session->SendRtcpByePacketL( aTranStreamId, aReason );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SendRtcpByePacket, Session not found" );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtcpAppPacketL()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtcpAppPacketL( TRtpId aTranStreamId,
                                     const TRtcpApp& aApp )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aTranStreamId );
    
    if ( session )
        {
        result = session->SendRtcpAppPacketL( aTranStreamId, aApp );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SendRtcpAppPacket, Session not found" );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtcpSrPacketL()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtcpSrPacketL( TRtpId aTranStreamId )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aTranStreamId );
    
    if ( session )
        {
        result = session->SendRtcpSrPacketL( aTranStreamId );
        }
    else
        {
       	RTP_DEBUG_DETAIL( "CRtpManager::SendRtcpSrPacket, Session not found" );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SendRtcpRrPacket()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SendRtcpRrPacketL( TRtpId aRecvStreamId )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aRecvStreamId );
    
    if ( session )
        {
        result = session->SendRtcpRrPacketL( aRecvStreamId );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SendRtcpRrPacket, Session not found" );
       	}
    return result;
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpManager::GetSessionId()
//
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::GetSessionId( TRtpId aStreamId )
    {
    CRtpSession* session = GetSession( aStreamId );

    if ( !session ) 
        { 
        RTP_DEBUG_DETAIL( "CRtpManager::GetSessionId, Session not found" );
        
        return KNullId;
        }
    else
        {
        return session->GetSessionId();
        }
    }

// ---------------------------------------------------------------------------
// RSocket* CRtpManager::GetRtpSocket()
//
// ---------------------------------------------------------------------------
//
RSocket* CRtpManager::GetRtpSocket( TRtpId aSessionId )
    {
    RSocket* socket = NULL;
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        socket = session->GetRtpSocket();
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::GetRtpSocket, Session not found" );
        }
    return socket;
    }

// ---------------------------------------------------------------------------
// RSocket* CRtpManager::GetRtcpSocket()
//
// ---------------------------------------------------------------------------
//
RSocket* CRtpManager::GetRtcpSocket( TRtpId aSessionId )
    {
    RSocket* socket = NULL;
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        socket = session->GetRtcpSocket();
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::GetRtpSocket, Session not found" );
        }
    return socket;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::GetStreamStatistics()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::GetStreamStatistics( TRtpId aStreamId, TRtpPeerStat& aStat )
    {
    TInt result( KErrGeneral );
    CRtpSession* session = GetSession( aStreamId );
    if ( session )
        {
        result = session->GetStreamStatistics( aStreamId, aStat );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::GetStreamStatistics, Session not found" );
    	}
    return result;
    }

// ---------------------------------------------------------------------------
// TUint32 CRtpManager::GetSamplingRate()
//
// ---------------------------------------------------------------------------
//
TUint32 CRtpManager::GetSamplingRate( TRtpPayloadType aPayloadType )
    {
    TUint32 sampleRate;

    if ( aPayloadType >= KRtpMaxPayloadTypes )
        {
        RTP_DEBUG_DETAIL( "CRtpManager::GetSamplingRate, payload type out of range" );
        return 0;
        }

    sampleRate = iProfileRTPTimeRates[aPayloadType];

    if ( sampleRate != 0 )
        {
        sampleRate = KMicrosecondPerSecond / sampleRate;
        }

    return sampleRate;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SetSamplingRate()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SetSamplingRate( TRtpPayloadType aPayloadType,
                                   TUint32 aSampleRate )
    {
    if ( aPayloadType >= KRtpMaxPayloadTypes )
        {
        
        RTP_DEBUG_DETAIL( "CRtpManager::SetSamplingRate, payload type out of range" );
        
        return KErrArgument;
        }

    // update payload table
    if ( aSampleRate != 0 )
        {
        iProfileRTPTimeRates[aPayloadType] =
            KMicrosecondPerSecond / aSampleRate;
        }
    else
        {
        
        RTP_DEBUG_DETAIL( "CRtpManager::SetSamplingRate, sample rate cannot be set to 0" );
        
        return KErrDivideByZero;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SetRtcpParameters()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SetRtcpParameters( TRtpId aSessionId,
                                     const TRtcpParams& aRtcpParams )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        result = session->SetRtcpParameters( aRtcpParams );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SetRtcpParameters, Session not found" );
       	}
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::SuspendRtcpSending()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::SuspendRtcpSending( TRtpId aSessionId,
                                      TBool aAutoSending )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        if ( !aAutoSending )
            {
            // Don't send RTCP reports automatically
            session->StopRtcpSending();
            result = KErrNone;
            }
        else
            {
            // Resume sending RTCP reports automatically
            session->ResumeRtcpSending();
            result = KErrNone;
            }
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::SuspendRtcpSending, Session not found" );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::IsRtcpSendingSuspended()
//
// ---------------------------------------------------------------------------
//
TInt CRtpManager::IsRtcpSendingSuspended( TRtpId aSessionId,
                                          TBool& aAutoSending )
    {
    TInt result( KErrNotFound );
    CRtpSession* session = GetSession( aSessionId );
    if ( session )
        {
        result = session->IsRtcpSendingSuspended( aAutoSending );
        }
    else
        {
        RTP_DEBUG_DETAIL( "CRtpManager::IsRtcpSendingSuspended, Session not found" );
        }
    return result;
    }


// ---------------------------------------------------------------------------
// CRtpSession* CRtpManager::GetSession()
//
// ---------------------------------------------------------------------------
//
CRtpSession* CRtpManager::GetSession( TRtpId aSessionId )
    {
    TUint uintAddress( 0 );
    TInt ret( FindRtpObject( aSessionId, uintAddress ) );
    CRtpSession* session = reinterpret_cast<CRtpSession*>( uintAddress );

    return ( ret != KErrNone ) ? NULL : session;
    }

// ---------------------------------------------------------------------------
// const TRtpId CRtpManager::AssignUniqueID()
// Assigns Unique ID to a RTP session or a stream
// ---------------------------------------------------------------------------
//
TRtpId CRtpManager::AssignUniqueID()
    {
    return ++iNumOfObjects;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::AddRtpObject()
// Add Object to array
// ---------------------------------------------------------------------------
//
TInt CRtpManager::AddRtpObject( TArrayStore aArrayID )
    {
    TRAPD( ret, iSessionArray->AppendL( aArrayID ) );
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::FindRtpObject()
// Find Object in array
// ---------------------------------------------------------------------------
//
TInt CRtpManager::FindRtpObject( const TRtpId aMagicKey,
                                 TUint& aSessionAddress )
    {
    TKeyArrayFix magicKey( TArrayStore::iOffset, ECmpTUint );
    TInt theIndex = -1;
    TArrayStore match( aMagicKey );
    if ( iSessionArray->Find( match, magicKey, theIndex ) == KErrNone
         && theIndex >= 0 )
        {
        aSessionAddress = iSessionArray->At( theIndex ).GetSessionAddress();
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpManager::RemoveRtpObject()
// Delete Object TArrayStore object from array
// ---------------------------------------------------------------------------
//
TInt CRtpManager::RemoveRtpObject( const TRtpId aMagicKey,
                                   const TObjectType aObjectType )
    {
    TKeyArrayFix magicKey( TArrayStore::iOffset, ECmpTUint );
    TInt theIndex = -1;
    CRtpSession* tempSession;
    TArrayStore match( aMagicKey );

    if ( iSessionArray->Find( match, magicKey, theIndex ) != KErrNone )
        {
        return KErrNotFound;
        }
    if ( theIndex < 0 )
        {
        return KErrNotFound;
        }

    if ( aObjectType == ESession )
        {
        TUint sessionAddr = iSessionArray->At( theIndex ).GetSessionAddress();
        tempSession = reinterpret_cast<CRtpSession*>( sessionAddr );

        // delete all streams in session array for this session
        TInt totalObjects = iSessionArray->Count();
        
        
        RTP_DEBUG_DETAIL_DVALUE( "SESSION ARRAY: Closing Session with ID ", 
                  iSessionArray->At( theIndex ).GetMagicKey() );
        
        
        for ( TInt k = 0; k < totalObjects; k++ )
            {
            if ( ( iSessionArray->At( k ).GetObjectType() == EStream ) &&
                 ( iSessionArray->At( k ).GetSessionAddress() == sessionAddr ) )
                {
                
               	RTP_DEBUG_DETAIL_DVALUE( "SESSION ARRAY: Stream found in session ",
                          tempSession->GetSessionId() );
                RTP_DEBUG_DETAIL_DVALUE( "SESSION ARRAY: Removed stream with ID ",
                          iSessionArray->At( k ).GetMagicKey() );
                
                iSessionArray->Delete( k );
                totalObjects = iSessionArray->Count();
                k--;
                }
            }
        delete tempSession;
        }
    else // EStream
        {
        RTP_DEBUG_DETAIL_DVALUE( "SESSION ARRAY: Removed Stream with ID ", 
                  iSessionArray->At( theIndex ).GetMagicKey() );
        }
    
    iSessionArray->Delete( theIndex );
    iSessionArray->Compress();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CRtpManager::RemoveRtpAllObjects()
// Delete all Objects from array
// ---------------------------------------------------------------------------
//
void CRtpManager::RemoveRtpAllObjects()
    {
    
   	RTP_DEBUG_DETAIL( "SESSION ARRAY: Closing all sessions" );
   
    if ( iSessionArray )
	    {
	    TInt totalObjects( iSessionArray->Count() );
        for ( TInt index = 0; index < totalObjects; index++ )
            {
            if ( iSessionArray->At( index ).GetObjectType() == ESession )
                {
                
                RTP_DEBUG_DETAIL_DVALUE( "SESSION ARRAY: Closing session with ID ", 
                          iSessionArray->At( index ).GetMagicKey() );
                
                    
                CRtpSession* tempSession = reinterpret_cast<CRtpSession*>(
                    ( iSessionArray->At( index ).GetSessionAddress() ) );
                delete tempSession; 
                }
            }

	    iSessionArray->Reset();
	    iSessionArray->Compress();
        }
    }

// ---------------------------------------------------------------------------
// CRtpManager::GetIapId()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::GetIapId( TUint32& aIapId )
	{
    TInt result( KErrNone );
	TUint count( 0 );
	TConnectionInfoBuf info;

    RTP_DEBUG_DETAIL( "CRtpManager::GetIapId()" );
    
    if ( !iConnPtr )
        {
        RTP_DEBUG_DETAIL( "Error: Connection not found"  );
 	    result = KErrNotReady;
        }
    if ( result == KErrNone )
        {
    	result = iConnPtr->EnumerateConnections( count );
        }
    if ( result == KErrNone )
        {
    	if ( count > 0 )
	    	{
    		result = iConnPtr->GetConnectionInfo( count, info );
            if ( result == KErrNone )
                {
                aIapId = info().iIapId;
                }
            else
                {
                RTP_DEBUG_DETAIL( "Error: Could not get connection info" );
         	    }
		    }
        else
            {
            RTP_DEBUG_DETAIL( "Error: No active connections found" );
     	    result = KErrNotFound;
            }
        }
    else
        {
        RTP_DEBUG_DETAIL( "Error: Could not enumerate active connections" );
 	    }

    return result;
	}

// ---------------------------------------------------------------------------
// CRtpManager::AddStreamToSession()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpManager::AddStreamToSession(TRtpId aSessionId, 
									TRtpId aStreamId)
	{
	// add stream to the server array along with the session address
    // to which it belongs.
   
    CRtpSession* session = GetSession( aSessionId );
    if (session)
    	{
    	TArrayStore arrayData( aStreamId, ( TUint ) session, EStream );
	    TInt ret = AddRtpObject( arrayData );

    	if ( ret != KErrNone )
	        {
	        
	        RTP_DEBUG_DETAIL_DVALUE("CRtpManager::CreateStreamL, stream array error ", ret );
	           
	        return KErrArgument;
	        }
    	return KErrNone;    
		}
	//Session is not found	
	return KErrNotFound;	
    }
	
// ---------------------------------------------------------------------------
// CRtpManager::CheckSdesCName()
// 
// ---------------------------------------------------------------------------
//	
void CRtpManager::CheckSdesCName()
	{
   	TRtpSdesParams aSdes;
	iDefaultSdes->GetSDES(aSdes);
    if (!aSdes.iCName.Length())
    	{
    	//Set CName first in case it could not set CNAME to ip address
    	aSdes.iCName.Set( _L8( "RtpUI user" ) );
    	SetLocalSdes(aSdes);
    	TInetAddr currentIP;
    	TRAPD( error, currentIP = GetLocalIPAddressL() );
    	if ( error == KErrNone && !currentIP.IsUnspecified() )
        	{
        	TBuf<KBufLength> addressbuffer;
        	currentIP.Output( addressbuffer );
        	TBuf8<KBufLength> addr;
        	CnvUtfConverter::ConvertFromUnicodeToUtf8(addr,addressbuffer);
        	if (addr.Compare(KNullDesC8))
        		{
        		aSdes.iCName.Set(addr);
        		SetLocalSdes(aSdes);	
        		}
        	}
    		
		}
	}
//  End of File
