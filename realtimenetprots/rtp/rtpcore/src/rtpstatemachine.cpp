// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// class for handling rtp state machine
// 
//

/**
 @file
 @internalComponent
*/

#include "rtpstatemachine.h"
#include "rtpcontroller.h"
#include "rtputils.h"
#include "rtcppacket.h"


CRtpState::CRtpState(TRtpSourceState aState,CRtpStateMachine& aStateMachine)
	{	
	__RTP_LOG(_L("CRtpState::CRtpState() ... "));
	iStateMachine = &aStateMachine;
	iState = aState;
	}

CRtpInitialisingState* CRtpInitialisingState::NewLC(CRtpStateMachine& aStateMachine)
{
	__RTP_LOG(_L("CRtpInitialisingState::NewLC() ... "));
	CRtpInitialisingState* self = new(ELeave) CRtpInitialisingState(aStateMachine);
	CleanupStack::PushL( self );
	return self;
}

CRtpInitialisingState::CRtpInitialisingState(CRtpStateMachine& aStateMachine)
: CRtpState(ERtpInitialising,aStateMachine)
	{
	}


TRtpSourceState CRtpInitialisingState::ProcessL(CRtpSourceEntry& aSourceEntry,
												TRtpSourceEvent /*aEvent*/,
												TUint16 aSequenceNo)
	{	
	__RTP_LOG(_L("CRtpInitialisingState::ProcessL() ... "));
	aSourceEntry.Reset(ERtpProbation,iStateMachine->MinSequential());
	aSourceEntry.SetMaxSequenceNo(aSequenceNo);
	return ERtpProbation;
	}


CRtpProbationState* CRtpProbationState::NewLC(CRtpStateMachine& aStateMachine/*,TUint aMinSequential*/)
{
	__RTP_LOG(_L("CRtpProbationState::NewLC() ... "));
	CRtpProbationState* self = new(ELeave) CRtpProbationState(aStateMachine/*,aMinSequential*/);
	CleanupStack::PushL( self );
	return self;
}

CRtpProbationState::CRtpProbationState(CRtpStateMachine& aStateMachine/*,TUint aMinSequential*/)
: CRtpState(ERtpProbation,aStateMachine)
	{
	}

/** 
New entries may be considered not valid until multiple packets  carrying the new SSRC 
have been received, or until an SDES RTCP packet containing a CNAME for that SSRC has been received; 
thus it shall be configurable. 

@internalComponent	
@return - new state
*/
	/*
	FROM RFC3550:
	If validation is enabled then a source is declared valid only 
	after MIN_SEQUENTIAL packets have been received in sequence.

	The validity check can be made stronger requiring more than two packets in sequence
	but there are other disadvantages, e.g. a larger number packets will be discarded.
			
	However, because the RTCP header validation is relatively strong, 
	if an RTCP packet is received from a source before the data packets, 
	the count could be adjusted  so that only two packets are required in sequence.  

	If initial data loss for a few seconds can be tolerated, 
	an application MAY choose to discard all data packets from a source until 
	a valid RTCP packet has been received from that source.
	*/
TRtpSourceState CRtpProbationState::ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
											 TUint16 aSequenceNo)
	{	
	TRtpSourceState state = iState;
	__RTP_LOG(_L("CRtpProbationState::ProcessL() ... "));
	__RTP_LOG1(_L("\t[Event] == %d"), static_cast<TInt>(aEvent));
	switch(aEvent)
		{
		
		case ERtpShortTimeout:
			{
			state = ERtpDelete;
			break;
			}
		case ERtpPacketReceived:
			{
			if(!aSourceEntry.IsSequenceInitialised())
				{
				// we haven't initialised sequence number yet!
				aSourceEntry.InitialiseSequenceNo(aSequenceNo);
				}
			if(aSourceEntry.IsValidSequence(aSequenceNo,iStateMachine->MaxDropout(),
											iStateMachine->MaxMisorder()))
				{
				// packet is in sequence
				if (0 == aSourceEntry.DecProbationCount()) 
					{
					aSourceEntry.InitialiseSequenceNo(aSequenceNo);
					aSourceEntry.IncPacketsReceived();
					state = ERtpValid;
					}
				}
			else 
				{
				aSourceEntry.SetProbationCount(iStateMachine->MinSequential());
				}
			break;
			}
		case ERtcpPacketReceived:
			{
			// we count only very first RTCP towards the number of packets received in sequence
			if(!aSourceEntry.IsSequenceInitialised())
				{
				aSourceEntry.InitialiseSequenceNo(aSequenceNo);
				// if MinSeqNumber is 1 we accept very first RTCP 
				// from that source as valid packet...
				if (0 == aSourceEntry.DecProbationCount())
					state = ERtpValid;
				}
			break;
			}
		case ERtcpByeReceived:
			{
			aSourceEntry.SetProbationCount(iStateMachine->MinSequential());
			state = ERtpBye;
			break;
			}
		default:
			{
			// log
			}
		}
	__RTP_LOG1(_L("\t==> %d"), static_cast<TInt>(state));
	return state;
	}

