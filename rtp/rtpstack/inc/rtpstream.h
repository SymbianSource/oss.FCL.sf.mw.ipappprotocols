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




#ifndef __RTPSTREAM_H
#define __RTPSTREAM_H

#include <rtpdef.h>
#include "rtputil.h"
#include "rtppacket.h"
#include "rtpstreamdef.h"

/**
*  Base class for RTP stream. 
*
*  @lib RtpService.dll
*/
class CRtpStream : public CBase
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
    // Friend class used for testing
        friend class CRtpUtRtpSession;
    #endif     
    public:
        ~CRtpStream();
        virtual TInt ResetStreamStat() = 0;
        inline TBool FirstPkg() const { return iFlagFirstPkg; };
        inline void SetFirstPkg( TBool aFlg ) { iFlagFirstPkg = aFlg; };
        inline TRtpSSRC GetLocalSSRC() const { return iLocalSSRC; };
        inline void SetLocalSSRC( TRtpSSRC aSSRC ) { iLocalSSRC = aSSRC; };
        inline void RegisterRtcpObserver( MRtcpObserver& aRtcpObserver ) { iRtcpObserver = &aRtcpObserver; };
        inline void UnRegisterRtcpObserver()  { iRtcpObserver = NULL; };
        inline TRtpId GetStreamID() const { return iStreamId; };
        inline void RtcpStats( TRtcpStats& aRtcpStat ) { aRtcpStat = iRtcpStats; }; 
        virtual TInt GetStreamStat( TRtpPeerStat& aStat ) = 0;
        virtual TRtpRtcpEnum RtpStreamProcessRtcpReportSectionL( CRtpPacket* aPkt ) = 0;
        virtual void RtpStreamCreateRtcpReportSection( CRtpPacket* aPkt ) = 0;

    protected:
        CRtpStream( const TRtpId aReceiveStreamId,
                    const TRtpId aSessionId,
                    const TUint32* aProfileRTPTimeRates,
                    MRtcpObserver* aRtcpObserver,
                    const TRtpPayloadType aPayloadType );

    protected: 
        TBool iFlagFirstPkg;
        TRtpSSRC iLocalSSRC;
        TRtpId iStreamId;
        TRtpId iSessionId; // id of session which the stream belongs to

        TRtpSequence iBaseSeqNum;
        TRtpSequence iSeqNum;
        TRtpSequence iSeqNumCycles;
        TRtpTimeStamp iTimeStamp;
        TRtpTimeSync iSyncInfo;          
        TRtcpStats iRtcpStats;
        TRtpPayloadType iPayload;     
        const TUint32* iProfileRTPTimeRates;

        MRtcpObserver* iRtcpObserver; // rtcp callback
     private:    
	    #ifdef EUNIT_TESTING
		  friend class UT_CRtpSession;
		  friend class UT_CRtpStream;
		#endif     	    
    
    };


#endif  // __RTPSTREAM_H

// End of File
