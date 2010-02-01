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
* Name          : MSIPSecEngineContext.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPSECENGINECONTEXT_H
#define MSIPSECENGINECONTEXT_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class MSIPSecEngineContext
	{
public:
	virtual void SetValueL(const TDesC8& aName, const TDesC8& aValue) = 0;
	virtual void RemoveValue(const TDesC8& aName) = 0;
	virtual const TDesC8& ValueL(const TDesC8& aName) = 0;
	};

#endif
