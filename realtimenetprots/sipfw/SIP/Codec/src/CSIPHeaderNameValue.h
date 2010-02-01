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
* Name          : CSIPHeaderNameValue.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPHEADERNAMEVALUE_H
#define CSIPHEADERNAMEVALUE_H

#include <e32base.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"


class CSIPHeaderNameValue : public CBase
	{
public:
    
	static CSIPHeaderNameValue* NewL (const TDesC8& aName, 
		                              const TPtrC8 aValue);

	static CSIPHeaderNameValue* NewLC (const TDesC8& aName, 
		                               const TPtrC8 aValue);    
    
	RStringF Name ();
	const TDesC8& Value ();

	~CSIPHeaderNameValue ();

private:

	void ConstructL (const TDesC8& aName);
    CSIPHeaderNameValue (const TPtrC8 aValue);

	TPtrC8 ParseNameL (const TDesC8& aName);

private: // Data

	RStringF iName;
	TPtrC8  iValue;

private: // For testing purposes
	
	UNIT_TEST(CSIPHeaderNameValueTest)
	};

#endif // CSIPHEADERNAMEVALUE_H

// End of File
