// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSIPRegistrationElement.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0
//



#include "sipregistrationelement.h"
#include "sipcontactelement.h"


// -----------------------------------------------------------------------------
// CSIPRegistrationElement::NewL
// -----------------------------------------------------------------------------
//
CSIPRegistrationElement* CSIPRegistrationElement::NewL()
	{
	CSIPRegistrationElement* self = new( ELeave ) CSIPRegistrationElement;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::~CSIPRegistrationElement
// -----------------------------------------------------------------------------
//
CSIPRegistrationElement::~CSIPRegistrationElement()
	{
	iContactElements.ResetAndDestroy();
	iState.Close();
	delete iAOR;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationElement::ConstructL()
	{
	iAOR = CUri8::NewL();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::CSIPRegistrationElement
// -----------------------------------------------------------------------------
//
CSIPRegistrationElement::CSIPRegistrationElement()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::ContactElements
// -----------------------------------------------------------------------------
//
RPointerArray< CSIPContactElement >& CSIPRegistrationElement::ContactElements()
	{
	return iContactElements;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::SetAORL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationElement::SetAORL( CUri8* aUri )
	{
	__ASSERT_ALWAYS( aUri, User::Leave( KErrCorrupt ) );
	delete iAOR;
	iAOR = aUri;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::AOR
// -----------------------------------------------------------------------------
//
const CUri8& CSIPRegistrationElement::AOR() const
	{
	return *iAOR;
	}
	
// -----------------------------------------------------------------------------
// CSIPRegistrationElement::SetState
// -----------------------------------------------------------------------------
//
void CSIPRegistrationElement::SetState( RString aState )
	{
	iState.Close();
	iState = aState.Copy();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationElement::State
// -----------------------------------------------------------------------------
//
RString CSIPRegistrationElement::State() const
	{
	return iState;
	}	
