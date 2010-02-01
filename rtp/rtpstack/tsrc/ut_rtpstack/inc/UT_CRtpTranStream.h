/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __UT_CRTPTRANSTREAM_H__
#define __UT_CRTPTRANSTREAM_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "rtpstream.h"
#include "rtprecvstream.h"
#include "rtptranstream.h"

#include "rtppacket.h"
#include "rtpdef.h"
#include "rtppacketparam.h"
#include "rtpsdes.h"
#include "rtpmanager.h"

//  FORWARD DECLARATIONS
class CRtpTranStream;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CRtpTranStream )
     : public CEUnitTestSuiteClass,
     public MRtpObserver,
      public MRtcpObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpTranStream* NewL();
        static UT_CRtpTranStream* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpTranStream();

    private:    // Constructors and destructors

        UT_CRtpTranStream();
        void ConstructL();

    public:     // From observer interface

       void RtpPacketReceived( TRtpId aStreamId, 
                                const TRtpRecvHeader& aHeaderInfo, 
                                const TDesC8& aPayloadData );   
                                
         // from MRtcpObserver
        void SdesReceived( TRtpSSRC aSSRC, const TRtpSdesParams& aParams );
        void ByeReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TDesC8& aReason );
        void AppReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TRtcpApp& aApp );
        void SrReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TTimeStamps& aTimeStamps );
        void RrReceived( TRtpId aStreamId, TRtpSSRC aSSRC );                           

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpTranStream_NewLL();
        
        
         void UT_CRtpTranStream_BuildRtpPacketL();
        
        
         void UT_CRtpTranStream_BuildRtcpBYEPacketL();
        
        
         void UT_CRtpTranStream_BuildRtcpAPPPacketL();
        
        
         void UT_CRtpTranStream_SentRTPPacketsL();
        
        
         void UT_CRtpTranStream_SetSentRTPPacketsL();
        
        
         void UT_CRtpTranStream_SentRtcpReportL();
        
        
         void UT_CRtpTranStream_SetRtcpReportFlagL();
        
        
         void UT_CRtpTranStream_ResetRtcpReportFlagL();
        
        
         void UT_CRtpTranStream_SetBandwidthL();
        
        
         void UT_CRtpTranStream_ResetStreamStatL();
        
        
         void UT_CRtpTranStream_GetStreamStatL();
        
        
         void UT_CRtpTranStream_RtpStreamProcessRtcpReportSectionL();
        
        
         void UT_CRtpTranStream_RtpStreamCreateRtcpReportSectionL();
        
		 void Hex(HBufC8& aString);
		 
    private:    // Data
		TUint32 iProfileRTPTimeRates[KRtpMaxPayloadTypes];
        
        CRtpTranStream* iTranStream;
        MRtpObserver* iObs;
        TBool iExpectingExtensionPacket; // indicates when a packet with extensions is expected
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRTPTRANSTREAM_H__

// End of file
