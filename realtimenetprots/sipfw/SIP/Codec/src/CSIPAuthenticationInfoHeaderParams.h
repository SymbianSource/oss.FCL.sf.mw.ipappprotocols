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
* Name          : CSIPAuthenticationInfoHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPAUTHENTICATIONINFOHEADERPARAMS_H
#define CSIPAUTHENTICATIONINFOHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPAuthenticationInfoHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPAuthenticationInfoHeaderParams* 
		NewL (const CSIPAuthenticationInfoHeaderParams& aParams);

	static CSIPAuthenticationInfoHeaderParams*
		NewLC (const CSIPAuthenticationInfoHeaderParams& aParams);

	CSIPAuthenticationInfoHeaderParams ();
	~CSIPAuthenticationInfoHeaderParams ();

private: // From CSIPParamContainerBase

    TBool AddQuotesWhenEncoding(RStringF aParamName) const;

	void CheckAndUpdateParamL(RStringF aName,
	                          TBool aHasValue,
	                          TPtrC8& aValue) const;	
    };

#endif // CSIPAUTHENTICATIONINFOHEADERPARAMS_H

// End of File
