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




#ifndef __UT_CRTPSTPPACKET_H__
#define __UT_CRTPSTPPACKET_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include <rtpdef.h>


//  FORWARD DECLARATIONS
class CRtpStpPacket;

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
NONSHARABLE_CLASS( UT_CRtpStpPacket )
     : public CEUnitTestSuiteClass,
     public MRtpObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpStpPacket* NewL();
        static UT_CRtpStpPacket* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpStpPacket();

    private:    // Constructors and destructors

        UT_CRtpStpPacket();
        void ConstructL();

    public:     // From observer interface

  	//MRtpObserver
   		void RtpPacketReceived( TRtpId aStreamId, 
                                        const TRtpRecvHeader& aHeaderInfo, 
                                        const TDesC8& aPayloadData );

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpStpPacket_CRtpStpPacketL();
        
        
         void UT_CRtpStpPacket_ConstructLL();
        
        
         void UT_CRtpStpPacket_CloseL();
        
        
         void UT_CRtpStpPacket_RtpPacketResetL();
        
        
         void UT_CRtpStpPacket_RtpPacketResetPtrL();
        
        
         void UT_CRtpStpPacket_RtpPacketBuildL();
        
        
         void UT_CRtpStpPacket_RtpPacketProcessL();
        
        
         void UT_CRtpStpPacket_RegisterRtpObserverL();
        
        
         void UT_CRtpStpPacket_UnregisterRtpObserverL();
        
        
         void UT_CRtpStpPacket_SetSizeL();
        
        
         void UT_CRtpStpPacket_SizeL();
        
        
         void UT_CRtpStpPacket_DataL();
        
        
         void UT_CRtpStpPacket_GetHBufL();
        
        
         void UT_CRtpStpPacket_DesL();
         

    private:    // Data
		CRtpStpPacket* iRtpStpPacket;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRTPSTPPACKET_H__

// End of file
