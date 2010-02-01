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
* Name          : CSIPSecEngineContext.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECENGINECONTEXT_H
#define CSIPSECENGINECONTEXT_H

// INCLUDES
#include <e32base.h>
#include "CSIPSecParam.h"
#include "MSIPSecEngineContext.h"

// FORWARD DECLARATIONS
class TSIPSecParamIter;

//CLASS DECLARATION

class CSIPSecEngineContext : public CBase,
							 public MSIPSecEngineContext
	{
public: // Constructors and destructor
	static CSIPSecEngineContext* NewL();
	static CSIPSecEngineContext* NewLC();

	~CSIPSecEngineContext();

public: // From MSIPSecEngineContext:
	void SetValueL(const TDesC8& aName, const TDesC8& aValue);
	void RemoveValue(const TDesC8& aName);
	const TDesC8& ValueL(const TDesC8& aName);	

private: // For internal use
	CSIPSecEngineContext();
	TBool FindParam(const TDesC8& aName, TSIPSecParamIter& aParams);

private: // Data
	RPointerArray<CSIPSecParam> iSIPSecParams;

private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSipSecEngineTest;
#endif
	};

#endif
