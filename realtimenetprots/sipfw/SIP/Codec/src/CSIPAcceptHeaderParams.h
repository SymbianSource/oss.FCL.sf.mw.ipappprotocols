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
* Name          : CSIPAcceptHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPACCEPTHEADERPARAMS_H
#define CSIPACCEPTHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPAcceptHeaderParams : public CSIPParamContainerBase
	{
public:


	static CSIPAcceptHeaderParams* 
		NewL (const CSIPAcceptHeaderParams& aParams);

	static CSIPAcceptHeaderParams*
		NewLC (const CSIPAcceptHeaderParams& aParams);

	CSIPAcceptHeaderParams ();
	~CSIPAcceptHeaderParams ();
	
	static CSIPAcceptHeaderParams* InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase
	
	void CheckParamL (CSIPParam& aParam) const;
    };

#endif // CSIPACCEPTHEADERPARAMS_H

// End of File
