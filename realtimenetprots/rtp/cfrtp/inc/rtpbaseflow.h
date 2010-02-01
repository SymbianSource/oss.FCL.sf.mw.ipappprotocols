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
// RTP Base Flow definitions.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef CRTPBASEFLOW_H
#define CRTPBASEFLOW_H

#include <e32base.h>
#include <e32std.h>

#include <comms-infras/ss_factorycontainermap.h>
#include <comms-infras/cfperfmetrics.h>
#include <comms-infras/ss_subconnflow.h>
#include <cs_subconparams.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_protflow.h>
#include <comms-infras/nifif.h>
#include <comms-infras/ss_nodemessages.h>
#include <in_sock.h>
#include <comms-infras/commsdebugutility.h>
#include <es_mbuf.h>

#include "cfrtphandler.h"

using namespace ESock;
class ESock::CSubConnectionFlowBase;

#if defined(_DEBUG) || defined(__RTP_LOG_ACTIVE)
 #define KRtpFlowTag KCFNodeTag
_LIT8(KRtpBaseFlowTag, "RtpBaseFlow");
#endif


_LIT (KRtpFlowPanic,"RtpFlowPanic");

namespace RtpFlowPanics
{
enum
	{
	KPanicNoServiceProvider = 1,
	KPanicNoControlProvider = 2,
	KPanicNoDataClient = 2,

	KPanicIncorrectMessage = 3,
	
	KPanicUnexpectedExecutionPath =4,
	KPanicIncorrectState = 5,
	KPanicPeerMisbehaving = 6,
	KPanicIncompleteBuffer = 7,
	};
}
//Fetches out an integer option. Returns if error.
#define GETOPTION_OR_RETURN(optDESC,optVal) do{TInt optRet; optRet = GetOptionFromDesc(optDESC,optVal); \
	                                                               if(KErrNone != optRet)return optRet;}while(0)

class CRtpBaseFlow :public ESock::CSubConnectionFlowBase, public ESock::MFlowBinderControl,
				    public MSessionControlNotify, MSessionDataNotify,ESock::MSessionData,ESock::MSessionControl,public MRtpDataSender
    /** CRtpBaseFlow is the base calss for both RTP and RTCP Flows

	@internalComponent 
	
	**/				    
	{
	
	friend class CRtpFlowFactory;

	public:
	
	// Handling SCPR messages
	void StopFlow(TCFDataClient::TStop& aMessage);
	void BindToL(TCFDataClient::TBindTo& aBindTo);
	void Destroy();

	void ProcessDCIdleState();
	void CompleteStart(TInt aError);

	TInt iIsStarting;
	TUint iStartActivityId:16;
	

	//CSubConnectionFlowBase	
 	virtual MFlowBinderControl* DoGetBinderControlL();
	
	// MFlowBinderControl Methods
	 virtual ESock::MSessionControl* GetControlL(TInt aSessionType, ESock::MSessionControlNotify& aSessionControlNotify);
	 virtual ESock::MSessionData* BindL(ESock::MSessionDataNotify& aNotify);
	 virtual void Unbind();
	 virtual CSubConnectionFlowBase& CloneFlowL();
	 virtual CSubConnectionFlowBase* Flow();
	 
	// APIs From MSessionControl
	public:
	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueueSize);
	virtual TInt PassiveOpen(TUint aQueueSize,const TDesC8& aConnectionData);
	virtual void AutoBind();
	virtual void LocalName(TSockAddr& anAddr) const;
#ifndef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
	virtual TInt SetLocalName(TSockAddr& anAddr);
