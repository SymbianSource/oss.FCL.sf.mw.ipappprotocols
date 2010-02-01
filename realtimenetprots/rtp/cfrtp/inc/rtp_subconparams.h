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
// RTP SubConParams Definitions.
// 
//

/**
 @file
 @internalComponent
 @released
 @since v9.5
*/

#ifndef __RTP_SUBCONPARAMS_H__
#define __RTP_SUBCONPARAMS_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include <comms-infras/metabuffer.h>
#include <comms-infras/ss_protocolparameterset.h>

#include <es_sock.h>

#include <rtp_uid.h>


const TInt32 KSubConnRTPGenericParamsType = 2;

class CSubConRTPGenericParamSet : public ESock::CSubConnectionProtocolParameterSet
	{
	friend class CSubConRTPParamFactory;
	public:

	inline static CSubConRTPGenericParamSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	
	inline static CSubConRTPGenericParamSet* NewL();
	
	virtual ~CSubConRTPGenericParamSet();
	
	virtual TInt SetTierID(TUid aTierId);
	
	/* RTP Stack Initialisation Parameters Get/Set */
  	virtual void SetBandWidth(TUint aBandWidth);
  
  	virtual void SetMaxDropOut(TUint aMaxDropout);

	virtual void SetMaxMisorder(TUint aMaxMisorder);

	virtual void SetMinSequencial(TUint aMinSequential);

	virtual void SetRtptimeConversion(TUint aRtpTimeConversion);

	virtual void SetRtpTimeNow(TUint aRtpTimeNow);

	virtual void SetRtpAutoSend(TBool aAutoSendRtcp);
	
	/* Set the UID of RTP Flow inplementation  */
	virtual void SetProtocolUIDRtp(TUid aRTPProtocolUiD);
			
	virtual void SetServiceProtocolID(TInt aServiceProtocolID);
	
	virtual void SetLowerSubConnType(RSubConnection::TSubConnType aSubConnType);
	
	/** Set the CNAME for the session
	
	@param aCName The CNAME descriptor.This should be same as the one used in the API:
    			  void RRtpSession::OpenL(RSocketServ& aServer, TSockAddr& aLocalAddr,
				  TSockAddr& aRemoteAddr,
				  TInt aMaxRXSize, RSubConnection& aSubConnection, TInt aPriority = EPriorityNormal, 
				  const TDesC8& aCNAME = KNullDesC8)    				
	*/
	
	virtual void SetCNAMEL(const TDesC8& aCName);
	
	virtual void SetDefDestinationAddr(TInetAddr& aDefDestAddr);

	virtual void SetRtcpReportInterval(const TTimeIntervalMicroSeconds32& aInterval);


    /* The Get Functions are not made Virtual. They are supposed to be accessed
       inside the CF Dll */
	TUid GetProtocolUiDRtp();
	
	TInt GetServiceProtocolID();
	
	RSubConnection::TSubConnType GetLowerSubConnType();

	TUint BandWidth() const;
	  
	TUint MaxDropOut() const;
	 
	TUint MaxMisorder() const;
	  
	TUint MinSequencial() const;

	TUint RtptimeConversion() const;

	TUint RtpTimeNow() const;

	TBool RtpAutoSend() const;

	const TTimeIntervalMicroSeconds32& GetRtcpReportInterval() const;
	
	const TDesC8& GetCNAME() const;
	
	const TInetAddr& GetDefDestinationAddr() const;


	DATA_VTABLE
	
	protected:
	
	CSubConRTPGenericParamSet();
    
	/* UID of RTP Flow factory */
	TUid								iRTPProtocolUiD;
	/* Who will be RTP SCPR's Service Provider? */
	TInt								iServiceProtocolID;
	/* Give EAttachToDefault to stack on Default IpScrpr */
	RSubConnection::TSubConnType		iSubConnType;
	
	/* Bandwdth Available to the RTP Application */
	TUint 								iBandWidth;
	/* Max Drop Allowed */ 
	TUint								iMaxDropout;
	/* Max misorder allowed */
	TUint 								iMaxMisorder;
    TUint 								iMinSequencial;
     
    /* mapping between RTP Time and "normal" time */
    TUint 								iRtpTimeConversion;
	TUint 								iRtpTimeNow;
	/* if ETrue sends RTCP report automatically */ 
	TBool 								iAutoSendRtcp;
	/* The CName for RTP Session */
	RBuf8 								iCName;
	/* Set the default destination address */
	TInetAddr                           iDefDestAddr;
	/* RTCP report interval. Overrides report calculation inside stack */
	TTimeIntervalMicroSeconds32			iRtcpInterval;
	};


class CSubConRTPParamFactory: public CBase
/** Factory used to create instances of the RTP SubConnection Parameter Extension Sets.

@internalComponent
*/
	{
public:
    enum { iUid = KSubConnRTPParamsImplUid };
	static CSubConParameterSet* NewL(TAny* aConstructionParameters);
	};


CSubConRTPGenericParamSet* CSubConRTPGenericParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
/** Creates a generic RTP family parameters sub-connection.

@param aFamily parameter sets.
@param aType parameter type (Requested/Acceptable/Granted) 
@return a CSubConRTPGenericParamSet object pointer if successful,
otherwise leaves with system error code.
*/
	{
	CSubConRTPGenericParamSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CSubConRTPGenericParamSet* CSubConRTPGenericParamSet::NewL()
/** Creates a generic RTP family parameters sub-connection.

@return a CSubConRTPGenericParamSet object pointer if successful,
otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(CSubConRTPParamFactory::iUid, KSubConnRTPGenericParamsType);
	return static_cast<CSubConRTPGenericParamSet*>(CSubConParameterSet::NewL(typeId));
	}

#endif	// __RTP_SUBCONPARAMS_H__
