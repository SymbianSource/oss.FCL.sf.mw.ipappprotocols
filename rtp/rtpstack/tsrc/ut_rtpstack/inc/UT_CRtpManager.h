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




#ifndef __UT_CRTPMANAGER_H__
#define __UT_CRTPMANAGER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "rtpmanager.h"

//  FORWARD DECLARATIONS
class CRtpManager;

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
NONSHARABLE_CLASS( UT_CRtpManager )
     : public CEUnitTestSuiteClass,
     public MRtpErrNotify,
     public MRtcpObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpManager* NewL();
        static UT_CRtpManager* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpManager();

    private:    // Constructors and destructors

        UT_CRtpManager();
        void ConstructL();

    public:     // From observer interface

        void ErrorNotify( TInt aErrCode ) ;
        
    	// from MRtcpObserver

        void SdesReceived( TRtpSSRC aSSRC,
                           const TRtpSdesParams& aParams );
    
        void ByeReceived( TRtpId aStreamId, TRtpSSRC aSSRC,
                          const TDesC8& aReason );

        void AppReceived( TRtpId aStreamId, TRtpSSRC aSSRC,
                          const TRtcpApp& aApp );
    
        void SrReceived( TRtpId aStreamId, TRtpSSRC aSSRC,
                         const TTimeStamps& aTimeStamps );
    
        void RrReceived( TRtpId aStreamId, TRtpSSRC aSSRC );    

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpManager_NewLL();
        
        
         void UT_CRtpManager_OpenLL();
        
        
         void UT_CRtpManager_StartConnectionL();
        
        
         void UT_CRtpManager_StartConnection_OneL();
        
         void UT_CRtpManager_StartConnection_TwoL();
        
         void UT_CRtpManager_CancelStartL();
        
        
         void UT_CRtpManager_CloseL();
        
        
         void UT_CRtpManager_SetLocalSdesL();
        
        
         void UT_CRtpManager_GetLocalIPAddressL1L();
         void UT_CRtpManager_GetLocalIPAddressL2L();
         void UT_CRtpManager_GetIAPIdL();
         void UT_CRtpManager_RemoveRtpObjectLL(  );
        
        
         void UT_CRtpManager_CreateSessionLL();
        
        
         void UT_CRtpManager_StartSessionL();
        
        
         void UT_CRtpManager_CloseSessionL();
        
        
         void UT_CRtpManager_SetRemoteAddressL();
        
        
         void UT_CRtpManager_SetRemoteRtcpAddressL();
        
        
         void UT_CRtpManager_CreateStreamLL();
        
        
         void UT_CRtpManager_CreateReceiveStreamLL();
        
        
         void UT_CRtpManager_CreateTransmitStreamLL();
        
        
         void UT_CRtpManager_CreateTransmitStreamExtLL();
        
        
         void UT_CRtpManager_CloseStreamL();
        
        
         void UT_CRtpManager_RegisterRtpObserverL();
        
        
         void UT_CRtpManager_UnregisterRtpObserverL();
        
        
         void UT_CRtpManager_SetNonRTPDataObserverL();
        
        
         void UT_CRtpManager_SendRtpPacketL();
        
        
         void UT_CRtpManager_SendRtpPacket_1L();
        
        
         void UT_CRtpManager_SendRtpPacket_2L();
        
        
         void UT_CRtpManager_SendDataLL();
        
        
         void UT_CRtpManager_CancelSendL();
        
        
         void UT_CRtpManager_RegisterRtcpObserverL();
        
        
         void UT_CRtpManager_UnregisterRtcpObserverL();
        
        
         void UT_CRtpManager_SendRtcpByePacketL();
        
        
         void UT_CRtpManager_SendRtcpAppPacketL();
        
        
         void UT_CRtpManager_SendRtcpRrPacketL();
        
        
         void UT_CRtpManager_SendRtcpSrPacketL();
        
        
         void UT_CRtpManager_SuspendRtcpSendingL();
        
        
         void UT_CRtpManager_IsRtcpSendingSuspendedL();
        
        
         void UT_CRtpManager_GetSessionIdL();
        
        
         void UT_CRtpManager_GetRtpSocketL();
        
        
         void UT_CRtpManager_GetRtcpSocketL();
        
        
         void UT_CRtpManager_GetStreamStatisticsL();
        
        
         void UT_CRtpManager_GetSamplingRateL();
        
        
         void UT_CRtpManager_SetSamplingRateL();
        
        
         void UT_CRtpManager_SetRtcpParametersL();
        
         void UT_CRtpManager_AddStreamToSessionL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        CRtpManager* iRtpManager ;
       	RSocketServ iSocketServ;
		RSocket iSocket;	
		TInt iConnectErr;
		
		RConnection iConnection;
		TUint iPriority;

    };

#endif      //  __UT_CRTPMANAGER_H__

// End of file
