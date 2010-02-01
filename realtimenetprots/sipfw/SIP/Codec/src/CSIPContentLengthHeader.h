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
* Name          : sipcontentlengthheader.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPCONTENTLENGTHHEADER_H
#define CSIPCONTENTLENGTHHEADER_H

#include "sipunsignedintheaderbase.h"
#include "_sipcodecdefs.h"


class CSIPContentLengthHeader : public CSIPUnsignedIntHeaderBase
	{
public:

	static RPointerArray<CSIPHeaderBase> BaseDecodeL(const TDesC8& aValue);
	CSIPContentLengthHeader(TUint aValue=0);
	~CSIPContentLengthHeader();

public: // From CSIPHeaderBase

	CSIPHeaderBase* CloneL() const;
	RStringF Name() const;
	TBool HasCompactName() const;
	RStringF CompactName() const;
    TBool ExternalizeSupported() const;
	TPreferredPlace PreferredPlaceInMessage() const;

private: // For testing purposes

	UNIT_TEST(CSIPContentLengthHeaderTest)
	};

#endif // CSIPCONTENTLENGTHHEADER_H

// End of File
