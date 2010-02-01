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
// subconparams.cpp
// 
//

/**
 @file 
 @internalComponent
*/

#include <e32std.h>
#include <e32test.h>
#include "rtp_subconparams.h"
#include "rtpcontrollerbase.h"


/* Factory Method */
CSubConParameterSet* CSubConRTPParamFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch(type)
		{
		case KSubConnRTPGenericParamsType:
		   return new(ELeave) CSubConRTPGenericParamSet();
		default:
		   User::Leave(KErrNotFound);
		}
	return NULL;
	}
 
 
CSubConRTPGenericParamSet::CSubConRTPGenericParamSet()
	: CSubConnectionProtocolParameterSet(),
	iRTPProtocolUiD(TUid::Uid(0)),
	iServiceProtocolID(0),
	iSubConnType(RSubConnection::EAttachToDefault),
	iMaxDropout(KRtpDefaultMaxDropout),
	iMaxMisorder(KRtpDefaultMaxMisorder),
	iMinSequencial(KRtpDefaultMinSequential),
	iRtcpInterval(0)
/** Empty CSubConRTPGenericParamSet constructor
*/
	{
	}
		
void CSubConRTPGenericParamSet::SetProtocolUIDRtp(TUid aRTPProtocolUiD)
	{
	iRTPProtocolUiD = aRTPProtocolUiD;	
	}
	 
	
void CSubConRTPGenericParamSet::SetServiceProtocolID(TInt aServiceProtocolID)
	{
	iServiceProtocolID = aServiceProtocolID;
	}
	
void CSubConRTPGenericParamSet::SetLowerSubConnType(RSubConnection::TSubConnType aSubConnType)
	{
	iSubConnType = aSubConnType;
	}
	
TUid CSubConRTPGenericParamSet::GetProtocolUiDRtp()
	{
	return iRTPProtocolUiD;
	}
		
TInt CSubConRTPGenericParamSet::GetServiceProtocolID()
	{
	return iServiceProtocolID;
	}
	
TInt CSubConRTPGenericParamSet::SetTierID(TUid aTierId)
	{
	ESock::TProtocolExtensionSpecifier extensionSpec(aTierId, TUid::Uid(KRtpScprFactoryImplementationUid));
	return AddExtension(extensionSpec);
	}
	
RSubConnection::TSubConnType CSubConRTPGenericParamSet::GetLowerSubConnType()
	{
	return iSubConnType;
	}


void CSubConRTPGenericParamSet::SetBandWidth(TUint aBandWidth)
  	{
	iBandWidth = aBandWidth;
 	}
  
void CSubConRTPGenericParamSet::SetMaxDropOut(TUint aMaxDropout)
  	{
    iMaxDropout = aMaxDropout;
 	}

void CSubConRTPGenericParamSet::SetMaxMisorder(TUint aMaxMisorder)
  	{
	iMaxMisorder = aMaxMisorder;
 	}

void CSubConRTPGenericParamSet::SetMinSequencial(TUint aMinSequential)
  	{
	iMinSequencial = aMinSequential;
  	}

void CSubConRTPGenericParamSet::SetRtptimeConversion(TUint aRtpTimeConversion)
  	{
	iRtpTimeConversion = aRtpTimeConversion;
  	}

void CSubConRTPGenericParamSet::SetRtpTimeNow(TUint aRtpTimeNow)
  	{
	iRtpTimeNow = aRtpTimeNow;
  	}

void CSubConRTPGenericParamSet::SetRtpAutoSend(TBool aAutoSendRtcp)
  	{
	iAutoSendRtcp = aAutoSendRtcp;
  	}


void CSubConRTPGenericParamSet::SetCNAMEL(const TDesC8& aCName)
  	{
  	iCName.CreateL(aCName);
  	return;
  	}
  
void CSubConRTPGenericParamSet::SetDefDestinationAddr(TInetAddr& aDefDestAddr)
	{
	iDefDestAddr = aDefDestAddr;
	return;
	}

void CSubConRTPGenericParamSet::SetRtcpReportInterval(const TTimeIntervalMicroSeconds32& aInterval)
	{
	iRtcpInterval = aInterval;
	return;
	}
 
TUint CSubConRTPGenericParamSet::BandWidth() const
  	{
	return iBandWidth;
 	}
  
TUint CSubConRTPGenericParamSet::MaxDropOut() const  
	{
    return iMaxDropout;
 	}

TUint CSubConRTPGenericParamSet::MaxMisorder() const  
	{
	return iMaxMisorder;
 	}

TUint CSubConRTPGenericParamSet::MinSequencial() const  
	{
	return iMinSequencial;
  	}

TUint CSubConRTPGenericParamSet::RtptimeConversion() const  
	{
	return iRtpTimeConversion;
  	}

TUint CSubConRTPGenericParamSet::RtpTimeNow() const 
 	{
	return iRtpTimeNow;
  	}

TBool CSubConRTPGenericParamSet::RtpAutoSend() const  
	{
	return iAutoSendRtcp;
  	}

const TTimeIntervalMicroSeconds32& CSubConRTPGenericParamSet::GetRtcpReportInterval() const
	{
	return iRtcpInterval;
	}

const TDesC8& CSubConRTPGenericParamSet::GetCNAME() const
	{
	return iCName;
	}
const TInetAddr& CSubConRTPGenericParamSet::GetDefDestinationAddr() const
	{
	return iDefDestAddr;
	}

CSubConRTPGenericParamSet::~CSubConRTPGenericParamSet()
	{
	iCName.Close();
	}
	

START_ATTRIBUTE_TABLE(  CSubConRTPGenericParamSet, KSubConnRTPParamsImplUid, KSubConnRTPGenericParamsType )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iRTPProtocolUiD, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iServiceProtocolID, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iSubConnType, TMeta<RSubConnection::TSubConnType> )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iBandWidth, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iMaxDropout, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iMaxMisorder, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iMinSequencial, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iRtpTimeConversion, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iRtpTimeNow, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iAutoSendRtcp, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iCName, TMetaBuf8)
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iDefDestAddr, TMeta<TInetAddr>)
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSet, iRtcpInterval,TMeta<TTimeIntervalMicroSeconds32>)
END_ATTRIBUTE_TABLE_BASE( CSubConnectionProtocolParameterSet,KSubConnGenericParamsImplUid)

