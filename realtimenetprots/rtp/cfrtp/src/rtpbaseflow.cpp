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
// RTP Base Flow Implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_logext.h>

#include <comms-infras/ss_protflow.h>

#include "rtpbaseflow.h"
#include "rtp_cfmessages.h"

#if defined(ESOCK_LOGGING_ACTIVE)
_LIT8(KRtpBaseFlowSubTag, "RtpbaseFlow");
#endif

ESock::MSessionControl* CRtpBaseFlow::GetControlL(TInt aSessionType,ESock::MSessionControlNotify& 
												aSessionControlNotify)
	{
	if(aSessionType != KSockDatagram)
		{
		User::Leave(KErrNotSupported);
		}
	iSessionControlNotify = &aSessionControlNotify;
	return this;
	}

ESock::MSessionData* CRtpBaseFlow::BindL(ESock::MSessionDataNotify& aSessionDataNotify)
	{
	
	ASSERT(!iSessionDataNotify);
	iSessionDataNotify = &aSessionDataNotify;
	iSubConnectionProvider.PostMessage(Id(),TCFControlProvider::TActive().CRef());
	return this;	
	}

void CRtpBaseFlow::Unbind()
	{
	iSessionControlNotify =NULL;
	iSessionDataNotify = NULL;

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Unbind() "), this));

	/* The Upper flow has left. ASSERT if we still have a Bearer */
	/* By this time we should have called UnBind() on the lower flow.
	   so something went wrong(??) */
	ASSERT(!iSSP && !iFlowBinder);

    ProcessDCIdleState();	

	if(iRecvdImmediateShutDown)
		{
		Destroy(); //BANG BANG!!
		}
	else
		{
		/* Once Defect with HandleDataClients is fixed uncomment this line! */
		//ProcessDCIdleState();
		}	
	}
									
CRtpBaseFlow::CRtpBaseFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
: CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf)
	{
	}

 
ESock::CSubConnectionFlowBase* CRtpBaseFlow::Flow()
/**
Return the Flow corresponding to the MFlowBinderControl
*/
	{	
	return this;
	}
	

ESock::CSubConnectionFlowBase& CRtpBaseFlow::CloneFlowL()
/**
Return the Flow corresponding to the MFlowBinderControl
*/
	{
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t CloneFlowL() "), this));

   	ASSERT(iProtocolIntf);
    TDefaultFlowFactoryQuery query (iProtocolIntf->ControlProviderId(), iSubConnectionProvider.RecipientId());
	ESock::CSubConnectionFlowBase& flow = static_cast<ESock::CSubConnectionFlowBase&>(*(Factory().CreateObjectL(query)));

	return flow;
	}	
	
CRtpBaseFlow::~CRtpBaseFlow()
	{
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t ~CRtpBaseFlow() "), this));
	}

// CSubConnectionFlowBase
// MCFNode 
    
void CRtpBaseFlow::ProcessDCIdleState()
	{
	iSubConnectionProvider.PostMessage(Id(),TCFControlProvider::TIdle().CRef());
	}
void CRtpBaseFlow::CompleteStart(TInt aError)
	{
	if (aError==KErrNone)
		{
		iStartRequest.ReplyTo(Id(), TCFDataClient::TStarted().CRef());
		}
	else
		{
		iStartRequest.ReplyTo(Id(), Messages::TEBase::TError(TCFDataClient::TStart::Id(),aError).CRef());
		}
	iIsStarting = EFalse;
	}
		
	
ESock::MFlowBinderControl* CRtpBaseFlow::DoGetBinderControlL()
	{		
    return this;
	}

