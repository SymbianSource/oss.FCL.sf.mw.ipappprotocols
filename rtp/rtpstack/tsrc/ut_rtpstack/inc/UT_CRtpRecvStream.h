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




#ifndef __UT_CRTPRECVSTREAM_H__
#define __UT_CRTPRECVSTREAM_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CRtpRecvStream;

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
NONSHARABLE_CLASS( UT_CRtpRecvStream )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpRecvStream* NewL();
        static UT_CRtpRecvStream* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpRecvStream();

    private:    // Constructors and destructors

        UT_CRtpRecvStream();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpRecvStream_NewLL();
        
        
         void UT_CRtpRecvStream_RtpStreamUpdateParamL();
        
        
         void UT_CRtpRecvStream_SetReceivedRTPPacketsL();
        
        
         void UT_CRtpRecvStream_RegisterRtpObserverL();
        
        
         void UT_CRtpRecvStream_UnRegisterRtpObserverL();
        
        
         void UT_CRtpRecvStream_RtpStreamProcessRtpPacketL();
        
        
         void UT_CRtpRecvStream_ReceivedRTPPacketsL();
        
        
         void UT_CRtpRecvStream_GetRemoteStreamInfoL();
        
        
         void UT_CRtpRecvStream_GetRemoteSSRCL();
        
        
         void UT_CRtpRecvStream_ResetStreamStatL();
        
        
         void UT_CRtpRecvStream_GetStreamStatL();
        
        
         void UT_CRtpRecvStream_RtpStreamProcessRtcpReportSectionL();
        
        
         void UT_CRtpRecvStream_RtpStreamCreateRtcpReportSectionL();
        

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRTPRECVSTREAM_H__

// End of file