CRtpValidState* CRtpValidState::NewLC(CRtpStateMachine& aStateMachine)
	{	
	__RTP_LOG(_L("CRtpValidState::NewLC() ... "));
	CRtpValidState* self = new(ELeave) CRtpValidState(aStateMachine);
	CleanupStack::PushL( self );
	return self;
	}

CRtpValidState::CRtpValidState(CRtpStateMachine& aStateMachine)
: CRtpState(ERtpValid,aStateMachine)
	{
	}

TRtpSourceState CRtpValidState::ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
										 TUint16 aSequenceNo)											 
	{	
	__RTP_LOG(_L("CRtpValidState::ProcessL() ... "));
	__ASSERT_DEBUG(iStateMachine,Panic(ERtpStateMachineNotCreated));
	TRtpSourceState state = iState;
	__RTP_LOG1(_L("\t[Event] == %d"), static_cast<TInt>(aEvent));
	switch(aEvent)
		{
		case ERtpPacketReceived:
			{
			if(aSourceEntry.IsValidSequence(aSequenceNo,iStateMachine->MaxDropout(),
											iStateMachine->MaxMisorder()))
				{
				//from RFC3550: The number of packets received is simply the count of packets as they arrive,
				//including any late or duplicate packets.
				//NOTE: In our view that does not include packets completely out of sequence
				aSourceEntry.IncPacketsReceived();
				}
			else
				{
				state = ERtpInvalid;
				// iState in aSourceEntry is updated in CRtpStateMachine.NextL()
				}
			break;
			}
		case ERtcpPacketReceived:
			{
			// RTCP time information is stored in the CRtpController::DoRtcpReceiveL()	
			// so we do nothing here...
			break;
			}
		case ERtcpByeReceived:
			{
			state = ERtpBye;
			break;
			}
		case ERtpShortTimeout:
			{
			state = ERtpInactive;
			break;
			}
		default:
			{
			// Is this OK?
			state = ERtpInvalid;
			}
		}
	__RTP_LOG1(_L("\t==> %d"), static_cast<TInt>(state));
	return state;
	}


CRtpInvalidState* CRtpInvalidState::NewLC(CRtpStateMachine& aStateMachine)
{
	__RTP_LOG(_L("CRtpInvalidState::NewLC() ... "));
	CRtpInvalidState* self = new(ELeave) CRtpInvalidState(aStateMachine);
	CleanupStack::PushL( self );
	return self;
}


CRtpInvalidState::CRtpInvalidState(CRtpStateMachine& aStateMachine)
: CRtpState(ERtpInvalid,aStateMachine)
	{
	}

