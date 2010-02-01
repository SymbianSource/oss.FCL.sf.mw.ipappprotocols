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
// Name          : CSIPContactElement.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0
//



#include "sipcontactelement.h"

// -----------------------------------------------------------------------------
// CSIPContactElement::NewL
// -----------------------------------------------------------------------------
//
CSIPContactElement* CSIPContactElement::NewL()
	{
	CSIPContactElement* self = new( ELeave ) CSIPContactElement;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::CSIPContactElement
// -----------------------------------------------------------------------------
//
CSIPContactElement::CSIPContactElement() 
 : iHasExpires(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPContactElement::ConstructL()
	{
	iUri8 = CUri8::NewL();
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::~CSIPContactElement
// -----------------------------------------------------------------------------
//
CSIPContactElement::~CSIPContactElement()
	{
	delete iUri8;
	iState.Close();
	iEvent.Close();
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::SetUri8L
// -----------------------------------------------------------------------------
//
void CSIPContactElement::SetUri8L( CUri8* aUri )
	{
	__ASSERT_ALWAYS( aUri, User::Leave( KErrCorrupt ) );
	delete iUri8;
	iUri8 = aUri;
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::Uri8
// -----------------------------------------------------------------------------
//
const CUri8& CSIPContactElement::Uri8() const
	{
	return *iUri8;
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::SetState
// -----------------------------------------------------------------------------
//
void CSIPContactElement::SetState( RString aState )
	{
	iState.Close();
	iState = aState.Copy();
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::State
// -----------------------------------------------------------------------------
//
RString CSIPContactElement::State() const
	{
	return iState;
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::SetEvent
// -----------------------------------------------------------------------------
//
void CSIPContactElement::SetEvent( RString aEvent )
	{
	iEvent.Close();
	iEvent = aEvent.Copy();
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::Event
// -----------------------------------------------------------------------------
//
RString CSIPContactElement::Event() const
	{
	return iEvent;
	}	
	
// -----------------------------------------------------------------------------
// CSIPContactElement::SetExpires
// -----------------------------------------------------------------------------
//
void CSIPContactElement::SetExpires( TUint aValue )
	{
	iExpires = aValue;
	iHasExpires = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::Expires
// -----------------------------------------------------------------------------
//
TUint CSIPContactElement::Expires() const
	{
	return iExpires;
	}

// -----------------------------------------------------------------------------
// CSIPContactElement::HasExpires
// -----------------------------------------------------------------------------
//	
TBool CSIPContactElement::HasExpires() const
    {
    return iHasExpires;
    }
