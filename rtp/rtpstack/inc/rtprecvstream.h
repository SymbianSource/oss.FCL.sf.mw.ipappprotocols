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




#ifndef RtpRecvStream_H
#define RtpRecvStream_H

#include "rtpstream.h" // parent class
#include "rtpsdes.h" 
#include "rtpstreamdef.h"
    
/**
*  Derived class for RTP receive stream. 
*
*  @lib RtpService.dll
*/
class CRtpRecvStream : public CRtpStream
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
	    //Friend class used for testing
	    friend class CRtpUtRtpStream; 
	    friend class CRtpUtRtpSession; 	    
    #endif 
    public:
        static CRtpRecvStream* NewL( const TRtpPayloadType aPayloadType,
                                     const TRtpId aSessionID,
                                     const TRtpId aRcvStreamID,
                                     MRtpObserver** aRtpObserver,
                                     MRtcpObserver* aRtcpObserver,
                                     const TUint32* aProfileRTPTimeRates,
                                     MSsrcCheckCallback* aCallback );
        ~CRtpRecvStream();

        TInt RtpStreamUpdateParamL( TRtpPacketType pType,
                                   TRtpPacketStreamParam* aStreamParam );

        void SetReceivedRTPPackets( TBool aFlag );
        
        void RegisterRtpObserver(MRtpObserver* aRtpObserver);        
        void UnRegisterRtpObserver();
        

        TInt RtpStreamProcessRtpPacketL( CRtpPacket* aPktRcv,
                                        TBool aAssignStream );

        TBool ReceivedRTPPackets() const;

        TInt GetRemoteStreamInfo( TRtpSdesParams& aSdes );

        TRtpSSRC GetRemoteSSRC();

    public: // from CRtpStream
        TInt ResetStreamStat();
        TInt GetStreamStat( TRtpPeerStat& aStat );
        TRtpRtcpEnum RtpStreamProcessRtcpReportSectionL( CRtpPacket* aPkt );
        void RtpStreamCreateRtcpReportSection( CRtpPacket* aPkt );


    public:
        CRtpSDES* iRemoteSDES;

    private:
        CRtpRecvStream( MSsrcCheckCallback* aCallback,
                        const TRtpPayloadType aPayloadType,
                        const TRtpId aSessionID,
                        const TRtpId aRcvStreamID,
                        MRtpObserver** aRtpObserver,
                        MRtcpObserver* aRtcpObserver,
                        const TUint32* aProfileRTPTimeRates );
        void ConstructL();

        void RtpStreamUpdateJitter( TRtpTimeStamp aCurrentTimeStamp );
        void RtpStreamSyncInfoInit();
        void RtpStreamJitterInit();

    private:
        // flag to indicate if a packet was received since last RTCP packet
        TBool iFlagReceivedRTPPackets;  

        TRtpTimeJitter iSyncJitter;

        // QoS statistics
        TUint32 iNumReceivedPackets;
        TUint32 iNumWrapAround;         // number of wrap around for 

        // backup of reported values for lost fraction calculation
        TUint32 iLastRR_numExpectedPackets;
        TUint32 iLastRR_numReceivedPackets;

        MRtpObserver** iRtpObserver; // RTP receive callback
        TRtpRecvHeader iHdr; // Header of received RTP packet
        TRtpHeaderExtension iHeaderExtension; // Header extension of RTP packet
        MSsrcCheckCallback* iCallback; // callback to CRtpSession
        TRtpSSRC iRemoteSSRC;
     private:    
	    #ifdef EUNIT_TESTING
		  friend class UT_CRtpStream;
		  friend class UT_CRtpSession;
		#endif     	    
            
    };


#endif	// RtpRecvStream_H
