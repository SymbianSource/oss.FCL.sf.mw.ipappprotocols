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
// RTCPFlow Implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>

#include "rtcpflow.h"
#include "rtppint.h"

#include "rtp_cfmessages.h"

#if defined(ESOCK_LOGGING_ACTIVE)
_LIT8(KRtcpFlowSubTag, "RtcpFlow");
#endif
 									
CRtcpFlow::CRtcpFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
:CRtpBaseFlow(aFactory, aSubConnId, aProtocolIntf)
{
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8(" CRtcpFlow %08x:\t "), this));
	LOG_NODE_CREATE(KESockFlowTag, CRtcpFlow);
}


CRtcpFlow* CRtcpFlow::NewL(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf)
	{
	CRtcpFlow *pRtcpFlow = new (ELeave) CRtcpFlow(aFactory, aSubConn, aProtocolIntf);
	
	CleanupStack::PushL(pRtcpFlow);
	pRtcpFlow->ConstructL();
	/* Register the flow with Rtp Pint */
	((CRtpProtocolIntf*)aProtocolIntf)->RegisterSubConnProvIDL(pRtcpFlow,aSubConn);
	CleanupStack::Pop();
	
	return pRtcpFlow;
	}

void CRtcpFlow::ConstructL()
	{
	iRtcpHandler = CCFRtpHandler::NewL(this,EPriorityNormal);
	iRtcpHandler->SetType(ERtcp);
	iRtcpBuffer.CreateL(KRtcpDefBufSz);
	iRtcpTempBuffer.CreateL(KRtcpDefBufSz);
	}


CRtcpFlow::~CRtcpFlow()
	{
	iRtcpBuffer.Close();
	iRtcpTempBuffer.Close();
	LOG_NODE_DESTROY(KESockFlowTag, CRtcpFlow);
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8(" ~CRtcpFlow %08x:\t "), this));
	}
	
void CRtcpFlow::CanSend()
	{
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t CanSend() "), this));
	if(!iBlockedWrite)
		{
		/* This can happen if after doing a write any buffer allocations
		   failed or an internal error happened. Just return and in the
		   next timeout will try writing again */
		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t CanSend() iBlockedWrite is false"), this));
		return;
		}
		
	TInt nwr = -1;
	/* Alloc the RMBuff Chain. The allocation might fail 
	   because there are not enough MBufs. This needs to be
	   handled */
	TInt ret = iRtcpDataChain.Alloc(iRtcpTempBuffer.Size()); 
	if(ret == KErrNone)
		{
		iRtcpDataChain.CopyIn(iRtcpTempBuffer);
		if(iSSPData)
			{
			nwr = iSSPData->Write(iRtcpDataChain, 0 , &iDestAddr);	
			}
		if(nwr == 0)
			{
			/* This will be a Strange Behaviour because we are in cansend() */
    		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t CanSend() nwr is 0"), this));	
			/* Free the MBuf */
			iRtcpDataChain.Free();
			return; /* IsBlocked is still ture */
			}
		iRtcpDataChain.Free();
		}
	iBlockedWrite = EFalse;
	return;	
	}

