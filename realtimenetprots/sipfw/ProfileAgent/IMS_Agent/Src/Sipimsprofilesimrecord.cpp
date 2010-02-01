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
// Name          : Sipimsprofilesimrecord.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0 
//



#include "Sipimsprofilesimrecord.h"
#include	"SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::NewL
// ----------------------------------------------------------------------------
//
CSIPProfileSIMRecord* CSIPProfileSIMRecord::NewL()
	{
	CSIPProfileSIMRecord* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::NewLC
// ----------------------------------------------------------------------------
//	
CSIPProfileSIMRecord* CSIPProfileSIMRecord::NewLC()
	{
	CSIPProfileSIMRecord* self = new (ELeave) CSIPProfileSIMRecord();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::CSIPProfileSIMRecord
// ----------------------------------------------------------------------------
//
CSIPProfileSIMRecord::CSIPProfileSIMRecord() : 
    iISIMPresent( EFalse )
	{
	}

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::ConstructL()
    {  
	iHNDN = KNullDesC8().AllocL();
	iIMPU = KNullDesC8().AllocL();
	iIMPI = KNullDesC8().AllocL();
	iHNDNsim = KNullDesC8().AllocL();
	iIMPUsim = KNullDesC8().AllocL();
	iIMPIsim = KNullDesC8().AllocL();
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::CSIPProfileSIMRecord
// ----------------------------------------------------------------------------
//
CSIPProfileSIMRecord::~CSIPProfileSIMRecord()
	{
	delete iHNDN;
	delete iIMPU;
	delete iIMPI;
	delete iHNDNsim;
	delete iIMPUsim;
	delete iIMPIsim;
	}

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetISIMPresent
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetISIMPresent( TBool aISIMPresent )
    {
    iISIMPresent = aISIMPresent;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetPrivateIdL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetPrivateIdL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS(aValue.Length(), User::Leave(KErrArgument));
    HBufC8* temp = aValue.AllocL();
	delete iIMPI;
	iIMPI = temp;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetFirstPublicUserNameL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetFirstPublicUserNameL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS(aValue.Length(), User::Leave(KErrArgument));
    HBufC8* temp = aValue.AllocL();
	delete iIMPU;
	iIMPU = temp;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetHomeNetworkDomainNameL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetHomeNetworkDomainNameL( const TDesC8& aValue )
    {
     __ASSERT_ALWAYS(aValue.Length(), User::Leave(KErrArgument));
     HBufC8* temp = aValue.AllocL();
    if ( FindSIPSchema( aValue ) ) 
    	{
    	delete temp;
    	temp = NULL;
    	temp = RemoveSIPSchemaL( aValue ).AllocL();
    	}
	delete iHNDN;
	iHNDN = temp;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::FindSIPSchema
// ----------------------------------------------------------------------------
//
TBool CSIPProfileSIMRecord::FindSIPSchema( const TDesC8& aValue )
    {
    _LIT8(KSIP, "sip:");
    _LIT8(KSIPS, "sips:");
    TPtrC8 value(aValue);
    if(value.FindF(KSIP) == 0 ||
       value.FindF(KSIPS) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::FindSIPSchema
// ----------------------------------------------------------------------------
//
TPtrC8 CSIPProfileSIMRecord::RemoveSIPSchemaL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrArgument));

	_LIT8(KSIPAndColon, "sip:");
	_LIT8(KSIPSAndColon, "sips:");

	TPtrC8 schemeAndColon;
	TInt sipSchemePos = aValue.FindF(KSIPAndColon);
	if (sipSchemePos < 0)
		{
		TInt sipsSchemePos = aValue.FindF(KSIPSAndColon);
		if (sipsSchemePos != 0) 
            {
            User::Leave (KErrArgument);
            }
		schemeAndColon.Set(KSIPSAndColon);
		}
	else
		{
		if (sipSchemePos != 0) 
            {
            User::Leave (KErrArgument);
            }
		schemeAndColon.Set(KSIPAndColon);
		}
	TPtrC8 remainder(aValue.Mid(schemeAndColon.Length()));
	return remainder;
    }    

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetSIMPrivateIdL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetSIMPrivateIdL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS(aValue.Length(), User::Leave(KErrArgument));
    HBufC8* temp = aValue.AllocL();
	delete iIMPIsim;
	iIMPIsim = temp;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetSIMFirstPublicUserNameL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetSIMFirstPublicUserNameL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS(aValue.Length(), User::Leave(KErrArgument));
    HBufC8* temp = aValue.AllocL();
	delete iIMPUsim;
	iIMPUsim = temp;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SetSIMHomeNetworkDomainNameL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::SetSIMHomeNetworkDomainNameL( const TDesC8& aValue )
    {
    __ASSERT_ALWAYS(aValue.Length(), User::Leave(KErrArgument));
    HBufC8* temp = aValue.AllocL();
	delete iHNDNsim;	
	iHNDNsim = temp;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::UserRecordFailed
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::UserRecordFailed( TInt aError )
    {
    iFailure = aError;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::IsISIMPresent
// ----------------------------------------------------------------------------
//
TBool CSIPProfileSIMRecord::IsISIMPresent()
    {
    return iISIMPresent;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::Failure
// ----------------------------------------------------------------------------
//
TInt CSIPProfileSIMRecord::Failure() const
    {    
    return iFailure;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::PrivateIdentity
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileSIMRecord::PrivateIdentity() const
    {
    return *iIMPI;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::PublicIdentity
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileSIMRecord::PublicIdentity() const
    {
    return *iIMPU;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::HomeNetworkDomainName
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileSIMRecord::HomeNetworkDomainName() const
    {
    return *iHNDN;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SIMPrivateIdentity
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileSIMRecord::SIMPrivateIdentity() const
    {
    return *iIMPIsim;
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SIMPublicIdentity
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileSIMRecord::SIMPublicIdentity() const
    {
    return *iIMPUsim;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::SIMHomeNetworkDomainName
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileSIMRecord::SIMHomeNetworkDomainName() const
    {
    return *iHNDNsim;
    }
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::AddSIPPrefixLC
// ----------------------------------------------------------------------------
//
HBufC8* CSIPProfileSIMRecord::AddSIPPrefixLC( const TDesC8& aValue )
    {
    _LIT8(KSIPprefix, "sip:");
    TUint length = KSIPprefix().Length() + aValue.Length();
	HBufC8* temp = HBufC8::NewLC(length);
	TPtr8 appendtemp = temp->Des();
	appendtemp.Append(KSIPprefix);
	appendtemp.Append(aValue);
    return temp;
    }    
    
// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::ResetValuesL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::ResetValuesL()
    {
    HBufC8* temp = NULL;
    temp = KNullDesC8().AllocL();
    delete iHNDN;
    iHNDN = temp;
    temp = KNullDesC8().AllocL();
    delete iIMPU;
	iIMPU = temp;
	temp = KNullDesC8().AllocL();
	delete iIMPI;
	iIMPI = temp;
	temp = KNullDesC8().AllocL();
	delete iHNDNsim;
	iHNDNsim = temp;
	temp = KNullDesC8().AllocL();
	delete iIMPUsim;
	iIMPUsim = temp;
	temp = KNullDesC8().AllocL();
	delete iIMPIsim;
	iIMPIsim = temp;
    }    

// ----------------------------------------------------------------------------
// CSIPProfileSIMRecord::ResetISIMValuesL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMRecord::ResetISIMValuesL()
    {
    HBufC8* temp = NULL;
    temp = KNullDesC8().AllocL();
    delete iHNDN;
    iHNDN = temp;
    temp = KNullDesC8().AllocL();
    delete iIMPU;
	iIMPU = temp;
	temp = KNullDesC8().AllocL();
	delete iIMPI;
	iIMPI = temp;
    }    
