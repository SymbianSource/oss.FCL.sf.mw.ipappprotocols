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
* Name          : CSIPURIParams.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPURIPARAMS_H
#define CSIPURIPARAMS_H

#include "CSIPParamContainerBase.h"


class CSIPURIParams : public CSIPParamContainerBase
    {
public:

	static CSIPURIParams* DecodeL (const TDesC8& aValue,
	                               TBool aSIPSURI);
	                               
	static CSIPURIParams* NewL (const CSIPURIParams& aParams,
	                            TBool aSIPSURI);
	
	static CSIPURIParams* NewLC (const CSIPURIParams& aParams,
	                             TBool aSIPSURI);

	CSIPURIParams (TBool aSIPSURI);
	~CSIPURIParams ();

	TBool operator==(const CSIPURIParams& aParams) const;

private: // From CSIPParamContainerBase

    void CheckAndUpdateParamL (RStringF aName,
	                           TBool aHasValue,
	                           TPtrC8& aValue) const;

private: // Checking functions

    TBool CheckOtherParam (RStringF aName,
	                       TBool aHasValue,
	                       const TDesC8& aValue) const;
	                       
	TBool CheckSIPURIParamToken (const TDesC8& aParamToken) const;

private: // Data

    TBool iSIPSURI;

private: // For testing purposes

	UNIT_TEST(CSIPURIParamsTest)
    };

#endif // CSIPURIPARAMS_H

// End of File