TInt CRtcpFlow::Write(RMBufChain& aData, TUint, TSockAddr* anAddr)
	{
	TInt nwr = -1;
		
	/* Findout the amount of Data contained in the RMbuf Chain */
	TInt rmbuflen = aData.Length();
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t Write() Len %d"), this, rmbuflen));
	if(iRtcpBuffer.MaxLength() < rmbuflen)
		{
		
		TInt ret;
		iRtcpBuffer.SetLength(0);
		ret = iRtcpBuffer.ReAlloc(rmbuflen);
		
		if(KErrNone != ret)
			{
			/* In case of error a protocol is supposed to Error and then return 0 */
			LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t Write() Alloc failed!"), this));
			iSessionControlNotify->Error(ret,MSessionControlNotify::EErrorSend);
			return 0;
			}
		}

	/* Copy the Data from RMbuf to Descriptor. Then Pass it to the RtpStack */
	iRtcpBuffer.SetLength(rmbuflen);
	aData.CopyOut(iRtcpBuffer);
	
	/* The responsibility of the Upper Layer ends here. */
	if(!NonRtcpSendData())
		{
		TRAPD(errPack,iRtpSession->ProcessRTCPRequestL(iRtcpBuffer));
		if(KErrNone != errPack)
			{
	 		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\tSendRTCPPacketL Leave with code %d"), this, errPack));
			iSessionControlNotify->Error(errPack,MSessionControlNotify::EErrorSend);
			return errPack;
			}	
		}
	else
		{
		TRequestStatus stat;
		stat = KRequestPending;
		if(anAddr)
			{
			SendData(iRtcpBuffer, *anAddr, 0, stat);
			}
		else
			{
			SendData(iRtcpBuffer, iRtcpHandler->DefaultRemoteAddress(), 0, stat);
			}
		User::WaitForRequest(stat);
		//Errors are ingnored
		}
		
	/* Uncomment this to bypass RTP Stack */ 
	//SendData(*anAddr, iRtcpBuffer); 
		
 	
	nwr = 1; //Always Success.	
    
	/* Correct Protocol behaviour is to free this data even though CSocket frees
	 * it. */
    aData.Free();	
	return nwr;
	}


void CRtcpFlow::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage)
    {    
	CRtpBaseFlow::ReceivedL(aSender,aRecipient,aMessage);
	
	if ( aMessage.IsMessage<Messages::TEBase::TError>() )
		{
        LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\tGot TError from Sunconn"), this));
        HandleError(static_cast<Messages::TEBase::TError&>(aMessage).iValue);
		}	
	else if (Messages::TEChild::ERealmId == aMessage.MessageId().Realm())
        {  	
        switch (aMessage.MessageId().MessageId())
        	{        	
        	case Messages::TEChild::TDestroy::EId:
                Destroy();
                break;
        	}

        }
	else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{	
        switch (aMessage.MessageId().MessageId())
        	{
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
            		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\tGot TDataClientStart no SP iIsStarting=>ETrue"), this));
            		iIsStarting = ETrue;		
            		}
            	}
                break;
                
            case TCFDataClient::TStop::EId :
            	{
            	StopFlow(static_cast<TCFDataClient::TStop&>(aMessage));
				LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t DataClientStopped"), this));
                break;
            	}
            
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
            
            case TCFDataClient::TBindTo::EId :
				{
				TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));
                TRAPD(err,BindToL(bindToMsg));
                
                if(err)
                    {
                    Messages::RClientInterface::OpenPostMessageClose(Id(), aSender,  Messages::TEBase::TError(TCFDataClient::TBindTo::Id(), err).CRef());
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
        switch (aMessage.MessageId().MessageId())
        	{
        	}
		}
    }
    

void CRtcpFlow::DoProvisionL(const CRtpProvisionConfig *apRtpProv)    
	{
	
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\tDoProvisionL()"), this));	
	
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow: \t RtpTimeNow=%d RtpTimeConv=%d "),apRtpProv->RtpTimeNow(),apRtpProv->RtptimeConversion()));	 

	/* Now that the Provision is available. Bring the RTCP Stack up. */
	// RTCP given more priority .. Verify :=@
	iRtpSession->CreateRtcpL(iRtcpHandler,apRtpProv->GetCNAME());
	
	iRtpSession->SetSessionBandwidth(apRtpProv->BandWidth());
	iRtpSession->SetRTPTimeConversion(apRtpProv->RtpTimeNow(),apRtpProv->RtptimeConversion());
	
	/* Auto Send Option */
	iRtpSession->SetAutoSendOption(apRtpProv->RtpAutoSend());
	
	TInetAddr destAddr = apRtpProv->GetDefDestinationAddr();
	destAddr.SetPort(destAddr.Port()+1);
	iRtcpHandler->SetDefaultRemoteAddress(destAddr);
	
	/* Report Interval */
	iRtpSession->SetRtcpInterval(apRtpProv->GetRtcpReportInterval());
	
	}

