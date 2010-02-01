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
// Header for class specific to RTP packet handling
// 
//

/**
 @file
 @internalComponent
*/
 
#if !defined(RTPAPI_H__)
#define RTPAPI_H__

#include <e32def.h>
#include <e32cmn.h>
#include <e32std.h>

#include "rtpsock.h"


const TUint KLayOutRtpMarkerOffset = 1;
const TUint KLayOutRtpPayloadTypeOffset = KLayOutRtpMarkerOffset;
const TUint KLayOutRtpSequenceNoOffset = 2;
const TUint KLayOutRtpTimeStampOffset = 4;
const TUint KLayOutRtpSSRCOffset = 8;
const TUint KLayOutRtpCSRCOffset = 12;
const TUint KLayOutRtpHeaderExtension = 4; // in bytes
const TUint KLayOutRtpFixedHeader = 12;   // in bytes 


// RTCP Constants

const TUint KRtcpFixedSR =  21; // FixedHeader(20) + PacketType (1)
const TUint KRtcpFixedRR =  25; // FixedHeader(24) + PacketType (1)
const TUint KRtcpFixedSDES = 6;// PacketType(1)  + Length(1) + SSRC(4)
const TUint KRtcpFixedBYE  = 5; // PacketType(1) + SSRC(4)
const TUint KRtcpFixedAPP  = 2; // PacketType (1) + AppType (1) 
const TUint KRtcpParamFixed = 6; // PacketType(1) + ParamType(1) + ParamValue (4)




enum TRtpParams 
{
	EBandwidth=0,
	ERtpTimeConversion,
	ERtpTimeConversionNow,
	ERtcpInterval,
	EMaxDropOut,
	EMaxMisOrder,
	EMinSequenical,
	EAutoSend,
	EStopReceive,
	ERtcpCompound,
	ESR,
	ERR
};

class TRtpParam
{
public:
	 TRtpParam(TInt aLen =0);
	 TRtpParam(TDes8& aBuffer);
	 void AssignL(RBuf8& aBuffer);
	 void SetRtpParam(TRtpParams aType, TUint32 aParam);
	 void SetRtpParam(TRtpParams aType,const TDesC8& aValue);
	 TRtpParams GetParamType();
	 TUint32 GetParam();
	 TPtrC8 GetData();
	 TInt Type();
private:
	TRtpParam();
	TUint8* iFirstOctet;
	TUint8* iHeaderOctet;
	TInt32 iLength;
};

class TRtcpSdesPack
/*
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    Attrib     |     length    | user and domain name        ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
	{
public:
	 TRtcpSdesPack(TUint32 aLength);
	 void AssignL(RBuf8& aBuffer);
	 TRtcpSdesPack(TDes8& aPacket);
	 void SetAttribute(TUint8 aAttrib,const TDesC8& aValue);
	
	
	//PRIV Packet Atrib =PRIV = 8
/*	
	0 				    1  					2					3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| PRIV=8        | length        | prefix length |prefix string...
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	... | value string ...
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
	 void SetAttribute(const TDesC8& aPreString,const TDesC8& aValue);
	

	 TUint8 Length();
	 TPtrC8 Attribute();
	 TUint8 Type();
	// in case of PRIV SDes Packet.....
	 TPtrC8 PreFixString();
	
private:
	TRtcpSdesPack();
	TUint8* iFirstOctet;
	TUint8* iHeaderOctet;
	TInt32 iLength;
	};


// APP Packet
class TRtcpAPPPack
	{
public:
	 TRtcpAPPPack(TInt aLength);
	 void AssignL(RBuf8& aBuffer);
	 TRtcpAPPPack(TDes8& aPacket);
	 void SetSubType(TUint aSubType);
	 void SetAppData(const TDesC8& aDesc);
	 void SetAppName(const TDesC8& aName);
	 TUint32 SubType();
	 TPtrC8 AppData();
	 TPtrC8 AppName();
	 TUint8 Type();
private:
	TRtcpAPPPack();
	TUint8* iFirstOctet;
	TUint8* iHeaderOctet;
	TInt32 iLength;
	};


// BYE Packet 
class TRtcpBYEPack
	{
public:
	 TRtcpBYEPack(TInt aLength);
	 void AssignL(RBuf8& aBuffer);
	 TRtcpBYEPack(TDes8& aPacket);
	 void SetSSRC(TUint32 aSSRC);
	 void SetReason(const TDesC8& aDesc);
	 TUint32 SSRC();
	 TPtrC8 Reason();
private:
	TRtcpBYEPack();
	TUint8* iFirstOctet;
	TUint8* iHeaderOctet;
	TInt32 iLength;
	};
	
#endif

