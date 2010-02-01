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
* Name          : CSIPRegInfoElement.h
* Part of       : ProfileAgent
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGINFOELEMENT_H
#define CSIPREGINFOELEMENT_H

// INCLUDES
#include <e32base.h>
#include <stringpool.h>

// FORWARD DECLARATIONS
class CSIPRegistrationElement;

// CLASS DEFINITION
/**
 * CSIPRegInfoElement implements a data container for SIP application
 * launching information.
 */
class CSIPRegInfoElement
	: public CBase
	{
	public: // Constructors and destructor

	   /**
		* Static constructor. Leaves pointer to cleanup stack.
		*
		* @return An initialized instance of this class.
		*/
		static CSIPRegInfoElement* NewLC();

		/// Destructor
		~CSIPRegInfoElement();

  	public: // New methods

		RPointerArray< CSIPRegistrationElement >& RegistrationElements();
		void SetVersion( TUint aVersion );
		TUint Version() const;
		TBool HasVersion() const;
		void SetState( RString aState );
		RString State() const;

	private: // Constructors

		/// Default constructor.
		CSIPRegInfoElement();

		/// 2nd phase constructor.
		void ConstructL();

	private: // Data

		// SIP header collection. Owned.
		RPointerArray< CSIPRegistrationElement > iRegistrationElements;
		TUint iVersion;
		TBool iHasVersion;
		//full  || partial
		RString iState;
	};

#endif // __CSIPREGINFOELEMENT_H__

