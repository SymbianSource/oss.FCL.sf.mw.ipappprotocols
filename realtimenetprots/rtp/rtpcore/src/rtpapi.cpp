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
// class for handling rtp data
// 
//

/**
 @file
 @internalComponent
*/

#include "rtpapi.h"
#include "rtputils.h"

TRtcpSdesPack::TRtcpSdesPack(TUint32 aLength)
	{
	iLength = aLength  + KRtcpFixedSDES;

	}
void TRtcpSdesPack::AssignL(RBuf8& aBuffer)
	{
	aBuffer.ReAllocL(iLength);
		
	TPtr8 ptr = aBuffer.MidTPtr(0);
	iHeaderOctet = const_cast<unsigned char*>(ptr.Ptr());
	// SDES packet Type 
	*(iHeaderOctet) = 202;
	iFirstOctet = iHeaderOctet + 1;
	aBuffer.SetLength(iLength);
	}

TRtcpSdesPack::TRtcpSdesPack(TDes8& aPacket)
	{
	iHeaderOctet = const_cast<TUint8*>(aPacket.Ptr());
	iLength = aPacket.Length();
	iFirstOctet = iHeaderOctet + 1;
	}

void TRtcpSdesPack::SetAttribute(TUint8 aAttrib,const TDesC8& aValue)
	{
	*iFirstOctet = aAttrib;
	*(iFirstOctet+1) = static_cast<TUint8>(aValue.Length());
	Mem::Copy(iFirstOctet+2, aValue.Ptr(), aValue.Length());
	}

TUint8 TRtcpSdesPack::Length()
	{
	return *(iFirstOctet+1);
	}

TPtrC8 TRtcpSdesPack::Attribute()
	{
	// Assert that the descriptor is of appropriate size
	const TUint8 len = *(iFirstOctet+1);
	return TPtrC8(iFirstOctet+2, len);
	}

TUint8 TRtcpSdesPack::Type()
	{
	return *iFirstOctet;
	}


