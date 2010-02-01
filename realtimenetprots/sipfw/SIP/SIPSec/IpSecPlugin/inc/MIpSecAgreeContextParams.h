/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MIpSecAgreeContextParams.h
* Part of       : SIPSec
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef MIPSECAGREECONTEXTPARAMS_H
#define MIPSECAGREECONTEXTPARAMS_H

#include "CState.h"

class MIpSecMechanismParams;
class TInetAddr;
class CSipSecAgreeRecord;
class RIpsecPolicyServ;

// IpSec Mechanism parameters
class MIpSecAgreeContextParams
	{
public:

	virtual MIpSecMechanismParams& MechParams() = 0;

	virtual TInetAddr LocalAddress() = 0;
	virtual TInetAddr RemoteAddress() = 0;
	virtual RIpsecPolicyServ& PolicyServer() = 0;
    virtual TBool HasOnlyOneRecord() const = 0;
	virtual TBool HasRecordInState( CState::TSecAgreeRecordState aState ) = 0;

	virtual void SaDeleted( CSipSecAgreeRecord* aRecord ) = 0;
	virtual void SaCleared( CSipSecAgreeRecord* aRecord ) = 0;
	
	virtual void SAReady( TBool aSuccess ) = 0;
	
	virtual TBool HasLongerLifetimeSA( TUint aTimeToCompareInMillisecs, 
	                                   TUint& aLongerLifetimeInMillisecs ) = 0;
	};

#endif // MIPSECAGREECONTEXTPARAMS_H

// End of File
