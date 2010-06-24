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
#include <rtpapi.h>
#include <e32std.h>
#include <e32base.h>

#include "rtpmanager.h"

/*****************************************************************************/ 
/****************************** API FUNCTIONS   ******************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpAPI::CRtpAPI( void ) 
    : iManager( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpAPI::ConstructL( MRtpErrNotify& aErrNotify )
    {
    iManager = CRtpManager::NewL( aErrNotify );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CRtpAPI* CRtpAPI::NewL( MRtpErrNotify& aErrNotify )
    {
    CRtpAPI* self = new ( ELeave ) CRtpAPI;

    CleanupStack::PushL( self );
    self->ConstructL( aErrNotify );   
    CleanupStack::Pop();  //self

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpAPI::~CRtpAPI( void )
    {
    delete iManager;
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::OpenL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::OpenL( const TRtpSdesParams& aSdesInfo,
                              const TDesC* aRtpPacketDll,
                              const RSocketServ* aSocketServPtr,
                              const RConnection* aConnPtr )
    {
	RSocketServ* socketServPtr = const_cast<RSocketServ*>( aSocketServPtr );
	RConnection* connPtr       = const_cast<RConnection*>( aConnPtr );
	
    return iManager->OpenL( aSdesInfo, aRtpPacketDll, socketServPtr, connPtr );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::StartConnection()
// Synchronous version.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::StartConnection( TInt aIapId )
    {
    return iManager->StartConnection( aIapId );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::StartConnection()
// Asynchronous version.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::StartConnection( TRequestStatus& aStatus, TInt aIapId )
    {
    return iManager->StartConnection( aStatus, aIapId );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::CancelStart()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::CancelStart()
    {
    iManager->CancelStart();
    }

// ---------------------------------------------------------------------------
// CRtpAPI::Close()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::Close( void )
    {
    iManager->Close();
    }

// ---------------------------------------------------------------------------
// CRtpAPI::SetLocalSdes()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::SetLocalSdes( const TRtpSdesParams& aSdesInfo )
    {
    iManager->SetLocalSdes( aSdesInfo );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::Version()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion CRtpAPI::Version() const
    {
    return TVersion(2, 0, 0);
    }

// ---------------------------------------------------------------------------
// TInetAddr& CRtpAPI::GetLocalIPAddressL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInetAddr& CRtpAPI::GetLocalIPAddressL()
    {
    return iManager->GetLocalIPAddressL();
    }
    
 

/*****************************************************************************/ 
/***************************** SESSION FUNCTIONS  ****************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// TRtpId CRtpAPI::CreateSessionL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TRtpId CRtpAPI::CreateSessionL( const TCreateSessionParams& aSessionParams,
                                         TUint& aPort,
                                         TBool aEnableRtcp,
                                         const TRtcpParams* aRtcpParams )
    {
    return iManager->CreateSessionL( aSessionParams, aPort, aEnableRtcp, aRtcpParams );
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpAPI::CreateSessionL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TRtpId CRtpAPI::CreateSessionL( const TCreateSessionParams& aSessionParams,
                                         TUint& aPort,
                                         TBool aEnableRtcp,
                                         const TRtcpParams* aRtcpParams,
                                         CSRTPSession& aSession )
    {
    return iManager->CreateSessionL( aSessionParams, aPort, aEnableRtcp, aRtcpParams, aSession );
    }


// ---------------------------------------------------------------------------
// TInt CRtpAPI::SetRemoteAddress()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SetRemoteAddress( TRtpId aSessionId, const TInetAddr& aRemoteAddr )
    {
    return iManager->SetRemoteAddress( aSessionId, const_cast< TInetAddr& >( aRemoteAddr ) );
    }


// ---------------------------------------------------------------------------
// TInt CRtpAPI::SetRemoteRtcpAddress()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SetRemoteRtcpAddress( TRtpId aSessionId, const TInetAddr& aRemoteRtcpAddr )
    {
    return iManager->SetRemoteRtcpAddress( aSessionId, const_cast< TInetAddr& >( aRemoteRtcpAddr ) );
    }
    
// ---------------------------------------------------------------------------
// TInt CRtpAPI::StartSession()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::StartSession( TRtpId aSessionId )
    {
    return iManager->StartSession( aSessionId );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::CloseSession()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::CloseSession( TRtpId aSessionId )
    {
    iManager->CloseSession( aSessionId );
    }

// ---------------------------------------------------------------------------
// TRtpId CRTPAPI::GetSessionId()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TRtpId CRtpAPI::GetSessionId( TRtpId aStreamId )
    {
    return iManager->GetSessionId( aStreamId );
    }
    




/*****************************************************************************/ 
/****************************** STREAM FUNCTIONS  ****************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// TRtpId CRtpAPI::CreateReceiveStreamL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TRtpId CRtpAPI::CreateReceiveStreamL( TRtpId aSessionId, 
                                               const TRcvStreamParams& aParams )
    {
    return iManager->CreateReceiveStreamL( aSessionId, aParams );
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpAPI::CreateTransmitStreamL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TRtpId CRtpAPI::CreateTransmitStreamL( TRtpId aSessionId, 
                                                const TTranStreamParams& aParams, 
                                                TRtpSSRC& aSSRC )
    {
    return iManager->CreateTransmitStreamL( aSessionId, aParams, aSSRC );
    }

// ---------------------------------------------------------------------------
// TRtpId CRtpAPI::CreateTransmitStreamExtL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TRtpId CRtpAPI::CreateTransmitStreamExtL( TRtpId aSessionId,
                                                   const TTranStreamParams& aParams,
                                                   const TRtpSSRC aSSRC )
    {
    return iManager->CreateTransmitStreamExtL( aSessionId, aParams, aSSRC );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::CloseStream()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::CloseStream( TRtpId aStreamId )
    {
    iManager->CloseStream( aStreamId );
    }
    
// ---------------------------------------------------------------------------
// TInt CRtpAPI::GetStreamStatistics()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::GetStreamStatistics( TRtpId aStreamId, TRtpPeerStat& aStat )
    {
    return iManager->GetStreamStatistics( aStreamId, aStat );
    }
    


/*****************************************************************************/ 
/****************************** RTP FUNCTIONS  *******************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// TInt CRtpAPI::RegisterRtpObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::RegisterRtpObserver( TRtpId aSessionId, MRtpObserver& aObserver )
    {
    return iManager->RegisterRtpObserver( aSessionId, aObserver );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::UnregisterRtpObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::UnregisterRtpObserver( TRtpId aSessionId )
    {
    iManager->UnregisterRtpObserver( aSessionId );
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SetNonRTPDataObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SetNonRTPDataObserver( TRtpId aSessionId, 
                                    MNonRTPDataObserver* aNonRTPDataObserver )
    {
    return iManager->SetNonRTPDataObserver( aSessionId, aNonRTPDataObserver );    
    }


// ---------------------------------------------------------------------------
// RSocket* CRtpAPI::GetRtpSocket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C RSocket* CRtpAPI::GetRtpSocket( TRtpId aSessionId )
    {
    return iManager->GetRtpSocket( aSessionId );
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtpPacket( TRtpId aTranStreamId,
                                      const TRtpSendHeader& aHeaderInfo,
                                      const TDesC8& aPayloadData )
    {
    return iManager->SendRtpPacket( aTranStreamId, aHeaderInfo, aPayloadData );
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtpPacket( TRtpId aTranStreamId,
                                      TRtpSequence aSequenceNum,
                                      const TRtpSendHeader& aHeaderInfo,
                                      const TDesC8& aPayloadData,
                                      TRequestStatus& aStatus )
    {
    return iManager->SendRtpPacket( aTranStreamId, aSequenceNum, aHeaderInfo, 
                                    aPayloadData, aStatus );
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtpPacket( TRtpId aTranStreamId,
                                      const TRtpSendHeader& aHeaderInfo,
                                      const TDesC8& aPayloadData,
                                      TRequestStatus& aStatus )
    {
    return iManager->SendRtpPacket( aTranStreamId, aHeaderInfo, aPayloadData, aStatus );
    }
	

// ---------------------------------------------------------------------------
// TInt CRtpAPI::RegisterRtpPostProcessingObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::RegisterRtpPostProcessingObserver( TRtpId aSessionId,
                                    MRtpPostProcessingObserver& aRtpPacketObserver )
    {
    return iManager->RegisterRtpPostProcessingObserver( aSessionId, aRtpPacketObserver );
    }


// ---------------------------------------------------------------------------
// CRtpAPI::UnregisterRtpPostProcessingObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::UnregisterRtpPostProcessingObserver( TRtpId aSessionId )
    {
    iManager->UnregisterRtpPostProcessingObserver( aSessionId );
    }   


// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtpPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtpPacket(const TRtpSendPktParams& aSendPktParam , TArray<TRtpCSRC> aCSRCList)
    {
    if(!aSendPktParam.iStatus)
        {
		//SYNC Version
        return iManager->SendRtpPacket( aSendPktParam.iTranStreamId, aSendPktParam.iHeaderInfo, 
                                              aSendPktParam.iPayloadData, &aCSRCList );
        }
    else if (!aSendPktParam.iSequenceNum) //ASYNC Versions With and Without SEQ Number
        {
        return iManager->SendRtpPacket( aSendPktParam.iTranStreamId, aSendPktParam.iHeaderInfo, 
                                              aSendPktParam.iPayloadData, *aSendPktParam.iStatus, &aCSRCList  );
        }
    else
        {
        return iManager->SendRtpPacket( aSendPktParam.iTranStreamId, *aSendPktParam.iSequenceNum, aSendPktParam.iHeaderInfo, 
                                              aSendPktParam.iPayloadData, *aSendPktParam.iStatus, &aCSRCList );
        }
    }


// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendData()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::SendDataL( TRtpId aSessionId,
                                TBool aUseRTPSocket,
                                const TDesC8& aData,
                                TRequestStatus& aStatus )
    {
    iManager->SendDataL( aSessionId, aUseRTPSocket, aData, aStatus );    
    }


// ---------------------------------------------------------------------------
// void CancelSend( TRtpId aSessionId )
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::CancelSend( TRtpId aSessionId )
    {
    iManager->CancelSend( aSessionId );
    }



/*****************************************************************************/ 
/****************************** RTCP FUNCTIONS  ******************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// TInt CRtpAPI::RegisterRtcpObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::RegisterRtcpObserver( TRtpId aSessionId, MRtcpObserver& aObserver )
    {
    return iManager->RegisterRtcpObserver( aSessionId, aObserver );
    }

// ---------------------------------------------------------------------------
// CRtpAPI::UnregisterRtcpObserver()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CRtpAPI::UnregisterRtcpObserver( TRtpId aSessionId )
    {
    iManager->UnregisterRtcpObserver( aSessionId );
    }

// ---------------------------------------------------------------------------
// RSocket* CRtpAPI::GetRtcpSocket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C RSocket* CRtpAPI::GetRtcpSocket( TRtpId aSessionId )
    {
    return iManager->GetRtcpSocket( aSessionId );
    }


// ---------------------------------------------------------------------------
// TInt CRtpAPI::SetRtcpParameters()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SetRtcpParameters( TRtpId aSessionId,
                                          const TRtcpParams& aRtcpParams )
    {
    return iManager->SetRtcpParameters( aSessionId, aRtcpParams );
    }


// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtcpByePacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtcpByePacket( TRtpId aTranStreamId,
                                          const TDesC8& aReason )
    {
    TInt functionValue=KErrNone;
    TRAPD(err, functionValue= iManager->SendRtcpByePacketL( aTranStreamId, aReason ));
    if (err)
    	{
    	return err;	
    	}
    else
    	{
    	return functionValue;
    	}	
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtcpAppPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtcpAppPacket( TRtpId aTranStreamId,
                                          const TRtcpApp& aApp )
    {
    TInt result= KErrNone;
    TRAPD(err, result = iManager->SendRtcpAppPacketL ( aTranStreamId, aApp ));
    if (err)
    	return err;
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtcpSrPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtcpSrPacket( TRtpId aTranStreamId )
    {
    TInt result= KErrNone;
    TRAPD(err, result = iManager->SendRtcpSrPacketL( aTranStreamId ));
    if (err)
    	return err;
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SendRtcpRrPacket()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SendRtcpRrPacket( TRtpId aTranStreamId )
    {
    TInt result= KErrNone;
    TRAPD(err, result = iManager->SendRtcpRrPacketL( aTranStreamId ));
    if (err)
    	return err;
    return result;
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SuspendRtcpSending()
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SuspendRtcpSending( TRtpId aSessionId,
                                           TBool aAutoSending )
    {
    return iManager->SuspendRtcpSending( aSessionId, aAutoSending );
    }
        
// ---------------------------------------------------------------------------
// TInt CRtpAPI::IsRtcpSendingSuspended()
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::IsRtcpSendingSuspended( TRtpId aSessionId,
                                               TBool& aAutoSending )
    {
    return iManager->IsRtcpSendingSuspended( aSessionId, aAutoSending );
    }

/*****************************************************************************/ 
/***************************** SAMPLING FUNCTIONS ****************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// TUint32 CRtpAPI::GetSamplingRate()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CRtpAPI::GetSamplingRate( TUint8 aPayloadType )
    {
    return iManager->GetSamplingRate( aPayloadType );
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::SetSamplingRate()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::SetSamplingRate( TUint8 aPayloadType, TUint32 aSampleRate )
    {
    return iManager->SetSamplingRate( aPayloadType, aSampleRate );
    }

/*****************************************************************************/ 
/****** Internal Use Function - Not In Use, Fix for Raptor *******************/ 
/*****************************************************************************/
// ---------------------------------------------------------------------------
// TInt CRtpAPI::NotInUseSetNonRTPDataObserver()

// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::NotInUseSetNonRTPDataObserver( )
    {
    return KErrNone; 
    }


/*****************************************************************************/ 
/***************************** CUSTOM FUNCTIONS ******************************/ 
/*****************************************************************************/ 
// ---------------------------------------------------------------------------
// TInt CRtpAPI::CustomCommandSync()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::CustomCommandSync( TInt /*aFunction*/,
                                          const TDesC8& /*aInputData1*/,
                                          const TDesC8& /*aInputData2*/,
                                          TDes8& /*aOutputData*/ )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// TInt CRtpAPI::CustomCommandAsync()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRtpAPI::CustomCommandAsync( TInt /*aFunction*/,
                                           const TDesC8& /*aInputData1*/,
                                           const TDesC8& /*aInputData2*/,
                                           TDes8& /*aOutputData*/,
                                           TRequestStatus& /*aStatus*/ )
    {
    return KErrNotSupported;
    }


/*****************************************************************************/ 
/******************************** DLL FUNCTIONS ******************************/ 
/*****************************************************************************/ 
#if !defined ( EKA2 ) && !defined ( RTP_UNIT_TEST )
// ---------------------------------------------------------------------------
// All E32 DLLs need an entry point...
// DLL entry point
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
    {
    return ( KErrNone );
    }
#endif

//  End of File
