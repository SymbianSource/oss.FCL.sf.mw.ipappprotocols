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
* Name          : CSIPFromToHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPFROMTOHEADERPARAMS_H
#define CSIPFROMTOHEADERPARAMS_H

#include "CSIPParamContainerBase.h"
#include "_sipcodecdefs.h"


class CSIPFromToHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPFromToHeaderParams* 
		NewL (const CSIPFromToHeaderParams& aParams);
	static CSIPFromToHeaderParams* 
		NewLC (const CSIPFromToHeaderParams& aParams);

	CSIPFromToHeaderParams ();
	~CSIPFromToHeaderParams ();

	TBool operator==(const CSIPFromToHeaderParams& aParams) const;
	static CSIPFromToHeaderParams* InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;

private: // For testing purposes

	UNIT_TEST(CSIPFromToHeaderParamsTest)
    };

#endif // CSIPFROMTOHEADERPARAMS_H

// End of File
