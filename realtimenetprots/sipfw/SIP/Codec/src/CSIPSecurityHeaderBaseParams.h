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
* Name          : CSIPSecurityHeaderBaseParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECURITYHEADERBASEPARAMS_H
#define CSIPSECURITYHEADERBASEPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPSecurityHeaderBaseParams : public CSIPParamContainerBase
	{
public:

	static CSIPSecurityHeaderBaseParams* 
		NewL (const CSIPSecurityHeaderBaseParams& aParams);
	static CSIPSecurityHeaderBaseParams* 
		NewLC (const CSIPSecurityHeaderBaseParams& aParams);

	CSIPSecurityHeaderBaseParams ();
	~CSIPSecurityHeaderBaseParams ();

	static CSIPSecurityHeaderBaseParams* 
		InternalizeL (RReadStream& aReadStream);

private: // From CSIPParamContainerBase

    TBool AddQuotesWhenEncoding(RStringF aParamName) const;

	void CheckAndUpdateParamL(RStringF aName,
	                          TBool aHasValue,
	                          TPtrC8& aValue) const;
    };

#endif // CSIPSECURITYHEADERBASEPARAMS_H

// End of File
