// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Basic Test cases for RTP Flows and SCPR
// @file
// @internalAll
// 
//

#include "cfrtp_basictests.h"
#include "te_cfrtpsuitedefs.h"
#include <rtp_proto_id.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <in_sock_internal.h>
#endif

CSubConStep::~CSubConStep()
/**
 * Destructor
 */
	{
	}

CSubConStep::CSubConStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KSubConTestStep);
	}

TVerdict CSubConStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CSubConStep::doTestStepL()
/**
 * Try opening a SubConnection and Set RTP Params on it.
 * @return - TVerdict code
 *
 */
	{
    SetTestStepResult(EFail);
    
    /* Open a RSubCon and Set rtp params on it */
    RSubConnection subConnection;


	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(iBandWidth);
	reqGenericParams->SetRtptimeConversion(iRtpTimeConv);
	reqGenericParams->SetRtpAutoSend(iAutoSend);
	reqGenericParams->SetRtpTimeNow(iRtpTimeNow);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(iCName);
	
	iIpAddrDest1.SetPort(iDestPort1); //For RTCP ?
	reqGenericParams->SetDefDestinationAddr(iIpAddrDest1);
	
	TUid idTier = TUid::Uid(iTierId);
	reqGenericParams->SetTierID(idTier);
	
	TInt err;
	err = subConnection.Open(iSocketServ,RSubConnection::ECreateNew,iCon);
	if(KErrNone != err)
		{
		SetTestStepResult(EFail);
		}
	else
		{
		// Set Properties of the sub-connection
		TInt res = subConnection.SetParameters(subconParams);
		if(res == KErrNone)
			{
			SetTestStepResult(EPass);
			}
		}
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
	subConnection.Close();
	
	return TestStepResult();
	}



TVerdict CSubConStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpSockTest::~CRtpSockTest()
/**
 * Destructor
 */
	{
	}

CRtpSockTest::CRtpSockTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpSockTestStep);
	}

TVerdict CRtpSockTest::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpSockTest::doTestStepL()
/**
 * @return - TVerdict code
 * Try opening a Socket on an RTP SCPR.
 */
	{
    SetTestStepResult(EFail);
    TInt err;
    if(KErrNone == CTe_CfrtpSuiteStepBase::InitSubConL())
    	{	
    	err = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(KErrNone == err)
    		{
    		SetTestStepResult(EPass);
    		}
    	}
    return TestStepResult();
	}


TVerdict CRtpSockTest::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtcpSockTest::~CRtcpSockTest()
/**
 * Destructor
 */
	{
	}

CRtcpSockTest::CRtcpSockTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtcpSockTestStep);
	}

TVerdict CRtcpSockTest::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtcpSockTest::doTestStepL()
/**
 * @return - TVerdict code
 * Try opening both RTP and RTCP Sockets.
 */
	{
    SetTestStepResult(EFail);
    TInt err;
    if(KErrNone == CTe_CfrtpSuiteStepBase::InitSubConL())
    	{	
    	err = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(KErrNone == err)
    		{
    		err = iRtcpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtcp, iSubCon);
    		if(KErrNone == err)
    			{
    			SetTestStepResult(EPass);
    			}
    		}
    	}
    return TestStepResult();
	}


TVerdict CRtcpSockTest::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpSendRecv::~CRtpSendRecv()
/**
 * Destructor
 */
	{
	}

CRtpSendRecv::CRtpSendRecv()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpSendRecvStep);
	}

TVerdict CRtpSendRecv::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpSendRecv::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    if(KErrNone == InitSocketsL())
    	{
		iIpAddrDest1.SetPort(iDestPort1);
		TBuf8<3000> sendBuf((TUint8*)"Hello World");
		
		TRequestStatus status;
		iRtpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
		User::WaitForRequest(status);
		if(status.Int() == KErrNone)
			{
			/* Now do a Receive */
			sendBuf.FillZ();
			iRtpSocket.RecvFrom(sendBuf,iIpAddrDest1,NULL,status);
			User::WaitForRequest(status);
			if(status.Int() == KErrNone)
				{
				sendBuf.SetLength(2500);
				TRequestStatus status;
				iRtpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
				User::WaitForRequest(status);
				if(status.Int() == KErrNone)
					{
					SetTestStepResult(EPass);	
					}
				}
			}
		
    	}
    return TestStepResult();
	}


