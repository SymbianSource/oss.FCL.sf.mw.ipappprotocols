/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : siphostport.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef SIPHOSTPORT_H
#define SIPHOSTPORT_H

//  INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"

// CLASS DECLARATION
/**
* Class provides functions for setting and getting host and port
* in hostport structure (host[":"port]) with syntax checking
*
* @lib sipcodec.lib
*/
class CSIPHostPort : public CBase
	{
	public: // Enumerations

		/** SIP host types */
		enum TType
			{
			ESIPNoHost=0,
			ESIPIpv4=1,
			ESIPIpv6=2,
			ESIPHostName=3 // fully qualified domain name
			};


	public:	// Constructors and destructor

		/**
		* Constructs a CSIPHostPort from textual representation 
		* @param aValue BNF: host[":"port]
		* @return a new instance of CSIPHostPort
		*/
		IMPORT_C static CSIPHostPort* DecodeL(const TDesC8& aValue);

        /**
        * Creates a deep copy of the given CSIPHostPort instance
        * @param aHostPort CSIPHostPort to be copied
        * @return a new instance of CSIPHostPort
        */
        IMPORT_C static CSIPHostPort* NewL (const CSIPHostPort& aHostPort);

	    /**
	    * Creates a deep copy of the given CSIPHostPort instance and 
        * pushes it to CleanupStack.
        * @param aHostPort CSIPHostPort to be copied
        * @return a new instance of CSIPHostPort
        */
	    IMPORT_C static CSIPHostPort* NewLC (const CSIPHostPort& aHostPort);

		/**
		* Destructor, deletes the resources of CSIPHostPort.
		*/
		IMPORT_C ~CSIPHostPort();


	public: // New functions

		/**
		* Sets the host
		* @param aHost host (host name / IPv4 / IPv6)
		*/
		IMPORT_C void SetHostL(const TDesC8& aHost);
	
		/**
		* Gets the host
		* @return the host 
		*/
		IMPORT_C const TDesC8& Host() const;
	
		/**
		* Gets the type of the host 
		* @return the host type (host name / IPv4 / IPv6)
		*/	
		IMPORT_C TType HostType() const;

		/**
		* Checks if port is present
		* @pre HasHost() == ETrue
		* @return ETrue if present, otherwise EFalse 
		*/
		IMPORT_C TBool HasPort() const;

		/**
		* Gets the port
		* @return the port
		*/	
		IMPORT_C TUint Port() const;

		/**
		* Sets the port
		* @param aPort the port to set
		*/	
		IMPORT_C void SetPort(TUint aPort);

		/**
		* Deletes the port part
		* @pre HasPort() == ETrue
		* @return KErrNotFound, if port was not present, otherwise KErrNone, 
		*         if deletion was successful
		*/	
		IMPORT_C TInt DeletePort();

		/**
		* Compares this object to another instance of CSIPHostPort
		* @param aURI a CSIPHostPort instance to compare to
		* @return ETrue if the objects are equal otherwise EFalse
		*/
		IMPORT_C TBool operator==(const CSIPHostPort& aHostPort) const;


	public: // For internal use

		HBufC8* ToTextLC() const;

	private: // Constructors

		CSIPHostPort();
		void ConstructL();
		void ConstructL(const CSIPHostPort& aHostPort);

	private: // New functions

		static TUint ParsePortL(const TDesC8& aValue);
	
	private: // Data

		HBufC8* iHost;
		TType iHostType;
		TBool iHasPort;
		TUint iPort;

	private: // For testing purposes
	
		UNIT_TEST(CSIPHostPortTest)
	};

#endif // end of SIPHOSTPORT_H

// End of File
