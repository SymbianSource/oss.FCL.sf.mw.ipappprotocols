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
* Name          : TSIPHeaderLookupEntry.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef TSIPHEADERLOOKUPENTRY_H
#define TSIPHEADERLOOKUPENTRY_H

#include <e32base.h>
#include <s32strm.h>
#include <stringpool.h>

class CSIPHeaderBase;


class TSIPHeaderLookupEntry
	{
public:
	
	TSIPHeaderLookupEntry(RStringF aName,
                          RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
                          CSIPHeaderBase*(*aInternalize)(RReadStream&) = 0);

	TSIPHeaderLookupEntry(RStringF aName,
                          RStringF aCompactName,
                          RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
                          CSIPHeaderBase*(*aInternalize)(RReadStream&) = 0);

	RStringF Name() const;
	RStringF CompactName() const;
	
	RPointerArray<CSIPHeaderBase> DecodeL(const TDesC8& aHeaderValue) const;

	CSIPHeaderBase* InternalizeL(RReadStream& aReadStream) const;

    TBool InternalizeSupported() const;

private: // Data

	RStringF iName;
	RStringF iCompactName;
	RPointerArray<CSIPHeaderBase> (*iDecodeFunction)(const TDesC8&);
	CSIPHeaderBase* (*iInternalizeFunction)(RReadStream&);
	};

#endif // TSIPHEADERLOOKUPENTRY_H

// End of File