TInt CRtcpFlow::GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr)
	{
	ASSERT(iSSPData);
	TInt ret;

    LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\tGetData()"), this));	

	ret = iSSPData->GetData(aData, aLength, aOptions, anAddr);
	if(ret < 0)
		{
		/* The Read failed. Just pass the error up */
		return ret;
		}
	
	/* Findout the amount of Data contained in the RMbuf Chain */
	TInt rbuflen = aData.Length();

	if(iRtcpBuffer.MaxLength() < rbuflen)
		{
		TInt ret;
		iRtcpBuffer.SetLength(0);
		ret = iRtcpBuffer.ReAlloc(rbuflen);

		if(KErrNone != ret)
			{
			return ret;
			}	
		}
	
	/* Copy the Data from RMbuf to Descriptor. Then Pass it to the RtpStack*/
	iRtcpBuffer.SetLength(rbuflen);		
	aData.CopyOut(iRtcpBuffer);
	if(anAddr)
		{
		iRtcpHandler->SetRecvAddress(*anAddr);
		}
	else
		{
		/* Connected Socket */
		if(!iConnected)
			{
			return KErrArgument;
			}
		iRtcpHandler->SetRecvAddress(iDestAddr);
		}
	
	TRAP(ret,iRtpSession->DoReceiveL(iRtcpBuffer,ERtcp))
	if(KErrNone == ret)
		{
		aData.CopyIn(iRtcpBuffer);
		}
		
	return ret;
	}

/* RTP Stack calls this function when ever it needs to send
   an RTCP report */
void CRtcpFlow::SendData(TDes8& aBuffer, TSockAddr &aAddr, TInt , TRequestStatus  &aStatus)
	{
		
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t RtcpSend()"), this));
	/* See if a Write is already pending. If it is then just update the buffered data with the new report
	   and return */
	 if(iBlockedWrite)
	 	{
		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t RtcpSend() Write Blocked already-OVERWRITE"), this));
	 	HandleWriteBlocked(aAddr, aBuffer);
		TRequestStatus *pStat = &aStatus;
		User::RequestComplete(pStat,KErrNone);
	 	return;
	 	}
	 
	
	TInt nwr = -1;
	/* Alloc the RMBuf Chain. The allocation might fail 
	   because there are not enough MBufs. 
	   [TO BE DONE] This needs to be handled */
	TInt ret = iRtcpDataChain.Alloc(aBuffer.Size()); 
	if(ret == KErrNone)
		{
		iRtcpDataChain.CopyIn(aBuffer);
		if(iSSPData)
			{
			nwr = iSSPData->Write(iRtcpDataChain, 0 /* Shud be ok */, &aAddr);	
			}
		else
			{
			LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t RtcpSend(): iSSPData is NULL (Flow was Stopped??"), this));
			}
			
		if(nwr == 0)
			{
			LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t RtcpSend(): WriteBlocked"), this));
			/* Flow not able to accept our data. Set the blocked flag and
			   make a temporary copy */
	 		 HandleWriteBlocked(aAddr, aBuffer);
			 }
		/* Free the Descriptor */
		iRtcpDataChain.Free();
		}
	else
		{
		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t RtcpSend(): RmbufChain Alloc Failed"), this));
		}
	TRequestStatus *pStat = &aStatus;
	User::RequestComplete(pStat,KErrNone);
	return;
	}
	
void CRtcpFlow::SetRtpSession(CRtpController *apRtpSession)
	{
	iRtpSession = apRtpSession;
	//iRtpSession->CreateRtcpL();
	}

void CRtcpFlow::NewData(TUint aCount)
	{
	__ASSERT_DEBUG(iSessionDataNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t NewData(): Count %d"), this, aCount));
	iSessionDataNotify->NewData(aCount);
	}
	
/* Buffer the data and Set the Blocked flag to True when a write failure
   happens */
void CRtcpFlow::HandleWriteBlocked(TSockAddr& aAddr,TDes8& aBuffer)
	{
	TInt ret;
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtcpFlowSubTag,_L8("CRtcpFlow %08x:\t HandleWriteBlocked()"), this));
	if(iRtcpTempBuffer.MaxLength() < aBuffer.Length())
		{
		iRtcpTempBuffer.SetLength(0);
		ret = iRtcpTempBuffer.ReAlloc(aBuffer.Length());
		if(KErrNone != ret)
			{
			/* donot do anything and just return. Write is already blocked
			   in CanSend() isBlockedWrite flag will not be True and normal
			   flow will be resumed */
			return;
			}
		}
	iBlockedWrite = ETrue;
	iRtcpTempBuffer = aBuffer;
	iDestAddr = aAddr;
	}
	
