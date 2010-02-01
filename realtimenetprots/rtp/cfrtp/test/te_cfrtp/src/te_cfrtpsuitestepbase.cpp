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
// @file 
// @internalAll
// 
//

#include <utf.h>
#include <rtp_proto_id.h>
#include "te_cfrtpsuitestepbase.h"
#include "te_cfrtpsuitedefs.h"

// Device driver constants

TVerdict CTe_CfrtpSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	/* Create a Connection with the ESock server. 
	   Create a open a RConnection */
	//for removing warning 11/06/2008	
	//TInt err;
	SetTestStepResult(EFail);
	
	/* Read all the relevant data from Config File Default section */
	TPtrC strIpAddrLocal,strIpAddrDest1,strIpAddrDest2;
	TPtrC strCName;
	
	if(!GetStringFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_CNAME1,strCName))
		{
		return TestStepResult();
		}
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iCName,strCName);
	
	/* Read all the IP Address */
	if(GetStringFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_LocalAddr,strIpAddrLocal))
		{
		if(KErrNone != iIpAddrLocal.Input(strIpAddrLocal))
			{
			return TestStepResult();
			}
		}
	else
		{
		return TestStepResult();
		}
	
	/* Read all the IP Address */
	if(GetStringFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_DestIpAddr1,strIpAddrDest1))
		{
		if(KErrNone != iIpAddrDest1.Input(strIpAddrDest1))
			{
			return TestStepResult();
			}
		}
	else
		{
		return TestStepResult();
		}
	
	/* Read all the IP Address */
	if(GetStringFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_DestIpAddr2,strIpAddrDest2))
		{
		if(KErrNone != iIpAddrDest2.Input(strIpAddrDest2))
			{
			return TestStepResult();
			}
		}
	else
		{
		return TestStepResult();
		}

	/* Read Port numbers */
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_LocalPort1,iLocalPort1))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_DestPort1,iDestPort1))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_DestPort2,iDestPort2))
	{
		return TestStepResult();
	}

    /* Other Params */
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_MaxDropOut,iMaxDropOut))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_MaxMisOrder,iMaxMisOrder))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_MinSequential,iMinSeq))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_BandWidth,iBandWidth))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_RtpTimeNow,iRtpTimeNow))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_RtpTimeConv,iRtpTimeConv))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_AutoSend,iAutoSend))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_RtcpInterval,iRtcpInterval))
	{
		return TestStepResult();
	}
 
    TInt tierID;
    if(!GetIntFromConfig(KTe_Cfrtp_SectDefault,KTe_Cfrtp_TierId,tierID))
	{
		return TestStepResult();
	}
    iTierId = tierID;
   
   	//for removing warning 11/06/2008	
	//err = iSocketServ.Connect();
	//err = iCon.Open(iSocketServ);
    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iCon.Open(iSocketServ));
	
	TRequestStatus status;
	TCommDbConnPref conPref;
	conPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	iCon.Start(conPref,status);
	User::WaitForRequest(status);
	if(status.Int() == KErrNone)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}

TVerdict CTe_CfrtpSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	
/* Open a RSubCon and Set rtp params on it */
TInt CTe_CfrtpSuiteStepBase::InitSubConL()
	{
	
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
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(iCName);
	
	
	iIpAddrDest1.SetPort(iDestPort1); //For RTCP ?
	reqGenericParams->SetDefDestinationAddr(iIpAddrDest1);

	TTimeIntervalMicroSeconds32 rtcpInter(iRtcpInterval);
	reqGenericParams->SetRtcpReportInterval(rtcpInter);
	
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
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
	
	return ret;
	}

TInt CTe_CfrtpSuiteStepBase::InitSocketsL()
	{
	TInt err = KErrGeneral;
    if(KErrNone == InitSubConL())
    	{	
    	err = iRtpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtp, iSubCon);
    	if(err == KErrNone)
    		{
    		iIpAddrLocal.SetPort(iLocalPort1);
    		iRtpSocket.Bind(iIpAddrLocal);
    		}
    	if(KErrNone == err)
    		{
    		err = iRtcpSocket.Open(iSocketServ, KAfInet, KSockDatagram, KProtocolRtcp, iSubCon);
    		if(err == KErrNone)
    			{
    			iIpAddrLocal.SetPort(iLocalPort1+1);
    			iRtcpSocket.Bind(iIpAddrLocal);
    			}
    		}
    	}
    return err;
	}

/* Open a RSubCon and Set Dumy rtp params on it */
TInt CTe_CfrtpSuiteStepBase::InitDummySubConL(CSubConRTPGenericParamSetDummy::TTestcaseId aTestId)
	{
	
	TInt ret = KErrNone;
	// Create the container for all sub connection parameters
	RSubConParameterBundle subconParams;
	CleanupClosePushL(subconParams);
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(subconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSetDummy* reqGenericParams = CSubConRTPGenericParamSetDummy::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	// Set the requested Generic Parameters
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	
	reqGenericParams->SetBandWidth(iBandWidth);
	reqGenericParams->SetRtptimeConversion(iRtpTimeConv);
	reqGenericParams->SetRtpTimeNow(iRtpTimeNow);
	reqGenericParams->SetRtpAutoSend(iAutoSend);
	
	
	reqGenericParams->SetMaxDropOut(iMaxDropOut);
	reqGenericParams->SetMaxMisorder(iMaxMisOrder);
	reqGenericParams->SetMinSequential(iMinSeq);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(iCName);
	
	
	iIpAddrDest1.SetPort(iDestPort1); //For RTCP ?
	reqGenericParams->SetDefDestinationAddr(iIpAddrDest1);
	
	TUid idTier = TUid::Uid(iTierId);
	/* Give the Dummy SCPR Id here */
	reqGenericParams->SetTierID(idTier);
	
	/* Set the test case Id on ParameterBundle */
	reqGenericParams->SetTestId(aTestId);
	
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
	// Destroy parameters
	CleanupStack::PopAndDestroy();         // subconParams
	
	return ret;
	}

CTe_CfrtpSuiteStepBase::~CTe_CfrtpSuiteStepBase()
	{
	iRtpSocket.Close();
	iRtcpSocket.Close();
	iSubCon.Close();
	iCon.Close();
	iSocketServ.Close();
	}

CTe_CfrtpSuiteStepBase::CTe_CfrtpSuiteStepBase()
	{
	}
