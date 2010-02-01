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
* Name        : sipsecsecuritymechanism.inl
* Part of     : SIPSec
* inline functions
* Version     : SIP/4.0
*
*/



#include "tsipsecmechanisminitparams.h"

const TUid KSIPSecMechanismInterfaceUid = {0x1020334C};


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSecSecurityMechanism::CSIPSecSecurityMechanism
// -----------------------------------------------------------------------------
//
inline CSIPSecSecurityMechanism::CSIPSecSecurityMechanism()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecSecurityMechanism::NewL
// -----------------------------------------------------------------------------
//
inline CSIPSecSecurityMechanism* CSIPSecSecurityMechanism::NewL(
	const TDesC8& aMatchString,
	TSIPSecMechanismInitParams& aSIPSecMechanismInitParams)
	{
	TEComResolverParams resolverParams; //giving params to the Ecom
	resolverParams.SetDataType(aMatchString);

	return reinterpret_cast<CSIPSecSecurityMechanism*>(
		REComSession::CreateImplementationL(KSIPSecMechanismInterfaceUid,
					   	_FOFF(CSIPSecSecurityMechanism, iDtor_ID_Key),
					    &aSIPSecMechanismInitParams,
					    resolverParams));
	}

// -----------------------------------------------------------------------------
// CSIPSecSecurityMechanism::~CSIPSecSecurityMechanism
// -----------------------------------------------------------------------------
//
inline CSIPSecSecurityMechanism::~CSIPSecSecurityMechanism()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