TVerdict CRtpSendRecv::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtcpSendRecv::~CRtcpSendRecv()
/**
 * Destructor
 */
	{
	}

CRtcpSendRecv::CRtcpSendRecv()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtcpSendRecvTestStep);
	}
 
TVerdict CRtcpSendRecv::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtcpSendRecv::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    if(KErrNone == InitSocketsL())
    	{
		iIpAddrDest1.SetPort(iDestPort1+1);
		TBuf8<3000> sendBuf((TUint8*)"Hello World");
		
		TRequestStatus status;
		iRtcpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
		User::WaitForRequest(status);
		if(status.Int() == KErrNone)
			{
			/* Now do a Receive */
			sendBuf.FillZ();
			iRtcpSocket.RecvFrom(sendBuf,iIpAddrDest1,NULL,status);
			User::WaitForRequest(status);
			if(status.Int() == KErrNone)
				{
				sendBuf.SetLength(2500);
				TRequestStatus status;
				iRtcpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
				User::WaitForRequest(status);
				if(status.Int() == KErrNone)
					{
					SetTestStepResult(EPass);	
					}
				}
			}
    	}
    return TestStepResult();
	}


TVerdict CRtcpSendRecv::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//SetTestStepResult(EFail);  
	return TestStepResult();
	}

//

CRtpConnect::~CRtpConnect()
/**
 * Destructor
 */
	{
	}

CRtpConnect::CRtpConnect()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpConnectTestStep);
	}
 
TVerdict CRtpConnect::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpConnect::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    if(KErrNone == InitSocketsL())
    	{
		iIpAddrDest1.SetPort(iDestPort1);
		TRequestStatus status;
		iRtpSocket.Connect(iIpAddrDest1,status);
		User::WaitForRequest(status);
		if(status.Int() == KErrNone)
			{
			TInetAddr rem;
			iRtpSocket.RemoteName(rem);
			if(rem == iIpAddrDest1)
				{
				TBuf8<1024> sendBuf((TUint8*)"Hello World");
		
				iRtpSocket.Send(sendBuf, NULL, status);
				User::WaitForRequest(status);
				if(status.Int() == KErrNone)
					{
					/* Now do a Receive */
					sendBuf.FillZ();
					iRtpSocket.Recv(sendBuf,NULL,status);
					User::WaitForRequest(status);
					if(status.Int() == KErrNone)
						{
						/* Next time Connect shud fail */
						iIpAddrDest1.SetPort(iDestPort1+2);
						iRtpSocket.Connect(iIpAddrDest1,status);
						User::WaitForRequest(status);
						if(status.Int() != KErrNone)
							{
							SetTestStepResult(EPass);
							}
						}
					}	
				}
			}
    	}
    return TestStepResult();
	}


TVerdict CRtpConnect::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtcpConnect::~CRtcpConnect()
/**
 * Destructor
 */
	{
	}

CRtcpConnect::CRtcpConnect()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpConnectTestStep);
	}
 
TVerdict CRtcpConnect::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtcpConnect::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    if(KErrNone == InitSocketsL())
    	{
		iIpAddrDest1.SetPort(iDestPort1+1);
		TRequestStatus status;
		iRtcpSocket.Connect(iIpAddrDest1,status);
		User::WaitForRequest(status);
		if(status.Int() == KErrNone)
			{
			TInetAddr rem;
			iRtcpSocket.RemoteName(rem);
			if(rem == iIpAddrDest1)
				{
				TBuf8<1024> sendBuf((TUint8*)"Hello World");
		
				iRtcpSocket.Send(sendBuf, NULL, status);
				User::WaitForRequest(status);
				if(status.Int() == KErrNone)
					{
					/* Now do a Receive */
					sendBuf.FillZ();
					iRtcpSocket.Recv(sendBuf,NULL,status);
					User::WaitForRequest(status);
					if(status.Int() == KErrNone)
						{
						/* Next time Connect shud fail */
						iIpAddrDest1.SetPort(iDestPort1+2);
						iRtcpSocket.Connect(iIpAddrDest1,status);
						User::WaitForRequest(status);
						if(status.Int() != KErrNone)
							{
							SetTestStepResult(EPass);
							}
						}
					}	
				}	
			}
    	}
    return TestStepResult();
	}


