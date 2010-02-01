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
// RTP controller inline definitions
// 
//

/**
 * @file
 * @internalComponent
 */

/** 
Gets a handle to the RTP socket; 
it provides a direct access to the functionality of RTP socket.

	*/
inline RRtpSocket& CRtpController::RtpSocket()
	{
	return RtpHandler()->RtpSocket();
	}

inline void CRtpController::SetSessionBandwidth(TUint aBandwidth)
	{
	iSessionBandwidth = aBandwidth;
	}

inline CRtpStateMachine* CRtpController::SourceStateMachine() const 
	{
	return iSourceStateMachine;
	}

inline CRtpCollisionMng* CRtpController::CollisionManager() const
	{
	return iCollisionManager;
	}

inline TBool CRtpController::UserSetInterval(TTimeIntervalMicroSeconds32& aInterval)
	{
	if (iRtcpInterval.Int() != 0)
		{
		aInterval = iRtcpInterval;
		return ETrue;
		}
	else return EFalse;
	}

inline void CRtpController::SetRtcpInterval(const TTimeIntervalMicroSeconds32& aInterval)
	{
	iRtcpInterval = aInterval;
	}
	
inline void CRtpController::DisableNonRtpdata() 
	{
	iEnableNonRtpData = 0;
	iEnableNonRtcpData = 0;
	}

