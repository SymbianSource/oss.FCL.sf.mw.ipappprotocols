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
* Name          : sipsecsecuritymechanism.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef __SIPSEC_SECURITY_MECHANISM_H__
#define __SIPSEC_SECURITY_MECHANISM_H__

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include "MSIPSecSecurityMechanism.h"

// FORWARD DECLARATIONS
class TSIPSecMechanismInitParams;


class CSIPSecSecurityMechanism : public CBase,
  								 public MSIPSecSecurityMechanism
	{
public:
	virtual ~CSIPSecSecurityMechanism();
	
	static CSIPSecSecurityMechanism* NewL(
				const TDesC8& aProfileType,
				TSIPSecMechanismInitParams& aSIPSecMechanismInitParams);

protected:
 	inline CSIPSecSecurityMechanism();

private:
	// Unique instance identifier key
	TUid iDtor_ID_Key;
 	};
 
#include "sipsecsecuritymechanism.inl"

#endif // end of __SIPSEC_SECURITY_MECHANISM_H__

// End of File
