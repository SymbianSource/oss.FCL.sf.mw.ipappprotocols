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
* Name        : sipresponse.h
* Part of     : SIP Codec
* Interface   : SIP Codec API
* Version     : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPRESPONSE_H
#define CSIPRESPONSE_H

//  INCLUDES
#include "sipmessage.h"
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class provides functions for manipulating a SIP response
* and the reason phrase.
*
* @lib sipcodec.lib
*/
class CSIPResponse : public CSIPMessage
    {
    public: // Constructors and destructor

        /** Response classes */
        enum TType
            {
            E1XX = 1,
            E2XX,
            E3XX,
            E4XX,
            E5XX,
            E6XX
            };

        /**
        * Creates a new instance of CSIPResponse
        * @pre (aResponseCode >= 100 && aResponseCode < 700)
        * @param aResponseCode a response code
        * @param aReasonPhrase a textual description of the response reason
        * @return new instance of CSIPResponse
        */
        IMPORT_C static CSIPResponse* NewL(TUint aResponseCode,
                                           RStringF aReasonPhrase);

        /**
        * Creates a new instance of CSIPResponse and pushes it to CleanupStack
        * @pre (aResponseCode >= 100 && aResponseCode < 700)
        * @param aResponseCode a response code
        * @param aReasonPhrase a textual description of the response reason
        * @return new instance of CSIPResponse
        */
        IMPORT_C static CSIPResponse* NewLC(TUint aResponseCode,
                                            RStringF aReasonPhrase);

        /**
        * Destructor
        */
        IMPORT_C ~CSIPResponse();

    public: // From CSIPMessage

		/**
		* From CSIPMessage IsRequest
		*/
        IMPORT_C TBool IsRequest() const;

    public: // New functions

        /**
        * Sets the SIP response code
        * @pre (aResponseCode >= 100 && aResponseCode < 700)
        * @param aResponseCode a response code
        */
        IMPORT_C void SetResponseCodeL(TUint aResponseCode);
        
        /**
        * Gets the SIP response code 
        * @return the SIP response code
        */
        IMPORT_C TUint ResponseCode() const;

        /**
        * Gets the SIP response class of the response 
        * @return the SIP response class
        */
        IMPORT_C TType Type() const;

        /**
        * Checks if the response is an error response (>=300)
        * @return ETrue if response code >= 300, otherwise EFalse  
        */
        IMPORT_C TBool IsErrorResponse() const;

        /**
        * Sets the SIP reason phrase
        * @param aReasonPhrase a reason phrase
        */
        IMPORT_C void SetReasonPhraseL(RStringF aReasonPhrase);
        
        /**
        * Gets the SIP reason phrase
        * @return the SIP reason phrase
        */
        IMPORT_C RStringF ReasonPhrase() const;

    public: // New functions, for internal use

        static CSIPResponse* NewL();
        
        static CSIPResponse* NewLC();
        
        void SetReasonPhraseL(const TDesC8& aReasonPhrase);    

    private: // Constructors

        CSIPResponse();
        
        void ConstructL();
        
        void ConstructL(
            TUint aResponseCode,
            RStringF aReasonPhrase);

    private: // From CSIPMessage

        HBufC8* ToTextFirstLineLC () const;

    private: // Data

        TUint iResponseCode;
        RStringF iReasonPhrase;
    };

#endif // CSIPRESPONSE_H

// End of File