void CRtcpFlow::DoCanClose(MSessionControlNotify::TDelete )
/**
Called from protocol to indicate that a graceful close has completed
*/
	{
	/* There is nothing much to Do here for RTCP. The Cleaning up will be done
	   by the base class. The RTPSession is owned by the RTPFlow and the Cleaning will
	   be handled from there */
	return;
	}

void CRtcpFlow::DoUpdateLocalAddress(const TSockAddr& )
/* Called by BaseFlow to notify that Local address has been 
   Updated. Ideally called only once */
	{
	/* Not handled Now */
	}


TInt CRtcpFlow::DoGetOption(TUint level, TUint name, TDes8 &aOption) const
	{
	TInt err = KErrNone;
	/* Socket option for RTP */
	if(KSolRtcp == level)
		{
		switch(name)
			{
			case KSoRtcpAutoSend:
				{
				TInt rtcpAutoSend = iRtpSession->AutoSendOption();
				CopyOptionToDesc(aOption,rtcpAutoSend);
				break;
				}
			case KSoRtcpStatistics:
				{
				/* The SSRC is specified in the Params structue.*/
				TRtcpStatisticsParams rtcpStatistics;
				GETOPTION_OR_RETURN(aOption,rtcpStatistics);
				err = iRtpSession->RtcpStatistics(rtcpStatistics.iSsrc,rtcpStatistics);
                if(KErrNone == err)
					{
					CopyOptionToDesc(aOption,rtcpStatistics);
					}
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

TInt CRtcpFlow::DoSetOption(TUint level, TUint name, const TDesC8 &aOption)
	{
	TInt err = KErrNone;
	/* Socket option for RTP */
	if(KSolRtcp == level)
		{
		switch(name)
			{
			case KSoRtcpAutoSend:
				{
				TInt optInt=0;
				GETOPTION_OR_RETURN(aOption,optInt);
				iRtpSession->SetAutoSendOption(optInt);
				break;
				}
			case KSoRtcpTimeInterval:
				{
				TTimeIntervalMicroSeconds32 timeInterval;
				GETOPTION_OR_RETURN(aOption,timeInterval);
				iRtpSession->SetRtcpInterval(timeInterval);
				break;
				}			
			case KSoRtcpCNAME:
				{
				TRAPD(err,iRtpSession->CreateRtcpL(iRtcpHandler,aOption));
				return err;
				}
			case KSoDefaultDestAddr:
				{
				TSockAddr destAddr;
				GETOPTION_OR_RETURN(aOption,destAddr);
				iRtcpHandler->SetDefaultRemoteAddress(destAddr);
				break;
				}
				
			case KSoSendNonRtcpData:
				{
				TBool nonRtcpData = EFalse;
				GETOPTION_OR_RETURN(aOption,nonRtcpData);
				SetNonRtcpSendData(nonRtcpData);
				break;	
				}
				
			case KSoReceiveNonRtcpData:
				{
				TInt nonRtcpData = 0;
				GETOPTION_OR_RETURN(aOption,nonRtcpData);
				iRtpSession->SetNonRtcpData(nonRtcpData);
				break;	
				}
			default:
				{
				return KErrNotSupported;
				}
			}
		return err;
		}

	if(iSSP)
		return iSSP->SetOption(level,name,aOption);
	return err;
	}
	
void CRtcpFlow::SetNonRtcpSendData(TBool aNonRtcpData)
	{
	iNonRtcpSendData = aNonRtcpData;
	}
	
TBool CRtcpFlow::NonRtcpSendData()
	{
	return iNonRtcpSendData;
	}