TVerdict CRtcpConnect::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//SetTestStepResult(EFail);  
	return TestStepResult();
	}

//

CRtpNoBind::~CRtpNoBind()
/**
 * Destructor
 */
	{
	}

CRtpNoBind::CRtpNoBind()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNoBindTestStep);
	}
 
TVerdict CRtpNoBind::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNoBind::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    TInt err;
    if(KErrNone == InitSubConL())
    	{
    	err = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(err == KErrNone)
			{
			iIpAddrDest1.SetPort(iDestPort1);
			TBuf8<1024> sendBuf((TUint8*)"Hello World");
		
			TRequestStatus status;
			iRtpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
			User::WaitForRequest(status);
			if(status.Int() == KErrNone)
				{
				SetTestStepResult(EPass);
				}
			}
    	}
    return TestStepResult();
	}


TVerdict CRtpNoBind::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpBind::~CRtpBind()
/**
 * Destructor
 */
	{
	}

CRtpBind::CRtpBind()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpBindTestStep);
	}
 
TVerdict CRtpBind::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpBind::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    if(KErrNone == InitSocketsL())
    	{
    	TInetAddr loc;
    	iRtpSocket.LocalName(loc);
    	iIpAddrLocal.SetPort(iLocalPort1);
    	if(loc == iIpAddrLocal)
			{
			SetTestStepResult(EPass);
			}
    	}
    return TestStepResult();
	}


TVerdict CRtpBind::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpSockOpt::~CRtpSockOpt()
/**
 * Destructor
 */
	{
	}

CRtpSockOpt::CRtpSockOpt()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpSockOptTestStep);
	}
 
TVerdict CRtpSockOpt::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpSockOpt::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EPass);
    if(KErrNone == InitSocketsL())
    	{
    	TInt err;
    	TInt rcvBufSz;
    	TInt bandwidth;
    	TInt ssrc;
		
    	err = iRtpSocket.SetOpt(KSoUdpRecvBuf,KSolInetUdp,5000);
    	User::LeaveIfError(err);
    	
    	err = iRtpSocket.GetOpt(KSoUdpRecvBuf,KSolInetUdp,rcvBufSz);
    	User::LeaveIfError(err);
    	
    	if(rcvBufSz != 5000)
    		{
    		SetTestStepResult(EFail);
    		}
    	
    	err = iRtpSocket.SetOpt(KSoRtpBandwidth,KSolRtp,5000);
    	User::LeaveIfError(err);
    	
    	err = iRtpSocket.GetOpt(KSoRtpBandwidth,KSolRtp,bandwidth);
    	if(KErrNotSupported != err)
    		{
    		SetTestStepResult(EFail);
    		}
    	    	
    	TRtpStreamParams streamParams;
    	streamParams.iMinSequential = iMinSeq;
    	streamParams.iMaxMisorder   = iMaxMisOrder;
    	streamParams.iMaxDropout    = iMaxDropOut;
    	TPckgBuf<TRtpStreamParams> streamParamsBuf(streamParams);
    	err = iRtpSocket.SetOpt(KSoStreamParams,KSolRtp,streamParamsBuf);
    	User::LeaveIfError(err);
    	
    	TRtpTimeConversion rtpTimeConv;
    	rtpTimeConv.iRtpTimeNow = iRtpTimeNow;
    	rtpTimeConv.iConversion = iRtpTimeConv;
    	TPckgBuf<TRtpTimeConversion> rtpTimeConvBuf(rtpTimeConv);
    	err = iRtpSocket.SetOpt(KSoRtpTimeConvertion,KSolRtp,rtpTimeConvBuf);
    	User::LeaveIfError(err);
    	
    	err = iRtpSocket.SetOpt(KSoLocalSSRC,KSolRtp,0xFACE);
    	if(KErrNotSupported != err)
    		{
    		SetTestStepResult(EFail);
    		}
    	
    	err = iRtpSocket.GetOpt(KSoLocalSSRC,KSolRtp,ssrc);
    	User::LeaveIfError(err);
    	    		
    	}
    
    return TestStepResult();
	}


TVerdict CRtpSockOpt::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtcpSockOpt::~CRtcpSockOpt()
/**
 * Destructor
 */
	{
	}