void CRtpBaseFlow::BindToL(TCFDataClient::TBindTo& aBindTo)
	{
	__ASSERT_DEBUG(!iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	
	const Messages::TNodeId& commsId = aBindTo.iNodeId;
	
#if defined(__GCCXML__)
    CSubConnectionFlowBase* flow = reinterpret_cast<CSubConnectionFlowBase*>(reinterpret_cast<Messages::ANode*>(commsId.Ptr()));
#else
    CSubConnectionFlowBase* flow = mcfnode_cast<CSubConnectionFlowBase>(reinterpret_cast<Messages::ANode*>(commsId.Ptr()));
#endif
   if (!flow)
        {
        iSubConnectionProvider.PostMessage(Id(),TCFDataClient::TStopped(KErrDisconnected).CRef());
        User::Leave(KErrArgument);
        }
     
     if (iFlowBinder && iFlowBinder->Flow() != flow )
    	{
    	//already bound -> unbind first.
    	iFlowBinder->Unbind();
    	iFlowBinder = NULL;
    	}
    	
    if (!iFlowBinder)
        {
        iFlowBinder = flow->GetBinderControlL();
      	iSSP = iFlowBinder->GetControlL(KSockDatagram, *this);
     	iSSPData = iFlowBinder->BindL(*this);
        }
 	}

void CRtpBaseFlow::StopFlow(TCFDataClient::TStop& aMessage)
	{

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t StopFlow() "), this));

	/* We shud UnBind from the lower flow ? */
	if (iFlowBinder)
		{
		iFlowBinder->Unbind();
		// Flowbinder takes care of itself from now onwards; forget all downwards pointers
		iSSP = NULL;
		iSSPData = NULL;
		iFlowBinder = NULL;
		iServiceProvider.Close();
		}
	/* Notify data Client is Stopped */
    iSubConnectionProvider.PostMessage(Id(),TCFDataClient::TStopped(aMessage.iValue).CRef());
	}
	
void CRtpBaseFlow::Destroy()
	{
 	DeleteThisFlow();
	}
	
// MSessionControl
void CRtpBaseFlow::Shutdown(MSessionControl::TCloseType , const TDesC8& )
	{
	
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Shutdown() Invalid Path PANIC!"), this));	

	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}

void CRtpBaseFlow::Shutdown(MSessionControl::TCloseType aOption)
	{
	
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Shutdown() "), this));

	if(iSSP)
		{
		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Calling Shutdown() - On iSSP"), this));
		iSSP->Shutdown(aOption);
		}
	
	if(MSessionControl::EImmediate == aOption)
		{
		/* So the lower layer will not call CanClose(). Handle everything here
		 * */
		LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Calling Shutdown() MSessionControl::EImmediate"), this));	
		if (iFlowBinder)
			{
			iFlowBinder->Unbind();
			}
		
		// Flowbinder takes care of itself from now onwards; forget all downwards pointers
		iSSP = NULL;
		iSSPData = NULL;
		iFlowBinder = NULL;
		iServiceProvider.Close();
		iRecvdImmediateShutDown = ETrue;
		}
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Shutdown() Done!"), this));
	}

void CRtpBaseFlow::ActiveOpen()
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath)); 
	}

void CRtpBaseFlow::ActiveOpen(const TDesC8& )
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath)); 
	}
	
TInt CRtpBaseFlow::PassiveOpen(TUint )
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath)); 
	return KErrNotSupported;
	}
	
TInt CRtpBaseFlow::PassiveOpen(TUint ,const TDesC8& )
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath)); 
	return KErrNotSupported;
	}

void CRtpBaseFlow::AutoBind()
	{

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t AutoBind() "), this));
	
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	iSSP->AutoBind();
	
	/* Retrive the local address and notify the flows */
	TSockAddr addr;
	iSSP->LocalName(addr);
	DoUpdateLocalAddress(addr);
	}
	
void CRtpBaseFlow::LocalName(TSockAddr& anAddr) const
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider)); 
	iSSP->LocalName(anAddr);
	}

#ifndef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
TInt CRtpBaseFlow::SetLocalName(TSockAddr& anAddr)
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	TInt ret;
	ret = iSSP->SetLocalName(anAddr);
	if(KErrNone == ret)
		{
		DoUpdateLocalAddress(anAddr);
		}
	return ret;
	}
#else
void  CRtpBaseFlow::SetLocalName(TSockAddr& anAddr)
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	
	iLocalAddress = anAddr;
	(void)iSSP->SetLocalName(anAddr);
	
	
	}
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING

void CRtpBaseFlow::RemName(TSockAddr& anAddr) const
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	iSSP->RemName(anAddr);
	return;
	}

TInt CRtpBaseFlow::SetRemName(TSockAddr& anAddr)
	{
	
	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t SetRemName() "), this));	

	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	/* "Normal" UDP Sockets can be connected over and over again. We should disable that because
	   this donot have any sense for RTP */
	if(iConnected)
		{
		return KErrNotSupported;
		}
	
	TInt ret = iSSP->SetRemName(anAddr);
	if(KErrNone == ret)
		{
		iDestAddr  =  anAddr;
		iConnected = ETrue;
		}
	return ret;
	}
	
void CRtpBaseFlow::ConnectComplete() 
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}
	
void CRtpBaseFlow::ConnectComplete(const TDesC8& ) 
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}
	
void CRtpBaseFlow::ConnectComplete(CSubConnectionFlowBase& ) 
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}
	
void CRtpBaseFlow::ConnectComplete(CSubConnectionFlowBase& ,const TDesC8& )
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}

TInt CRtpBaseFlow::Error(TInt anError,TUint anOperationMask)
	{
	TInt ret = KErrNone;

	LOG( ESockLogExternal::Printf(KESockFlowTag,KRtpBaseFlowSubTag,_L8("CRtpBaseFlow %08x:\t Error() "), this));	

	/* Error message From down or flow error */
	if(iSessionControlNotify)
		{
		ret = iSessionControlNotify->Error(anError,anOperationMask);
		if (KErrNone != ret)
	    	{
	    	if (iFlowBinder)
				{
				iFlowBinder->Unbind();
				}
			// Flowbinder takes care of itself from now onwards; forget all downwards pointers
			iSSP = NULL;
			iSSPData = NULL;
			iFlowBinder = NULL;
			iServiceProvider.Close();
	    	}
		}
	return ret;
	}
		
void CRtpBaseFlow::Disconnect()
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}
	
void CRtpBaseFlow::Disconnect(TDesC8& ) 
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}
	
