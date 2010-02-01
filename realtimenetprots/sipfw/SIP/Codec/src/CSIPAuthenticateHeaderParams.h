/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPAuthenticateHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPAUTHENTICATEHEADERPARAMS_H
#define CSIPAUTHENTICATEHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPAuthenticateHeaderParams : public CSIPParamContainerBase
	{
	public: // Constructors and destructor 

		static CSIPAuthenticateHeaderParams* 
			NewL(const CSIPAuthenticateHeaderParams& aParams);
	
		static CSIPAuthenticateHeaderParams*
			NewLC(const CSIPAuthenticateHeaderParams& aParams);

		CSIPAuthenticateHeaderParams();
		~CSIPAuthenticateHeaderParams();

		static CSIPAuthenticateHeaderParams* 
			InternalizeL(RReadStream& aReadStream);

	private: // From CSIPParamContainerBase

        TBool AddQuotesWhenEncoding(RStringF aParamName) const;

		void CheckAndUpdateParamL(RStringF aName,
		                          TBool aHasValue,
		                          TPtrC8& aValue) const;
    };

#endif // CSIPAUTHENTICATEHEADERPARAMS_H

// End of File
