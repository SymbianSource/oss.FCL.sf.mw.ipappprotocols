// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SIPDialog.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include <uri8.h>
#include "SipDialogImplementation.h"
#include "SipConnectionImplementation.h"
#include "sipfromheader.h"
#include "siptoheader.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPDialog::NewL
// -----------------------------------------------------------------------------
//
CSIPDialog* CSIPDialog::NewL(CSIPConnectionImplementation& aConnImplementation)
    {
    CSIPDialog* self = CSIPDialog::NewLC(aConnImplementation);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPDialog::NewLC
// -----------------------------------------------------------------------------
//
CSIPDialog* CSIPDialog::NewLC(CSIPConnectionImplementation& aConnImplementation)
    {
    CSIPDialog* self = new (ELeave) CSIPDialog();
    CleanupStack::PushL(self);
    self->ConstructL(aConnImplementation);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPDialog::NewL
// -----------------------------------------------------------------------------
//
CSIPDialog* CSIPDialog::NewL(CSIPConnectionImplementation& aConnImplementation,
                             const MSIPRegistrationContext& aContext)
    {
    CSIPDialog* self = CSIPDialog::NewLC(aConnImplementation, aContext);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPDialog::NewLC
// -----------------------------------------------------------------------------
//	
CSIPDialog* CSIPDialog::NewLC(CSIPConnectionImplementation& aConnImplementation,
                              const MSIPRegistrationContext& aContext)
    {    
    CSIPDialog* self = new (ELeave) CSIPDialog();
    CleanupStack::PushL(self);
    self->ConstructL(aConnImplementation, aContext);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPDialog::CSIPDialog
// -----------------------------------------------------------------------------
//
CSIPDialog::CSIPDialog()
    {
    }

// -----------------------------------------------------------------------------
// CSIPDialog::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPDialog::ConstructL(CSIPConnectionImplementation& aConnImplementation)
    {    
    iImplementation = CSIPDialogImplementation::NewL(this, aConnImplementation);
    }

// -----------------------------------------------------------------------------
// CSIPDialog::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPDialog::ConstructL(CSIPConnectionImplementation& aConnImplementation,
                   			const MSIPRegistrationContext& aContext)
	{
	iImplementation = CSIPDialogImplementation::NewL(this,
													 aConnImplementation,
													 aContext);
	}
               		
// -----------------------------------------------------------------------------
// CSIPDialog::~CSIPDialog
// Doesn't own iImplementation
// -----------------------------------------------------------------------------
//
CSIPDialog::~CSIPDialog()
    {    
    }

// -----------------------------------------------------------------------------
// CSIPDialog::State
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPDialog::TState CSIPDialog::State() const
    {
    __TEST_INVARIANT;
    return iImplementation->State();   
    }

// -----------------------------------------------------------------------------
// CSIPDialog::SIPDialogAssociations
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<CSIPDialogAssocBase>&
CSIPDialog::SIPDialogAssociations() const
    {
    __TEST_INVARIANT;
    return iImplementation->SIPDialogAssociations();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::RegistrationContext
// -----------------------------------------------------------------------------
//
EXPORT_C const MSIPRegistrationContext* CSIPDialog::RegistrationContext() const
    {
    __TEST_INVARIANT;
    return iImplementation->RegistrationContext();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::IsAssociated
// -----------------------------------------------------------------------------
//
EXPORT_C TBool
CSIPDialog::IsAssociated(const CSIPDialogAssocBase& aDialogAssoc) const
    {
    __TEST_INVARIANT;
    return iImplementation->IsAssociated(aDialogAssoc);
    }

// -----------------------------------------------------------------------------
// CSIPDialog::Connection
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection* CSIPDialog::Connection()
    {
    __TEST_INVARIANT;
   	return iImplementation->Connection();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::Connection
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPConnection* CSIPDialog::Connection() const
    {
    __TEST_INVARIANT;
	return iImplementation->Connection();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::FromHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPFromHeader& CSIPDialog::FromHeader() const
    {
    __TEST_INVARIANT;
    return iImplementation->FromHeader();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::ToHeader
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPToHeader& CSIPDialog::ToHeader() const
    {
    __TEST_INVARIANT;
    return iImplementation->ToHeader();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::RemoteURI
// -----------------------------------------------------------------------------
//
EXPORT_C const CUri8& CSIPDialog::RemoteURI() const
    {
    __TEST_INVARIANT;
    return iImplementation->RemoteURI();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::CallIdL
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPCallIDHeader& CSIPDialog::CallIdL() const
	{
	__TEST_INVARIANT;
	return iImplementation->CallIdL();	
	}

// -----------------------------------------------------------------------------
// CSIPDialog::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPDialog::operator==(const CSIPDialog& aDialog) const    
    {
    __TEST_INVARIANT;
    return *iImplementation == *aDialog.iImplementation;
    }

// -----------------------------------------------------------------------------
// CSIPDialog::ReuseInitialRequestData
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPDialog::ReuseInitialRequestData()
    {
    __TEST_INVARIANT;
    return iImplementation->ReuseInitialRequestData();
    }

// -----------------------------------------------------------------------------
// CSIPDialog::Implementation
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation& CSIPDialog::Implementation()
	{
	__TEST_INVARIANT;
	return *iImplementation;
	}
	
// -----------------------------------------------------------------------------
// CSIPDialog::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPDialog::__DbgTestInvariant() const
	{
	if (!iImplementation)
		{		
		User::Invariant();
		}
	}