CRtcpSockOpt::CRtcpSockOpt()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtcpSockOptTestStep);
	}
 
TVerdict CRtcpSockOpt::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtcpSockOpt::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EPass);
    if(KErrNone == InitSocketsL())
    	{
    	TInt err;
    	TInt rcvBufSz;
    	TInt timeInt;
    	TBool autosend;
    			
		err = iRtcpSocket.SetOpt(KSoUdpRecvBuf,KSolInetUdp,5000);
		User::LeaveIfError(err);
		
		err = iRtcpSocket.GetOpt(KSoUdpRecvBuf,KSolInetUdp,rcvBufSz);
		User::LeaveIfError(err);
		        	
		if(rcvBufSz != 5000)
			{
		 	SetTestStepResult(EFail);
		 	}
		
		err = iRtcpSocket.SetOpt(KSoRtcpAutoSend,KSolRtcp,iAutoSend);
		User::LeaveIfError(err);
		
		err = iRtcpSocket.GetOpt(KSoRtcpAutoSend,KSolRtcp,autosend);
		User::LeaveIfError(err);
		    		
		if(autosend != iAutoSend)
			{
			SetTestStepResult(EFail);
			}
		
		err = iRtcpSocket.SetOpt(KSoRtcpTimeInterval,KSolRtcp,0);
		User::LeaveIfError(err);
		
		err = iRtcpSocket.SetOpt(KSoRtcpCNAME,KSolRtcp,iCName);
		if(KErrNone == err)
			{
			SetTestStepResult(EFail);
			}
		
		iIpAddrDest1.SetPort(iDestPort1); 
		TPckgBuf<TInetAddr> defDestBuf(iIpAddrDest1);
		err = iRtcpSocket.SetOpt(KSoDefaultDestAddr,KSolRtcp,defDestBuf);
		User::LeaveIfError(err);
		
		err = iRtcpSocket.SetOpt(KSoLocalSSRC,KSolRtcp,0xFACE);
		if(KErrNotSupported != err)
			{
			SetTestStepResult(EFail);
			}
		
		err = iRtcpSocket.GetOpt(KSoRtcpTimeInterval,KSolRtcp,timeInt);
		if(KErrNotSupported != err)
			{
			SetTestStepResult(EFail);
			}
		}
    
    return TestStepResult();
	}


TVerdict CRtcpSockOpt::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpIoctl::~CRtpIoctl()
/**
 * Destructor
 */
	{
	}

CRtpIoctl::CRtpIoctl()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpIoctlTestStep);
	}
 
TVerdict CRtpIoctl::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpIoctl::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    if(KErrNone == InitSocketsL())
    	{
		TRequestStatus stat;
		TSoInetLastErr lastError;
		TPckg<TSoInetLastErr> packError(lastError);
    	iRtpSocket.Ioctl(KIoctlInetLastError,stat,&packError,KSolInetIp);
    	User::WaitForRequest(stat);
    	if(KErrNone == stat.Int())
			{
			SetTestStepResult(EPass);
			iRtpSocket.Ioctl(KIoctlInetLastError,stat,&packError,KSolInetIp);
			iRtpSocket.CancelIoctl();
			}
    	}
    return TestStepResult();
	}


TVerdict CRtpIoctl::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CBigSendRecvRtp::~CBigSendRecvRtp()
/**
 * Destructor
 */
	{
	}

CBigSendRecvRtp::CBigSendRecvRtp()
/**
 * Constructor
 */
	{
	SetTestStepName(KBigSendRecvRtp);
	}
 
