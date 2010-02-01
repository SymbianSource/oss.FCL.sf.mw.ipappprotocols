/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Contains SRTP definitions and constants.
*
*/




#ifndef __SRTP_DEF_H__
#define __SRTP_DEF_H__

// INCLUDES
#include <e32def.h>
#include <e32des8.h>

// CONSTANTS
//Block size of the 128 bit AES-CM in bytes
const TInt KAESCM128BlockSizeInBytes = 0x10;

const TInt KMinSizeRtpHeader = 12; // 96 bits in header, 96/8 = 12
const TInt KMinSizeRtcpHeader = 8; // 96 bits in header, 96/8 = 12

// The 80 bit length of the HMAC-SHA1 authentication tag (n_tag)
const TUint KSRTPAuthTagLength80 = 80;

//recommending to use in RCCm1 or RCCm2 as 14 octets
const TUint KSRTPAuthTagLength112 = 112;
// 4 octets32 bits
const TUint KSRTPROCLength4 = 4; 

// The 32 bit length of the HMAC-SHA1 authentication tag (n_tag)
// Also recommending in RCCm3 as 4 octects
const TUint KSRTPAuthTagLength32 = 32;

// The default bit length of the HMAC-SHA1 authentication tag (n_tag)
const TUint KSRTPAuthTagDefaultLength = KSRTPAuthTagLength80;

// The default length of the SRTP prefix
const TUint KSRTPPrefixLength = 0;

// default SRTP key derivation rate: 0
const TUint16 KSRTPDefaultKeyDerivationRate = 0x00;

// default SRTP ROC Transmission rate: 1
const TUint16 KSRTPDefaultROCTransRate = 0x01;

const TUint KSRTCPPacketIndexLength4 = 4; 
// default label values
const TUint KSRTPEncryptionLabel = 0x00;
const TUint KSRTPAuthenticationLabel = 0x01;
const TUint KSRTPSaltingLabel = 0x02;
const TUint KSRTCPEncryptionLabel = 0x03;
const TUint KSRTCPAuthenticationLabel = 0x04;
const TUint KSRTCPSaltingLabel = 0x05;

// default SRTP SESSION key lengths:
// default bit length (n_e) for session encryption key (k_e):
// 128 bits (16 octets)
const TUint KSRTPDefSessionEncryptionKeyLength = 0x80;
// default bit length (n_s) for session salt key (k_s):
// 112 bits (14 octets)
const TUint KSRTPDefSessionSaltingKeyLength = 0x70;
// default bit length (n_a) for session authentication key (k_a):
// 160 bits (20 octets)
const TUint KSRTPDefSessionAuthenticationKeyLength = 0xa0;

// default SRTP MASTER key lengths:
// default bit length for SRTP master key (128-bit AES):
// 128 bits (16 octets)
const TUint16 KSRTPDefaultMasterKeyLength = 0x80;
// default bit length for SRTP master salt 
// 112 bits (14 octets)
const TUint16 KSRTPDefaultMasterSaltLength = 0x70;

// SRTP limit values
// max SRTP plain/cipher text length is 2^23 == 0x80000
// since we use 8 bit descriptors, divide this with 2
const TUint KSRTPMaxTextLength = 0x40000;
// max SRTP keystream 128 bit blocks is 2^16 == 0x10000
// for an IV value we can not create more keystream blocks
// KSRTPMaxKeyStreamBlocks * 128 equals KSRTPMaxTextLength * 2 
const TUint KSRTPMaxKeyStreamBlocks = 0x10000;

const TInt KReplayWindowSize = 64;
//2^48 -1 for SRTP
const TUint64 KSRTPMasterKeysLifeTime = 0xFFFFFFFFFFFF;
const TUint32 KLeftNumOfPacketBeforeReKey = 0x64;
//2^31 -1 for SRTCP
const TUint64 KSRTCPMaterKeysLifeTime = 0x7FFFFFFF;
#endif // __SRTP_DEF_H__
