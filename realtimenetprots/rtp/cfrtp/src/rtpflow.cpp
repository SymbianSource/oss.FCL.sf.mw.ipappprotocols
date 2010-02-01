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
// RTPFlow Implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>

#include "rtpflow.h"
#include "rtppint.h"
#include "rtp_cfmessages.h"


#if defined(ESOCK_LOGGING_ACTIVE)
_LIT8(KRtpFlowSubTag, "RtpFlow");
#endif

									
CRtpFlow::CRtpFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
:CRtpBaseFlow(aFactory, aSubConnId, aProtocolIntf)
{
	LOG_NODE_CREATE(KESockFlowTag, CRtpFlow);
}

CRtpFlow* CRtpFlow::NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
	{
	CRtpFlow *pRtpFlow = new (ELeave) CRtpFlow(aFactory, aSubConn, aProtocolIntf);
	
	CleanupStack::PushL(pRtpFlow);
	pRtpFlow->ConstructL();
	/* Register the flow with Rtp Pint */
	((CRtpProtocolIntf*)aProtocolIntf)->RegisterSubConnProvIDL(pRtpFlow,aSubConn);
	CleanupStack::Pop();

	return pRtpFlow;
	}

void CRtpFlow::ConstructL()
	{
	TSockAddr addr;
	/* Create the RTP Handler */
	iRtpHandler = CCFRtpHandler::NewL(this,EPriorityNormal);
	iRtpSession = CRtpController::NewL(*iRtpHandler,EPriorityNormal,addr, _L8(""));
	iRtpBuffer.ReAllocL(KRtpDefBufSz);
	}


CRtpFlow::~CRtpFlow()
	{
	delete iRtpSession;
	iRtpBuffer.Close();
	LOG_NODE_DESTROY(KESockFlowTag, CRtpFlow);
	}
	
void CRtpFlow::CanSend()
	{
	__ASSERT_DEBUG(iSessionDataNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));
	CRtpBaseFlow::CanSend();
	}

TInt CRtpFlow::Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr )
	{

	TInt nwr = -1;
	TInt ret = KErrNone;
	/* Findout the amount of Data contained in the RMbuf Chain */
	TInt rmbuflen = aData.Length();
	TPtr8 rtpBufPtr(NULL, 0);
	
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tWrite() Len=%d"), this, rmbuflen));
	
	if(!(aData.First()->Next()))
		{
		RMBuf *buffer = aData.First();
		__ASSERT_DEBUG(rmbuflen == buffer->Length() ,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicIncompleteBuffer));
		rtpBufPtr.Set(buffer->Ptr(), rmbuflen, rmbuflen);
		}
	else
		{
		if(iRtpBuffer.MaxLength() < rmbuflen)
			{
			iRtpBuffer.SetLength(0);
			ret = iRtpBuffer.ReAlloc(rmbuflen);
		
			if(KErrNone != ret)
				{
				LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tWrite() -ALLOC Failed!"), this));
				/* In case of error a protocol is supposed to Error and then return 0 */
				iSessionControlNotify->Error(ret,MSessionControlNotify::EErrorSend);
				return 0;
				}
			}

		/* Copy the Data from RMbuf to Descriptor. Then Pass it to the RtpStack*/
		iRtpBuffer.SetLength(rmbuflen);
		aData.CopyOut(iRtpBuffer);
		rtpBufPtr.Set(&iRtpBuffer[0], rmbuflen, iRtpBuffer.MaxLength());
		}
	
	if(!NonRtpSendData())
		{
		iRtpSession->PacketBeingSent(rtpBufPtr, ERtp); 
		}
	//It is assumed that a Properly Created packet is passed down here.
	//Maybe we should handle a return code for PacketbeingSent function.
	if(aData.First()->Next())
		{
		aData.CopyIn(iRtpBuffer);
		}
		 
	__ASSERT_DEBUG(iSSPData,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	nwr = iSSPData->Write(aData, aOptions, anAddr);

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tWrite() Done nwr=%d"), this,nwr));	

	return nwr;
	
	}
 
 
