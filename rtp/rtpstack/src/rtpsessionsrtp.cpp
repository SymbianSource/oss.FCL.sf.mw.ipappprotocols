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
#include "rtpsessionsrtp.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpSessionSrtp::CRtpSessionSrtp( const TRtpId aSessionId,
                          const TUint32* aProfileRTPTimeRates,
                          const TBool aStandardRtp,
                          MRtpErrNotify& aErrNotify,
                          const CRtpSDES* aSdesInfo , 
                          MRtpAsignUniqueID& aAssignUniqueID,
                          CSRTPSession& aSRTPSession) 
    :CRtpSession( aSessionId,
                  aProfileRTPTimeRates,
                 aStandardRtp, aErrNotify,
                 aSdesInfo, aAssignUniqueID),
     iSRTPSession( aSRTPSession ),
     iSndPacket(NULL),
     iSndRtcpPacket(NULL)
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpSessionSrtp::ConstructL( const TCreateSessionParams& aParams,
                              TUint& aPort,
                              TBool aEnableRtcp,
                              const TRtcpParams* aRtcpParams,
                              RSocketServ& aSocketServ,
                              RConnection& aRConn,
                              const RLibrary& aLibrary,
                              MRtpErrNotify& aErrNotify )
    {
    CRtpSession::ConstructL( aParams, aPort, aEnableRtcp, aRtcpParams, aSocketServ, 
                      aRConn, aLibrary, aErrNotify );
    iIsSrtp=ETrue;                  
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpSessionSrtp* CRtpSessionSrtp::NewL( const TCreateSessionParams& aParams,
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
                                MRtpAsignUniqueID& aAssignUniqueID,
                                CSRTPSession& aSRTPSession )
    {
    CRtpSessionSrtp* self = new ( ELeave ) CRtpSessionSrtp( aSessionId, 
                                                    aProfileRTPTimeRates,
                                                    aStandardRtp, aErrNotify,
                                                    aSdes,aAssignUniqueID,
                                                    aSRTPSession);
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
CRtpSessionSrtp::~CRtpSessionSrtp()
    {
    
 	delete iSndPacket; iSndPacket=NULL;
 	delete iRecvPacket; iRecvPacket=NULL;
 	if(iSndRtcpPacket)
 		delete iSndRtcpPacket; iSndRtcpPacket=NULL;
 	if(iRecvRtcpPacket)
 		delete iRecvRtcpPacket;iRecvRtcpPacket=NULL;
    }
    
                          
// ---------------------------------------------------------------------------
// CRtpSessionSrtp::SendRtpPacket
// ---------------------------------------------------------------------------
//
            
TInt CRtpSessionSrtp::SendRtpPacket( TRtpId aTranStreamId, 
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
 
		if(iSndPacket)
			{
			delete iSndPacket;
			iSndPacket=NULL;
			}
		TRAPD( error, iSndPacket =iSRTPSession.ProtectRTPL(tempStream->GetLocalSSRC(), iPktSnd->Des()));
		if(error==KErrNone)
			{
			return error = iCommNet->Send( ERTPPort, *iSndPacket);
			}
		return error;	
        }
    return ret;
	
	}
                          
// ---------------------------------------------------------------------------
// CRtpSessionSrtp::SendRtpPacket
// ---------------------------------------------------------------------------
//
        
        
TInt CRtpSessionSrtp::SendRtpPacket( TRtpId aTranStreamId,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
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
        if(iSndPacket)
        	{
        	delete iSndPacket;
        	iSndPacket=NULL;
        	}
        	
        TRAPD(error, iSndPacket =iSRTPSession.ProtectRTPL(tempStream->GetLocalSSRC(), iPktSnd->Des()));
        if(error==KErrNone)
       		{
       		iCommNet->Send( ERTPPort, *iSndPacket, aStatus );
       		}
       	return error;	
        }
    return ret;
	}


// ---------------------------------------------------------------------------
// CRtpSessionSrtp::SendRtpPacket
// ---------------------------------------------------------------------------
//

        
TInt CRtpSessionSrtp::SendRtpPacket( TRtpId aTranStreamId,
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
        if(iSndPacket)
        	{
        	delete iSndPacket;
        	iSndPacket=NULL;
        	}
        TRAPD( error, iSndPacket =iSRTPSession.ProtectRTPL(tempStream->GetLocalSSRC(), iPktSnd->Des()));
        if(error==KErrNone)
       		{
        	iCommNet->Send( ERTPPort, *iSndPacket, aStatus );	
        	}
        return error;	
        }
    return ret;
	}

 // ---------------------------------------------------------------------------
// CRtpSessionSrtp::OnRtpReceivedL
// ---------------------------------------------------------------------------
//       
   
void CRtpSessionSrtp::OnRtpReceivedL()
	{
	RTP_DEBUG_DETAIL( "CRtpSessionSrtp::OnRtpReceivedL Entry" );
    	
	TRtpSSRC ssrc(0);
	FindRtpRxStreamSSRC(ssrc);
	if( ssrc!=0 )
		{
		//should always has one recv to match it so ssrc should not be zero
		if( UnprotectRtp(ssrc)==KErrNone )	
			{
			RTP_DEBUG_DETAIL( "Decrypted RTP data succeed" );
			CRtpSession::OnRtpReceivedL();	
			}
		else //packet is not successful decrypted
			{
			RTP_DEBUG_DETAIL( "Decrypted RTP data failed" );
    	//pass as non-rtp data to application instead of droping msg IssueRtpRecv(); 
			//including STUN and FC messages
			if (iNonRTPDataObserver)
            	{
            	// notify user
            	RTP_DEBUG_DETAIL( "Notify decrypted failed RTP data as non rtp data" );
            	iNonRTPDataObserver->NonRTPDataReceived(GetRtpSocket()->LocalPort(), 
                                    ETrue, iRtpRecvBuf);
            	} 
            IssueRtpRecv();  	                          
			}	
		
		}
	else
		{
		//pass as non-rtp data to application
		//including STUN and FC messages
		RTP_DEBUG_DETAIL( "Encrypted incoming Packet SSRC is not found from RX streams" );
    
		if (iNonRTPDataObserver)
            {
            // notify user
            RTP_DEBUG_DETAIL( "Notify non rtp data" );
            	
            iNonRTPDataObserver->NonRTPDataReceived(GetRtpSocket()->LocalPort(), 
                                   ETrue, iRtpRecvBuf);
            }   
        IssueRtpRecv();      
		}
	RTP_DEBUG_DETAIL( "CRtpSessionSrtp::OnRtpReceivedL Exit" );
	}
 
 // ---------------------------------------------------------------------------
