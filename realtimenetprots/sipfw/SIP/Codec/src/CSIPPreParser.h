/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPPreParser.h
* Part of       : SIP Codec
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPREPARSER_H
#define CSIPPREPARSER_H

#include <e32base.h>
#include "_sipcodecdefs.h"

class CSIPHeaderNameValue;


class CSIPPreParser : public CBase
	{
public:
	static CSIPPreParser* NewL (HBufC8* aSipMessageBuf);
	static CSIPPreParser* NewLC (HBufC8* aSipMessageBuf);

	~CSIPPreParser ();

    TPtrC8 FirstLineL ();
	HBufC8* ContentL ();

private:

	CSIPPreParser (const TDesC8& aSipMessageDes);
	void ConstructL (HBufC8* aSipMessageBuf);

	TUint HeaderCount ();
	CSIPHeaderNameValue* ParseNameValueL (TInt aIndex);
	TPtrC8 GetLineL (TInt aLineStartPosition);

	void ParseL ();
	void CreateHeaderBufL (const TDesC8& aSipMessageDes);
    void FindHeaderPartEndPositionL (const TDesC8& aSipMessageDes,
                                     TInt& aPosition);

	TBool HandleCrL ();
	TBool HandleLfL ();
	void SkipSpacesAndTabsL ();	
	void MoveToNextLineL ();
	inline TChar GetCharL();
	inline TChar GetChar();
	inline void UnGetChar();

private: // Data

	HBufC8*       iSipMessageBuf;
	HBufC8*       iHeaderBuf;
	TInt          iOriginalHeaderPartLength;
    RArray<TUint> iHeaderStartArray;
    TLex8         iLexBuf;

	friend class TSIPHeaderNameValueIter;
	
private: // For testing purposes

	UNIT_TEST(CSIPPreParserTest)
	};

#endif // CSIPPREPARSER_H

// End of File
