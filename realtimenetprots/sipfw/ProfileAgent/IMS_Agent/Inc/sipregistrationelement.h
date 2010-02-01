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
* Name          : CSIPRegistrationElement.h
* Part of       : ProfileAgent
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONELEMENT_H
#define CSIPREGISTRATIONELEMENT_H

// INCLUDES
#include <e32base.h>
#include <uri8.h>
#include <stringpool.h>

// FORWARD DECLARATIONS
class CSIPContactElement;

// CLASS DEFINITION
/**
 * CSIPRegistrationElement implements a data container for SIP application
 * launching information.
 */
class CSIPRegistrationElement
	: public CBase
	{
	public: // Constructors and destructor

		static CSIPRegistrationElement* NewL();
		~CSIPRegistrationElement();
		
  	public: // New methods

	    RPointerArray< CSIPContactElement >& ContactElements();
	    void SetAORL( CUri8* aUri );
	    const CUri8& AOR() const;
	    void SetState( RString aState );
	    RString State() const;

	private: // Constructors

		CSIPRegistrationElement();
		void ConstructL();

	private: // Data

		RPointerArray< CSIPContactElement > iContactElements;
		CUri8* iAOR;
		RString iState;
	};

#endif // CSIPREGISTRATIONELEMENT_H

