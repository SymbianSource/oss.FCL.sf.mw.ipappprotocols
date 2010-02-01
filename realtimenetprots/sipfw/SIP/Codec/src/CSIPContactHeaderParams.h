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
* Name          : CSIPContactHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPCONTACTHEADERPARAMS_H
#define CSIPCONTACTHEADERPARAMS_H

#include "CSIPParamContainerBase.h"
#include "_sipcodecdefs.h"


class CSIPContactHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPContactHeaderParams* 
		NewL (const CSIPContactHeaderParams& aParams);
	static CSIPContactHeaderParams* 
		NewLC (const CSIPContactHeaderParams& aParams);

	CSIPContactHeaderParams ();
	~CSIPContactHeaderParams ();

	TBool operator==(const CSIPContactHeaderParams& aParams) const;
	static CSIPContactHeaderParams* InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;

private: // For testing purposes

	UNIT_TEST(CSIPContactHeaderParamsTest)
    };

#endif // CSIPCONTACTHEADERPARAMS_H

// End of File
