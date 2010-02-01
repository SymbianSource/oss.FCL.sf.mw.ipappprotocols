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




#ifndef RtpCommRecv_H
#define RtpCommRecv_H

#include <rtpdef.h>
#include "rtpcommdef.h"
#include "rtputil.h"

/**
*  Class definition for receiving
*
*  @lib RtpService.dll
*/
class CRtpCommRecv : public CActive
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
    // Friend class used for testing
        friend class CRtpUtRtpCommRecv;
        friend class CRtpUtRtpComm;
    #endif 
    public:
        static CRtpCommRecv* NewL( TPortType aPortType, RSocket& aSocket, 
                                   TInt aPriority, MRtpErrNotify& aErrNotify,
                                   TBool aNonRtpObserverSet ); 

        ~CRtpCommRecv();
        void RegisterReceivedNotify( MReceivedNotify* aNotify );
        TInt Recv( TDes8& aPacket );
        void SetRemoteAddress(TInetAddr& aAddr);     
        void SetNonRtpObserverFlag(TBool aValue);      
        TBool IsSetRemoteAdress();     

    public: // from CActive
        void RunL();
        void DoCancel();

    private:
        CRtpCommRecv( TPortType aPortType, RSocket& aSocket, 
                      TInt aPriority, MRtpErrNotify& aErrNotify,
                      TBool aNonRtpObserverSet );
        void ConstructL();

    private:
        TPortType iPortType;
        MReceivedNotify* iRecvNotify;
        RSocket& iSocket;
        TInetAddr iFromAddr;
        TInetAddr iRemoteAddr;
        TBool iRemoteAddrSet;        
        MRtpErrNotify&  iRtpErrNotify; 
        TBool iNonRtpObserverSet;        
	 
	 private:
	 #ifdef EUNIT_TESTING
	  friend class UT_CRtpComm;
	  friend class UT_CRtpCommRecv;
	  friend class UT_CRtpSession;
	#endif     	    
    };
#endif	// RtpCommRecv_H