/*
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    Attrib     |     length    | user and domain name        ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

//PRIV Packet Atrib =PRIV = 8
/*	
	0 				    1  					2					3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| PRIV=8 | length | prefix length |prefix string...
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	... | value string ...
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

void TRtcpSdesPack::SetAttribute(const TDesC8& aPreString,const TDesC8& aValue)
	{
	// PRIV SDES type
	*iFirstOctet = 8;
	*(iFirstOctet+1) = static_cast<TUint8> (aPreString.Length());
	Mem::Copy(iFirstOctet+2,aPreString.Ptr(),aPreString.Length());
	
	
	Mem::FillZ(iFirstOctet + *(iFirstOctet + 1),aValue.Length());
	
	*(iFirstOctet + *(iFirstOctet + 1) + 2 ) = static_cast<TUint8> (aValue.Length());
	Mem::Copy(iFirstOctet + *(iFirstOctet + 1) + 3,aValue.Ptr(),aValue.Length());
	
	}

TPtrC8 TRtcpSdesPack::PreFixString()
	{
	const TUint8 len = *(iFirstOctet + *(iFirstOctet + 1));
	return TPtrC8(iFirstOctet + *(iFirstOctet + 1),len );
	}


TRtcpAPPPack::TRtcpAPPPack(TInt aLength)
	{
	// App Standard Header Packet Type + APP Type = 2 bytes 
	iLength = aLength + KRtcpFixedAPP;
	}
void TRtcpAPPPack::AssignL(RBuf8& aBuffer)
	{
	aBuffer.ReAllocL(iLength);
		
	TPtr8 ptr = aBuffer.MidTPtr(0);
	iHeaderOctet = const_cast<unsigned char*>(ptr.Ptr());
	*(iHeaderOctet) = 204;
	iFirstOctet = iHeaderOctet + 1;
	aBuffer.SetLength(iLength);
	}
	
TUint8 TRtcpAPPPack::Type()
	{
	return *(iHeaderOctet);
	}
TRtcpAPPPack::TRtcpAPPPack(TDes8& aPacket)
	{
	iHeaderOctet = const_cast<unsigned char*>(aPacket.Ptr());
	iFirstOctet = iHeaderOctet + 1;
	iLength = aPacket.Length();
	}

void TRtcpAPPPack::SetAppName(const TDesC8& aName)
	{
	*(iFirstOctet + 1) = aName[0];
	*(iFirstOctet + 2) = aName[1];
	*(iFirstOctet + 3) = aName[2];
	*(iFirstOctet + 4) = aName[3];
	}
	
void TRtcpAPPPack::SetSubType(TUint aSubType)
	{
	*(iFirstOctet) = aSubType;	
	}
	
void TRtcpAPPPack::SetAppData(const TDesC8& aDesc)
	{
	TUint8* ptr = 
		reinterpret_cast<TUint8*>(iFirstOctet +  5);
	TPtr8 tptr(ptr,aDesc.Length());
    tptr = aDesc;
	}

	
TUint32 TRtcpAPPPack::SubType()
	{
	return (*(reinterpret_cast<TUint8*> (iFirstOctet)));
	}
	
TPtrC8 TRtcpAPPPack::AppData()
	{
	const TUint8* ptr = reinterpret_cast<const TUint8*>(iFirstOctet+5);
	return TPtrC8(ptr, iLength - 5 );
	}
	
TPtrC8 TRtcpAPPPack::AppName()
	{
	const TUint8* ptr = reinterpret_cast<const TUint8*>(iFirstOctet + 1);
	return TPtrC8(ptr,4);
	}
	
TRtcpBYEPack::TRtcpBYEPack(TInt aLength)
	{
	// Bye Fixed Header - 5 bytes...PacketType + SSRC
	// EByetype - 
	iLength = aLength + KRtcpFixedBYE;
	}
void TRtcpBYEPack::AssignL(RBuf8& aBuffer)
	{
	aBuffer.ReAllocL(iLength);
	
	TPtr8 ptr = aBuffer.MidTPtr(0);
	iHeaderOctet = const_cast<unsigned char*>(ptr.Ptr());
	
	// Bye Packet Type
	*(iHeaderOctet) = 203;
	iFirstOctet = iHeaderOctet + 1;
	aBuffer.SetLength(iLength);
	}
	
TRtcpBYEPack::TRtcpBYEPack(TDes8& aPacket)
	{
	iHeaderOctet = const_cast<unsigned char*>(aPacket.Ptr());
	iFirstOctet = iHeaderOctet + 1;
	iLength = aPacket.Length();
	}

	
void TRtcpBYEPack::SetSSRC(TUint32 aSSRC)
	{
	*(iFirstOctet) = static_cast<TUint8>(aSSRC>>24);
	*(iFirstOctet+1) = static_cast<TUint8>(aSSRC>>16);
	*(iFirstOctet+2) = static_cast<TUint8>(aSSRC>>8);
	*(iFirstOctet+3) = static_cast<TUint8>(aSSRC);
	}
	
void TRtcpBYEPack::SetReason(const TDesC8& aDesc)
	{
	TUint8* ptr = 
		reinterpret_cast<TUint8*>(iFirstOctet +  4);
	TPtr8 tptr(ptr,aDesc.Length());
    tptr = aDesc;
	}

	
TUint32 TRtcpBYEPack::SSRC()
	{
	return (*(iFirstOctet+3))+
			(*(iFirstOctet+2)<<8)+
			(*(iFirstOctet+1)<<16)+
			(*(iFirstOctet)<<24);

	}
TPtrC8 TRtcpBYEPack::Reason()
	{
	const TUint8* ptr = reinterpret_cast<const TUint8*>(iFirstOctet+4);
	return TPtrC8(ptr, iLength - 4 );
	}
	
// BYE Packet 

// RtpParams

 TRtpParam::TRtpParam(TInt aLen)
	{
	// TRtpParam Fixed Header - 1 + 1+ 4...PacketType  + ParamType + ParamValue
	
	iLength = KRtcpParamFixed + aLen;
	}
void TRtpParam::AssignL(RBuf8& aBuffer)
	{
	aBuffer.ReAllocL(iLength);
	
	TPtr8 ptr = aBuffer.MidTPtr(0);
	iHeaderOctet = const_cast<unsigned char*>(ptr.Ptr());
	
	// TRtpParams Packet Type
	*(iHeaderOctet) = 199;
	iFirstOctet = iHeaderOctet + 1;
	aBuffer.SetLength(iLength);
	
	}
TInt TRtpParam::Type()
	{
	return *(iHeaderOctet);
	}
	
TRtpParam::TRtpParam(TDes8& aBuffer)
	{
	iHeaderOctet = const_cast<unsigned char*>(aBuffer.Ptr());
	iFirstOctet = iHeaderOctet + 1;
	iLength = aBuffer.Length();
	}
	
	
void TRtpParam::SetRtpParam(TRtpParams aType, TUint32 aParam)
	{
	*(iFirstOctet) = aType;
	*(iFirstOctet+1) = static_cast<TUint8>(aParam>>24);
	*(iFirstOctet+2) = static_cast<TUint8>(aParam>>16);
	*(iFirstOctet+3) = static_cast<TUint8>(aParam>>8);
	*(iFirstOctet+4) = static_cast<TUint8>(aParam);
	}
void TRtpParam::SetRtpParam(TRtpParams aType, const TDesC8& aValue)
	{
	*(iFirstOctet) = aType;
	TUint8* ptr = 
		reinterpret_cast<TUint8*>(iFirstOctet +  1);
	TPtr8 tptr(ptr,aValue.Length());
    tptr = aValue;
		
	}
TRtpParams TRtpParam::GetParamType()
	{
	return TRtpParams(*(iFirstOctet));
	}
TUint32 TRtpParam::GetParam()
	{
	return (*(iFirstOctet+4))+
		(*(iFirstOctet+3)<<8)+
		(*(iFirstOctet+2)<<16)+
		(*(iFirstOctet+1)<<24);
	}
TPtrC8 TRtpParam::GetData()
	{
	const TUint8* ptr = reinterpret_cast<const TUint8*>(iFirstOctet+1);
	return TPtrC8(ptr, iLength - 1);
	}
