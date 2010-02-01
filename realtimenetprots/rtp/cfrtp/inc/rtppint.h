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
// RTP PINT definitions.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef RTPPINT_H_
#define RTPPINT_H_

#include <e32base.h>
#include <e32std.h>
#include <e32hashtab.h>
#include <rtp_proto_id.h>
#include <comms-infras/ss_protflow.h>
#include "rtpflowfactory.h"	
#include "rtpbaseflow.h"

class CRtpFlow;
class CRtcpFlow;

NONSHARABLE_CLASS(CRtpProtocolIntfFactory) : public ESock::CProtocolIntfFactoryBase
	/**
	Factory for creation RTP PINT.

	@internalComponent 
	
	**/				    
	{
friend class CRtpFlowFactory;
public:
	static CRtpProtocolIntfFactory* NewL(TUid aUid, ESock::CProtocolIntfFactoryContainer& aParentContainer);
	CProtocolIntfBase* DoCreateProtocolIntfL(TFactoryQueryBase& aQuery);
protected:
	CRtpProtocolIntfFactory(TUid /*aFactoryId*/, ESock::CProtocolIntfFactoryContainer& /*aParentContainer*/);
	};


/* This class holds an Instances of RTP and RTCP flows.
 * This is TClass because it donot own this flows and donot manage them
 * in any way */
class TRtpFlowHolder
	{
	/**
	Helper class for managing flows belonging to different SCPRs.
	This is needed because we have only one PINT per CPR.

	@internalComponent.
	**/				    

	CRtpFlow  *iRtpFlow;
	CRtcpFlow *iRtcpFlow;
	/* The SCPR to which this flows belon to */
	const Messages::TNodeId iScprId;
public:
	TRtpFlowHolder(const Messages::TNodeId& aScprId):iRtpFlow(0),iRtcpFlow(0),iScprId(aScprId)
		{
		}
	void SetRtpFlow(CRtpFlow  *aRtpflow)
		{
		iRtpFlow = aRtpflow;
		}
	void SetRtcpFlow(CRtcpFlow *aRtcpFlow)
		{
		iRtcpFlow = aRtcpFlow;
		}
	CRtpFlow* GetRtpFlow()
		{
		return iRtpFlow;
		}
	CRtcpFlow* GetRtcpFlow()
		{
		return iRtcpFlow;
		}
	const  Messages::TNodeId& ScprId()
		{
		return iScprId;
		}
	};

/* The PINT is singleton per instance of CPR. 
 * In our case an RTCP SCPR symbolises one RTP Session. This information is
 * contained inside the PINT. Each PINT will hold  Pointers to RTP and RTCP
 * flows and it will club the flows together by their SCPR ID */
class CRtpProtocolIntf : public ESock::CProtocolIntfBase
	{
	/**
	RTP PINT.
	@internalComponent.
	**/				    

	friend class CRtpProtocolIntfFactory;
protected:
	CRtpProtocolIntf(ESock::CProtocolIntfFactoryBase& aFactory,const Messages::TNodeId& aId);
	
	// from CProtocolIntfBase
	void DoFlowCreated(ESock::CSubConnectionFlowBase& aFlow);
	void DoFlowBeingDeleted(ESock::CSubConnectionFlowBase& aFlow);
	
	private:
	RArray<TRtpFlowHolder> iFlowArray;

	TRtpFlowHolder* FindInFlowArray(const Messages::TNodeId& aScprId);
	
	public:
	
	~CRtpProtocolIntf();
	
	/* The Flows after getting Created will call this function to Register Them selves.
	   Before registration the flows will be held in a Temporary Array. Once registered
	   they will be associated with a SubConnID and moved to a Lookup table. Might leave
	   if Insert fails */
	void RegisterSubConnProvIDL(CRtpBaseFlow* aRtpflow, const Messages::TNodeId& aSprId);
	
	};
	
#endif 
