/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TSIPLogLineParser.h
* Part of       : Logging
* Version       : SIP/2.0 
*
*/




/**
 @internalComponent
*/


#ifndef __TSIP_LOG_LINE_PARSER__
#define __TSIP_LOG_LINE_PARSER__

#include <e32std.h>


class TSIPLogLineParser
	{
public:

	TSIPLogLineParser (const TDesC8& aMessage,
		               TUint aMaxLineLength);

	TBool End();
    TPtrC8 GetLine ();

private:

	TInt FindNextLineEndPos (const TDesC8& aDes, TUint& aLineEndOffset);

	TInt iMaxLineLength;
	TPtrC8 iMessage;
	TInt iCurrentPos;
#ifdef CPPUNIT_TEST
	friend class CSIPLogLineParserTest;
#endif
	};

#endif // end of __TSIP_LOG_LINE_PARSER__

// End of File
