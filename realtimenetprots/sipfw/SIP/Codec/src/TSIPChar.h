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
* Name          : TSIPChar.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef TSIPCHAR_H
#define TSIPCHAR_H

#include <e32std.h>


class TSIPChar : public TChar
	{
public:

	inline TSIPChar();
	inline TSIPChar(TUint aChar);
	inline TSIPChar(const TChar &aChar);

	inline TBool IsAlpha() const;
	inline TBool IsAlphaDigit() const;
	inline TBool IsReserved() const;
	inline TBool IsUnreserved() const;
	inline TBool IsMark() const;
	inline TBool IsTokenChar(RArray<TSIPChar>* aArray = NULL) const;
	inline TBool IsSeparator() const;
	inline TBool IsWordChar() const;
	inline TBool IsQuotedPairChar() const;
	inline TBool IsUTF8NonAsciiStartChar() const;
	inline TBool IsUTF8ContChar() const;
	inline TBool IsGdTextChar() const;
    inline TBool IsCText() const;
	};

#include "TSIPChar.inl"

#endif // TSIPCHAR_H

// End of File
