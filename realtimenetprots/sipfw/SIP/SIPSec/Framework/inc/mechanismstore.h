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
* Name          : mechanismstore.h
* Part of       : SIPSec
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef __MSIPSEC_MECHANISMSTORE_H__
#define __MSIPSEC_MECHANISMSTORE_H__

// INCLUDES
#include <e32base.h>
#include "TSIPSecMechanismIter.h"

// CLASS DECLARATION
/**
*  @lib SipServer.exe
*/
class MSIPSecMechanismStore
{
public:
	/**
	* Returns an iteration to the list of supported
	* security mechanism.
	* 
	* @return TSIPSecMechanismIter	
	*/
	virtual TSIPSecMechanismIter& MechanismIter() = 0;
	
	/**
	* Search for a security mechanism matching the mechanism name.
	* 
	* @return MSIPSecSecurityMechanism, or NULL if not found. Ownership is not
	*	transferred.
	*/
	virtual MSIPSecSecurityMechanism*
		MechanismByName( const TDesC8& aMechanismName ) = 0;
};

#endif