// CRtpSessionSrtp::OnRtpReceived
// ---------------------------------------------------------------------------
//       
   
TInt CRtpSessionSrtp::UnprotectRtp(TRtpSSRC ssrc)
	{	
		//should always has one recv to match it so ssrc should not be zero
	if(iRecvPacket)
		{
		delete iRecvPacket;
		iRecvPacket = NULL;	
		}	
	TRAPD( err, iRecvPacket= iSRTPSession.UnprotectRTPL(ssrc, iRtpRecvBuf); );		
	if(err==KErrNone)
		{
		iRtpRecvBuf=*iRecvPacket/*iRecvPacket->Des()*/;		
		}
	
	return err;
	
	}
 
// ---------------------------------------------------------------------------
// CRtpSessionSrtp::SendRTCPReport()
// 
// ---------------------------------------------------------------------------
//
void CRtpSessionSrtp::SendSRTCPReportL(TRtpSSRC aSSRC)
    {
    TInt err= KErrNone;
    if ( !iRtcpEnabled )
        {
        return;
        }
    if (iSndRtcpPacket)
    	{
    	delete iSndRtcpPacket;
    	iSndRtcpPacket=NULL;
    	}
    TRAPD( error, 
    iSndRtcpPacket =iSRTPSession.ProtectRTCPL(aSSRC, iPktRtcpSnd->Des()));    
    
    if ( error==KErrNone && iSndRtcpPacket )
        {
        err =iCommNet->Send( ERTCPPort, *iSndRtcpPacket );
        if (err  == KErrDisconnected)
        		( void ) StopRtcpSending();
        AverageRtcpSize( iSndRtcpPacket->Size() );
        	
        }
    // update RTCP average packet size
    
    iPktRtcpSnd->RtpPacketReset();
    }

 // ---------------------------------------------------------------------------
// CRtpSessionSrtp::OnRtcpReceived
// ---------------------------------------------------------------------------
//       
void CRtpSessionSrtp::OnRtcpReceivedL()
    {
    RTP_DEBUG_DETAIL( "CRtpSessionSrtp::OnRtcpReceivedL Entry" );
    
    if ( !iRtcpEnabled )
        {
        RTP_DEBUG_DETAIL( "No RTCP obs" );
        return;
        }
    TRtpSSRC sourceSSRC( NULL );
    iPktRtcpRcv->SetSize( iRtcpRecvBuf.Size() );

    // set packet pointer to the beginning
    iPktRtcpRcv->RtpPacketResetPtr();

    // parse RTCP packet header    
    iPktRtcpRcv->SetType( ERTCP_HEADER );
    // Get SSRC of packet sender for this report
    // (This operation does not move the packet pointer)
    sourceSSRC = iPktRtcpRcv->RtpPacketGetSSRC(); 
	if( sourceSSRC!=0 )
		{
		//should always has one recv to match it so ssrc should not be zero
		if( UnprotectRtcp(sourceSSRC) == KErrNone )	
			{
			RTP_DEBUG_DETAIL( "Decrypted RTCP successful" );
            	
			CRtpSession::OnRtcpReceivedL();	
			}
		else //packet is not successful decrypted
			{
			if (iNonRTPDataObserver )
				{
				RTP_DEBUG_DETAIL( "Notify decrypted failed RTP data as non rtp data" );
            	
				iNonRTPDataObserver->NonRTPDataReceived(GetRtcpSocket()->LocalPort(), 
												EFalse,
                                                iRtcpRecvBuf);	
				}
			IssueRtcpRecv();
			}	
		
		}
	else
		{
		RTP_DEBUG_DETAIL( "Encrypted incoming Packet SSRC is not found from RX streams" );
    	if (iNonRTPDataObserver )
				{
				iNonRTPDataObserver->NonRTPDataReceived(GetRtcpSocket()->LocalPort(), 
												EFalse,
                                                iRtcpRecvBuf);
				}
		IssueRtcpRecv();		
		}
  
    }
    
// ---------------------------------------------------------------------------
// CRtpSessionSrtp::UnprotectRtcp
// ---------------------------------------------------------------------------
//       
   
TInt CRtpSessionSrtp::UnprotectRtcp(TRtpSSRC ssrc)
	{	
		//should always has one recv to match it so ssrc should not be zero

	if(iRecvRtcpPacket)
		{
		delete iRecvRtcpPacket;	iRecvRtcpPacket=NULL;
		}
	TRAPD( err, iRecvRtcpPacket= iSRTPSession.UnprotectRTCPL(ssrc, *(iPktRtcpRcv->GetHBuf())); );		
	if(err==KErrNone)
		{
		iRtcpRecvBuf=*iRecvRtcpPacket;		
		}
		
	return err;
		
	}
   
