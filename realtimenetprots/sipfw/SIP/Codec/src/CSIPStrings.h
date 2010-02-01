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
* Name          : CSIPStrings.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSTRINGS_H
#define CSIPSTRINGS_H

#include <e32base.h>
#include <stringpool.h>


class CSIPStrings : public CBase
	{
public:

	static CSIPStrings* NewL();
	static CSIPStrings* NewLC();
	~CSIPStrings();
	
	void IncrementUsageCount();
	void DecrementUsageCount();
	TUint UsageCount();	
	
	RStringPool& Pool();
	static const TStringTable& Table();

private:

	CSIPStrings();
	void ConstructL();

private: // Data
		
	RStringPool iPool;
	TUint iUsageCount;
	};

#endif // CSIPSTRINGS_H
