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




#ifndef __UT_CRTPCOMM_H__
#define __UT_CRTPCOMM_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include "rtpsession.h"

#include <commdbconnpref.h>
#include <es_sock.h>

// include the class to be tested
#include "rtpcomm.h"

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CRtpComm;

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
NONSHARABLE_CLASS( UT_CRtpComm )
     : public CEUnitTestSuiteClass,
      public MRtpErrNotify, 
      public MReceivedNotify
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpComm* NewL();
        static UT_CRtpComm* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpComm();

    private:    // Constructors and destructors

        UT_CRtpComm();
        void ConstructL();

    public:     // From observer interface

        void OnRtpReceivedL();
        void OnRtcpReceivedL();
        void OnReceptionError( TPortType aPort, TInt aError );
        void OnPacketRejected(TPortType aPort);  
        void ErrorNotify( TInt aErrCode ) ; 

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpComm_NewLL();
        
        
         void UT_CRtpComm_ConstructSenderLL();
        
        
         void UT_CRtpComm_ConstructReceiverLL();
        
        
         void UT_CRtpComm_SetToAddressL();
        
        
         void UT_CRtpComm_SetAcceptedFromAddressL();
        
        
         void UT_CRtpComm_SetRecvCallbackL();
        
        
         void UT_CRtpComm_SendL();
        
        
         void UT_CRtpComm_Send_1L();
        
        
         void UT_CRtpComm_CancelSendL();
        
        
         void UT_CRtpComm_ReceiveL();
        
        
         void UT_CRtpComm_RegisterReceivedNotifyL();
        
        
         void UT_CRtpComm_GetSocketL();
        
        
         void UT_CRtpComm_SetNonRtpObserverFlagL();
         
         void UT_CRtpComm_CommReceiveL(  );
         
         void UT_CRtpComm_MaxSocketSizeL();
         
    private:
    	static TInt StopScheduler( TAny* aThis );
	
		void DelayL();    

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        CActiveSchedulerWait iWait;
		TInt iEventCount;
		TBool iCallBackCalled;
		CPeriodic* iTimer;
		RSocketServ iSocketServ;
    	RConnection iConnection;
        CRtpComm* iRtpComm;
        TUint iLocalPort;
    	TInt iErr;

    };

#endif      //  __UT_CRTPCOMM_H__

// End of file
