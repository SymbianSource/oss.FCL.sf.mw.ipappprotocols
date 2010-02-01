/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CancelUAC.h
* Part of       : TransactionUser
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CANCELUAC_H
#define CANCELUAC_H

// INCLUDES
#include "NormalUAC.h"

// FORWARD DECLARATIONS
class CInviteUAC;

// CLASS DECLARATION

class CCancelUAC : public CNormalUAC
	{
public:
	static CCancelUAC* NewL(CUserAgentCreateParams& aParams,							
                            MSipConnectionMgr& aConnectionMgr,
                            MSipUriResolver& aResolver,							
							CSIPSec& aSIPSec,
							TUint32 aCSeqNumber);
	~CCancelUAC() {}


	void CreateAndSendCancelL(TTransactionId aInviteTaId);	

	static CCancelUAC& Ptr(CUserAgent& aUserAgent);

private:
	CCancelUAC(CUserAgentCreateParams& aParams,
			   MSipUriResolver& aResolver,
			   CSIPSec& aSIPSec,
			   TUint32 aCSeqNumber);

	void CreateCancelRequestL(TTransactionId aInviteTaId,
							  CInviteUAC& aInviteUac);

	void SendCancelRequestL(CUserAgentClient& aInviteUac);
	};

#endif // end of CANCELUAC_H

// End of File
