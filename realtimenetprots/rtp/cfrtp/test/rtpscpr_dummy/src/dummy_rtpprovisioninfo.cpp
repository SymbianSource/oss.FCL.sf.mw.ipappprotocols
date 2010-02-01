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
// RTP ProvisionInfo. Encompasses the provision info that will be sent to RTP
// Flows
// 
//

/**
 @file
 @internalComponent
*/

#include "rtpprovisioninfo.h"

using namespace Meta;

CRtpProvisionConfig*  CRtpProvisionConfig::NewLC()
	{
	CRtpProvisionConfig *pRtpProv = CRtpProvisionConfig::NewL();
	CleanupStack::PushL(pRtpProv);
	return pRtpProv;
	}

CRtpProvisionConfig*  CRtpProvisionConfig::NewL()
	{
	CRtpProvisionConfig *pRtpProv = new(ELeave) CRtpProvisionConfig();
	return pRtpProv;	
	}

START_ATTRIBUTE_TABLE(CRtpProvisionConfig, KRtpProvisionUid, KRtpProvisionConfigType )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iBandWidth, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iMaxDropout, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iMaxMisorder, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iMinSequential, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iRtpTimeConversion, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iRtpTimeNow, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iAutoSendRtcp, TMetaNumber )
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iCName, TMetaBuf8 ) 
	REGISTER_ATTRIBUTE( CRtpProvisionConfig, iDefDestAddr, TMeta<TInetAddr>) 
END_ATTRIBUTE_TABLE()

