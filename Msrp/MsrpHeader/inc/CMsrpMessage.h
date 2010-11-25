/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef CMSRPMESSAGE_H
#define CMSRPMESSAGE_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <f32file.h>

#include "CMSRPMessageBase.h"

/**
*  @publishedAll
*
*  Class provides functions for creation and manipulation of MSRP headers
*/
class CMSRPMessage : public CMSRPMessageBase
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
	    IMPORT_C CMSRPMessage();

        /**
        * Destructor.
        */
		IMPORT_C ~CMSRPMessage();

    public: // New functions

		/**
		* Sets the content of the message to be sent
		* Message maximum length is 2048 bytes, longer message will
		* have to be sent in chunks by the client
		* @param aContent message to be sent
		*/
		IMPORT_C void SetContent( HBufC8* aContent );
		
		/**
		* Gets the content buffer, if set
		* @return pointer to content buffer, ownership is not transferred
		*/
		IMPORT_C const TDesC8& Content( );
		
		/**
		* Check if the contents of the message have been set
		* @return true if set
		*/
		IMPORT_C TBool IsContent( ) const;

		/**
		* Check if the given externalised buffer contains MSRP Message
 		* @param aBuffer a Buffer containing the value of the
		*        externalized object
		* @return true if Buffer contains MSRP message
		*/
		IMPORT_C static TBool IsMessage( const TDesC8& aBuffer );
        
		  /**
		  * Checks if File
		  */
		        
		IMPORT_C TBool IsFile( );
		        
		 /**
		  * Sets the FileName for a FileSession
		  */
		IMPORT_C void SetFileName( const TFileName& aFileName );
		        
		 /**
		 * Gets the FileName for a FileSession
		 */
		IMPORT_C TFileName& GetFileName( );
		        
		/**
		* Internalizes the class from given stream
 		* @param aReadStream a stream containing the value of the
		*        externalized object
		* @return Internalized CMSRPMessage object, ownership is transferred
		*/
		IMPORT_C static CMSRPMessage* InternalizeL( RReadStream& aReadStream );

	public: // from base classes

		// from CMSRPMessageBase
		IMPORT_C void ExternalizeL( RWriteStream& aWriteStream );
		
    private:

        /**
        * Internalizes this message
 		* @param aReadStream a stream containing the value of the
		*        externalized object (header name not included).
        */
	    void DoInternalizeL( RReadStream& aReadStream  );
	    
        /**
        * Converts the buffer to file
        */
        void ConvertBufferToFileL( );
        
    private: // Data

		// content buffer
		HBufC8* iContentBuffer;

	   // filename
	   TFileName iFileName;
	};

#endif // CMSRPMESSAGE_H

// End of File
