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
* Name        : sipmessageparser.h
* Part of     : SIP Codec
* Interface   : SIP Codec API
* Version     : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPMESSAGEPARSER_H
#define CSIPMESSAGEPARSER_H

//  INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPMessage;
class CSIPRequest;
class CSIPResponse;

// CLASS DECLARATION
/**
* Class provides a function to create a CSIPMessages
* from textual representation of a SIP message.
*
* @lib sipcodec.lib
*/
class CSIPMessageParser : public CBase
    {
    public: // Constructors and destructor

        /**
        * Creates a CSIPMessageParser instance
        * @returns a new instance of CSIPMessageParser
        */
        IMPORT_C static CSIPMessageParser* NewL();
    
        /**
        * Creates a CSIPMessageParser instance and pushes it to CleanupStack
        * @returns a new instance of CSIPMessageParser
        */
        IMPORT_C static CSIPMessageParser* NewLC();
    
        /**
        * Destructor
        */
        IMPORT_C ~CSIPMessageParser();

        /**
        * Constructs a CSIPMessage from its textual representation.
        * @pre aInputMessage != 0
        * @pre *aSIPMessage == 0
        * @param aInputMessage the textual representation of a SIP message
        * @param aSIPMessage, the pointer that points to 
        *        the created CSIPMessage, when the function returns. 
        * @return KErrNone if the message was successfully decoded.
        *         In this case aSIPMessage points to a syntactically 
        *         correct SIP message.
        *         Returns SIP parser error (@see sipcodecerr.h) or 
        *         a system wide error. In this case aSIPMessage may or 
        *         may not point to a partially parsed SIP message depending on 
        *         which phase of the parsing the error occured.
        */
        IMPORT_C TInt FromText(
            HBufC8* aInputMessage,
            CSIPMessage** aSIPMessage);

    private: // Constructors

        CSIPMessageParser();
        void ConstructL();

    private: // New functions

        CSIPMessage* FromTextL(
            HBufC8* aInputSIPMessage,
            TInt& aParseErrorInHeaders);

        CSIPMessage* CreateSIPMessageLC(const TDesC8& aFirstLine);
        
        CSIPRequest* CreateSIPRequestLC(const TDesC8& aRequestLine);
        
        CSIPResponse* CreateSIPResponseLC(const TDesC8& aStatusLine);
        
        TUint ParseResponseCodeL(const TDesC8& aResponseCode);

    private: // For testing purposes
    
        UNIT_TEST(CSIPMessageParserTest)
    };

#endif // CSIPMESSAGEPARSER_H

// End of File
