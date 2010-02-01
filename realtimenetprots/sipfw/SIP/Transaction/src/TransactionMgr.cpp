// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TransactionMgr.cpp
// Part of       : Transaction
// Version       : SIP/5.0
//



#include "SipAssert.h"

#include "TransactionMgr.h"
#include "NormalClientTa.h"
#include "InviteClientTa.h"
#include "NormalServerTa.h"
#include "InviteServerTa.h"
#include "NormalClientTaStates.h"
#include "InviteClientTaStates.h"
#include "NormalServerTaStates.h"
#include "InviteServerTaStates.h"


// -----------------------------------------------------------------------------
// CTransactionMgr::NewL
// -----------------------------------------------------------------------------
//
CTransactionMgr* CTransactionMgr::NewL(MTimerManager& aTimers,								   
									   MTransactionStore& aTransactionStore)
	{
	CTransactionMgr* self = new (ELeave) CTransactionMgr(aTimers,														 
														 aTransactionStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransactionMgr::CTransactionMgr
// -----------------------------------------------------------------------------
//
CTransactionMgr::CTransactionMgr(MTimerManager& aTimers,
								 MTransactionStore& aTransactionStore) :
	iTimers(aTimers),
	iTransactionStore(aTransactionStore)
	{
	}

// -----------------------------------------------------------------------------
// CTransactionMgr::ConstructL
// -----------------------------------------------------------------------------
//
void CTransactionMgr::ConstructL()
	{
	//Invite client transaction states
	iInviteClientTa_Calling = new (ELeave) CInviteClientTa_Calling;
	iInviteClientTa_Proceeding = new (ELeave) CInviteClientTa_Proceeding;
	iInviteClientTa_Completed = new (ELeave) CInviteClientTa_Completed;
	
	iInviteClientTa_Calling->SetNeighbourStates(*iInviteClientTa_Proceeding,
												*iInviteClientTa_Completed);
	iInviteClientTa_Proceeding->SetNeighbourStates(*iInviteClientTa_Completed);

	
	//Non-Invite client transaction states
	iNormalClientTa_Trying = new (ELeave) CNormalClientTa_Trying;
	iNormalClientTa_Proceeding = new (ELeave) CNormalClientTa_Proceeding;
	iNormalClientTa_Completed = new (ELeave) CNormalClientTa_Completed;
	
	iNormalClientTa_Trying->SetNeighbourStates(*iNormalClientTa_Proceeding,
											   *iNormalClientTa_Completed);
	iNormalClientTa_Proceeding->SetNeighbourStates(*iNormalClientTa_Completed);


	//Invite server transaction states
	iInviteServerTa_Proceeding = new (ELeave) CInviteServerTa_Proceeding;
	iInviteServerTa_Completed = new (ELeave) CInviteServerTa_Completed;
	iInviteServerTa_Confirmed = new (ELeave) CInviteServerTa_Confirmed;
	iInviteServerTa_Sending2xx = new (ELeave) CInviteServerTa_Sending2xx;
	
	iInviteServerTa_Proceeding->SetNeighbourStates(*iInviteServerTa_Completed,
												   *iInviteServerTa_Sending2xx);
	iInviteServerTa_Completed->SetNeighbourStates(*iInviteServerTa_Confirmed);


	//Non-Invite server transaction states
	iNormalServerTa_Trying = new (ELeave) CNormalServerTa_Trying;
	iNormalServerTa_Proceeding = new (ELeave) CNormalServerTa_Proceeding;
	iNormalServerTa_Completed = new (ELeave) CNormalServerTa_Completed;
	
	iNormalServerTa_Trying->SetNeighbourStates(*iNormalServerTa_Proceeding,
											   *iNormalServerTa_Completed);
	iNormalServerTa_Proceeding->SetNeighbourStates(*iNormalServerTa_Completed);
	}

// -----------------------------------------------------------------------------
// CTransactionMgr::~CTransactionMgr
// -----------------------------------------------------------------------------
//
CTransactionMgr::~CTransactionMgr()
	{	
	delete iInviteClientTa_Calling;
	delete iInviteClientTa_Proceeding;
	delete iInviteClientTa_Completed;
	
	delete iNormalClientTa_Trying;
	delete iNormalClientTa_Proceeding;
	delete iNormalClientTa_Completed;	

	delete iInviteServerTa_Proceeding;
	delete iInviteServerTa_Completed;
	delete iInviteServerTa_Confirmed;	
	delete iInviteServerTa_Sending2xx;

	delete iNormalServerTa_Trying;
	delete iNormalServerTa_Proceeding;
	delete iNormalServerTa_Completed;
	}

// -----------------------------------------------------------------------------
// CTransactionMgr::CreateTransactionL
// -----------------------------------------------------------------------------
//
CTransactionBase*
CTransactionMgr::CreateTransactionL(CTransactionBase::TTransactionType aType,
									CUserAgentBase& aUserAgent,
									CTransmitter& aTransmitter,
									TTimerValues& aTimerValues,
									TBool aRetransmitInvite)
	{
	CTransactionBase* ta = NULL;

	switch (aType)
		{
		case CTransactionBase::KServerTransaction:
			{
			ta = CNormalServerTransaction::NewL(aUserAgent,
											    aTransmitter,
											    iTimers,
											    *iNormalServerTa_Trying,
											    aTimerValues);
			break;
			}

		case CTransactionBase::KServerInviteTransaction:
			{
			ta = CInviteServerTransaction::NewL(aUserAgent,
											    aTransmitter,
											    iTimers,
											    *iInviteServerTa_Proceeding,
											    aTimerValues);
			break;
			}

		case CTransactionBase::KClientTransaction:
			{
			ta = CNormalClientTransaction::NewL(aUserAgent,
											    aTransmitter,
											    iTimers,
											    *iNormalClientTa_Trying,
											    aTimerValues,
											    iTransactionStore);
			break;
			}

		case CTransactionBase::KClientInviteTransaction:
			{
			ta = CInviteClientTransaction::NewL(aUserAgent,
											    aTransmitter,
											    iTimers,
											    *iInviteClientTa_Calling,
											    aTimerValues,
											    iTransactionStore,
											    aRetransmitInvite);
			break;
			}

		default:
			User::Leave(KErrArgument);
		}
	return ta;
	}

// -----------------------------------------------------------------------------
// CTransactionMgr::IsPossibleToSendCancel
// -----------------------------------------------------------------------------
//
TBool CTransactionMgr::IsPossibleToSendCancel(
	const CTransactionBase& aTransaction) const
	{
	return (!aTransaction.HasTerminated() &&
		    (static_cast<const CTransaction&>(aTransaction).State() ==
                iInviteClientTa_Proceeding));
	}