TRtpSourceState CRtpInvalidState::ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
											TUint16 aSequenceNo)
	{	
	TRtpSourceState state = iState;
	__RTP_LOG(_L("CRtpInvalidState::ProcessL() ... "));
	__RTP_LOG1(_L("\t[Event] == %d"), static_cast<TInt>(aEvent));
	switch(aEvent)
		{
		case ERtpPacketReceived:
			{
			// Is the next packet received with next higher sequence number?  
			// If so it is considered the valid start of a new packet sequence.
			if (aSequenceNo == aSourceEntry.BadSequenceNo()) 
				{
				// Two sequential packets -- assume that the other side
				// restarted without telling us so just re-sync
				// (i.e., pretend this was the first packet).
				aSourceEntry.InitialiseSequenceNo(aSequenceNo);
				// Since multiple complete sequence number cycles may have been missed, 
				// the packet loss statistics need to be  reset. 
				// NOTE: InitialiseSequenceNo() above already resets some stats!!
				aSourceEntry.ResetRtcpStatistics();
				state = ERtpValid;
				}
			else 
				{
				aSourceEntry.Reset(ERtpProbation,iStateMachine->MinSequential());
				//aSourceEntry.SetProbationCount(iStateMachine->MinSequential());
				state = ERtpProbation;
				}
			break;
			}
		case ERtcpPacketReceived:
			// could we validate upon RTCP packet? not for now...
			break;
		case ERtcpByeReceived:
			{
			state = ERtpDelete;
			break;
			}
		case ERtpShortTimeout:
		default:
			{
			state = ERtpInactive;
			}
		}
	__RTP_LOG1(_L("\t==> %d"), state);
	return state;
	}

CRtpInactiveState* CRtpInactiveState::NewLC(CRtpStateMachine& aStateMachine)
{
	__RTP_LOG(_L("CRtpInactiveState::NewLC() ... "));
	CRtpInactiveState* self = new(ELeave) CRtpInactiveState(aStateMachine);
	CleanupStack::PushL( self );
	return self;
}

CRtpInactiveState::CRtpInactiveState(CRtpStateMachine& aStateMachine)
: CRtpState(ERtpInactive,aStateMachine)
	{
	}

TRtpSourceState CRtpInactiveState::ProcessL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
											TUint16 aSequenceNo)
	{
	/*RFC3550 6.2.1 Maintaining the number of session members:

	A participant MAY mark another site inactive, or delete it if not yet valid, 
	if no RTP or RTCP packet has been received for a small number of RTCP report intervals (5 is RECOMMENDED).
	This provides some robustness against packet loss.  
	All sites must have the same value for this multiplier 
	and must calculate roughly the same value for the RTCP report interval in order for this timeout to work properly.
	Therefore, this multiplier SHOULD be fixed for a particular profile.
	NOTE: The multiplier hardcoded for now.
	*/
	__RTP_LOG(_L("CRtpInactiveState::ProcessL() ... "));
	__ASSERT_DEBUG(iStateMachine,Panic(ERtpStateMachineNotCreated));
	TRtpSourceState state = iState;
	// iState in aSourceEntry is updated in CRtpStateMachine.NextL()
	__RTP_LOG1(_L("\t[Evetn] == %d"), static_cast<TInt>(aEvent));
	switch(aEvent)
		{
		case ERtpPacketReceived:
			{
			// if valid sequence number lets go back to VALID state...
			if(aSourceEntry.IsValidSequence(aSequenceNo,iStateMachine->MaxDropout(),
											iStateMachine->MaxMisorder()))
				{
				aSourceEntry.IncPacketsReceived();
				state = ERtpValid;		
				}
			else
				{
				state = ERtpInvalid;
				}

			break;
			}
		case ERtcpPacketReceived:
			{
			// could we go back to VALID state just like that when RTCP received?
			//state = ERtpValid;		
			break;
			}
		case ERtpLongTimeout:
		case ERtcpByeReceived:
			{
			state = ERtpDelete;
			break;
			}
		
		default:
			{
			break;
			}
		}
	__RTP_LOG1(_L("\t==> %d"), static_cast<TInt>(state));
	return state;
	}

CRtpByeState* CRtpByeState::NewLC(CRtpStateMachine& aStateMachine)
{
	__RTP_LOG(_L("CRtpByeState::NewLC() ... "));
	CRtpByeState* self = new(ELeave) CRtpByeState(aStateMachine);
	CleanupStack::PushL( self );
	return self;
}