TVerdict CBigSendRecvRtp::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{	
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CBigSendRecvRtp::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
	SetTestStepResult(EFail);
	if(KErrNone == InitSocketsL())
		{
		iIpAddrDest1.SetPort(9000);
		RBuf8 sendBuf;
		sendBuf.CreateMax(KBufferSize1);
		sendBuf.CleanupClosePushL();
		RBuf8 recvBuf;
		recvBuf.CreateMax(KBufferSize1);
		recvBuf.CleanupClosePushL();
		
		/* Fill the buffer with given character upto its length */
		sendBuf.Fill('Q');
		TRequestStatus status;
		/* do a Send of the data */
		iRtpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());
		/* Now do a Receive */
		recvBuf.FillZ();
		iRtpSocket.RecvFrom(recvBuf,iIpAddrDest1,NULL,status);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());
		
		/* Obtain a TPtr of the data excluding the RTP header */
		TPtr8 sendBufPtr = sendBuf.MidTPtr(KRtpHeaderSize);
		TPtr8 recvBufPtr = recvBuf.MidTPtr(KRtpHeaderSize);
		/* Check if the data received is the same as the data sent */
		TInt ret = sendBufPtr.Compare(recvBufPtr);
		if(ret == 0)
			{
			/* Increase the buffer size and fill it up with given data */
			sendBuf.ReAlloc(KBufferSize2);
			recvBuf.ReAlloc(KBufferSize2);
			sendBuf.Fill('Q', KBufferSize2);
			
			/* Send the larger data */
			iRtpSocket.SendTo(sendBuf, iIpAddrDest1, NULL, status);
			User::WaitForRequest(status);
			User::LeaveIfError(status.Int());
			/* Now do a Receive */
			recvBuf.FillZ(KBufferSize2);
			iRtpSocket.RecvFrom(recvBuf,iIpAddrDest1,NULL,status);
			User::WaitForRequest(status);
			User::LeaveIfError(status.Int());
			/* Get pointer to data and compare both of them */
			TPtr8 sendBufPtr = sendBuf.MidTPtr(KRtpHeaderSize);
			TPtr8 recvBufPtr = recvBuf.MidTPtr(KRtpHeaderSize);
			TInt ret = sendBufPtr.Compare(recvBufPtr);
			if(ret == 0)
				{
				SetTestStepResult(EPass);
				}
			}
		CleanupStack::PopAndDestroy(2);
		}
	return TestStepResult();
	}

TVerdict CBigSendRecvRtp::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpNegTests_01::~CRtpNegTests_01()
/**
 * Destructor
 */
	{
	}

CRtpNegTests_01::CRtpNegTests_01()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNegTests_01TestStep);
	}
 
TVerdict CRtpNegTests_01::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNegTests_01::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
	
	TInt ret = KErrNone;
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	
	reqGenericParams->SetBandWidth(iBandWidth);
	reqGenericParams->SetRtptimeConversion(iRtpTimeConv);
	reqGenericParams->SetRtpTimeNow(iRtpTimeNow);
	reqGenericParams->SetRtpAutoSend(iAutoSend);
	
	
	reqGenericParams->SetMaxDropOut(iMaxDropOut);
	reqGenericParams->SetMaxMisorder(iMaxMisOrder);
	reqGenericParams->SetMinSequencial(iMinSeq);
	
	//Give an unsupported protocol
	reqGenericParams->SetServiceProtocolID(KSolInetIcmp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(iCName);
	
	
	iIpAddrDest1.SetPort(iDestPort1); //For RTCP ?
	reqGenericParams->SetDefDestinationAddr(iIpAddrDest1);
	
	TUid idTier = TUid::Uid(iTierId);
	reqGenericParams->SetTierID(idTier);
	
	TInt err;
	err = iSubCon.Open(iSocketServ,RSubConnection::ECreateNew,iCon);
	if(KErrNone != err)
		{
		ret = KErrGeneral;
		}
	else
		{
		// Set Properties of the sub-connection
		TInt res = iSubCon.SetParameters(subconParams);
		if(res != KErrNone)
			{
			ret = KErrGeneral;
			}
		}
	if(KErrNone == ret)
		{
		ret = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(ret != KErrNone)
    		{
    		SetTestStepResult(EPass);
    		}
		}
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
    return TestStepResult();
	}


TVerdict CRtpNegTests_01::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpNegTests_02::~CRtpNegTests_02()
/**
 * Destructor
 */
	{
	}

CRtpNegTests_02::CRtpNegTests_02()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNegTests_02TestStep);
	}
 
TVerdict CRtpNegTests_02::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNegTests_02::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    TInt err;
    TRequestStatus stat;
    if(KErrNone == InitSubConL())
    	{
    	RSocket sock;
    	err = sock.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp, iSubCon);
    	if(KErrNone == err)
    		{
    		iIpAddrLocal.SetPort(iLocalPort1);
    		sock.Bind(iIpAddrLocal);
    		iIpAddrDest1.SetPort(iDestPort1);
    		sock.Connect(iIpAddrDest1,stat);
    		User::WaitForRequest(stat);
    		if(KErrNone != stat.Int())
    			{
    			SetTestStepResult(EPass);
    			}
			sock.Close();
    		}
    	
	 	}    
    return TestStepResult();
	}


