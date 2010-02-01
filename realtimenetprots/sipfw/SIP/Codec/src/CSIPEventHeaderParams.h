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
* Name          : CSIPEventHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPEVENTHEADERPARAMS_H
#define CSIPEVENTHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPEventHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPEventHeaderParams* NewL (const CSIPEventHeaderParams& aParams);
	static CSIPEventHeaderParams* NewLC (const CSIPEventHeaderParams& aParams);

	CSIPEventHeaderParams ();
	~CSIPEventHeaderParams ();

	TBool operator==(const CSIPEventHeaderParams& aParams) const;

	static CSIPEventHeaderParams* InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;
    };

#endif // CSIPEVENTHEADERPARAMS_H

// End of File
