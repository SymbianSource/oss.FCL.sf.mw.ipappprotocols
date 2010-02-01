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
// @file 
// @internalAll
// 
//

#include <e32std.h>
#include <e32test.h>
#include "dummy_rtp_subconparams.h"


/* Factory Method */
CSubConParameterSet* CSubConDummyRTPParamFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch(type)
		{
		case KSubConnDummyRTPGenericParamsType:
		   return new(ELeave) CSubConRTPGenericParamSetDummy();
		default:
		   User::Leave(KErrNotFound);
		}
	return NULL;
	}
 
 
CSubConRTPGenericParamSetDummy::CSubConRTPGenericParamSetDummy()
	: CSubConnectionProtocolParameterSet(),
	iRTPProtocolUiD(TUid::Uid(0)),
	iServiceProtocolID(0),
	iSubConnType(RSubConnection::EAttachToDefault),
	iNoBearerFail(0),
	iWrongProvision(0),
	iRandomFail(0),
	iTestData(0)
/** Empty CSubConRTPGenericParamSetDummy constructor
*/
	{
	}
		
void CSubConRTPGenericParamSetDummy::SetProtocolUIDRtp(TUid aRTPProtocolUiD)
	{
	iRTPProtocolUiD = aRTPProtocolUiD;	
	}
	 
	
void CSubConRTPGenericParamSetDummy::SetServiceProtocolID(TInt aServiceProtocolID)
	{
	iServiceProtocolID = aServiceProtocolID;
	}
	
void CSubConRTPGenericParamSetDummy::SetLowerSubConnType(RSubConnection::TSubConnType aSubConnType)
	{
	iSubConnType = aSubConnType;
	}
	
TUid CSubConRTPGenericParamSetDummy::GetProtocolUiDRtp()
	{
	return iRTPProtocolUiD;
	}
		
TInt CSubConRTPGenericParamSetDummy::GetServiceProtocolID()
	{
	return iServiceProtocolID;
	}

void CSubConRTPGenericParamSetDummy::SetTestId(CSubConRTPGenericParamSetDummy::TTestcaseId aTestId)
	{
	switch(aTestId)
		{
		case KFailNoBearer:
			{
			iNoBearerFail = ETrue;
			break;
			}
		case KWrongProvision:
			{
			iWrongProvision = ETrue;
			break;
			}
		case KRandomError:
			{
			iRandomFail = ETrue;
			break;
			}
		}
	}
	
TInt CSubConRTPGenericParamSetDummy::SetTierID(TUid aTierId)
	{
	ESock::TProtocolExtensionSpecifier extensionSpec(aTierId, TUid::Uid(KDummyRtpScprFactoryImplementationUid));
	return AddExtension(extensionSpec);
	}
	
RSubConnection::TSubConnType CSubConRTPGenericParamSetDummy::GetLowerSubConnType()
	{
	return iSubConnType;
	}


void CSubConRTPGenericParamSetDummy::SetBandWidth(TUint aBandWidth)
  	{
	iBandWidth = aBandWidth;
 	}
  
void CSubConRTPGenericParamSetDummy::SetMaxDropOut(TUint aMaxDropout)
  	{
    iMaxDropout = aMaxDropout;
 	}

void CSubConRTPGenericParamSetDummy::SetMaxMisorder(TUint aMaxMisorder)
  	{
	iMaxMisorder = aMaxMisorder;
 	}

void CSubConRTPGenericParamSetDummy::SetMinSequential(TUint aMinSequential)
  	{
	iMinSequential = aMinSequential;
  	}

void CSubConRTPGenericParamSetDummy::SetRtptimeConversion(TUint aRtpTimeConversion)
  	{
	iRtpTimeConversion = aRtpTimeConversion;
  	}

void CSubConRTPGenericParamSetDummy::SetRtpTimeNow(TUint aRtpTimeNow)
  	{
	iRtpTimeNow = aRtpTimeNow;
  	}

void CSubConRTPGenericParamSetDummy::SetRtpAutoSend(TBool aAutoSendRtcp)
  	{
	iAutoSendRtcp = aAutoSendRtcp;
  	}


void CSubConRTPGenericParamSetDummy::SetCNAMEL(const TDesC8& aCName)
  	{
  	iCName.CreateL(aCName);
  	iCName.SetLength(aCName.Length());
  	return;
  	}
  
void CSubConRTPGenericParamSetDummy::SetDefDestinationAddr(TInetAddr& aDefDestAddr)
	{
	iDefDestAddr = aDefDestAddr;
	return;
	}
 
TUint CSubConRTPGenericParamSetDummy::GetBandWidth() const
  	{
	return iBandWidth;
 	}
  
TUint CSubConRTPGenericParamSetDummy::GetMaxDropOut() const  
	{
    return iMaxDropout;
 	}

TUint CSubConRTPGenericParamSetDummy::GetMaxMisorder() const  
	{
	return iMaxMisorder;
 	}

TUint CSubConRTPGenericParamSetDummy::GetMinSequential() const  
	{
	return iMinSequential;
  	}

TUint CSubConRTPGenericParamSetDummy::GetRtptimeConversion() const  
	{
	return iRtpTimeConversion;
  	}

TUint CSubConRTPGenericParamSetDummy::GetRtpTimeNow() const 
 	{
	return iRtpTimeNow;
  	}

TBool CSubConRTPGenericParamSetDummy::GetRtpAutoSend() const  
	{
	return iAutoSendRtcp;
  	}

const TDesC8& CSubConRTPGenericParamSetDummy::GetCNAME() const
	{
	return iCName;
	}
	
const TInetAddr& CSubConRTPGenericParamSetDummy::GetDefDestinationAddr() const
	{
	return iDefDestAddr;
	}

TInt CSubConRTPGenericParamSetDummy::GetNoBearerFlag()
	{
	return iNoBearerFail;
	}

TInt CSubConRTPGenericParamSetDummy::GetWrongProvsion()
	{
	return iWrongProvision;
	}
	

TInt CSubConRTPGenericParamSetDummy::GetRandomErrorFlag()
	{
	return iRandomFail;
	}
	
CSubConRTPGenericParamSetDummy::~CSubConRTPGenericParamSetDummy()
	{
	iCName.Close();
	}
	
/* Test case selector */

	 

START_ATTRIBUTE_TABLE(  CSubConRTPGenericParamSetDummy, KDummySubConnRTPParamsImplUid, KSubConnDummyRTPGenericParamsType )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iRTPProtocolUiD, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iServiceProtocolID, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iSubConnType, TMeta<RSubConnection::TSubConnType> )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iBandWidth, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iMaxDropout, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iMaxMisorder, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iMinSequential, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iRtpTimeConversion, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iRtpTimeNow, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iAutoSendRtcp, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iCName, TMetaBuf8)
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iDefDestAddr, TMeta<TInetAddr>)
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iNoBearerFail, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iWrongProvision, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iRandomFail, TMetaNumber )
	REGISTER_ATTRIBUTE( CSubConRTPGenericParamSetDummy, iTestData, TMetaNumber )
END_ATTRIBUTE_TABLE_BASE( CSubConnectionProtocolParameterSet,KSubConnGenericParamsImplUid)
 
