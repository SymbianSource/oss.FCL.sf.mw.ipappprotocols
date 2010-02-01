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
* Name          : CSIPContentTypeHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPCONTENTTYPEHEADERPARAMS_H
#define CSIPCONTENTTYPEHEADERPARAMS_H

#include "CSIPParamContainerBase.h"
#include "_sipcodecdefs.h"


class CSIPContentTypeHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPContentTypeHeaderParams* 
		NewL (const CSIPContentTypeHeaderParams& aParams);

	static CSIPContentTypeHeaderParams*
		NewLC (const CSIPContentTypeHeaderParams& aParams);

	CSIPContentTypeHeaderParams ();
	~CSIPContentTypeHeaderParams ();
	
	static CSIPContentTypeHeaderParams* 
		InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;

private: // For testing purposes

    UNIT_TEST(CSIPContentTypeHeaderParamsTest)
    };

#endif // CSIPCONTENTTYPEHEADERPARAMS_H

// End of File
