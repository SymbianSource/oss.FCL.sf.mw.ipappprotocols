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
// Name          : CSIPRegInfoElement.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0
//



#include "sipreginfoelement.h"
#include "sipregistrationelement.h"


// -----------------------------------------------------------------------------
// CSIPRegInfoElement::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegInfoElement* CSIPRegInfoElement::NewLC()
	{
	CSIPRegInfoElement* self = new( ELeave ) CSIPRegInfoElement;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::~CSIPRegInfoElement
// -----------------------------------------------------------------------------
//
CSIPRegInfoElement::~CSIPRegInfoElement()
	{
	iRegistrationElements.ResetAndDestroy();
	iState.Close();
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegInfoElement::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::CSIPRegInfoElement
// -----------------------------------------------------------------------------
//
CSIPRegInfoElement::CSIPRegInfoElement()
 : iHasVersion(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::RegistrationElement
// -----------------------------------------------------------------------------
//
RPointerArray< CSIPRegistrationElement >& 
CSIPRegInfoElement::RegistrationElements()
	{
	return iRegistrationElements;
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::SetVersion
// -----------------------------------------------------------------------------
//
void CSIPRegInfoElement::SetVersion( TUint aVersion )
	{
	iVersion = aVersion;
	iHasVersion = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::Version
// -----------------------------------------------------------------------------
//
TUint CSIPRegInfoElement::Version() const
	{
	return iVersion;
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::HasVersion
// -----------------------------------------------------------------------------
//	
TBool CSIPRegInfoElement::HasVersion() const
    {
    return iHasVersion;
    }

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::SetState
// -----------------------------------------------------------------------------
//
void CSIPRegInfoElement::SetState( RString aState )
	{
	iState.Close();
	iState = aState.Copy();
	}

// -----------------------------------------------------------------------------
// CSIPRegInfoElement::State
// -----------------------------------------------------------------------------
//
RString CSIPRegInfoElement::State() const
	{
	return iState;
	}

