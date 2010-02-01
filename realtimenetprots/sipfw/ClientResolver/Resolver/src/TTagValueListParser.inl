/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TTagValueListParser.inl
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/



#ifndef TTAGVALUELIST_INL
#define TTAGVALUELIST_INL

// ---------------------------------------------------------------------------
// TTagValueListParser::TTagValueListParser 
// ---------------------------------------------------------------------------
//
inline TTagValueListParser::TTagValueListParser() 
    : TDelimitedParserBase8()
    {
    SetDelimiter(',');
    }

// ---------------------------------------------------------------------------
// TTagValueListParser::Parse
// ---------------------------------------------------------------------------
//
inline void TTagValueListParser::Parse(const TDesC8& aValue)
    {
    TDelimitedParserBase8::Parse(aValue);
    }

#endif // TTAGVALUELIST_INL

// End of File
