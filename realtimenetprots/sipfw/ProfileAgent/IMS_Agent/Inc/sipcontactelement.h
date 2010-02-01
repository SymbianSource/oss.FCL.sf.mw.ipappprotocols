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
* Name          : CSIPContactElement.h
* Part of       : ProfileAgent
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCONTACTELEMENT_H
#define CSIPCONTACTELEMENT_H

// INCLUDES
#include <e32base.h>
#include <uri8.h>
#include <stringpool.h>

// FORWARD DECLARATIONS


// CLASS DEFINITION
/**
 * CSIPContactElement implements a data container for a contact
 * in registration state event information. 
 */
class CSIPContactElement
	: public CBase
	{
	public: // Constructors and destructor

		static CSIPContactElement* NewL();
		~CSIPContactElement();

  	public: // New methods
  	
		void SetUri8L( CUri8* aUri );
		const CUri8& Uri8() const;
		void SetState( RString aState );
		RString State() const;
		void SetEvent( RString aEvent );
		RString Event() const;
		void SetExpires( TUint aValue );
		TUint Expires() const;
		TBool HasExpires() const;

	private: // Constructors

		CSIPContactElement();
		void ConstructL();

	private: // Data

		CUri8* iUri8;
		RString iState;
		RString iEvent;
		HBufC8* iId;
		TUint iExpires;
		TBool iHasExpires;		
	};

#endif // CSIPCONTACTELEMENT_H