#else
	virtual void SetLocalName(TSockAddr& anAddr);
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
	virtual void RemName(TSockAddr& anAddr) const;
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual void Shutdown(MSessionControl::TCloseType option);
	virtual void Shutdown(MSessionControl::TCloseType option, const TDesC8& aDisconnectionData);
	virtual TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);
	virtual TInt GetOption(TUint level, TUint name, TDes8 &anOption) const;
	virtual TInt SetOption(TUint level, TUint name, const TDesC8 &anOption);
	virtual void Ioctl(TUint level, TUint name, TDes8 *anOption);
	virtual void CancelIoctl(TUint aLevel, TUint aName);
	virtual void Start();
	// END MSessionControl
	
	public:
	//APIs from MSessionData 
	virtual TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* anAddr = NULL);
	virtual TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr = NULL )=0;
	virtual void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL);
	virtual TInt GetData(RMBufChain& aData, TUint aLength,TUint aOptions,TSockAddr* anAddr=NULL)=0;
	//End MSessionData
	
	public:
	//APIs from MSessionDataNotify 
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	//End MSessionDataNotify
	
	public:
	/* from MSessionControlNotify */
	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CSubConnectionFlowBase& aSSP);
    virtual void ConnectComplete(CSubConnectionFlowBase& aSSP,const TDesC8& aConnectData);

	/* From MRtpDataSender */
	virtual void SendData(TDes8& aBuffer,TSockAddr& aRemoteAddr, TInt aFlags, TRequestStatus& aStatus)=0;
    
	virtual void CanClose(MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete);
	virtual TInt Error(TInt anError,TUint anOperationMask=MSessionControlNotify::EErrorAllOperations);
	virtual void Disconnect();
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8* aBuf);
	virtual void DisconnectFromListener(CSubConnectionFlowBase& )
		{
		return;
		}
#ifdef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
	//New callback to notify the completion of the asynchrnous call SetLocalName
	virtual void SetLocalNameComplete();
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING

    /* Custom cleaning up */
    virtual void DoCanClose(MSessionControlNotify::TDelete aDelete)=0;
    
    /* Update Local address*/
    virtual void DoUpdateLocalAddress(const TSockAddr& anAddr)=0;

	virtual TInt DoSetOption(TUint level, TUint name, const TDesC8 &anOption)=0;
	virtual TInt DoGetOption(TUint level, TUint name, TDes8 &anOption) const=0;
    
    virtual ~CRtpBaseFlow();
    
	protected:
	CRtpBaseFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const  Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	
	/* Helper funtion that will send a No Bearer and save the Activity ID */
	TInt HandleProvisionMessage(Messages::TSignatureBase& aMessage);
	/* Creates and Sends a provision Done message with the Error Code */
	TInt HandleBindToMessage(Messages::TSignatureBase& aMessage, TInt aErrCode);
	/* handles a TError Message from SubConnection */
	TInt HandleError(TInt aErrorCode);
	
	/* Fetches an option of type T from a descriptor */
	template <class T> 
	inline TInt GetOptionFromDesc(const TDesC8& aOption, T &opt) const
		{
		if(aOption.Length() < sizeof(T) || aOption.Length() > sizeof(T))
			{
			return KErrArgument;
			}
		TPckgBuf<T> optParams;
		optParams.Copy(aOption);
		opt = optParams();
		return KErrNone;
		}
	
	/* Fetches an option of type T from a descriptor */
	template <class T> 
	inline TInt CopyOptionToDesc(TDes8& aOption,const T& opt) const
		{
		if(aOption.Length() < sizeof(T))
			{
			return KErrOverflow;
			}
		// Descriptor containing Option 
		TPckgBuf<T> optParams(opt);
		aOption.Copy(optParams);
		return KErrNone;
		}

	protected:	
	ESock::MSessionDataNotify* iSessionDataNotify;
	ESock::MSessionControlNotify* iSessionControlNotify;
	
	// Rtp Buffer
	RMBufChain iRtpUpperLayerBuffer, iRtpLowerLayerBuffer;
	
	MSessionControl* iSSP; 
	MSessionData* iSSPData;
	MFlowBinderControl* iFlowBinder;
	Messages::RNodeInterface iServiceProvider;
			
	/* Incase flows wants to hold on to an Activity */
	TUint iScprActivityId; 
	
	/* Store destination address if connected */
	TSockAddr iDestAddr;
	/* Connected already .. thats it */
	TBool     iConnected;
	
	/* We have issued a NoBearer and Waiting for its result */
	TBool     iWaitingForBindTo;
 	
	/* We received an imediate ShutDown. When UnBind is called 
	 * shoot ourselves */
	TBool     iRecvdImmediateShutDown;
	
	Messages::RRequestOriginator iStartRequest;
	
#ifdef SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING
	TSockAddr iLocalAddress;	
#endif //SYMBIAN_STRICT_EXPLICIT_SOCKET_BINDING	
};




#endif // CRTPBASEFLOW_H


