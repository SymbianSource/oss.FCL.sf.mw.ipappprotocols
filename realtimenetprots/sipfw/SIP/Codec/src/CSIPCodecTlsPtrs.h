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
* Name          : CSIPCodecTlsPtrs.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPCODECTLSPTRS_H
#define CSIPCODECTLSPTRS_H

#include <e32base.h>

class CSIPStrings;
class CSIPHeaderLookupTable;


class CSIPCodecTlsPtrs : public CBase
    {
public:
    
    CSIPCodecTlsPtrs(CSIPStrings* aStrings);
    CSIPCodecTlsPtrs(CSIPHeaderLookupTable* aHeaderLookup);
	~CSIPCodecTlsPtrs();

	void DestroyStrings();
	void DestroyHeaderLookup();

    CSIPStrings* iStrings;
    CSIPHeaderLookupTable* iHeaderLookup;
    
private:

	CSIPCodecTlsPtrs();    
    };

#endif // CSIPCODECTLSPTRS_H

// End of File
