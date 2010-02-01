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
* Name          : TSIPSecParamIter.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPSECPARAMITER_H
#define TSIPSECPARAMITER_H

// INCLUDES
#include <e32base.h>
#include "CSIPSecParam.h"

// CLASS DECLARATION
class TSIPSecParamIter
	{
public:	// Constructor
	TSIPSecParamIter(RPointerArray<CSIPSecParam>& aSIPSecParams);

public: // New functions
	CSIPSecParam* Next();
	CSIPSecParam* First();
	CSIPSecParam* Current();
	void DeleteCurrent();
	TInt Count() const;

private: // Data

	TInt iPos;
	RPointerArray<CSIPSecParam>& iSIPSecParams;
	};

#endif
