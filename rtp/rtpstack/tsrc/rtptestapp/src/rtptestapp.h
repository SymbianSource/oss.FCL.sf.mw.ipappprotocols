// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header for class specific to rtp functional tests
*
*/



#ifndef __RTPTESTAPP_H
#define __RTPTESTAPP_H

#include "rtpapi.h"


// Class Declaration
class CRtpTestApp : public CBase, public MRtpErrNotify, public MRtcpObserver, public MRtpObserver, public MRtpPostProcessingObserver
	{
public:
    
    static CRtpTestApp*  NewL();
        
    void ConstructL();
    
    void SendRtpPacketL();
    
    void SendRtpPacketLoopbackL();
    
    void SendZeroLengthRtpPacketL();
    
	~CRtpTestApp();
	
	//From observer interface
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
    
    // From MRtpObserver
    void RtpPacketReceived( TRtpId aStreamId, const TRtpRecvHeader& aHeaderInfo, const TDesC8& aPayloadData );
    
    // From MRtpPostProcessingObserver
     
    void ReadyToSendRtpPacket( TRtpId aTranStreamId, TPtr8 &aPacket );
        
	
	TInt iError;
	TBool iIgnoreIncrease;

	private:
    CRtpTestApp();
	CRtpAPI* iRtpApi ;
    
    TRtpSdesParams iParams;
    TCreateSessionParams iSessionParams;
    TBool       iEnableRtcp;
    TRtcpParams iRtcpParams;
    TRtpId iSessionId1;
    TRtpId iSessionId2;  
    TUint iPort1;
    TUint iPort2;
    CArrayFixFlat<TUint32>* iCsrclist;
    TInt iNoOfRtpPackets;
    TRtpId iS1T1;
    TRtpId iS2R1;
	};


class CrtpDataSender: public CActive
    {
public:
    static CrtpDataSender* NewL(CRtpAPI* aRtpApi, TRtpId sessId)
        {
        CrtpDataSender *pSelf = new(ELeave) CrtpDataSender();
        pSelf->iRtpApi = aRtpApi;
        pSelf->iSessId = sessId;
        return pSelf;
        }
    
    virtual void DoCancel()
        {
        iRtpApi->CancelSend(iSessId);
        }
    
    void RunL();
    
    void SendRtpData( TRtpSendPktParams& aHeaderInfo , TArray<TRtpCSRC> aCSRCList)
        {
        aHeaderInfo.iStatus = &iStatus;
        iRtpApi->SendRtpPacket(aHeaderInfo, aCSRCList);
        SetActive();
        }
private:
    CrtpDataSender():CActive(EPriorityStandard)
        {
        CActiveScheduler::Add(this);
        }
    CRtpAPI *iRtpApi;
    TRtpId  iSessId;
    };

#endif


