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
* Name          : CSIPParam.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPPARAM_H
#define CSIPPARAM_H

#include <e32base.h>
#include <s32mem.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

/**
* General class for SIP-URI- and header-parameter manipulation.
* Class provides functions for setting and getting SIP parameter
* as name/value pairs.
*/
class CSIPParam : public CBase
	{
public:

	static CSIPParam* DecodeL (const TDesC8& aValue);
	static TBool ParseL(const TDesC8& aInput, TPtrC8& aName, TPtrC8& aValue);

	static CSIPParam* NewL (RStringF aName);
	static CSIPParam* NewLC (RStringF aName);
	
	static CSIPParam* NewL (const TDesC8& aName);
	static CSIPParam* NewLC (const TDesC8& aName);	

	static CSIPParam* NewL (RStringF aName, RStringF aValue);
	static CSIPParam* NewLC (RStringF aName, RStringF aValue);

	static CSIPParam* NewL (RStringF aName, const TDesC8& aValue);
	static CSIPParam* NewLC (RStringF aName, const TDesC8& aValue);
	
	static CSIPParam* NewL (const TDesC8& aName, const TDesC8& aValue);
	static CSIPParam* NewLC (const TDesC8& aName, const TDesC8& aValue);	

	static CSIPParam* NewL (const CSIPParam& aSIPParam);
	static CSIPParam* NewLC (const CSIPParam& aSIPParam);

	~CSIPParam();

	RStringF Name () const;
	TBool HasValue () const;
	RStringF Value() const;
	const TDesC8& DesValue() const;
	void SetValueL(const TDesC8& aValue, TBool aAddQuotesWhenEncoding);

	TBool operator==(const CSIPParam& aSIPParam) const;

	HBufC8* ToTextLC () const;

	static CSIPParam* InternalizeL (RReadStream& aReadStream);
	void ExternalizeL (RWriteStream& aWriteStream) const;
	
private:

	CSIPParam ();
	CSIPParam (const CSIPParam& aSIPParam);
	void ConstructL (RStringF aName);
	void ConstructL (const TDesC8& aName);		
	void ConstructL (RStringF aName, RStringF aValue);
	void ConstructL (RStringF aName, const TDesC8& aValue);
	void ConstructL (const TDesC8& aName, const TDesC8& aValue);
	void ConstructL (const CSIPParam& aSIPParam);
	void DoInternalizeL (RReadStream& aReadStream);
	void SetL(RStringF& aString,
	          const TDesC8& aDes,
	          TBool aCheckLength=ETrue);
	
private: // Data	

	RStringF iName;
	RStringF iValue;
	TBool iHasValue;
    HBufC8* iDesValue;
    TBool iAddQuotesWhenEncoding;

	UNIT_TEST(CSIPParamTest)
	};

#endif // CSIPPARAM_H

// End of File
