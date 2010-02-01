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
// Utilities for RtpSock Test Suite
// 
//

/**
 @file
 @internalComponent
*/

#include <rtp_subconparams.h>
#include "trtpsock_utils.h"


_LIT8(KRtpSessionCNAME, "Hello There!");

void TRtpSockUtils::CreateRtpSubconParameterSetL(RSubConParameterBundle &aSubconParams)
	{	
 
 	// Create a container for RTP sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* subConFamily = CSubConParameterFamily::NewL(aSubconParams, KProtocolExtensionFamily);

 	// Create the requested generic parameter set for RTP (RTP family takes ownership)
	CSubConRTPGenericParamSet* reqGenericParams = CSubConRTPGenericParamSet::NewL(*subConFamily, CSubConParameterFamily::ERequested);
	
	
	TUid idRTP = TUid::Uid(KRtpFlowImplementationUid);
	reqGenericParams->SetProtocolUIDRtp(idRTP);
	reqGenericParams->SetBandWidth(1234);
	reqGenericParams->SetRtptimeConversion(100);
	reqGenericParams->SetRtpAutoSend(ETrue);
	reqGenericParams->SetRtpTimeNow(0);
	
	reqGenericParams->SetServiceProtocolID(KProtocolInetUdp);
	reqGenericParams->SetLowerSubConnType(RSubConnection::EAttachToDefault);
	reqGenericParams->SetCNAMEL(KRtpSessionCNAME());
	
	TUid idTier = TUid::Uid(2048);
	reqGenericParams->SetTierID(idTier);

	return;
	}

