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
* Name          : CSIPViaHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPVIAHEADERPARAMS_H
#define CSIPVIAHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPViaHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPViaHeaderParams* DecodeL (const TDesC8& aValue);
	static CSIPViaHeaderParams* NewL (const CSIPViaHeaderParams& aParams);
	static CSIPViaHeaderParams* NewLC (const CSIPViaHeaderParams& aParams);

	CSIPViaHeaderParams ();
	~CSIPViaHeaderParams ();

	TBool operator==(const CSIPViaHeaderParams& aParams) const;

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;

private: // For testing purposes

	UNIT_TEST(CSIPViaHeaderParamsTest)
    };

#endif // CSIPVIAHEADERPARAMS_H

// End of File
