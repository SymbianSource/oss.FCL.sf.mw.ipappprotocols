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
* Name          : TTagValueListParser.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TTAGVALUELISTPARSER_H
#define TTAGVALUELISTPARSER_H

//  INCLUDES
#include <delimitedparser8.h>

// CLASS DECLARATION
/**
* Class for parsing comma separated tag-value-list defined in RFC 3840.
* @lib sipresolver.lib
*/
class TTagValueListParser : public TDelimitedParserBase8
    {
public:

    inline TTagValueListParser();
    inline void Parse(const TDesC8& aValue);
    };

#include "TTagValueListParser.inl"

#endif // TTAGVALUELISTPARSER_H

// End of File
