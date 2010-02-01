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




#ifndef __UT_CRTPCOMMRECV_H__
#define __UT_CRTPCOMMRECV_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <in_sock.h>
#include <commdbconnpref.h>
#include "rtpdef.h"
#include "rtpcommdef.h"
//  FORWARD DECLARATIONS
class CRtpCommRecv;

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
NONSHARABLE_CLASS( UT_CRtpCommRecv )
     : public CEUnitTestSuiteClass,
     public MRtpErrNotify,
     public MReceivedNotify
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpCommRecv* NewL();
        static UT_CRtpCommRecv* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpCommRecv();

    private:    // Constructors and destructors

        UT_CRtpCommRecv();
        void ConstructL();

    public:     // From observer interface

        void ErrorNotify( TInt aErrCode ) ;
        
        void OnRtpReceivedL() ;
        void OnRtcpReceivedL() ;
        void OnReceptionError( TPortType aPort, TInt aError ) ;
        void OnPacketRejected(TPortType aPort) ;   

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpCommRecv_NewLL();
        
        
         void UT_CRtpCommRecv_RegisterReceivedNotifyL();
        
        
         void UT_CRtpCommRecv_RecvL();
        
        
         void UT_CRtpCommRecv_SetRemoteAddressL();
        
        
         void UT_CRtpCommRecv_SetNonRtpObserverFlagL();
        
        
         void UT_CRtpCommRecv_RunL1L();
         void UT_CRtpCommRecv_RunL2L();
         void UT_CRtpCommRecv_RunL3L();
         void UT_CRtpCommRecv_RunL4L(  );
         void UT_CRtpCommRecv_RunL5L(  );
         void UT_CRtpCommRecv_DoCancelL();
         void UT_CRtpCommRecv_IsSetRemoteAdressL();
		 static TInt StopScheduler( TAny* aThis );
		 void DelayL();        

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
		RSocketServ iSocketServ;
		RSocket iSocket;	
		CRtpCommRecv* iReceiver;
		TInt iErr;
		TInt iConnectErr;
		
		HBufC8* iData;
		CActiveSchedulerWait iWait;
		TInt iEventCount;
		TBool iCallBackCalled;
		CPeriodic* iTimer;
		RConnection iConnection;
		TUint iPriority;
		TPortType iPortType;
		TInt iPacketCount;
	};

#endif      //  __UT_CRTPCOMMRECV_H__

// End of file
