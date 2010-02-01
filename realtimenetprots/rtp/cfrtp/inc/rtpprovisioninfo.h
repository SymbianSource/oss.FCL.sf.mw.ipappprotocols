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
// RTP Provision info definitions.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __RTP_PROVISION_INFO_H
#define __RTP_PROVISION_INFO_H

#include <e32base.h>
#include <e32std.h>
#include <in_sock.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include <comms-infras/metabuffer.h>
#include <es_enum.h>

/* Giving RTPSCPR Dlls UID */
 const TUint KRtpProvisionUid        = 0xE10042F0;
 const TUint KRtpProvisionConfigType = 0x100;
 
 
class CRtpProvisionConfig: public CBase, public Meta::SMetaData
	{
	 /** 
	 *  CRtpProvisionConfig is used by RtpScpr to pass the provision info 
	 *  to RTP and RTCP Flows
	 *  @internalComponent 
	 **/				    
      public:
	  static CRtpProvisionConfig*  NewLC();
	  static CRtpProvisionConfig*  NewL();

	  void SetBandWidth(TUint aBandWidth)
	  	{
		iBandWidth = aBandWidth;
	 	}
	  
	  void SetMaxDropOut(TUint aMaxDropout)
	  	{
        iMaxDropout = aMaxDropout;
	 	}

	  void SetMaxMisorder(TUint aMaxMisorder)
	  	{
		iMaxMisorder = aMaxMisorder;
	 	}

	  void SetMinSequential(TUint aMinSequential)
	  	{
		iMinSequential = aMinSequential;
	  	}

	  void SetRtptimeConversion(TUint aRtpTimeConversion)
	  	{
		iRtpTimeConversion = aRtpTimeConversion;
	  	}

	  void SetRtpTimeNow(TUint aRtpTimeNow)
	  	{
		iRtpTimeNow = aRtpTimeNow;
	  	}

	  void SetRtpAutoSend(TBool aAutoSendRtcp)
	  	{
		iAutoSendRtcp = aAutoSendRtcp;
	  	}
	  	
	  void SetDefDestinationAddr(const TInetAddr& aDestAddr)
	  	{
	  	iDefDestAddr = aDestAddr;
	  	}

	  void SetCNAMEL(const TDesC8& aCName)
	  	{
	  	iCName.CreateL(aCName);
	  	return;
	  	}

	  void SetRtcpReportInterval(const TTimeIntervalMicroSeconds32& aInterval)
	  	{
		iRtcpInterval = aInterval;
		return;
	  	}
	  	
	  TUint BandWidth() const
	  	{
		return iBandWidth;
	 	}
	  
	  TUint MaxDropOut() const
	  	{
        return iMaxDropout;
	 	}

	  TUint MaxMisorder() const
	  	{
		return iMaxMisorder;
	 	}

	   TUint MinSequential() const
	  	{
		return iMinSequential;
	  	}

	   TUint RtptimeConversion() const
	  	{
		return iRtpTimeConversion;
	  	}

	  TUint RtpTimeNow() const
	  	{
		return iRtpTimeNow;
	  	}

	  TBool RtpAutoSend() const
	  	{
		return iAutoSendRtcp;
	  	}
	  	
	  const TDesC8& GetCNAME() const
	  	{
	  	return iCName;
	  	}
	  	
	  const TInetAddr& GetDefDestinationAddr() const
	  	{
	  	return iDefDestAddr;
	  	}

	  const TTimeIntervalMicroSeconds32& GetRtcpReportInterval() const
	  	{
		return iRtcpInterval;
	  	}
	  	
	  ~CRtpProvisionConfig()
	  	{
	  	iCName.Close();
	  	}

	  private:
	  
	  CRtpProvisionConfig()
	  	{
	  	
	  	}
      
	  DATA_VTABLE

	  TUint iBandWidth;
	  TUint iMaxDropout;
	  TUint iMaxMisorder;
      TUint iMinSequential;
      TUint iRtpTimeConversion;
	  TUint iRtpTimeNow;
	  TBool iAutoSendRtcp;
	  RBuf8 iCName;
	  TInetAddr iDefDestAddr;
	  TTimeIntervalMicroSeconds32 iRtcpInterval;
	   
	};

#endif //__RTP_PROVISION_INFO_H