TRtpSourceState CRtpByeState::ProcessL(CRtpSourceEntry& /*aSourceEntry*/,TRtpSourceEvent aEvent,
										 TUint16 /*aSequenceNo*/)
	{	
	TRtpSourceState state = iState;
	__RTP_LOG(_L("CRtpByeState::ProcessL() ... "));
	__RTP_LOG1(_L("\t[Event] == %d"), static_cast<TInt>(aEvent));
	switch(aEvent)
		{
		case ERtpByeTimeout:
			{
			// we've waited long enough for straggler data packets after the BYE 
			// and can go to delete state now...
			state = ERtpDelete;
			}
		default:
			{
			// we're interested in Bye Timeout only anything else is discarded
			}
		}
	__RTP_LOG1(_L("\t==> %d"), static_cast<TInt>(state));
	return state;
	}

CRtpByeState::CRtpByeState(CRtpStateMachine& aStateMachine)
: CRtpState(ERtpBye,aStateMachine)
	{
	}

CRtpDeletedState* CRtpDeletedState::NewLC(CRtpStateMachine& aStateMachine)
{
	__RTP_LOG(_L("CRtpDeletedState::NewLC() ... "));
	CRtpDeletedState* self = new(ELeave) CRtpDeletedState(aStateMachine);
	CleanupStack::PushL( self );
	return self;
}

TRtpSourceState CRtpDeletedState::ProcessL(CRtpSourceEntry& /*aSourceEntry*/,TRtpSourceEvent /*aEvent*/,
										 TUint16 /*aSequenceNo*/)
	{
	__RTP_LOG(_L("CRtpDeletedState::ProcessL() ... "));
	__RTP_LOG1(_L("\t==> %d"), static_cast<TInt>(iState));
	return iState;
	}

CRtpDeletedState::CRtpDeletedState(CRtpStateMachine& aStateMachine)
: CRtpState(ERtpDelete,aStateMachine)
	{
	}

CRtpStateMachine* CRtpStateMachine::NewL(TInt aPriority)
{
	__RTP_LOG(_L("CRtpStateMachine::NewL() ... "));
	CRtpStateMachine* self = new(ELeave) CRtpStateMachine;
	CleanupStack::PushL( self );
	self->ConstructL(aPriority);
	CleanupStack::Pop();
	return self;
}

CRtpStateMachine::~CRtpStateMachine()
	{

	__RTP_LOG(_L("CRtpStateMachine::~CRtpStateMachine() ... "));
	// The destructor deals with cancel
	delete iTimerQueue;
	iStateHandlers.ResetAndDestroy();

	}


void CRtpStateMachine::ConstructL(TInt aPriority)
{
	iMaxDropout = KRtpDefaultMaxDropout;
	iMaxMisorder = KRtpDefaultMaxMisorder;
	iMinSequential = KRtpDefaultMinSequential;
	// initialise a default RTCP interval for now
	// it's set as soon as CRtcpSessionData is created
	// and is updated reqularly from then on in CRtcpSessionData::StartTimer()
	iRtcpTransmissionInterval = KRtcpMinTimeMicroSeconds32;
	// Create a new timed event queue with the specified active object priority 
	// and default timer granularity (100millsec) ...
	iTimerQueue = CDeltaTimer::NewL(aPriority);
	//
	CRtpState* state = CRtpInitialisingState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
	state = CRtpProbationState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
	state = CRtpValidState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
	state = CRtpInvalidState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
	state = CRtpInactiveState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
	state = CRtpByeState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
	state = CRtpDeletedState::NewLC(*this);
	User::LeaveIfError(iStateHandlers.Append(state));
	CleanupStack::Pop(state);
}


void CRtpStateMachine::SetRtpStreamParameters(TUint aMaxDropout,
												TUint aMaxMisorder,
												TUint aMinSequential)
	{	
	__RTP_LOG(_L("CRtpStateMachine::SetRtpStreamParameters() ... "));
	__RTP_LOG1(_L("\t[MaxMisorder] == %u"), aMaxMisorder);
	__RTP_LOG1(_L("\t[MinSequential] == %u"), aMinSequential);


	iMaxDropout = aMaxDropout;
	iMaxMisorder = aMaxMisorder;
	iMinSequential = aMinSequential;
	}

