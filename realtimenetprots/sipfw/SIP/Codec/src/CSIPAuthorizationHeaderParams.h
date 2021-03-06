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
* Name          : CSIPAuthorizationHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPAUTHORIZATIONHEADERPARAMS_H
#define CSIPAUTHORIZATIONHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPAuthorizationHeaderParams : public CSIPParamContainerBase
	{
	public:

		static CSIPAuthorizationHeaderParams* 
			NewL(const CSIPAuthorizationHeaderParams& aParams);
	
		static CSIPAuthorizationHeaderParams*
			NewLC(const CSIPAuthorizationHeaderParams& aParams);

		CSIPAuthorizationHeaderParams ();
		~CSIPAuthorizationHeaderParams ();

		static CSIPAuthorizationHeaderParams* 
			InternalizeL(RReadStream& aReadStream);

	private: // From CSIPParamContainerBase

        TBool AddQuotesWhenEncoding(RStringF aParamName) const;

	    void CheckAndUpdateParamL(RStringF aName,
		                          TBool aHasValue,
		                          TPtrC8& aValue) const;
    };

#endif // CSIPAUTHORIZATIONHEADERPARAMS_H

// End of File
