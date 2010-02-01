/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : SipSecIpsecParams.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef SIPSECIPSECPARAMS_H
#define SIPSECIPSECPARAMS_H

// MACROS

#define PID 	RProcess().Id()
#define MD5_KEYLENGTH	128
#define SHA_KEYLENGTH	160

// Max and min values for ports and SPI

const TUint32 KMinPort = 49152;
const TUint32 KMaxPort = 65535;
const TUint32 KMinSPI = 256;
const TUint32 KMaxSPI = 4294967295L;

// Sec Agree IpSec parameters

_LIT8(KIpSec3gpp, "ipsec-3gpp");
_LIT8(KHmacMd596, "hmac-md5-96");
_LIT8(KHmacSha196, "hmac-sha-1-96");
_LIT8(KPreference, "q");
_LIT8(KAlgorithm, "alg");
_LIT8(KMode, "mod");
_LIT8(KTransMode, "trans");
_LIT8(KProtocol, "prot");
_LIT8(KEspProtocol, "esp");
_LIT8(KSpic, "spi-c");
_LIT8(KSpis, "spi-s");
_LIT8(KPortc, "port-c");
_LIT8(KPorts, "port-s");

// For retrieving keying material from SIPSec FW

_LIT8(KSIPSecIK, "ik");

// Error messages

_LIT(KSendPanic, "RSADB::Send failed, status=%d");
_LIT(KMsgPanic, "SADB msg format, status=%d");

// Housekeeping params

const TUint8 KWordLen = 8;
const TUint8 KSizeOfSADBSupported = 8;
const TUint8 KSizeOfSADBAlg = 8;

const TUint8 KAuthKeyMaxSize = 20;
const TUint8 KAuthKeyFromSIMSize = 16;

const TUint KTempTime = 240000;

const TUint K200Response = 200;
const TUint K401Response = 401;

// IpSec capabilities
class TSipSecIpsecCaps
	{
public: // Data
	TPtrC8 iProt;
	TPtrC8 iMode;	
	TPtrC8 iAlgMd5;
	TPtrC8 iAlgSha;
	};

// SA parameters
class TSipSecSaParams
	{
public: // Data
	TUint iSpi_uc;
	TUint iSpi_us;
	TUint iPort_uc;
	TUint iPort_us;
	TUint iSpi_pc;
	TUint iSpi_ps;
	TUint iPort_pc;
	TUint iPort_ps;
	TUint8 iAuthAlg;
	TBuf8<KAuthKeyMaxSize> iAuthKey;
	};

#endif // SIPSECIPSECPARAMS_H


// End of File
