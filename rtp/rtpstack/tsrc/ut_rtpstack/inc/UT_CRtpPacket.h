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




#ifndef __UT_CRTPPACKET_H__
#define __UT_CRTPPACKET_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "rtppacket.h"
#include "rtpsession.h"


//  FORWARD DECLARATIONS
class CRtpPacket;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

const TUint KUTRtpMaxPayloadTypes = 128; // number of Payload types available.

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CRtpPacket )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpPacket* NewL();
        static UT_CRtpPacket* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpPacket();

    private:    // Constructors and destructors

        UT_CRtpPacket();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpPacket_NewLL();
        
        
         void UT_CRtpPacket_RtpPacketResetL();
        
        
         void UT_CRtpPacket_RtpPacketResetPtrL();
        
        
         void UT_CRtpPacket_RtpPacketGetSSRCL();
        
        
         void UT_CRtpPacket_RtpPacketGetPayloadTypeL();
        
        
         void UT_CRtpPacket_RtpPacketBuildL();
         void UT_CRtpPacket_RtcpPacketBuildL(  );
		 void UT_CRtpPacket_RtpPacketBuildAppL();
		 void UT_CRtpPacket_RtpPacketBuildByeL();
		 void UT_CRtpPacket_RtpPacketBuildRRL();
		 void UT_CRtpPacket_RtpPacketBuildSRL();
		 void UT_CRtpPacket_RtpPacketBuildSDESL();
		 void UT_CRtpPacket_RtpPacketProcessAppL();
		 void UT_CRtpPacket_RtpPacketProcessByeL();
		 void UT_CRtpPacket_RtpPacketProcessRRL();
		 void UT_CRtpPacket_RtpPacketProcessSRL();
		 void UT_CRtpPacket_RtpPacketProcessSDESL();
        
        
         void UT_CRtpPacket_RtpPacketProcessL();
        
         void UT_CRtpPacket_RtpPacketProcess2L(  );
        
         void UT_CRtpPacket_SetSizeL();
        
        
         void UT_CRtpPacket_SetTypeL();
        
        
         void UT_CRtpPacket_SizeL();
        
        
         void UT_CRtpPacket_DataL();
        
        
         void UT_CRtpPacket_GetHBufL();
        
        
         void UT_CRtpPacket_TypeL();
        
        
         void UT_CRtpPacket_DesL();
         void Hex(HBufC8& aString);
        

    private:    // Data
		TUint32 iProfileRTPTimeRates[KUTRtpMaxPayloadTypes];
        CRtpPacket* iPktSnd;
        CRtpPacket* iPktRtcpSnd;
        HBufC8* iTooLongData;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRTPPACKET_H__

// End of file
