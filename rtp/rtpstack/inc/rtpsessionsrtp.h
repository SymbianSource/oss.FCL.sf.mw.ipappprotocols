/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __RtpSessionSrtp_H
#define __RtpSessionSrtp_H

// INCLUDES
#include <e32base.h>
#include <rtpheader.h>
#include "rtpsession.h"
#include <srtpsession.h>


/**
*  class for rtp session. 
*
*  @lib RtpService.dll
*/
class CRtpSessionSrtp :public CRtpSession
        { 
   
    public: // new functions
        static CRtpSessionSrtp* NewL( const TCreateSessionParams& aParams,
                                  TUint& aPort,
                                  TBool aEnableRtcp,
                                  const TRtcpParams* aRtcpParams,
                                  RSocketServ& aSocketServ,
                                  RConnection& aRConn,
                                  const TRtpId aSessionId,
                                  const CRtpSDES* aSdes,
                                  const TUint32* aProfileRTPTimeRates,
                                  const TBool aStandardRtp,
                                  const RLibrary& aLibrary,
                                  MRtpErrNotify& aErrNotify,
                                  MRtpAsignUniqueID& aAssignUniqueID,
                                  CSRTPSession& aSRTPSession);
                                  
        ~CRtpSessionSrtp();

        TInt SendRtpPacket( TRtpId aTranStreamId, const TRtpSendHeader& aHeaderInfo, 
                            const TDesC8& aPayloadData, const TArray<TRtpCSRC> *aCsrcList = NULL );

        TInt SendRtpPacket( TRtpId aTranStreamId,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
                            const TArray<TRtpCSRC> *aCsrcList = NULL);

        
        TInt SendRtpPacket( TRtpId aTranStreamId,
                            TRtpSequence aSequenceNum,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
                            const TArray<TRtpCSRC> *aCsrcList = NULL );

        //TInt SendRtcpByePacket( TRtpId aTranStreamId, const TDesC8& aReason );
        //TInt SendRtcpAppPacket( TRtpId aTranStreamId, const TRtcpApp& aApp );
        //TInt SendRtcpSrPacket( TRtpId aTranStreamId );
        //TInt SendRtcpRrPacket( TRtpId aRecvStreamId );
		void SendSRTCPReportL(TRtpSSRC aSSRC);
   		TInt StartSession();

    public: // from MReceivedNotify
        void OnRtpReceivedL();
        void OnRtcpReceivedL();
 
    private: // construction
        
        CRtpSessionSrtp( const TRtpId aSessionId,
                     const TUint32* aProfileRTPTimeRates,
                     const TBool aStandardRtp,
                     MRtpErrNotify& aErrNotify,
                     const CRtpSDES* aSdesInfo,
                     MRtpAsignUniqueID& aAssignUniqueID,
                     CSRTPSession& aSRTPSession );

        void ConstructL( const TCreateSessionParams& aParams,
                         TUint& aPort,
                         TBool aEnableRtcp,
                         const TRtcpParams* aRtcpParams,
                         RSocketServ& aSocketServ,
                         RConnection& aRConn,
                         const RLibrary& aLibrary,
                         MRtpErrNotify& aErrNotify );

    	TInt UnprotectRtp(TRtpSSRC ssrc);
    	TInt UnprotectRtcp(TRtpSSRC ssrc);
    private:  
  
         CSRTPSession& iSRTPSession;
    	 HBufC8* iSndPacket;
    	 HBufC8* iRecvPacket;
    	 HBufC8* iSndRtcpPacket;
    	 HBufC8* iRecvRtcpPacket;
    	 
    };

#endif   // __RtpSessionSrtp_H
 
// End of File