void CRtpFlow::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage)
    {
	CRtpBaseFlow::ReceivedL(aSender,aRecipient,aMessage);

    LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tReceivedL() Msg %d"), this, aMessage.MessageId().MessageId()));

	if ( aMessage.IsMessage<Messages::TEBase::TError>() )
		{
        LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tGot TError from Sunconn"), this));
        HandleError(static_cast<Messages::TEBase::TError&>(aMessage).iValue);
		}	
    else if (Messages::TEChild::ERealmId == aMessage.MessageId().Realm())
        {
    	switch (aMessage.MessageId().MessageId())
        	{
   		case Messages::TEChild::TDestroy::EId :
    			{
            	Destroy();
            	break;
    			}
        	}
		}
	else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
	    switch (aMessage.MessageId().MessageId())
        	{
        	case TCFDataClient::TProvisionConfig::EId:
         		{	
    			TCFDataClient::TProvisionConfig& aMess = Messages::message_cast<TCFDataClient::TProvisionConfig>(aMessage);
				
				iAccessPointConfig.Close();
				iAccessPointConfig.Open(aMess.iConfig);
				
				const CRtpProvisionConfig& rtpProv = static_cast<const CRtpProvisionConfig&>(AccessPointConfig().FindExtensionL(STypeId::CreateSTypeId(KRtpProvisionUid, KRtpProvisionConfigType)));
				DoProvisionL(&rtpProv);
			
				/* Deafult Processing */
				HandleProvisionMessage(aMessage);				
        		break;
         		}
 
	        case TCFDataClient::TStart::EId :
        		{
	        	/* Check if we Have a Bearer. If we donot Have
	        	   complete when we Have one */
        		User::LeaveIfError(iStartRequest.Open(iSubConnectionProvider,aSender));
	        	if(iSSPData)
	        		{
	        		CompleteStart(KErrNone);
	        		}
	        	else
	        		{
	        		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tGot TDataClientStart no SP iIsStarting=>ETrue"), this));
	        		iIsStarting = ETrue;		
	        		}
	        	}
	            break;

	         case TCFDataClient::TStop::EId :
	         	{
	        	StopFlow(static_cast<TCFDataClient::TStop&>(aMessage));
	            break;
	         	}
	         
	         case TCFDataClient::TBindTo::EId :
				{
				TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));
                TRAPD(err,BindToL(bindToMsg));
                
                if(err)
                    {
                    Messages::RClientInterface::OpenPostMessageClose(Id(), aSender, Messages::TEBase::TError(TCFDataClient::TBindTo::Id(), err).CRef());
                    }
                else
                    {
                    Messages::RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
                    }
                
				//If we have received TDataClientStart before (when we did not yet have a bearer),
				//we complete the start here as well
				if (iIsStarting)
					{
					CompleteStart(err);
					}
				
				HandleBindToMessage(aMessage,err);	
                break;			
				}
 
        	}
		}
	else if (TCFControlProvider::ERealmId == aMessage.MessageId().Realm())
		{
		/* VRAMY : LOG Here!!! */
		}
	}


void CRtpFlow::DoProvisionL(const CRtpProvisionConfig *apRtpProv)    
	{
	iRtpSession->SetSessionBandwidth(apRtpProv->BandWidth());
	iRtpSession->SetRTPTimeConversion(apRtpProv->RtpTimeNow(),apRtpProv->RtptimeConversion());
	iRtpSession->SetRtpStreamParameters(apRtpProv->MaxDropOut(),
										apRtpProv->MaxMisorder(),
										apRtpProv->MinSequential());
	}

TInt CRtpFlow::GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr)
	{
	ASSERT(iSSPData);
	TInt ret;

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tGetData "), this ));

	ret = iSSPData->GetData(aData, aLength, aOptions, anAddr);
	if(ret < 0)
		{
		/* The Read failed. Just pass the error up */
		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tGetData failed (ret =%d)"), this));
		return ret;
		}
	
	/* Findout the amount of Data contained in the RMbuf Chain */
	TInt rmbuflen = aData.Length();
	TPtr8 rtpBufPtr(NULL, 0);
	
	if(!(aData.First()->Next()))
		{
		RMBuf *buffer = aData.First();
		__ASSERT_DEBUG(rmbuflen == buffer->Length() ,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicIncompleteBuffer));
		rtpBufPtr.Set(buffer->Ptr(), rmbuflen, rmbuflen);
		}
	else
		{
		if(iRtpBuffer.MaxLength() < rmbuflen)
			{
			iRtpBuffer.SetLength(0);
			ret = iRtpBuffer.ReAlloc(rmbuflen);
			if(KErrNone != ret)
				{
				return ret;
				}	
			}
	
		iRtpBuffer.SetLength(rmbuflen);
		aData.CopyOut(iRtpBuffer);
		rtpBufPtr.Set(&iRtpBuffer[0], rmbuflen, iRtpBuffer.MaxLength());
		}
	
	if(anAddr)
		{
		iRtpHandler->SetRecvAddress(*anAddr);
		}
	else
		{
		/* Connected Socket */
		__ASSERT_DEBUG(iConnected,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicIncorrectState));
		iRtpHandler->SetRecvAddress(iDestAddr);
		}
		
	TRAP(ret,iRtpSession->DoReceiveL(rtpBufPtr,ERtp));
	
	if((KErrNone == ret) && (aData.First()->Next()))
		{
		aData.CopyIn(iRtpBuffer);
		}

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tGetData()) done ret=%d"), this,ret));

	return ret;
	}