TVerdict CRtpNegTests_02::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpNegTests_03::~CRtpNegTests_03()
/**
 * Destructor
 */
	{
	}

CRtpNegTests_03::CRtpNegTests_03()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNegTests_03TestStep);
	}
 
TVerdict CRtpNegTests_03::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNegTests_03::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    TInt err;
    TRequestStatus stat;
    if(KErrNone == InitSocketsL())
    	{
    	/* try to Open one more Socket on Subconnection. This should fail */
    	RSocket sock;
    	err = sock.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolInetUdp, iSubCon);
    	if(KErrNone != err)
    		{
    		SetTestStepResult(EPass);
    		}
		sock.Close();
	 	}    
    return TestStepResult();
	}


TVerdict CRtpNegTests_03::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpNegTests_04::~CRtpNegTests_04()
/**
 * Destructor
 */
	{
	}

CRtpNegTests_04::CRtpNegTests_04()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNegTests_04TestStep);
	}
 
TVerdict CRtpNegTests_04::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNegTests_04::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
	
	TInt ret = KErrNone;
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	
	reqGenericParams->SetBandWidth(iBandWidth);
	reqGenericParams->SetRtptimeConversion(iRtpTimeConv);
	reqGenericParams->SetRtpTimeNow(iRtpTimeNow);
	reqGenericParams->SetRtpAutoSend(iAutoSend);
	
	
	reqGenericParams->SetMaxDropOut(iMaxDropOut);
	reqGenericParams->SetMaxMisorder(iMaxMisOrder);
	reqGenericParams->SetMinSequencial(iMinSeq);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::ECreateNew);
	reqGenericParams->SetCNAMEL(iCName);
	
	
	iIpAddrDest1.SetPort(iDestPort1); //For RTCP ?
	reqGenericParams->SetDefDestinationAddr(iIpAddrDest1);
	
	TUid idTier = TUid::Uid(iTierId);
	reqGenericParams->SetTierID(idTier);
	
	TInt err;
	//Giving ECreateNew .. Socket open should fail
	err = iSubCon.Open(iSocketServ,RSubConnection::ECreateNew,iCon);
	if(KErrNone != err)
		{
		ret = KErrGeneral;
		}
	else
		{
		// Set Properties of the sub-connection
		TInt res = iSubCon.SetParameters(subconParams);
		if(res != KErrNone)
			{
			ret = KErrGeneral;
			}
		}
	if(KErrNone == ret)
		{
		ret = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(ret != KErrNone)
    		{
    		SetTestStepResult(EPass);
    		}
		}
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
    return TestStepResult();
	}


TVerdict CRtpNegTests_04::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpNegTests_05::~CRtpNegTests_05()
/**
 * Destructor
 */
	{
	}

CRtpNegTests_05::CRtpNegTests_05()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNegTests_05TestStep);
	}
 
TVerdict CRtpNegTests_05::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNegTests_05::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
	
	TInt ret = KErrNone;
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	//Wrong UID
	TUid idRTP = TUid::Uid(0xBABE);
	// Set the requested Generic Parameters
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	
	reqGenericParams->SetBandWidth(iBandWidth);
	reqGenericParams->SetRtptimeConversion(iRtpTimeConv);
	reqGenericParams->SetRtpTimeNow(iRtpTimeNow);
	reqGenericParams->SetRtpAutoSend(iAutoSend);
	
	
	reqGenericParams->SetMaxDropOut(iMaxDropOut);
	reqGenericParams->SetMaxMisorder(iMaxMisOrder);
	reqGenericParams->SetMinSequencial(iMinSeq);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(iCName);
	
	
	iIpAddrDest1.SetPort(iDestPort1); //For RTCP ?
	reqGenericParams->SetDefDestinationAddr(iIpAddrDest1);
	
	TUid idTier = TUid::Uid(iTierId);
	reqGenericParams->SetTierID(idTier);
	
	TInt err;
	err = iSubCon.Open(iSocketServ,RSubConnection::ECreateNew,iCon);
	if(KErrNone != err)
		{
		ret = KErrGeneral;
		}
	else
		{
		// Set Properties of the sub-connection
		TInt res = iSubCon.SetParameters(subconParams);
		if(res != KErrNone)
			{
			ret = KErrGeneral;
			}
		}
	if(KErrNone == ret)
		{
		ret = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(ret != KErrNone)
    		{
    		SetTestStepResult(EPass);
    		}
		}
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
    return TestStepResult();
	}


