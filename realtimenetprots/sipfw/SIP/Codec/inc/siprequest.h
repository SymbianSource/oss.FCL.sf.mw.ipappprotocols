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
* Name        : siprequest.h
* Part of     : SIP Codec
* Interface   : SIP Codec API
* Version     : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREQUEST_H
#define CSIPREQUEST_H

//  INCLUDES
#include "sipmessage.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CURIContainer;

// CLASS DECLARATION
/**
* Class provides functions for manipulating a SIP request.
*
* @lib sipcodec.lib
*/
class CSIPRequest : public CSIPMessage
    {
    public: // Constructors and destructor

        /**
        * Creates a new instance of CSIPRequest
        * @return new instance of CSIPRequest
        */
        IMPORT_C static CSIPRequest* NewL();

        /**
        * Creates a new instance of CSIPRequest and pushes it to CleanupStack
        * @return new instance of CSIPRequest
        */
        IMPORT_C static CSIPRequest* NewLC();

        /**
        * Creates a new instance of CSIPRequest
        * @param aMethod a SIP method (has to be a SIP token)
        * @return new instance of CSIPRequest
        */
        IMPORT_C static CSIPRequest* NewL(RStringF aMethod);

        /**
        * Creates a new instance of CSIPRequest and pushes it to CleanupStack
        * @param aMethod a SIP method (has to be a SIP token)
        * @return new instance of CSIPRequest
        */
        IMPORT_C static CSIPRequest* NewLC(RStringF aMethod);

        /**
        * Destructor
        */
        IMPORT_C ~CSIPRequest();

    public: // From CSIPMessage

		/**
		* From CSIPMessage IsRequest
		*/
        IMPORT_C TBool IsRequest() const;

    public: // New functions

        /**
        * Sets the SIP method
        * @param aMethod a SIP method (has to be a SIP token)
        */
        IMPORT_C void SetMethodL(const TDesC8& aMethod);
        
        /**
        * Sets the SIP method
        * @param aMethod a SIP method (has to be a SIP token)
        */
        IMPORT_C void SetMethodL(RStringF aMethod);    
        
        /**
        * Gets the SIP method 
        * @return the SIP method
        */
        IMPORT_C RStringF Method() const;

    public: // New function, for internal use

        /**
        * Sets the request URI
        * @pre aRequestURI != 0
        * @param aRequestURI the ownership is transferred
        */
        IMPORT_C void SetRequestURIL(CURIContainer* aRequestURI);
        
        /**
        * Gets the request URI
        * @return the request URI
        * The ownership is NOT transferred
        */    
        IMPORT_C CURIContainer* RequestURI();

    private: // Constructors

        CSIPRequest();
        void ConstructL();
        void ConstructL(RStringF aMethod);

    private: // From CSIPMessage

        HBufC8* ToTextFirstLineLC() const;

    private: // Data

        RStringF iMethod;
        CURIContainer* iRequestURI;
        };

#endif // CSIPREQUEST_H

// End of File
