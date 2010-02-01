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
* Name          : CSIPHeaderLookupTable.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPHEADERLOOKUPTABLE_H
#define CSIPHEADERLOOKUPTABLE_H

#include <e32base.h>
#include <s32mem.h>
#include <stringpool.h>
#include "TSIPHeaderLookupEntry.h"


class CSIPHeaderLookupTable : public CBase
	{
public:

	static CSIPHeaderLookupTable* NewL ();
	static CSIPHeaderLookupTable* NewLC ();

	~CSIPHeaderLookupTable ();
	
	TBool IsSupported (RStringF aHeaderName) const;	

	CSIPHeaderBase* InternalizeL (RStringF aHeaderName,
	                              RReadStream& aReadStream);

    TBool InternalizeSupported (RStringF aName) const;

	RPointerArray<CSIPHeaderBase> 
		CreateHeaderL (RStringF aName, const TDesC8& aValue);

	void IncrementUsageCount ();
	void DecrementUsageCount ();
	TUint UsageCount ();
    TBool ConvertToSIPURI();
    void SetConvertToSIPURI(TBool aNewValue);

private:

	CSIPHeaderLookupTable ();
	void ConstructL ();

    void AddL (RStringF aFullName,
               RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
               CSIPHeaderBase*(*aInternalize)(RReadStream&) = 0);

    void AddL (RStringF aFullName,
               RStringF aCompactName,
               RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
               CSIPHeaderBase*(*aInternalize)(RReadStream&) = 0);

	TInt FindIndex (RStringF aName) const;


private: // Data

	RArray<TSIPHeaderLookupEntry> iLookupTable;
	TUint iUsageCount;
    TBool iConvertToSIPURI;
	};

#endif // CSIPHEADERLOOKUPTABLE_H

// End of File