TVerdict CRtpNegTests_05::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpNegTestsFailNoBearer::~CRtpNegTestsFailNoBearer()
/**
 * Destructor
 */
	{
	}

CRtpNegTestsFailNoBearer::CRtpNegTestsFailNoBearer()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpNegTests_FailNoBearer);
	}
 
TVerdict CRtpNegTestsFailNoBearer::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpNegTestsFailNoBearer::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
    SetTestStepResult(EFail);
    TInt err;
    TRequestStatus stat;
    if(KErrNone == InitDummySubConL(CSubConRTPGenericParamSetDummy::KFailNoBearer))
    	{
    	/* try to Open one more Socket on Subconnection. This should fail */
    	RSocket sock;
    	err = sock.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolInetUdp, iSubCon);
    	if(KErrNone != err)
    		{
    		SetTestStepResult(EPass);
    		}
		sock.Close();
	 	}    
    return TestStepResult();
	}


TVerdict CRtpNegTestsFailNoBearer::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CRtpShutDownImmediate::~CRtpShutDownImmediate()
/**
 * Destructor
 */
	{
	}

CRtpShutDownImmediate::CRtpShutDownImmediate()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtpShutDownImmediate);
	}
 
TVerdict CRtpShutDownImmediate::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtpShutDownImmediate::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
	SetTestStepResult(EFail);
	TRequestStatus stat;
	TInt err;
    if(KErrNone == InitSubConL())
    	{
    	err = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(KErrNone == err)
			{
			iRtpSocket.Shutdown(RSocket::EImmediate,stat);
			User::WaitForRequest(stat);
			if(KErrNone == stat.Int())
				{
				SetTestStepResult(EPass);
				}
			}
		}
	return TestStepResult();
	}

TVerdict CRtpShutDownImmediate::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


//

CRtcpShutDownImmediate::~CRtcpShutDownImmediate()
/**
 * Destructor
 */
	{
	}

CRtcpShutDownImmediate::CRtcpShutDownImmediate()
/**
 * Constructor
 */
	{
	SetTestStepName(KRtcpShutDownImmediate);
	}
 
TVerdict CRtcpShutDownImmediate::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CRtcpShutDownImmediate::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
	SetTestStepResult(EFail);
	TRequestStatus stat;
    if(KErrNone == InitSocketsL())
    	{
		iRtcpSocket.Shutdown(RSocket::EImmediate,stat);
		User::WaitForRequest(stat);
		if(KErrNone == stat.Int())
			{
			SetTestStepResult(EPass);
			}
		}
	return TestStepResult();
	}

TVerdict CRtcpShutDownImmediate::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

//

CShutDownImmediate::~CShutDownImmediate()
/**
 * Destructor
 */
	{
	}

CShutDownImmediate::CShutDownImmediate()
/**
 * Constructor
 */
	{
	SetTestStepName(KShutDownImmediate);
	}
 
TVerdict CShutDownImmediate::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(CTe_CfrtpSuiteStepBase::doTestStepPreambleL());
	return TestStepResult();
	}


TVerdict CShutDownImmediate::doTestStepL()
/**
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("IMPORTANT A Panic in this Place means a Destroy was Posted(and handled) by IpDefScpr before RTPScpr :("));
	SetTestStepResult(EFail);
	TRequestStatus stat;
    if(KErrNone == InitSocketsL())
    	{
		iRtpSocket.Shutdown(RSocket::EImmediate,stat);
		User::WaitForRequest(stat);
		if(KErrNone == stat.Int())
			{
			iRtcpSocket.Shutdown(RSocket::EImmediate,stat);
			User::WaitForRequest(stat);
			if(KErrNone == stat.Int())
				{
				SetTestStepResult(EPass);
				}
			}
		}
	return TestStepResult();
	}

TVerdict CShutDownImmediate::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

