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
// RTP Pint Implementation
// 
//

/**
 @file
 @internalComponent
*/


#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "rtppint.h"
#include "rtpflow.h"
#include "rtcpflow.h"


using namespace ESock;

class CRtpController;
// RTP Protocol-Interface

CRtpProtocolIntf::CRtpProtocolIntf(CProtocolIntfFactoryBase& aFactory,const Messages::TNodeId& aId)
: CProtocolIntfBase(aFactory,aId)
	{
	}

void CRtpProtocolIntf::DoFlowCreated(CSubConnectionFlowBase&  )
	{
	/* nothing to Do Here */
	return;
	}
	
void CRtpProtocolIntf::DoFlowBeingDeleted(CSubConnectionFlowBase& aFlow)
	{
	/* The flows shud be removed from the Hash Table */
	TInt sz = iFlowArray.Count();
	for(TInt i=0; i < sz; i++)
		{
		/* When RTP Flow is removed. We remove the entry from the hash table.
		 * When RTCP Flow is removed this entry is still maintained        */
		if(iFlowArray[i].GetRtpFlow() == &aFlow)
			{
			iFlowArray.Remove(i);
			return;
			}
		}
	return;
	}

void CRtpProtocolIntf::RegisterSubConnProvIDL(CRtpBaseFlow* aRtpBaseflow, const Messages::TNodeId& aScprId)
	{
	TRtpFlowHolder *pFlowHolder;
	
	/* Query if the flow Created was RTP or RTCP */
	/* Verify the flow Creation Order is Correct */
	/* Initialise RTP and RTCP flows with proper params */
	pFlowHolder = FindInFlowArray(aScprId);
	if(!pFlowHolder)
		{	
		/* This flow is an RTP Flow */
		TRtpFlowHolder flowHolder(aScprId);
		flowHolder.SetRtpFlow(static_cast<CRtpFlow*>(aRtpBaseflow));
		iFlowArray.AppendL(flowHolder);
		}
	else
		{
		/* This flow is RTCP. The fact that pHolder exists means
		 an RTP Socket was already opened */
		pFlowHolder->SetRtcpFlow(static_cast<CRtcpFlow*>(aRtpBaseflow));
		/* Here ask the RTP Flow to create an RTCP Sender.
		   Pass on the Parameters to RTCP Flow */
		CRtpFlow *pRtpFlow   = pFlowHolder->GetRtpFlow();	
		CRtcpFlow *pRtcpFlow = pFlowHolder->GetRtcpFlow();
		pRtcpFlow->SetRtpSession(pRtpFlow->GetRtpSession());
		}
	}


//Tries to find a FlowHolder belonging to the supplied SCPR. Will return a
//pointer to the flowholder or NULL if cannot be found 
TRtpFlowHolder* CRtpProtocolIntf::FindInFlowArray(const Messages::TNodeId& aScprId)
	{
	TInt sz = iFlowArray.Count();
	for(TInt i=0; i < sz; i++)
		{
		if(iFlowArray[i].ScprId() == aScprId)
			{
			return &iFlowArray[i];
			}
		}
    return 0;	
	}

CRtpProtocolIntf::~CRtpProtocolIntf()
	{
	iFlowArray.Close();
	}