void CRtpStateMachine::StartL()
	{
	// well do we have to do anything here??
	}


TRtpSourceState CRtpStateMachine::NextL(CRtpSourceEntry& aSourceEntry,TRtpSourceEvent aEvent,
									   TUint16 aSequenceNo)
	{	
	__RTP_LOG(_L("CRtpStateMachine::NextL() ... "));
	iTimerQueue->Remove(aSourceEntry.DeltaTimerEntry());
	aSourceEntry.ClearDeltaTimerEntry();

	CRtpState* stateHandler = iStateHandlers[0];
	TRtpSourceState currentSourceState = aSourceEntry.State();
	for(int i = 0;i < iStateHandlers.Count(); i++)
		{
		// we don't have to check stateHandler ptr for NULL here,
		// since the operator raises a USER-130 panic anyway 
		// if the index is outside array boundaries.
		stateHandler = iStateHandlers[i];
		if(currentSourceState == stateHandler->State())
			break;
		}
	
	currentSourceState = stateHandler->ProcessL(aSourceEntry,aEvent,aSequenceNo);
	aSourceEntry.SetState(currentSourceState);
	
	switch(currentSourceState)
		{
		case ERtpInitialising:
			break;
		case ERtpProbation:
		case ERtpValid:
		case ERtpInvalid:
			{
			TCallBack timerCallBack(CRtpSourceEntry::HandleTimeoutL,&aSourceEntry); // construction of the callback
			aSourceEntry.SetDeltaTimerEntry(timerCallBack,ERtpShortTimeout);
			iTimerQueue->Queue(ShortTimeout(),aSourceEntry.DeltaTimerEntry());
			break;
			}
		case ERtpInactive:
			{
			TCallBack timerCallBack(CRtpSourceEntry::HandleTimeoutL,&aSourceEntry); // construction of the callback
			aSourceEntry.SetDeltaTimerEntry(timerCallBack,ERtpLongTimeout);
			iTimerQueue->Queue(LongTimeout(),aSourceEntry.DeltaTimerEntry());
			break;
			}
		case ERtpBye:
			{
			TCallBack timerCallBack(CRtpSourceEntry::HandleTimeoutL,&aSourceEntry); // construction of the callback
			//TDeltaTimerEntry deltaTimerEntry(timerCallBack);
			aSourceEntry.SetDeltaTimerEntry(timerCallBack,ERtpByeTimeout);
			iTimerQueue->Queue(ByeTimeout(),aSourceEntry.DeltaTimerEntry());
			break;
			}
		case ERtpDelete:
			break;
		default:
			// is panic ok? or a bit harsh
			Panic(ERtpStateMachineUnknownState);
		}

	__RTP_LOG1(_L("\t==> %d"), static_cast<TInt>(currentSourceState));	
	return currentSourceState;
	}

TTimeIntervalMicroSeconds32 CRtpStateMachine::ByeTimeout()
	{

	__RTP_LOG(_L("CRtpStateMachine::ByeTimeout() ... "));
	__ASSERT_DEBUG((iRtcpTransmissionInterval.Int()*KRtcpByeTimeoutMultiplier)>0,Panic(ERtpStateMachineZeroTimeout));
	return TTimeIntervalMicroSeconds32(iRtcpTransmissionInterval.Int()*KRtcpByeTimeoutMultiplier);

	}

TTimeIntervalMicroSeconds32 CRtpStateMachine::ShortTimeout()
	{

	__ASSERT_DEBUG((iRtcpTransmissionInterval.Int()*KRtcpShortTimeoutMultiplier)>0,Panic(ERtpStateMachineZeroTimeout));
	return TTimeIntervalMicroSeconds32(iRtcpTransmissionInterval.Int()*KRtcpShortTimeoutMultiplier);
	
	}

TTimeIntervalMicroSeconds32 CRtpStateMachine::LongTimeout()
	{

	__ASSERT_DEBUG(KRtpLongTimeout>0,Panic(ERtpStateMachineZeroTimeout));
	return TTimeIntervalMicroSeconds32(KRtpLongTimeout);

	}


