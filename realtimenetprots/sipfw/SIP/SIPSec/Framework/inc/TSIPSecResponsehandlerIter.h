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
* Name          : TSIPSecResponsehandlerIter.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPSECRESPONSEHANDLERITER_H
#define TSIPSECRESPONSEHANDLERITER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPSecResponseHandler;

// CLASS DECLARATION
class TSIPSecResponsehandlerIter
	{
public: // Constructor
	TSIPSecResponsehandlerIter(
		RPointerArray<CSIPSecResponseHandler>& aResponseHandlers);

public: // New functions
	CSIPSecResponseHandler* Next();
	CSIPSecResponseHandler* First();	

private: // For internal use
	TInt Count() const;

private: // Data
	TInt iPos;
	RPointerArray<CSIPSecResponseHandler>& iResponseHandlers;
	};

#endif
