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
* Name          : CSIPQueryConditionBase.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPQUERYCONDITIONBASE_H
#define CSIPQUERYCONDITIONBASE_H

//INCLUDES
#include <e32base.h>

// CONSTANTS
const TUint KSIPDefaultPort = 5060;
const TUint KSIPsDefaultPort = 5061;

// FORWARD DECLARATIONS
class CSIPQueryBase;

// CLASS DEFINITION
/**
 * CSIPQueryConditionBase implements a
 */
class CSIPQueryConditionBase : public CBase
	{
	public:// Constructors and destructor

		virtual ~CSIPQueryConditionBase();
		
	public:
	
		virtual CSIPQueryBase& QueryBase() = 0;

		virtual CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus ) = 0;

	protected:
	
		CSIPQueryConditionBase();

	};
#endif // end of CSIPQUERYCONDITIONBASE_H

// End of File
