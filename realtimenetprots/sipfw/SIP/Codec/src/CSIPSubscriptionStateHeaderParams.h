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
* Name          : CSIPSubscriptionStateHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSUBSCRIPTIONSTATEHEADERPARAMS_H
#define CSIPSUBSCRIPTIONSTATEHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPSubscriptionStateHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPSubscriptionStateHeaderParams* 
		NewL (const CSIPSubscriptionStateHeaderParams& aParams);
	static CSIPSubscriptionStateHeaderParams* 
		NewLC (const CSIPSubscriptionStateHeaderParams& aParams);

	CSIPSubscriptionStateHeaderParams ();
	~CSIPSubscriptionStateHeaderParams ();

	static CSIPSubscriptionStateHeaderParams* 
		InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;
	
private: // For testing purposes

	UNIT_TEST(CSIPSubscriptionStateHeaderTest)
    };

#endif // CSIPSUBSCRIPTIONSTATEHEADERPARAMS_H

// End of File
