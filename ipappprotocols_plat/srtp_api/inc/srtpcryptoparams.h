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
* Description:    Contains a default cryptographic context for SRTP/SRTCP streams.
*
*/




#ifndef __SRTP_CRYPTOPARAMS_H__
#define __SRTP_CRYPTOPARAMS_H__

#include <e32base.h>
#include "srtpcryptocontext.h"

/**
*  SRTP/SRTCP crypto context param 
*  RFC 3711, 4568
*
*  @lib srtp.dll
*/

/**
* Encyption algorithms.
*/
enum TSRTPEncAlg
    {
    ENullAlg = 0,
    EEncAES_CM
    };

/**
* Authentication algorithms.
* With RCCm1, RCCm2 and RCCm3, ROC will be synchronized if SEQ%R==0
*/
enum TSRTPAuthAlg
    {
    EAuthNull = 0,
    EAuthHMAC_SHA1,
    EAuthRCCm1,
    EAuthRCCm2,
    EAuthRCCm3
    };
    

class TSrtpCryptoParams
	{
	public: 
	TSrtpCryptoParams();
	
	TSRTPEncAlg iSrtpEncAlg; 
	TSRTPEncAlg iSrtcpEncAlg;                
	TSRTPAuthAlg iSrtpAuthAlg; 
	TSRTPAuthAlg iSrtcpAuthAlg; 
	TUint iSrtpAuthTagLen;
	TUint iSrtcpAuthTagLen;             
	TUint iSrtpKeyDervRate; //The key derivation rate
	TUint iSrtcpKeyDervRate;    
	TUint iPrefixLen;   
	TBool iSrtpReplayProtection;
	TBool iSrtcpReplayProtection;   
	TUint16 iROCTransRate;   
	TUint64 iMasterKeysLifeTime; //Set both for SRTP/SRTCP otherwise use default
	TInt iReplayWindowSizeHint; //Set both for SRTP/SRTCP otherwise use default
	TBool iIsRCCm3Sync;

	};

//IF any of the following param changes it will cause the caculation changes.
inline TSrtpCryptoParams::TSrtpCryptoParams() : 
	  	iSrtpEncAlg( EEncAES_CM ), //encoding algorithm
       	iSrtcpEncAlg( EEncAES_CM ), 
        iSrtpAuthAlg( EAuthHMAC_SHA1 ), // authentication algorithm
        iSrtcpAuthAlg(EAuthHMAC_SHA1 ), 
        iSrtpAuthTagLen(KSRTPAuthTagDefaultLength),
        iSrtcpAuthTagLen(KSRTPAuthTagDefaultLength),
        iSrtpKeyDervRate( KSRTPDefaultKeyDerivationRate ),
        iSrtcpKeyDervRate( KSRTPDefaultKeyDerivationRate ),
        iPrefixLen( KSRTPPrefixLength ), // prefix length	 must be 0
        iSrtpReplayProtection( ETrue ), // replay protection on/off
        iSrtcpReplayProtection( ETrue),
        iROCTransRate(KSRTPDefaultROCTransRate), // Integrity transform parameter
        iMasterKeysLifeTime (0),
        iReplayWindowSizeHint (KReplayWindowSize),
        iIsRCCm3Sync(EFalse) // using in RCCm3 only defined in RFC srtp-rcc04 Section 4	
                  
	{
	};       


	
#endif // __SRTP_CRYPTOPARAMS_H__