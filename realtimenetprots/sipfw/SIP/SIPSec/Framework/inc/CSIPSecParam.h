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
* Name          : CSIPSecParam.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECPARAM_H
#define CSIPSECPARAM_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CSIPSecParam : public CBase
	{
public: // Constructors and destructor
	static CSIPSecParam* NewL(const TDesC8& aName, const TDesC8& aValue);
	static CSIPSecParam* NewLC(const TDesC8& aName, const TDesC8& aValue);
	~CSIPSecParam();

public: // New functions
	void UpdateValueL(const TDesC8& aValue);
	const TDesC8& ParamName () const;
	const TDesC8& Value () const;

private: // For internal use
	void ConstructL(const TDesC8& aName, const TDesC8& aValue);
	CSIPSecParam();

private: // Data
	HBufC8* iParamName; //owned
	HBufC8* iValue; //owned
	};

#endif
