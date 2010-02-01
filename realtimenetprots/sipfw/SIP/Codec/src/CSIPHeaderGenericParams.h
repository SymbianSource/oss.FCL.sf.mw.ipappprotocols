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
* Name          : CSIPHeaderGenericParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPHEADERGENERICPARAMS_H
#define CSIPHEADERGENERICPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPHeaderGenericParams : public CSIPParamContainerBase
	{
public:

	static CSIPHeaderGenericParams* 
		NewL (const CSIPHeaderGenericParams& aParams);
	static CSIPHeaderGenericParams* 
		NewLC (const CSIPHeaderGenericParams& aParams);

	CSIPHeaderGenericParams ();
	~CSIPHeaderGenericParams ();

	static CSIPHeaderGenericParams* InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;
    };

#endif // CSIPHEADERGENERICPARAMS_H

// End of File
