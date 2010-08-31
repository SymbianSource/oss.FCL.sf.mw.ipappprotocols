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




#ifndef __UT_CRTPSTREAM_H__
#define __UT_CRTPSTREAM_H__

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
class CRtpStream;

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
NONSHARABLE_CLASS( UT_CRtpStream )
     : public CEUnitTestSuiteClass,
      public MRtpObserver,
      public MRtcpObserver,
      public MSsrcCheckCallback
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpStream* NewL();
        static UT_CRtpStream* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpStream();

    private:    // Constructors and destructors

        UT_CRtpStream();
        void ConstructL();

    public:     // From observer interface

        // from MRtpObserver
        void RtpPacketReceived( TRtpId aStreamId, 
                                const TRtpRecvHeader& aHeaderInfo, 
                                const TDesC8& aPayloadData );    
    // from MRtcpObserver
        void SdesReceived( TRtpSSRC aSSRC, const TRtpSdesParams& aParams );
        void ByeReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TDesC8& aReason );
        void AppReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TRtcpApp& aApp );
        void SrReceived( TRtpId aStreamId, TRtpSSRC aSSRC, const TTimeStamps& aTimeStamps );
        void RrReceived( TRtpId aStreamId, TRtpSSRC aSSRC );
	// from MSsrcCheckCallback
        TInt CheckRemoteSsrcL( TRtpSSRC& aSSRC );
        TBool UT_CRtpStream::CheckRemoteAddr( );

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpStream_ResetStreamStatL();
        
        
         void UT_CRtpStream_FirstPkgL();
        
        
         void UT_CRtpStream_SetFirstPkgL();
        
        
         void UT_CRtpStream_GetLocalSSRCL();
        
        
         void UT_CRtpStream_SetLocalSSRCL();
        
        
         void UT_CRtpStream_RegisterRtcpObserverL();
        
        
         void UT_CRtpStream_UnRegisterRtcpObserverL();
        
        
         void UT_CRtpStream_GetStreamIDL();
        
        
         void UT_CRtpStream_RtcpStatsL();
        
        
         void UT_CRtpStream_GetStreamStatL();
        
        
         void UT_CRtpStream_RtpStreamProcessRtcpReportSectionL();
        
        
         void UT_CRtpStream_RtpStreamCreateRtcpReportSectionL();
        
         void UT_CRtpStream_UpdateParamL();
         	
         void UT_CRtpStream_RtpByeAppMethodsL();
         
         void UT_CRtpStream_TestRtpSRMethodsL();
         
         void UT_CRtpStream_TestRtpRRMethodsL();
         
         void UT_CRtpStream_TestStreamStatAndSdesMethods();
         void UT_CRtpStream_RtpStreamProcessRtpPacketL();
         
         void Hex(HBufC8& aString);

    private:    // Data
		TUint32 iProfileRTPTimeRates[KRtpMaxPayloadTypes];
        CRtpRecvStream* iRecvStream;
        CRtpTranStream* iTranStream;
        MRtpObserver* iObs;
        TBool iExpectingExtensionPacket; // indicates when a packet with extensions is expected
        TInt iWantedResult; // indicates the error we want from CheckRemoteSsrc()
        EUNIT_DECLARE_TEST_TABLE; 
   #ifdef EUNIT_TESTING
	  friend class UT_CRtpStream;
	#endif     	    
    };

#endif      //  __UT_CRTPSTREAM_H__

// End of file
