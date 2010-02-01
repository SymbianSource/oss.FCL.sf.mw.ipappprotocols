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
* Name          : TSIPSecMechanismIter.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPSECMECHANISMITER_H
#define TSIPSECMECHANISMITER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSIPSecSecurityMechanism;

// CLASS DECLARATION
class TSIPSecMechanismIter
	{
public: // Constructor
	TSIPSecMechanismIter(RPointerArray<MSIPSecSecurityMechanism>& aMechanisms);

public: // New functions
	MSIPSecSecurityMechanism* Next();
	MSIPSecSecurityMechanism* First();	

	TInt Count();

private: // Data
	TInt iPos;
	RPointerArray<MSIPSecSecurityMechanism>& iMechanisms;
	};

#endif // __SIPSEC_MECHANISM_ITER_H__