void CRtpFlow::SendData(TDes8& , TSockAddr &, TInt , TRequestStatus  &aStatus)
	{
		
		TRequestStatus *stat = &aStatus;
		User::RequestComplete(stat,KErrNone);
	}
	

void CRtpFlow::NewData(TUint aCount)
	{
	__ASSERT_DEBUG(iSessionDataNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpFlowSubTag,_L8("CRtpFlow %08x:\tNewData()"), this));

	CRtpBaseFlow::NewData(aCount);
	}
	

/* When an RTCP flow is created call this function to enable RTCP in
   RtpSession. The function will return a Pointer to RTPSession which
   can be passed on to RTCP Flow */
CRtpController* CRtpFlow::GetRtpSession() const
	{
	return 	iRtpSession;
	}

void CRtpFlow::DoCanClose(MSessionControlNotify::TDelete )
/**
Called from protocol to indicate that a graceful close has completed
*/
	{
	/* RTP Session is no longer Valid and Can be Closed now */
	/* OOPS no API to close the RtpSession :(*/
	}


void CRtpFlow::DoUpdateLocalAddress(const TSockAddr& anAddr)
/* Called by BaseFlow to notify that Local address has been 
   Updated. Ideally called only once */
	{
	iRtpSession->SetLocalAddress(anAddr);
	}

TInt CRtpFlow::DoGetOption(TUint level, TUint name, TDes8 &aOption) const
	{
	/* Socket option for RTP */
	if(KSolRtp == level)
		{
		switch(name)
			{
			case KSoLocalSSRC:
				{
				TUint locSSRC = iRtpSession->GetLocalSSRC();
				CopyOptionToDesc(aOption,locSSRC);
				break;
				}
			default:
				{
				return KErrNotSupported;
				}
			}
		return KErrNone;
		}

	if(iSSP)
	 return iSSP->GetOption(level,name,aOption);
	return KErrNone;
	}

TInt CRtpFlow::DoSetOption(TUint level, TUint name, const TDesC8 &aOption)
	{
	
	/* Socket option for RTP */
	if(KSolRtp == level)
		{
		TInt ret = KErrNone;
		switch(name)
			{
			case KSoRtpBandwidth:
				{
				TInt optInt=0;
				GETOPTION_OR_RETURN(aOption,optInt);
				iRtpSession->SetSessionBandwidth(optInt);
				break;
				}
			case KSoStreamParams:
				{
				TRtpStreamParams strmParams;
				GETOPTION_OR_RETURN(aOption,strmParams);
				iRtpSession->SetRtpStreamParameters(strmParams.iMaxDropout,
						                            strmParams.iMaxMisorder,
						                            strmParams.iMinSequential);
				break;
				}

			case KSoRtpTimeConvertion:
				{
				TRtpTimeConversion rtpTimeConv;
				GETOPTION_OR_RETURN(aOption,rtpTimeConv);
				iRtpSession->SetRTPTimeConversion(rtpTimeConv.iRtpTimeNow,
												  rtpTimeConv.iConversion);
				break;
				}
			
			case KSoSendNonRtpData:
				{
				TInt nonRtpData = 0;
				GETOPTION_OR_RETURN(aOption,nonRtpData);
				SetNonRtpSendData(nonRtpData);
				break;	
				}
				
			case KSoReceiveNonRtpData:
				{
				TInt nonRtpData = 0;
				GETOPTION_OR_RETURN(aOption,nonRtpData);
				iRtpSession->SetNonRtpData(nonRtpData);
				break;	
				}
			case KSoSamplingRate:
				{
				TSamplingRateInfo sampRateInfo;
				GETOPTION_OR_RETURN(aOption,sampRateInfo);
				ret = iRtpSession->SetSamplingRate(sampRateInfo.iPayloadType,
											 sampRateInfo.iSamplingRate);
				break;
				}

			default:
				{
				return KErrNotSupported;
				}
			}
		return ret;
		}
	if(iSSP)
		return iSSP->SetOption(level,name,aOption);
	return KErrNone;
	}
	
void CRtpFlow::SetNonRtpSendData(TInt aNonRtpData)
	{
	iNonRtpSendData = aNonRtpData;
	}
	
TInt CRtpFlow::NonRtpSendData()
	{
	return iNonRtpSendData;
	}


