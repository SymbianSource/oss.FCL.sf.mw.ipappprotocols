/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPAcceptContactHeaderParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPACCEPTCONTACTHEADERPARAMS_H
#define CSIPACCEPTCONTACTHEADERPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPAcceptContactHeaderParams : public CSIPParamContainerBase
	{
public:

	static CSIPAcceptContactHeaderParams* 
		NewL (const CSIPAcceptContactHeaderParams& aParams);

	static CSIPAcceptContactHeaderParams*
		NewLC (const CSIPAcceptContactHeaderParams& aParams);

	CSIPAcceptContactHeaderParams ();
	~CSIPAcceptContactHeaderParams ();

    static CSIPAcceptContactHeaderParams* 
        InternalizeL (RReadStream& aReadStream);
    
    TBool operator==(const CSIPAcceptContactHeaderParams& aParams) const;

private: // From CSIPParamContainerBase

	void CheckParamL (CSIPParam& aParam) const;
    };

#endif // CSIPACCEPTCONTACTHEADERPARAMS_H

// End of File
