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
* Name          : CSIPRetryAfterHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPRETRYAFTERHEADERPARAMS_H
#define CSIPRETRYAFTERHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPRetryAfterHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPRetryAfterHeaderParams* 
		NewL (const CSIPRetryAfterHeaderParams& aParams);
	static CSIPRetryAfterHeaderParams* 
		NewLC (const CSIPRetryAfterHeaderParams& aParams);

	CSIPRetryAfterHeaderParams ();
	~CSIPRetryAfterHeaderParams ();

	static CSIPRetryAfterHeaderParams* InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;

private: // For testing purposes

	UNIT_TEST(CSIPRetryAfterHeaderTest)
    };

#endif // CSIPRETRYAFTERHEADERPARAMS_H

// End of File
