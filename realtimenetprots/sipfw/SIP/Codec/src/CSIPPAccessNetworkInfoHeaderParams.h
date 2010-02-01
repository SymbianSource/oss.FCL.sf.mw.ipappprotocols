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
* Name          : CSIPPAccessNetworkInfoHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPPACCESSNETWORKINFOHEADERPARAMS_H
#define CSIPPACCESSNETWORKINFOHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPPAccessNetworkInfoHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPPAccessNetworkInfoHeaderParams* 
		NewL (const CSIPPAccessNetworkInfoHeaderParams& aParams);

	static CSIPPAccessNetworkInfoHeaderParams*
		NewLC (const CSIPPAccessNetworkInfoHeaderParams& aParams);

	CSIPPAccessNetworkInfoHeaderParams ();
	~CSIPPAccessNetworkInfoHeaderParams ();

    static CSIPPAccessNetworkInfoHeaderParams* 
        InternalizeL (RReadStream& aReadStream);
        
private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;
    };

#endif // CSIPPACCESSNETWORKINFOHEADERPARAMS_H

// End of File
