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

#ifndef __DUMMY_RTP_SUBCONPARAMS_H__
#define __DUMMY_RTP_SUBCONPARAMS_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include <comms-infras/metabuffer.h>
#include <comms-infras/ss_protocolparameterset.h>

#include <es_sock.h>


#include "dummy_rtp_uid.h"

const TUint32 KSubConnDummyRTPGenericParamsType = 2;


class CSubConRTPGenericParamSetDummy : public ESock::CSubConnectionProtocolParameterSet
	{
	friend class CSubConDummyRTPParamFactory;
	public:
	enum TTestcaseId
		{
		KFailNoBearer,
		KWrongProvision,
		KRandomError,
		};

	inline static CSubConRTPGenericParamSetDummy* CSubConRTPGenericParamSetDummy::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	
	inline static CSubConRTPGenericParamSetDummy* CSubConRTPGenericParamSetDummy::NewL();
	
	virtual ~CSubConRTPGenericParamSetDummy();
	
	virtual TInt SetTierID(TUid aTierId);
	
	/* RTP Stack Initialisation Parameters Get/Set */
  	virtual void SetBandWidth(TUint aBandWidth);
  
  	virtual void SetMaxDropOut(TUint aMaxDropout);

	virtual void SetMaxMisorder(TUint aMaxMisorder);

	virtual void SetMinSequential(TUint aMinSequential);

	virtual void SetRtptimeConversion(TUint aRtpTimeConversion);

	virtual void SetRtpTimeNow(TUint aRtpTimeNow);

	virtual void SetRtpAutoSend(TBool aAutoSendRtcp);
	
	virtual void SetProtocolUIDRtp(TUid aRTPProtocolUiD);
			
	virtual void SetServiceProtocolID(TInt aServiceProtocolID);
	
	virtual void SetLowerSubConnType(RSubConnection::TSubConnType aSubConnType);
	
	virtual void SetCNAMEL(const TDesC8& aCName);
	
	virtual void SetDefDestinationAddr(TInetAddr& aDefDestAddr);
	
	virtual void SetTestId(CSubConRTPGenericParamSetDummy::TTestcaseId aTestId);


    /* The Get Functions are not made Virtual. They are supposed to be accessed
       inside the CF Dll */
       	
	TUid GetProtocolUiDRtp();
	
	TInt GetServiceProtocolID();
	
	RSubConnection::TSubConnType GetLowerSubConnType();

	TUint GetBandWidth() const;
	  
	TUint GetMaxDropOut() const;
	 
	TUint GetMaxMisorder() const;
	  
	TUint GetMinSequential() const;

	TUint GetRtptimeConversion() const;

	TUint GetRtpTimeNow() const;

	TBool GetRtpAutoSend() const;
	
	const TDesC8& GetCNAME() const;
	
	const TInetAddr& GetDefDestinationAddr() const;
	
	TInt GetNoBearerFlag();

	TInt GetWrongProvsion();
	
	TInt GetRandomErrorFlag();

	DATA_VTABLE
	
	protected:
	
	CSubConRTPGenericParamSetDummy();
    
	/* UID of RTP Flow factory */
	TUid								iRTPProtocolUiD;
	/* Who will be RTP SCPR's Service Provider? */
	TInt								iServiceProtocolID;
	/* Give EAttachToDefault to stack on Default IpScrpr */
	RSubConnection::TSubConnType		iSubConnType;
	
	/* Bandwdth Available to the RTP Application */
	TUint 								iBandWidth;
	/* Max Drop Allowec */ //:=@ ??
	TUint								iMaxDropout;
	/* Max misorder allowed */
	TUint 								iMaxMisorder;
	//:=@ ??
    TUint 								iMinSequential;
    /* mapping between RTP Time and "normal" time */
    TUint 								iRtpTimeConversion;
	TUint 								iRtpTimeNow;
	/* if ETrue sends RTCP report automatically */ 
	TBool 								iAutoSendRtcp;
	/* The CName for RTP Session */
	RBuf8 								iCName;
	/* Set the default destination address */
	TInetAddr                           iDefDestAddr;
	/* Extra Fields for Test Case Selection */
	
	/* Fails the NoBearer from Flow */
	TBool 								iNoBearerFail;
	TBool								iWrongProvision;
	TBool                               iRandomFail;
	TUint								iTestData;
	};


class CSubConDummyRTPParamFactory: public CBase
/** Factory used to create instances of the RTP SubConnection Parameter Extension Sets.

@internalAll
*/
	{
public:
    enum { iUid = KDummySubConnRTPParamsImplUid };
	static CSubConParameterSet* NewL(TAny* aConstructionParameters);
	};


CSubConRTPGenericParamSetDummy* CSubConRTPGenericParamSetDummy::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
/** Creates a generic RTP family parameters sub-connection.

@param aFamily parameter sets.
@param aType parameter type (Requested/Acceptable/Granted) 
@return a CSubConRTPGenericParamSet object pointer if successful,
otherwise leaves with system error code.
*/
	{
	CSubConRTPGenericParamSetDummy* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CSubConRTPGenericParamSetDummy* CSubConRTPGenericParamSetDummy::NewL()
/** Creates a generic RTP family parameters sub-connection.

@return a CSubConRTPGenericParamSet object pointer if successful,
otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KDummySubConnRTPParamsImplUid, KSubConnDummyRTPGenericParamsType);
	return static_cast<CSubConRTPGenericParamSetDummy*>(CSubConParameterSet::NewL(typeId));
	}

#endif	// __DUMMY_RTP_SUBCONPARAMS_H__
