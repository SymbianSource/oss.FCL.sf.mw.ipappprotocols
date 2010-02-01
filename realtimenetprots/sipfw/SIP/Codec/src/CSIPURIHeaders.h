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
* Name          : CSIPURIHeaders.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPURIHEADERS_H
#define CSIPURIHEADERS_H

#include <e32base.h>
#include <badesca.h>
#include "_sipcodecdefs.h"

class CSIPParam;


class CSIPURIHeaders : public CBase
	{
public:

	static CSIPURIHeaders* DecodeL (const TDesC8& aValue);
	static CSIPURIHeaders* NewL (const CSIPURIHeaders& aHeaders);
	static CSIPURIHeaders* NewLC (const CSIPURIHeaders& aHeaders);

	CSIPURIHeaders ();
	~CSIPURIHeaders ();

	TBool HasHeaders (const TDesC8& aName) const;

	CPtrC8Array* HeaderValuesL (const TDesC8& aName) const;

	void AddHeaderL (const TDesC8& aName, const TDesC8& aValue);

	void DeleteAll ();

	TBool operator==(const CSIPURIHeaders& aHeaders) const;

	HBufC8* ToTextLC () const;

private:

	void ConstructL (const CSIPURIHeaders& aHeaders);
	void AddHeaderL (const CSIPParam* aHeader);
	void CheckHeaderL (const CSIPParam* aHeader) const;
	static TBool CheckSIPURIHeaderToken (const TDesC8& aHeaderToken);
	TBool HasHeader (const CSIPParam& aHeader) const;

	// Needed for cleanup of a RPointerArray<HBufC8>:
	static void ResetAndDestroy (TAny* anArray);

private: // Data

	RPointerArray<CSIPParam> iHeaders;

private: // For testing purposes

    UNIT_TEST(CSIPURIHeadersTest)
	};

#endif // CSIPURIHEADERS_H

// End of File
