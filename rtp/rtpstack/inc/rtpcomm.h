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



#ifndef __RTPCOMM_H
#define __RTPCOMM_H

// INCLUDES
#include "rtpcommdef.h"
#include "rtpcommrecv.h"
#include "rtpcommsend.h"


/**
*  Class for socket communication. 
*
*  @lib RtpService.dll
*/
class CRtpComm : public CBase
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
    // Friend class used for testing
        friend class CRtpUtRtpComm;
        friend class CRtpUtRtpSession;
    #endif 
    public:
        static CRtpComm* NewL( TUint& aLocalPort, 
                               RSocketServ& aSocketServ,
                               RConnection& aRConn, 
                               const TCreateSessionParams& aParams,
                               MRtpErrNotify& aErrNotify,
                               TBool aEnableRtcp );
        ~CRtpComm();


        /**
        * Create the resources necessary for sending RTP, and RTCP if necessary
        */
        void ConstructSenderL(TInetAddr& aRtpAddr, TInetAddr& aRtcpAddr);

        /**
        * Create the resources necessary for receiving RTP, and RTCP if
        * necessary.
        */
        void ConstructReceiverL(TBool aNonRtpObserverSet);

        TInt SetToAddress( TPortType aPortType, TInetAddr& aAddr );
        void SetAcceptedFromAddress(TInetAddr& aAddr);
        void SetRecvCallback( TPortType aPortType, TCallBack& aCallback );

        TInt Send( TPortType aPortType, const TDesC8& aSendBuf );
        void Send( TPortType aPortType, const TDesC8& aSendBuf,
                   TRequestStatus& aStatus ); 
        void CancelSend( TPortType aPortType );

        TInt Receive( TPortType aPortType, TDes8& aRecvBuf );

        void RegisterReceivedNotify( MReceivedNotify* aNotify );

        RSocket* GetSocket( TPortType aPortType );
        
        void SetNonRtpObserverFlag(TBool aValue);
        
        TInt MaxSocketSize();

    private:
        CRtpComm();
        void ConstructL( TUint& aLocalPort, 
                         RSocketServ& aSocketServ,             
                         RConnection& aRConn, 
                         const TCreateSessionParams& aParams,
                         MRtpErrNotify& aErrNotify,
                         TBool aEnableRtcp );
        void Close();
		
   	protected:
   		CRtpCommRecv* iReceiver[KMaxPorts];
    private:
        RSocket iSocket[KMaxPorts];
        CRtpCommSend* iSender[KMaxPorts];
        
        TUint iPriority;
        MRtpErrNotify* iErrNotify;
        TBool iRtcpEnabled;
        MReceivedNotify* iReceivedNotify;
        TInt iSocketSize;
    
    private: 
    	friend class CRtpSession;	
    #ifdef EUNIT_TESTING
	  friend class UT_CRtpAPI;
	  friend class UT_CRtpComm;
	  friend class UT_CRtpSession;
	  friend class UT_CRtpSessionSrtp;
	#endif     	    

    };

#endif  // __RTPCOMM_H

// End of File