TInt CRtpBaseFlow::SecurityCheck(MProvdSecurityChecker* aSecurityChecker)
	{ 
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
    /* Pass on the Security check Params to DownStream */
    return iSSP->SecurityCheck(aSecurityChecker); 
	}
	 
TInt CRtpBaseFlow::GetOption(TUint level, TUint name, TDes8 &anOption) const
	{ 
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	return DoGetOption(level,name,anOption);
	}
	
TInt CRtpBaseFlow::SetOption(TUint level, TUint name, const TDesC8 &anOption)
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	return DoSetOption(level, name, anOption);
	}
	
void CRtpBaseFlow::Ioctl(TUint level, TUint name, TDes8 *anOption)
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	iSSP->Ioctl(level,name,anOption);
	return;
	}
	
void CRtpBaseFlow::CanSend()
	{
	__ASSERT_DEBUG(iSessionDataNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));
	iSessionDataNotify->CanSend();
	}

void CRtpBaseFlow::IoctlComplete(TDesC8* aBuf)
	{
	__ASSERT_DEBUG(iSessionControlNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));
	iSessionControlNotify->IoctlComplete(aBuf);
	}
	 
void CRtpBaseFlow::CancelIoctl(TUint aLevel, TUint aName)
	{
	__ASSERT_DEBUG(iSSP,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoServiceProvider));
	iSSP->CancelIoctl(aLevel,aName);
	return;
	}

void CRtpBaseFlow::Start()
	{
	return;
	}

// MSessionData
TUint CRtpBaseFlow::Write(const TDesC8& ,TUint ,TSockAddr* )
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath)); 
	return 0;	
	}

void CRtpBaseFlow::GetData(TDes8& ,TUint ,TSockAddr* )
	{
	__ASSERT_DEBUG(0,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicUnexpectedExecutionPath));
	}

void CRtpBaseFlow::NewData(TUint aCount)
	{
	__ASSERT_DEBUG(iSessionDataNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));
	iSessionDataNotify->NewData(aCount);
	}
	
void CRtpBaseFlow::CanClose(MSessionControlNotify::TDelete aDelete)
/**
Called from protocol to indicate that a graceful close has completed
*/
	{
	
	DoCanClose(aDelete);

	if (iFlowBinder)
		{
		iFlowBinder->Unbind();
		}
		
	// Flowbinder takes care of itself from now onwards; forget all downwards pointers
	iSSP = NULL;
	iSSPData = NULL;
	iFlowBinder = NULL;
	iServiceProvider.Close();
	
	/* Now pass this up to the ControlProvider */
	iSessionControlNotify->CanClose(aDelete);
	}

void CRtpBaseFlow::CanClose(const TDesC8& , MSessionControlNotify::TDelete aDelete)
/**
Called from protocol to indicate that a graceful close has completed with disconnect data
*/
	{
	/* Will we ever reach here? */
	CanClose(aDelete);
	}
	

TInt CRtpBaseFlow::HandleProvisionMessage(Messages::TSignatureBase&)
/* Helper funtion that will send a No Bearer and save the Activity ID */
	{
	/* The custom message TRtpMessageReadyForBind should be send to the SCPR once we get
    the response for NoNearer. The RtpScprBinderRequest activity will be waiting for this
    request */
	iSubConnectionProvider.PostMessage(Id(),TCFControlProvider::TNoBearer().CRef());
		
	iWaitingForBindTo = ETrue;
	
	return KErrNone;
	}


TInt CRtpBaseFlow::HandleBindToMessage(Messages::TSignatureBase& , TInt aErrCode)
/* Creates and Sends a provision Done message with the Error Code */
	{
	/* Got the Bearer. Finish off the BinderRequest acitivity  */
	TRtpMessages::TRtpMessageReadyForBind provDoneMsg(aErrCode);
	iSubConnectionProvider.PostMessage(Id(),provDoneMsg);
	iWaitingForBindTo = EFalse;
	return KErrNone;
	}


TInt CRtpBaseFlow::HandleError(TInt aErrorCode)
/* Handles a TError Message from SubConnection */
	{
	/* Check if we were waiting for BindToMessage. If we were waiting then
	   send a failure to Provision Done Message. This will free up the SCPR
	   activity socket open will fail */
	if(iWaitingForBindTo)
		{
		TRtpMessages::TRtpMessageReadyForBind provDoneMsg(KErrCompletion);
		iSubConnectionProvider.PostMessage(Id(),provDoneMsg);
		iWaitingForBindTo = EFalse;
		/* At this point we shud not have a CP shud we be doing this? */
		Error(aErrorCode);
		/*The bearer has failed. So kill ourselves*/
		Destroy();
		}
	else 
		{
		Error(aErrorCode);
		}
	return KErrNone;
	}

#ifdef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
void CRtpBaseFlow::SetLocalNameComplete()
{
  __ASSERT_DEBUG(iSessionControlNotify,User::Panic(KRtpFlowPanic, RtpFlowPanics::KPanicNoControlProvider));
  DoUpdateLocalAddress(iLocalAddress);
  iSessionControlNotify->SetLocalNameComplete();
 }
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDIN

