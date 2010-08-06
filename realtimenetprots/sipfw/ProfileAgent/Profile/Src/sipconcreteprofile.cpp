// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipconcreteprofile.cpp
// Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
// Implementation
// Version     : %version: 3.1.2.1.2 %
//



//  INCLUDE FILES
#include "sipconcreteprofile.h"
#include "sipprofileparameter.h"
#include "sipprofile.h"
#include "sipmanagedprofile.h"
#include "extensionintparam.h"
#include "extensionboolparam.h"
#include "extensiondescrparam.h"
#include "extensiondescrarrayparam.h"
#include <random.h>
#include <utf.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>
#include <e32const.h>
#include "SipProfileLog.h"

const TInt KBitsInByte = 8;
const TInt KContactUserNameLength = 20;
const TUint KParameterLength = 25;
const TUint32 KDefaultSNAPIdentifier = KMaxTUint32;
_LIT8(KSIPProfileParameter, "%u");

using namespace CommsDat;
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfile* CSIPConcreteProfile::NewL()
	{
	CSIPConcreteProfile* self = CSIPConcreteProfile::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfile* CSIPConcreteProfile::NewLC()
	{
	CSIPConcreteProfile* self = new (ELeave) CSIPConcreteProfile();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CSIPConcreteProfile
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::CSIPConcreteProfile()
	{
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::ConstructL()
	{
	iSIPAORUri8 = CUri8::NewL();
	iSIPRegistrar = HBufC8::NewL(0);
	iSIPProviderName = HBufC8::NewL(0);
	iSIPPrivateIdentity = HBufC8::NewL(0);
	iSIPOutboundProxy = HBufC8::NewL(0);
		
	iSIPSigComp = EFalse;
	iSecurityNegotiation = EFalse;
	iSIPAutoRegistered = EFalse;

	iStatus = EUnregistered;
	iDefaultProfile = EFalse;

	iDynamicProxyResolving = EFalse;
	iDynamicProxy = HBufC8::NewL(0);

	iSIPContactHeaderParams = new (ELeave) CDesC8ArrayFlat(1);

	iSIPRegisteredAORs = new (ELeave) CDesC8ArrayFlat(1);
	iSIPNegotiatedSecurityMechanism = HBufC8::NewL(0);
	
	iSIPHeaders = new (ELeave) CDesC8ArrayFlat(1);
	
	HBufC8* username = CreateContactUserNameLC();
	SetExtensionParameterL(KSIPContactHeaderUser,*username);
	CleanupStack::PopAndDestroy(username);
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CloneL
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPConcreteProfile* CSIPConcreteProfile::CloneL() const
	{
	return CloneL(EFalse);
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CloneWithDynamicValuesL
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPConcreteProfile* CSIPConcreteProfile::CloneWithDynamicValuesL() const
	{
	return CloneL(ETrue);
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CloneL(TBool)
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPConcreteProfile::CloneL(TBool aWithDynamicValues) const
	{
	CSIPConcreteProfile* newCopy = CSIPConcreteProfile::NewLC();

	newCopy->SetIapId(IapId());
	newCopy->SetProfileType(ProfileType());
	newCopy->SetRegistrarL(*iSIPRegistrar);
	newCopy->SetOutboundProxyL(*iSIPOutboundProxy);
	newCopy->SetPrivateIdentityL(PrivateIdentity());
	newCopy->SetProviderNameL(ProviderName());


	newCopy->SetServerParameterL(KSIPRegistrar, KSIPDigestRealm,
		ServerParameter(KSIPRegistrar, KSIPDigestRealm));

	newCopy->SetServerParameterL(KSIPRegistrar, KSIPDigestUserName,
		ServerParameter(KSIPRegistrar, KSIPDigestUserName));

	newCopy->SetServerParameterL(KSIPRegistrar, KSIPDigestPassword,
		ServerParameter(KSIPRegistrar, KSIPDigestPassword));

	TInt i = 0;
	for (i = 0; i < iSIPRegistrarExtParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPRegistrarExtParameter[i];
		newCopy->SetServerExtensionParameterL(KSIPRegistrar, param->Key(), param->ValueL());
		}

	newCopy->SetServerParameterL(KSIPOutboundProxy, KSIPDigestRealm,
		ServerParameter(KSIPOutboundProxy, KSIPDigestRealm));

	newCopy->SetServerParameterL(KSIPOutboundProxy, KSIPDigestUserName,
		ServerParameter(KSIPOutboundProxy, KSIPDigestUserName));

	newCopy->SetServerParameterL(KSIPOutboundProxy, KSIPDigestPassword,
		ServerParameter(KSIPOutboundProxy, KSIPDigestPassword));

	for (i = 0; i < iSIPOutboundProxyExtParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPOutboundProxyExtParameter[i];
		newCopy->SetServerExtensionParameterL(KSIPOutboundProxy, param->Key(), param->ValueL());
		}

	newCopy->SetAORL(AOR());

	newCopy->EnableSigComp(IsSigCompEnabled());
	newCopy->EnableSecurityNegotiation(IsSecurityNegotiationEnabled());
	newCopy->SetAutoRegistrationEnabled(IsAutoRegistrationEnabled());
	newCopy->SetDynamicProxyL(DynamicProxy());

	newCopy->SetContactHeaderParamsL(ContactHeaderParams());

	newCopy->SetSIPHeadersL(SIPHeaders());

	newCopy->SetStatus(EUnregistered);
	newCopy->SetContextId(0);
	newCopy->SetDefault(EFalse);
	newCopy->SetLastRegistrationError(KErrNone);

	CloneExtensionParamsL(*newCopy);

	if (aWithDynamicValues) // All vales are copied
		{
		newCopy->SetId(Id());
		newCopy->SetStatus(Status());
		newCopy->SetContextId(ContextId());
		newCopy->SetLastRegistrationError(LastRegistrationError());				
		}
	else
		{
		newCopy->SetStatus(EUnregistered);
		newCopy->SetContextId(0);
		newCopy->SetDefault(EFalse);
		newCopy->SetLastRegistrationError(KErrNone);

	    // The Contact-header's user-part must be unique. 
	    // It is stored in the extension parameters to preserve data compatibility.
		HBufC8* username = CreateContactUserNameLC();
		newCopy->SetExtensionParameterL(KSIPContactHeaderUser,*username);
		CleanupStack::PopAndDestroy(username);			
		}

	CleanupStack::Pop(newCopy);
	return newCopy;
	}		


// -----------------------------------------------------------------------------
// CSIPConcreteProfile::~CSIPConcreteProfile
// Destructor.
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPConcreteProfile::~CSIPConcreteProfile()
	{
	if (iSIPHeaders)
	    {
	    iSIPHeaders->Reset();   
	    }
	delete iSIPHeaders;
	
	delete iSIPNegotiatedSecurityMechanism;
	
	if (iSIPRegisteredAORs)
	    {
	    iSIPRegisteredAORs->Reset();
	    }
	delete iSIPRegisteredAORs;
    
    if (iSIPContactHeaderParams)
        {
        iSIPContactHeaderParams->Reset();
        }
	delete iSIPContactHeaderParams;
	
	iSIPOutboundProxyParameter.ResetAndDestroy();
	iSIPOutboundProxyExtParameter.ResetAndDestroy();
	iSIPRegistrarParameter.ResetAndDestroy();
	iSIPRegistrarExtParameter.ResetAndDestroy();	

	delete iDynamicProxy;
	delete iSIPAORUri8;
	delete iSIPRegistrar;
	delete iSIPProviderName;
	delete iSIPPrivateIdentity;
	delete iSIPOutboundProxy;

	iExtensionIntParams.Close();
	iExtensionBoolParams.Close();
	iExtensionDescrParams.ResetAndDestroy();
	iExtensionDescrArrayParams.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::Id
// -----------------------------------------------------------------------------
//	
EXPORT_C TUint32 CSIPConcreteProfile::Id() const
	{
	return iSIPProfileId;
	}
        
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetId
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetId(TUint32 aId)
	{
	iSIPProfileId = aId;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetIapId
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetIapId(TUint32 aIapId)
	{
	iSIPIAPId = aIapId;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IapId
// -----------------------------------------------------------------------------
//	
EXPORT_C TUint32 CSIPConcreteProfile::IapId() const
	{
	return iSIPIAPId;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetProfileTypeL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetProfileType(
    const TSIPProfileTypeInfo& aProfileType) 
	{
	iSIPProfileType.iSIPProfileClass = aProfileType.iSIPProfileClass;
	iSIPProfileType.iSIPProfileName = aProfileType.iSIPProfileName;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ProfileType
// -----------------------------------------------------------------------------
//	
EXPORT_C const TSIPProfileTypeInfo& CSIPConcreteProfile::ProfileType() const
	{
	return iSIPProfileType;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::Server
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::Server(TUint32 aServer) const
	{
	switch(aServer) 
		{
		case KSIPOutboundProxy:
			return *iSIPOutboundProxy;
		case KSIPRegistrar:
			return *iSIPRegistrar;
		default:
			return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ServerParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::ServerParameter(
    TUint32 aServer,
    TUint32 aServerParam) const
	{
	if (aServerParam == KSIPDigestUserName || 
		aServerParam == KSIPDigestRealm ||
		aServerParam == KSIPDigestPassword)
		{
		TBuf8<KParameterLength> parameter;
		parameter.Format(KSIPProfileParameter, aServerParam);

		switch(aServer) 
			{
			case KSIPOutboundProxy:
				return Parameter(&iSIPOutboundProxyParameter, parameter);
			case KSIPRegistrar:
				return Parameter(&iSIPRegistrarParameter, parameter);
			default:
				return KNullDesC8;
			}
		}
	else
		{
		return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ServerExtensionParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::ServerExtensionParameter(
    TUint32 aServer,
	const TDesC8& aName) const
	{
	switch(aServer) 
		{
		case KSIPOutboundProxy:
			return Parameter(&iSIPOutboundProxyExtParameter, aName);
		case KSIPRegistrar:
			return Parameter(&iSIPRegistrarExtParameter, aName);
		default:
			return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetServerL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetServerL(
    TUint32 aServer, 
							     const TDesC8& aSipUri)
	{
	switch(aServer) 
		{
		case KSIPOutboundProxy:
			SetOutboundProxyL(aSipUri);
			break;
		case KSIPRegistrar:
			SetRegistrarL(aSipUri);
			break;
		default:
			User::Leave (KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetServerParameterL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetServerParameterL(
    TUint32 aServer,
    TUint32 aServerParam,
    const TDesC8& aValue)
	{
	__ASSERT_ALWAYS(aServerParam == KSIPDigestUserName ||
		aServerParam == KSIPDigestRealm ||
		aServerParam == KSIPDigestPassword , User::Leave(KErrArgument));

	TBuf8<KParameterLength> parameter;
	parameter.Format(KSIPProfileParameter, aServerParam);

	switch(aServer) 
		{
		case KSIPOutboundProxy:
			SetParameterL(&iSIPOutboundProxyParameter, parameter, aValue);
			break;
		case KSIPRegistrar:
			SetParameterL(&iSIPRegistrarParameter, parameter, aValue);
			break;
		default:
			User::Leave (KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetServerExtensionParameterL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetServerExtensionParameterL(
    TUint32 aServer,
    const TDesC8& aName,
    const TDesC8& aValue)
	{
	switch(aServer) 
		{
		case KSIPOutboundProxy:
			SetParameterL(&iSIPOutboundProxyExtParameter, aName, aValue);
			break;
		case KSIPRegistrar:
			SetParameterL(&iSIPRegistrarExtParameter, aName, aValue);
			break;
		default:
			User::Leave (KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetAORL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetAORL(const TDesC8& aAOR)
	{
    TUriParser8 parser;
    User::LeaveIfError(parser.Parse(aAOR));
    CUri8* tmp = CUri8::NewL(parser);
	delete iSIPAORUri8;
	iSIPAORUri8 = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::AOR
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::AOR() const
	{
	return iSIPAORUri8->Uri().UriDes();
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::AORUri8
// -----------------------------------------------------------------------------
//	
EXPORT_C const TUriC8& CSIPConcreteProfile::AORUri8() const
    {
    return iSIPAORUri8->Uri();
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::PrivateIdentity
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::PrivateIdentity() const
	{
	return *iSIPPrivateIdentity;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetPrivateIdentityL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetPrivateIdentityL(
    const TDesC8& aPrivateIdentity)
	{
	HBufC8* tmp = aPrivateIdentity.AllocL();
	delete iSIPPrivateIdentity;
	iSIPPrivateIdentity = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetProviderNameL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetProviderNameL(
    const TDesC8& aProviderName)
	{
	HBufC8* tmp = aProviderName.AllocL();
	delete iSIPProviderName;
	iSIPProviderName = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ProviderName
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::ProviderName() const
	{
	return *iSIPProviderName;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt
CSIPConcreteProfile::ExtensionParameter(TUint aID, TUint32& aValue) const
	{
	TInt index = ExtensionIntParamIndex(aID);
	if (index != KErrNotFound)
		{
		aValue = iExtensionIntParams[index].Value();
		return KErrNone;
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
    TUint aID,
    TBool& aValue) const
	{
	TInt index = ExtensionBoolParamIndex(aID);
	if (index != KErrNotFound)
		{
		aValue = iExtensionBoolParams[index].Value();
		return KErrNone;
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
    TUint aID,
    TDesC8 const *& aValue) const
	{
	TInt index = ExtensionDescrParamIndex(aID);
	if (index != KErrNotFound)
		{
		aValue = &iExtensionDescrParams[index]->Value();
		return KErrNone;
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
    TUint aID,
    MDesC8Array const *& aValue) const
	{
	TInt index = ExtensionDescrArrayParamIndex(aID);
	if (index != KErrNotFound)
		{
		aValue = &iExtensionDescrArrayParams[index]->Value();
		return KErrNone;
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetExtensionParameterL
// If parameter with the same ID already exists, remove the old parameter.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConcreteProfile::SetExtensionParameterL(
    TUint32 aID, 
    TUint32 aValue)
	{
	TInt index = ExtensionIntParamIndex(aID);
	if (index != KErrNotFound)
		{
		iExtensionIntParams.Remove(index);
		}

	TExtensionIntParam param(aID, aValue);
	iExtensionIntParams.AppendL(param);	
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetExtensionParameterL
// If parameter with the same ID already exists, remove the old parameter.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConcreteProfile::SetExtensionParameterL(
    TUint32 aID,
    TBool aValue)
	{
	TInt index = ExtensionBoolParamIndex(aID);
	if (index != KErrNotFound)
		{
		iExtensionBoolParams.Remove(index);			
		}

	TExtensionBoolParam param(aID, aValue);
	iExtensionBoolParams.AppendL(param);
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetExtensionParameterL
// If parameter with the same ID already exists, remove the old parameter.
// -----------------------------------------------------------------------------
//		 
EXPORT_C void CSIPConcreteProfile::SetExtensionParameterL(
    TUint32 aID,
    const TDesC8& aValue)
	{
	TInt index = ExtensionDescrParamIndex(aID);
	if (index != KErrNotFound)
		{
		CExtensionDescrParam* param = iExtensionDescrParams[index];
		iExtensionDescrParams.Remove(index);
		delete param;
		}

	if (aValue.Length() > 0)
		{		
		CExtensionDescrParam* param = CExtensionDescrParam::NewLC(aID, aValue);
		iExtensionDescrParams.AppendL(param);
		CleanupStack::Pop(param);
		}
	
	TUint32 iVal;
	if(aID == KSIPAccessPointName && !IapId())
		{
		iVal = FetchIAPIDL(aValue);
		SetIapId(iVal);
		}
	
	
	if(aID == KSIPSnapName && ExtensionParameter(KSIPSnapId, iVal) != KErrNone )
		{
		iVal = FetchSNAPIDL(aValue);
		SetExtensionParameterL(KSIPSnapId,iVal);
		}

	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetExtensionParameterL
// If parameter with the same ID already exists, remove the old parameter.
// -----------------------------------------------------------------------------
//		 
EXPORT_C void CSIPConcreteProfile::SetExtensionParameterL(
    TUint32 aID,
    const MDesC8Array& aValue)
	{
	TInt index = ExtensionDescrArrayParamIndex(aID);
	if (index != KErrNotFound)
		{	
		CExtensionDescrArrayParam* param = iExtensionDescrArrayParams[index];
		iExtensionDescrArrayParams.Remove(index);
		delete param;
		}

	if (aValue.MdcaCount() > 0)
		{
		CExtensionDescrArrayParam* param =
			CExtensionDescrArrayParam::NewLC(aID, aValue);
		iExtensionDescrArrayParams.AppendL(param);
		CleanupStack::Pop(param);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionIntParamIndex
// -----------------------------------------------------------------------------
//
TInt CSIPConcreteProfile::ExtensionIntParamIndex(TUint32 aID) const
	{
	for (TInt i = 0; i < iExtensionIntParams.Count(); ++i)
		{
		if (iExtensionIntParams[i].ID() == aID)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionBoolParamIndex
// -----------------------------------------------------------------------------
//
TInt CSIPConcreteProfile::ExtensionBoolParamIndex(TUint32 aID) const
	{
	for (TInt i = 0; i < iExtensionBoolParams.Count(); ++i)
		{
		if (iExtensionBoolParams[i].ID() == aID)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionDescrParamIndex
// -----------------------------------------------------------------------------
//
TInt CSIPConcreteProfile::ExtensionDescrParamIndex(TUint32 aID) const
	{
	for (TInt i = 0; i < iExtensionDescrParams.Count(); ++i)
		{
		if (iExtensionDescrParams[i]->ID() == aID)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionDescrArrayParamIndex
// -----------------------------------------------------------------------------
//
TInt CSIPConcreteProfile::ExtensionDescrArrayParamIndex(TUint32 aID) const
	{
	for (TInt i = 0; i < iExtensionDescrArrayParams.Count(); ++i)
		{
		if (iExtensionDescrArrayParams[i]->ID() == aID)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetDefault
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetDefault(TBool aOn)
	{
	iDefaultProfile = aOn;
	}
	
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IsDefault
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CSIPConcreteProfile::IsDefault() const
	{
	//This parameter is set as ETrue, only temporarily.
	//The value of iDefaultProfile is normally EFalse. 
	return iDefaultProfile;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::EnableSigComp
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::EnableSigComp(TBool aOn)
	{
	iSIPSigComp = aOn;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IsSigCompEnabled
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CSIPConcreteProfile::IsSigCompEnabled() const
	{
	return iSIPSigComp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::EnableSecurityNegotiation
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::EnableSecurityNegotiation(TBool aOn)
	{
	iSecurityNegotiation = aOn;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IsSecurityNegotiationEnabled
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CSIPConcreteProfile::IsSecurityNegotiationEnabled() const
	{
	return iSecurityNegotiation;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetAutoRegistrationEnabled
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetAutoRegistrationEnabled(TBool aOn)
	{
	iSIPAutoRegistered = aOn;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IsAutoRegistrationEnabled
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CSIPConcreteProfile::IsAutoRegistrationEnabled() const
	{
	return iSIPAutoRegistered;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::TStatus
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPConcreteProfile::TStatus CSIPConcreteProfile::Status() const
	{
	return iStatus;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetStatus
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetStatus(
    CSIPConcreteProfile::TStatus aStatus)
	{
	iStatus = aStatus;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ContextId
// -----------------------------------------------------------------------------
//	
EXPORT_C TUint32 CSIPConcreteProfile::ContextId() const
	{
	return iContextId;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetContextId
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetContextId(TUint32 id)
	{
	iContextId = id;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetLastRegistrationError
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetLastRegistrationError(TInt aError)
	{
	iLastRegistrationError = aError;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::LastRegistrationError
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPConcreteProfile::LastRegistrationError() const
	{
	return iLastRegistrationError;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::InternalizeL
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPConcreteProfile* CSIPConcreteProfile::InternalizeL(
    RReadStream& aReadStream,
    TBool aAll)
	{
	CSIPConcreteProfile* self = new(ELeave)CSIPConcreteProfile();
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream, aAll);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExternalizeL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::ExternalizeL(
    RWriteStream& aWriteStream, 
    TBool aAll) const
	{
	TInt i = 0;

	aWriteStream.WriteUint32L(iSIPProfileId);

	aWriteStream.WriteUint32L(iSIPIAPId);

	aWriteStream.WriteInt32L(iSIPProfileType.iSIPProfileName.Length());
	aWriteStream.WriteL(iSIPProfileType.iSIPProfileName);
	aWriteStream.WriteUint8L(iSIPProfileType.iSIPProfileClass);

	aWriteStream.WriteInt32L(iSIPRegistrar->Length());
	aWriteStream.WriteL(*iSIPRegistrar);

	aWriteStream.WriteUint32L(iSIPRegistrarParameter.Count());
	for (i = 0; i < iSIPRegistrarParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPRegistrarParameter[i];
		param->ExternalizeL(aWriteStream);
		}

	aWriteStream.WriteUint32L(iSIPRegistrarExtParameter.Count());
	for (i = 0; i < iSIPRegistrarExtParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPRegistrarExtParameter[i];
		param->ExternalizeL(aWriteStream);
		}

	aWriteStream.WriteInt32L(iSIPOutboundProxy->Length());
	aWriteStream.WriteL(*iSIPOutboundProxy);

	aWriteStream.WriteUint32L(iSIPOutboundProxyParameter.Count());
	for (i = 0; i < iSIPOutboundProxyParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPOutboundProxyParameter[i];
		param->ExternalizeL(aWriteStream);
		}

	aWriteStream.WriteUint32L(iSIPOutboundProxyExtParameter.Count());
	for (i = 0; i < iSIPOutboundProxyExtParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPOutboundProxyExtParameter[i];
		param->ExternalizeL(aWriteStream);
		}

	aWriteStream.WriteInt32L(iSIPProviderName->Length());
	aWriteStream.WriteL(*iSIPProviderName);

	aWriteStream.WriteInt32L(iSIPPrivateIdentity->Length());
	aWriteStream.WriteL(*iSIPPrivateIdentity);

	aWriteStream.WriteInt32L(AOR().Length());
	aWriteStream.WriteL(AOR());
	
	aWriteStream.WriteInt8L(iSIPSigComp);
	aWriteStream.WriteInt8L(iSecurityNegotiation);
	aWriteStream.WriteInt8L(iSIPAutoRegistered);
	aWriteStream.WriteInt8L(iDynamicProxyResolving);

    ExternalizeDesArrayL(*iSIPContactHeaderParams,aWriteStream);
    ExternalizeDesArrayL(*iSIPHeaders,aWriteStream);

	if (aAll) 
		{
		aWriteStream.WriteInt8L(iStatus);
		aWriteStream.WriteUint32L(iContextId);
		aWriteStream.WriteInt8L(iDefaultProfile);
		aWriteStream.WriteUint32L(iLastRegistrationError);
		}

	//Extension parameters
	aWriteStream.WriteUint32L(iExtensionIntParams.Count());
	for (i = 0; i < iExtensionIntParams.Count(); ++i)
		{
		iExtensionIntParams[i].ExternalizeL(aWriteStream);
		}
	
	aWriteStream.WriteUint32L(iExtensionBoolParams.Count());
	for (i = 0; i < iExtensionBoolParams.Count(); ++i)
		{
		iExtensionBoolParams[i].ExternalizeL(aWriteStream);
		}
	
	aWriteStream.WriteUint32L(iExtensionDescrParams.Count());
	for (i = 0; i < iExtensionDescrParams.Count(); ++i)
		{
		iExtensionDescrParams[i]->ExternalizeL(aWriteStream);
		}
		
	aWriteStream.WriteUint32L(iExtensionDescrArrayParams.Count());
	for (i = 0; i < iExtensionDescrArrayParams.Count(); ++i)
		{
		iExtensionDescrArrayParams[i]->ExternalizeL(aWriteStream);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExternalizedSizeL
// -----------------------------------------------------------------------------
//	
EXPORT_C TUint CSIPConcreteProfile::ExternalizedSizeL(TBool aAll) const
	{	
	TUint size = 14 * sizeof(TUint32) +
				 5 * sizeof(TInt8) +
				 iSIPProfileType.iSIPProfileName.Length() +
  			     iSIPRegistrar->Length() +
			     iSIPOutboundProxy->Length() +
			     iSIPProviderName->Length() +
			     iSIPPrivateIdentity->Length() +
			     AOR().Length();
	TInt i(0);
	for (i = 0; i < iSIPRegistrarParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPRegistrarParameter[i];
		size = size + param->ExternalizedSizeL();
		}

	for (i = 0; i < iSIPRegistrarExtParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPRegistrarExtParameter[i];
		size = size + param->ExternalizedSizeL();
		}

	for (i = 0; i < iSIPOutboundProxyParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPOutboundProxyParameter[i];
		size = size + param->ExternalizedSizeL();
		}

	for (i = 0; i < iSIPOutboundProxyExtParameter.Count(); i++)
		{
		CSIPProfileParameter* param = iSIPOutboundProxyExtParameter[i];
		size = size + param->ExternalizedSizeL();
		}

	for (i = 0; i < iSIPContactHeaderParams->Count(); i++)
		{
		const TPtrC8& value = (*iSIPContactHeaderParams)[i];
		size = size + sizeof(TInt32) + value.Length();
		}

	for (i = 0; i < iSIPHeaders->Count(); i++)
		{
		const TPtrC8& value = (*iSIPHeaders)[i];
		size = size + sizeof(TInt32) + value.Length();
		}	

	if (aAll)
		{
		size = size + 10; // 2 + 8
		}

	//Every extension parameter array writes a Uint32 that tells how many
	//elements that array has.
	const TInt KAmountOfExtensionArrays = 4;
	size = size + KAmountOfExtensionArrays * sizeof(TUint32);

	for (i = 0; i < iExtensionIntParams.Count(); ++i)
		{
		size = size + iExtensionIntParams[i].ExternalizedSizeL();
		}
	
	for (i = 0; i < iExtensionBoolParams.Count(); ++i)
		{
		size = size + iExtensionBoolParams[i].ExternalizedSizeL();
		}
		
	for (i = 0; i < iExtensionDescrParams.Count(); ++i)
		{
		size = size + iExtensionDescrParams[i]->ExternalizedSizeL();
		}
		
	for (i = 0; i < iExtensionDescrArrayParams.Count(); ++i)
		{
		size = size + iExtensionDescrArrayParams[i]->ExternalizedSizeL();
		}

	return size;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::operator==
// -----------------------------------------------------------------------------
//	
TBool CSIPConcreteProfile::operator==(
    const CSIPConcreteProfile& aProfile)
	{
	iLastComparisonResult = 0;

	if (iSIPProfileId != aProfile.Id())
		{
	    iLastComparisonResult = iLastComparisonResult | EId;
		}

	if (iSIPIAPId != aProfile.IapId())
		{
	    iLastComparisonResult = iLastComparisonResult | EIAPId;
		}

	if	(
		(iSIPProfileType.iSIPProfileName.Compare(aProfile.ProfileType().iSIPProfileName) != 0)||
		(iSIPProfileType.iSIPProfileClass != aProfile.ProfileType().iSIPProfileClass)
		)
		{
		iLastComparisonResult = iLastComparisonResult | ETypeInfo;
		}

	if (iSIPRegistrar->Compare(aProfile.Server(KSIPRegistrar)) != 0)
		{
	    iLastComparisonResult = iLastComparisonResult | ERegistrar;
		}

	if (iSIPOutboundProxy->Compare(aProfile.Server(KSIPOutboundProxy)) != 0)
		{
	    iLastComparisonResult = iLastComparisonResult | EOutboundProxy;
		}

	if (iSIPPrivateIdentity->Compare(aProfile.PrivateIdentity()) != 0)
		{
	    iLastComparisonResult = iLastComparisonResult | EPrivateIdentity;
		}

	if (iSIPProviderName->Compare(aProfile.ProviderName()) != 0)
		{
	    iLastComparisonResult = iLastComparisonResult | EName;
		}


	if ((iSIPRegistrarParameter.Count() != aProfile.iSIPRegistrarParameter.Count())||
		(ServerParameter(KSIPRegistrar, KSIPDigestRealm).
		Compare(aProfile.ServerParameter(KSIPRegistrar, KSIPDigestRealm)) != 0)||
		(ServerParameter(KSIPRegistrar, KSIPDigestUserName).
		Compare(aProfile.ServerParameter(KSIPRegistrar, KSIPDigestUserName)) != 0)||
		(ServerParameter(KSIPRegistrar, KSIPDigestPassword).
		Compare(aProfile.ServerParameter(KSIPRegistrar, KSIPDigestPassword)) != 0)
		)
		{
	    iLastComparisonResult = iLastComparisonResult | ERegistrarParameters;
		}


	if (iSIPRegistrarExtParameter.Count() != iSIPRegistrarExtParameter.Count())
		{
	    iLastComparisonResult = iLastComparisonResult | ERegistrarExtensionParameters;
		}

	TInt i = 0;
	if	(iLastComparisonResult != ERegistrarExtensionParameters)
		{
		for (i = 0; i < iSIPRegistrarExtParameter.Count(); i++)
			{
			CSIPProfileParameter* param = iSIPRegistrarExtParameter[i];
			if (param->Value().Compare(aProfile.ServerExtensionParameter(KSIPRegistrar,
																	 param->Key())) != 0)
				{
				iLastComparisonResult = iLastComparisonResult | ERegistrarExtensionParameters;
				}
			}
		}
	
	if	(
		(iSIPOutboundProxyParameter.Count() != aProfile.iSIPOutboundProxyParameter.Count())||
		(ServerParameter(KSIPOutboundProxy, KSIPDigestRealm).
		Compare(aProfile.ServerParameter(KSIPOutboundProxy, KSIPDigestRealm)) != 0)||
		(ServerParameter(KSIPOutboundProxy, KSIPDigestUserName).
		Compare(aProfile.ServerParameter(KSIPOutboundProxy, KSIPDigestUserName)) != 0)||
		(ServerParameter(KSIPOutboundProxy, KSIPDigestPassword).
		Compare(aProfile.ServerParameter(KSIPOutboundProxy, KSIPDigestPassword)) != 0)
		)
		{
		iLastComparisonResult = iLastComparisonResult | EOutboundProxyParameters;
		}		
		
	if (iSIPOutboundProxyExtParameter.Count() != 
	    aProfile.iSIPOutboundProxyExtParameter.Count())
		{
	    iLastComparisonResult = iLastComparisonResult | EOutboundProxyExtensionParameters;
		}

	if	(iLastComparisonResult != EOutboundProxyExtensionParameters)
		{
		for (i = 0; i < iSIPOutboundProxyExtParameter.Count(); i++)
			{
			CSIPProfileParameter* param = iSIPOutboundProxyExtParameter[i];
			if (param->Value().Compare(
			        aProfile.ServerExtensionParameter(KSIPOutboundProxy,param->Key())) != 0)
				{
				iLastComparisonResult = 
				    iLastComparisonResult | EOutboundProxyExtensionParameters;
				}
			}
		}
	
    TInt aorComparisonErr = iSIPAORUri8->Uri().Equivalent(aProfile.AORUri8());
	if (aorComparisonErr != KErrNone && 
	    aorComparisonErr != KErrNoMemory)
		{
	    iLastComparisonResult = iLastComparisonResult | EAOR;
		}

	if (iSIPSigComp != aProfile.IsSigCompEnabled())
		{
		iLastComparisonResult = iLastComparisonResult | ESigComp;
		}

	if (iSecurityNegotiation != aProfile.IsSecurityNegotiationEnabled()) 
		{
		iLastComparisonResult = iLastComparisonResult | ESecurityNegotiation;
		}

	if (iSIPAutoRegistered != aProfile.IsAutoRegistrationEnabled()) 
		{
		iLastComparisonResult = iLastComparisonResult | EAutoRegistration;
		}

	if (iDynamicProxy->Compare(aProfile.DynamicProxy()) != 0)
		{
		iLastComparisonResult = iLastComparisonResult | EProxyResolving;
		}
	
	TBool done = EFalse;
	const MDesC8Array& params = aProfile.ContactHeaderParams();
	if (iSIPContactHeaderParams->Count() != params.MdcaCount())
		{
		iLastComparisonResult = iLastComparisonResult | EContactHeaderParams;
		done = ETrue;
		}
		
	
	for (i = 0; i < params.MdcaCount() && !done; i++)
		{
		TInt pos = 0;

		if (iSIPContactHeaderParams->Find(params.MdcaPoint(i), pos) != 0)
			{
			iLastComparisonResult = iLastComparisonResult | EContactHeaderParams;
			done = ETrue;
			}
		}
	
	const MDesC8Array& headers = aProfile.SIPHeaders();	
	if (iSIPHeaders->Count() != headers.MdcaCount())
		{
		iLastComparisonResult = iLastComparisonResult | ESIPHeaders;
		}
	for (i = 0; i < headers.MdcaCount(); i++)
		{
		TInt pos = 0;

		if (iSIPHeaders->Find(headers.MdcaPoint(i), pos) != 0)
			{
			iLastComparisonResult = iLastComparisonResult | ESIPHeaders;
			}
		}
	
	return iLastComparisonResult == 0;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::LastCompareResult
// -----------------------------------------------------------------------------
//	
TInt32 CSIPConcreteProfile::LastCompareResult() const
	{
	return iLastComparisonResult;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CloneExtensionParamsL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::CloneExtensionParamsL(
	CSIPConcreteProfile& aNewCopy) const
	{
	TInt i(0);
	for (i = 0; i < iExtensionIntParams.Count(); ++i)
		{
		TExtensionIntParam param = iExtensionIntParams[i];
		aNewCopy.SetExtensionParameterL(param.ID(), param.Value());
		}
		
	for (i = 0; i < iExtensionBoolParams.Count(); ++i)
		{
		TExtensionBoolParam param = iExtensionBoolParams[i];
		aNewCopy.SetExtensionParameterL(param.ID(), param.Value());
		}

	for (i = 0; i < iExtensionDescrParams.Count(); ++i)
		{
		CExtensionDescrParam* param = iExtensionDescrParams[i];
		aNewCopy.SetExtensionParameterL(param->ID(), param->Value());
		}
		
	for (i = 0; i < iExtensionDescrArrayParams.Count(); ++i)
		{
		CExtensionDescrArrayParam* param = iExtensionDescrArrayParams[i];
		aNewCopy.SetExtensionParameterL(param->ID(), param->Value());
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetDynamicProxyL
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::DynamicProxy() const
	{
	return *iDynamicProxy;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetDynamicProxyL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetDynamicProxyL(const TDesC8& aDynamicProxy)
	{
	HBufC8* tmp = aDynamicProxy.AllocL();
	delete iDynamicProxy;
	iDynamicProxy = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ContactHeaderParams
// -----------------------------------------------------------------------------
//	
EXPORT_C const MDesC8Array& CSIPConcreteProfile::ContactHeaderParams() const
	{
	return *iSIPContactHeaderParams;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetContactHeaderParamsL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetContactHeaderParamsL(
	const MDesC8Array& aParams)
	{
	CDesC8ArrayFlat* tmp = CopyDesArrayL(aParams);
    iSIPContactHeaderParams->Reset();
    delete iSIPContactHeaderParams;
	iSIPContactHeaderParams = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::RegisteredAORs
// -----------------------------------------------------------------------------
//	
EXPORT_C const MDesC8Array& CSIPConcreteProfile::RegisteredAORs() const
	{
	return *iSIPRegisteredAORs;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetRegisteredAORsL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetRegisteredAORsL(
	const MDesC8Array& aParams)
	{
	CDesC8ArrayFlat* tmp = CopyDesArrayL(aParams);
	iSIPRegisteredAORs->Reset();
	delete iSIPRegisteredAORs;
	iSIPRegisteredAORs = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::NegotiatedSecurityMechanism
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPConcreteProfile::NegotiatedSecurityMechanism() const
	{
	return *iSIPNegotiatedSecurityMechanism;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetNegotiatedSecurityMechanismL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::SetNegotiatedSecurityMechanismL(
	const TDesC8& aMechanism)
	{
	HBufC8* tmp = aMechanism.AllocL();
	delete iSIPNegotiatedSecurityMechanism;
	iSIPNegotiatedSecurityMechanism = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SIPHeaders
// -----------------------------------------------------------------------------
//	
EXPORT_C const MDesC8Array& CSIPConcreteProfile::SIPHeaders() const
    {
    return *iSIPHeaders;
    }
    
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetSIPHeadersL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConcreteProfile::SetSIPHeadersL(const MDesC8Array& aHeaders)
    {
	CDesC8ArrayFlat* tmp = CopyDesArrayL(aHeaders);	
	iSIPHeaders->Reset();
	delete iSIPHeaders;	
	iSIPHeaders = tmp;		
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetEnabled
// -----------------------------------------------------------------------------
//   
EXPORT_C void CSIPConcreteProfile::SetEnabled(TBool aIsEnabled)
    {
    iIsEnabled = aIsEnabled;
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IsEnabled
// -----------------------------------------------------------------------------
//   
EXPORT_C TBool CSIPConcreteProfile::IsEnabled() const
    {
    return iIsEnabled;
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetStorageId
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConcreteProfile::SetStorageId(TUint32 aId)
    {
    iStorageId = aId;
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::StorageId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CSIPConcreteProfile::StorageId() const
    {
    return iStorageId;
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::InternalizeRegisteredAORsL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::InternalizeRegisteredAORsL(
	RReadStream& aReadStream)
	{
	CDesC8ArrayFlat* tmp = InternalizeDesArrayL(aReadStream);
	iSIPRegisteredAORs->Reset();
	delete iSIPRegisteredAORs;
	iSIPRegisteredAORs = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExternalizeRegisteredAORsL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConcreteProfile::ExternalizeRegisteredAORsL(
	RWriteStream& aWriteStream) const
	{
	ExternalizeDesArrayL(*iSIPRegisteredAORs,aWriteStream);
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExternalizedRegisteredAORsSizeL
// -----------------------------------------------------------------------------
//	
EXPORT_C TUint CSIPConcreteProfile::ExternalizedRegisteredAORsSizeL() const
	{
	TUint size = sizeof(TUint32);
	for (TInt i = 0; i < iSIPRegisteredAORs->Count(); i++)
		{
		const TPtrC8& value = (*iSIPRegisteredAORs)[i];
		size = size + sizeof(TInt32);
		size = size + value.Length();
		}
	return size;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionBoolParameterCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionBoolParameterCount() const
	{
	return iExtensionBoolParams.Count();
	}	

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionDesArrayParameterCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionDesArrayParameterCount() const
	{
	return iExtensionDescrArrayParams.Count();
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionDesParameterCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionDesParameterCount() const
	{
	return iExtensionDescrParams.Count();
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionIntParameterCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionIntParameterCount() const
	{
	return iExtensionIntParams.Count();
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
	TInt aIndex,
	TUint& aID,
	TUint32& aValue) const
	{
	TInt retVal(KErrNotFound);
	if (aIndex < iExtensionIntParams.Count())	
		{
		aID = iExtensionIntParams[aIndex].ID();
		aValue = iExtensionIntParams[aIndex].Value();
		retVal = KErrNone;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
	TInt aIndex,
	TUint& aID,
	TBool& aValue ) const
	{
	TInt retVal(KErrNotFound);
	if (aIndex < iExtensionBoolParams.Count())	
		{
		aID = iExtensionBoolParams[aIndex].ID();
		aValue = iExtensionBoolParams[aIndex].Value();
		retVal = KErrNone;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
	TInt aIndex,
	TUint& aID,
	MDesC8Array const *& aValue ) const
	{
	TInt retVal(KErrNotFound);
	if (aIndex < iExtensionDescrArrayParams.Count())	
		{
		aID = iExtensionDescrArrayParams[aIndex]->ID();
		aValue = &iExtensionDescrArrayParams[aIndex]->Value();
		retVal = KErrNone;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ExtensionParameter(
	TInt aIndex,
	TUint& aID,
	TDesC8 const *& aValue ) const
	{
	TInt retVal(KErrNotFound);
	if (aIndex < iExtensionDescrParams.Count())	
		{
		aID = iExtensionDescrParams[aIndex]->ID();
		aValue = &iExtensionDescrParams[aIndex]->Value();
		retVal = KErrNone;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ServerExtensionParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ServerExtensionParameter(
	TUint32 aServer,
	TInt aIndex,
	TDesC8 const *& aKey,
	TDesC8 const *& aValue) const
	{
	TInt retVal(KErrNotFound);
	switch(aServer) 
			{
			case KSIPOutboundProxy:
				{
				if (aIndex < iSIPOutboundProxyExtParameter.Count())
					{
					aKey = &iSIPOutboundProxyExtParameter[aIndex]->Key();
					aValue = &iSIPOutboundProxyExtParameter[aIndex]->Value();
					retVal = KErrNone;
					}
				break;
				}
			case KSIPRegistrar:
				{
				if (aIndex < iSIPRegistrarExtParameter.Count())
					{
					aKey = &iSIPRegistrarExtParameter[aIndex]->Key();
					aValue = &iSIPRegistrarExtParameter[aIndex]->Value();
					retVal = KErrNone;
					}
				break;
				}
			default:
				break;
			}
	return retVal;		
	}
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ServerExtensionParameterCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ServerExtensionParameterCount(
	TUint32 aServer) const
	{
	switch(aServer) 
		{
		case KSIPOutboundProxy:
			return  iSIPOutboundProxyExtParameter.Count();
		case KSIPRegistrar:
			return iSIPRegistrarExtParameter.Count();
		default:
			return KErrNotFound;	
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ServerParameter
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ServerParameter(
	TUint32 aServer, 
	TInt aIndex,
	TUint& aID,
	TDesC8 const *& aValue) const
	{
	TInt retVal(KErrNotFound);
	TLex8 lex;
	switch(aServer) 
			{
			case KSIPOutboundProxy:
				{
				if (aIndex < iSIPOutboundProxyParameter.Count())
					{
					lex.Assign(iSIPOutboundProxyParameter[aIndex]->Key());
					lex.Val(aID);
					aValue = &iSIPOutboundProxyParameter[aIndex]->Value();
					retVal = KErrNone;
					}
				break;
				}
			case KSIPRegistrar:
				{
				if (aIndex < iSIPRegistrarParameter.Count())
					{
					lex.Assign(iSIPRegistrarParameter[aIndex]->Key());
					lex.Val(aID);
					aValue = &iSIPRegistrarParameter[aIndex]->Value();
					retVal = KErrNone;
					}
				break;
				}
			default:
				break;
			}
	return retVal;		
	}
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ServerParameterCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPConcreteProfile::ServerParameterCount(
	TUint32 aServer) const
	{
	switch(aServer) 
		{
		case KSIPOutboundProxy:
			return  iSIPOutboundProxyParameter.Count();
		case KSIPRegistrar:
			return iSIPRegistrarParameter.Count();
		default:
			return KErrNotFound;	
		}
	}
// CSIPConcreteProfile::DoInternalizeL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::DoInternalizeL(RReadStream& aReadStream, TBool aAll)
	{
	iDynamicProxy = HBufC8::NewL(0);
	iSIPRegisteredAORs = new (ELeave) CDesC8ArrayFlat(1);
	iSIPNegotiatedSecurityMechanism = HBufC8::NewL(0);

	iSIPProfileId = aReadStream.ReadUint32L();
	iSIPIAPId = aReadStream.ReadUint32L();

	TInt32 typeLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS(typeLength >= 0, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(typeLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	HBufC8* typeBuf = HBufC8::NewL (typeLength);
	CleanupStack::PushL(typeBuf);
	TPtr8 type(typeBuf->Des());
	aReadStream.ReadL (type, typeLength);
	iSIPProfileType.iSIPProfileName = *typeBuf;
	CleanupStack::PopAndDestroy(typeBuf);
	iSIPProfileType.iSIPProfileClass = 
		(enum TSIPProfileTypeInfo::TSIPProfileClass) aReadStream.ReadUint8L();
		
	InternalizeRegistrarL(aReadStream);	
	InternalizeProxyL(aReadStream);

	TInt32 providerLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS(providerLength >= 0, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(providerLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	iSIPProviderName = HBufC8::NewL (providerLength);
	TPtr8 provider(iSIPProviderName->Des());
	aReadStream.ReadL (provider, providerLength);

	TInt32 privateLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS(privateLength >= 0, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(privateLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	iSIPPrivateIdentity = HBufC8::NewL (privateLength);
	TPtr8 priv(iSIPPrivateIdentity->Des());
	aReadStream.ReadL (priv, privateLength);

	TInt32 aorLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS(aorLength >= 0, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(aorLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	HBufC8* tmpAorBuf = HBufC8::NewLC(aorLength);
	TPtr8 aorPtr(tmpAorBuf->Des());
	aReadStream.ReadL(aorPtr, aorLength);
	SetAORL(aorPtr);
	CleanupStack::PopAndDestroy(tmpAorBuf);

	iSIPSigComp = aReadStream.ReadInt8L();
	iSecurityNegotiation = aReadStream.ReadInt8L();
	iSIPAutoRegistered = aReadStream.ReadInt8L();
	iDynamicProxyResolving = aReadStream.ReadInt8L();

	iSIPContactHeaderParams = InternalizeDesArrayL(aReadStream);
    iSIPHeaders = InternalizeDesArrayL(aReadStream);

	if (aAll) 
		{
		iStatus = (enum TStatus) aReadStream.ReadUint8L();
		iContextId = aReadStream.ReadUint32L();
		iDefaultProfile = aReadStream.ReadInt8L();
		iLastRegistrationError = aReadStream.ReadUint32L();
		}
	else 
		{
		iStatus = EUnregistered;
		iContextId = 0;
		iDefaultProfile = EFalse;
		iLastRegistrationError = KErrNone;
		}
	
	InternalizeExtensionParamsL(aReadStream);

    // The Contact-header's user-part must be unique present in all profiles. 
    // If the profile is imported from an older platform 
    // that does not have SIP 6.0, generate KSIPContactHeaderUser-parameter 
    // to preserve data compatibility.
    // This profile can also be returned to the older platform
    // as KSIPContactHeaderUser is an extension parameter.
    if (ExtensionDescrParamIndex(KSIPContactHeaderUser) < 0)
        {
	    HBufC8* username = CreateContactUserNameLC();
	    SetExtensionParameterL(KSIPContactHeaderUser,*username);
	    CleanupStack::PopAndDestroy(username);
        }
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::InternalizeRegistrarL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::InternalizeRegistrarL(RReadStream& aReadStream)
	{
	TUint i = 0;
	TInt32 registrarLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS(registrarLength >= 0, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(registrarLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	iSIPRegistrar = HBufC8::NewL (registrarLength);
	TPtr8 registrar(iSIPRegistrar->Des());
	aReadStream.ReadL (registrar, registrarLength);

	TUint count = aReadStream.ReadUint32L();
	for (i = 0; i < count; i++)
		{
		CSIPProfileParameter* param = 
		    CSIPProfileParameter::InternalizeL(aReadStream);
		CleanupStack::PushL(param);
		iSIPRegistrarParameter.AppendL(param);
		CleanupStack::Pop(param);
		}

	count = aReadStream.ReadUint32L();
	for (i = 0; i < count; i++)
		{
		CSIPProfileParameter* param = 
		    CSIPProfileParameter::InternalizeL(aReadStream);
		CleanupStack::PushL(param);
		iSIPRegistrarExtParameter.AppendL(param);
		CleanupStack::Pop(param);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::InternalizeProxyL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::InternalizeProxyL(RReadStream& aReadStream)
	{
	TUint i = 0;
	TInt32 proxyLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS(proxyLength >= 0, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(proxyLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	iSIPOutboundProxy = HBufC8::NewL (proxyLength);
	TPtr8 proxy(iSIPOutboundProxy->Des());
	aReadStream.ReadL (proxy, proxyLength);

	TUint count = aReadStream.ReadUint32L();
	for (i = 0; i < count; i++)
		{
		CSIPProfileParameter* param = 
		    CSIPProfileParameter::InternalizeL(aReadStream);
		CleanupStack::PushL(param);
		iSIPOutboundProxyParameter.AppendL(param);
		CleanupStack::Pop(param);
		}

	count = aReadStream.ReadUint32L();
	for (i = 0; i < count; i++)
		{
		CSIPProfileParameter* param = 
		    CSIPProfileParameter::InternalizeL(aReadStream);
		CleanupStack::PushL(param);
		iSIPOutboundProxyExtParameter.AppendL(param);
		CleanupStack::Pop(param);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::InternalizeExtensionParamsL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::InternalizeExtensionParamsL(RReadStream& aReadStream)
	{
	TUint count = aReadStream.ReadUint32L();
	TUint i(0);
	for (i = 0; i < count; ++i)
		{
		TExtensionIntParam param =
			TExtensionIntParam::InternalizeL(aReadStream);
		iExtensionIntParams.AppendL(param);
		}
	
	count = aReadStream.ReadUint32L();
	for (i = 0; i < count; ++i)
		{
		TExtensionBoolParam param =
			TExtensionBoolParam::InternalizeL(aReadStream);
		iExtensionBoolParams.AppendL(param);
		}
	
	count = aReadStream.ReadUint32L();
	for (i = 0; i < count; ++i)
		{
		CExtensionDescrParam* param =
			CExtensionDescrParam::InternalizeL(aReadStream);
		CleanupStack::PushL(param);
		iExtensionDescrParams.AppendL(param);
		CleanupStack::Pop(param);
		}
	
	count = aReadStream.ReadUint32L();
	for (i = 0; i < count; ++i)
		{
		CExtensionDescrArrayParam* param =
			CExtensionDescrArrayParam::InternalizeL(aReadStream);
		CleanupStack::PushL(param);
		iExtensionDescrArrayParams.AppendL(param);
		CleanupStack::Pop(param);
		}	
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::IndexOf
// -----------------------------------------------------------------------------
//	
TInt CSIPConcreteProfile::IndexOf(const TDesC8& aKey, 
	const RPointerArray<CSIPProfileParameter>* aArray) const
	{
	TInt index = KErrNotFound;
	TBool found = EFalse;

	for (TInt i = 0; i < aArray->Count() && !found; i ++)
		{
		if ((*aArray)[i]->Key().Compare(aKey) == 0)
			{
			index = i;
			found = ETrue;
			}
		}
	return index;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetOutboundProxyL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::SetOutboundProxyL(const TDesC8& aOutboundProxy)
	{
	HBufC8* tmp = aOutboundProxy.AllocL();
	delete iSIPOutboundProxy;
	iSIPOutboundProxy = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetRegistrarL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::SetRegistrarL(const TDesC8& aRegistrar)
	{
	HBufC8* tmp = aRegistrar.AllocL();
	delete iSIPRegistrar;
	iSIPRegistrar = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::SetParameterL(
	RPointerArray<CSIPProfileParameter>* aArray, 
	const TDesC8& aName, 
	const TDesC8& aValue)
	{
	TInt index = IndexOf(aName, aArray);
	if (index != KErrNotFound) 
		{
		CSIPProfileParameter* parameter = (*aArray)[index];
		aArray->Remove(index);
		delete parameter;

		if(aValue.Length() > 0)
			{
			parameter = CSIPProfileParameter::NewLC(aName, aValue);
			aArray->AppendL(parameter);
			CleanupStack::Pop(parameter);
			}
		}
	else
		{
		if(aValue.Length() > 0)
			{
			CSIPProfileParameter* parameter = 
				CSIPProfileParameter::NewLC(aName, aValue);
			aArray->AppendL(parameter);
			CleanupStack::Pop(parameter);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::Parameter
// -----------------------------------------------------------------------------
//	
const TDesC8& CSIPConcreteProfile::Parameter(
	const RPointerArray<CSIPProfileParameter>* aArray, 
	const TDesC8& aName) const
	{
	TInt index = IndexOf(aName, aArray);

	if (index != KErrNotFound) 
		return (*aArray)[index]->Value();

	return KNullDesC8;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ExternalizeDesArrayL
// -----------------------------------------------------------------------------
//	
void CSIPConcreteProfile::ExternalizeDesArrayL(const MDesC8Array& aArray, 
		                                       RWriteStream& aWriteStream)
    {
	aWriteStream.WriteUint32L(aArray.MdcaCount());
	for (TInt i = 0; i < aArray.MdcaCount(); i++)
		{
		const TPtrC8& value = aArray.MdcaPoint(i);
		aWriteStream.WriteInt32L(value.Length());
		aWriteStream.WriteL(value);
		}    
    }
    
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::InternalizeDesArrayL
// -----------------------------------------------------------------------------
//    
CDesC8ArrayFlat* 
CSIPConcreteProfile::InternalizeDesArrayL(RReadStream& aReadStream)
    {
	CDesC8ArrayFlat* array = new (ELeave) CDesC8ArrayFlat(1);
	CleanupStack::PushL(array);
	TInt count = aReadStream.ReadUint32L();
	for (TInt i = 0; i < count; i++)
		{
		TInt32 valueLen = aReadStream.ReadInt32L();
		__ASSERT_ALWAYS(valueLen >= 0, User::Leave(KErrCorrupt));
		__ASSERT_ALWAYS(valueLen < KMaxTInt/2, User::Leave (KErrCorrupt));
		HBufC8* valueBuf = HBufC8::NewLC(valueLen);
		TPtr8 value = valueBuf->Des();
		aReadStream.ReadL (value, valueLen);
		array->AppendL(*valueBuf);
		CleanupStack::PopAndDestroy(valueBuf);
		}
    CleanupStack::Pop(array);
    return array;
    }
    
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CopyDesArrayL
// -----------------------------------------------------------------------------
//    
CDesC8ArrayFlat* CSIPConcreteProfile::CopyDesArrayL(const MDesC8Array& aArray)
    {
	CDesC8ArrayFlat* tmp = new (ELeave) CDesC8ArrayFlat(1);
	CleanupStack::PushL(tmp);
	for (TInt i = 0; i < aArray.MdcaCount(); i++)
		{
		tmp->AppendL(aArray.MdcaPoint(i));
		}
	CleanupStack::Pop(tmp);
	return tmp;    
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::CreateContactUserNameLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPConcreteProfile::CreateContactUserNameLC() const
    {
    CSystemRandom* random = CSystemRandom::NewLC();
    HBufC8* data = HBufC8::NewLC(KContactUserNameLength);
    TPtr8 dataPtr(data->Des());
    dataPtr.FillZ(dataPtr.MaxLength());
    random->GenerateBytesL(dataPtr);
    
    HBufC8* result = HBufC8::NewLC(KContactUserNameLength);
    TPtr8 resultPtr(result->Des());
    
    //2^6 = 64 valid chars that can be put to descriptor
	const TInt KMaxBitsReturned = 6;
	_LIT8(KValidChars,
          "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_");
	TUint counter = 0;
	for (TInt i = 0; i < KContactUserNameLength; i++)
		{
		TInt index = 
		    GetNextBits(*data,KMaxBitsReturned,counter)%KValidChars().Length();
		resultPtr.Append(KValidChars()[index]);
		}

	CleanupStack::Pop(result);   
    CleanupStack::PopAndDestroy(data);
    CleanupStack::PopAndDestroy(random);
    CleanupStack::PushL(result);
    return result;
    }

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::GetNextBits
// -----------------------------------------------------------------------------
//
TUint8 CSIPConcreteProfile::GetNextBits(
    const TDesC8& aBuf, 
    TInt aBits, 
    TUint& aCounter) const
	{
	if (aBuf.Length() == 0)
		{
		return 0;
		}

	//Amount of aBits long bit sequences in aBuf
	TUint sixBitItems = aBuf.Length() * KBitsInByte / aBits;

	if (aCounter >= sixBitItems)
		{
		aCounter = 0;
		}

	//The position in aBuf, of the byte containing the first bit of the aBits
	//long bit sequence. Zero means the first byte.
	TInt startByte = aCounter * aBits / KBitsInByte;
	TUint16 result = static_cast<TUint16>(aBuf[startByte] << KBitsInByte);

	if (++startByte >= aBuf.Length())
		{
		startByte = 0;
		}
	
	result = static_cast<TUint16>(result | aBuf[startByte]);
	
	
	//The position of the first bit of the aBits long bit sequence, within the
	//byte. Zero means the first bit.
	TUint offsetInsideByte = (aCounter * aBits) % KBitsInByte;

	//Remove excess bits from the result
	result = static_cast<TUint16>(result << offsetInsideByte);	
	result >>= ((2 * KBitsInByte) - aBits);
	
    aCounter++;
    return static_cast<TUint8>(result);
	}
	
// -----------------------------------------------------------------------------
// CSIPConcreteProfile::FetchIAPIDL
// -----------------------------------------------------------------------------
//
TUint32 CSIPConcreteProfile::FetchIAPIDL(const TDesC8& aValue)
	{
	TUint32 iVal(0);
	TBuf16<1024> recordname;
	recordname.SetLength(aValue.Length());
	CnvUtfConverter::ConvertToUnicodeFromUtf8(recordname,aValue);
	CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1);
	db->SetAttributeMask( ECDHidden );
	CCDIAPRecord* ptrIAPRecord = static_cast<CCDIAPRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL( ptrIAPRecord );
	ptrIAPRecord->iRecordName.SetMaxLengthL(recordname.Length());
	ptrIAPRecord->iRecordName = recordname;
	if(ptrIAPRecord->FindL(*db))
		{
	      ptrIAPRecord->LoadL( *db );
	      iVal = ptrIAPRecord->RecordId();
		}
	else
	    {
		  User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy( ptrIAPRecord );
	CleanupStack::PopAndDestroy( db );
	return iVal;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::FetchSNAPIDL
// -----------------------------------------------------------------------------
//
TUint32 CSIPConcreteProfile::FetchSNAPIDL(const TDesC8& aValue)
	{
	TUint32 iVal(0);
	TBuf16<1024> recordname;
	recordname.SetLength(aValue.Length());
	CnvUtfConverter::ConvertToUnicodeFromUtf8(recordname,aValue);
	CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1);
	db->SetAttributeMask( ECDHidden );
	CCDAccessPointRecord* ptrAPRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(ptrAPRecord);
	ptrAPRecord->iRecordName.SetMaxLengthL(recordname.Length());
	ptrAPRecord->iRecordName = recordname;
	if(ptrAPRecord->FindL(*db))
		{
	      ptrAPRecord->LoadL( *db );
	      iVal = ptrAPRecord->RecordId();
	   	}
	else
	    {
		  User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy( ptrAPRecord );
	CleanupStack::PopAndDestroy( db );
	return iVal;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfile::ValidateProfileParamsL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPConcreteProfile::ValidateProfileParamsL() 
	{
	TBool res(ETrue);
	TUint32 iVal(0);
	const TDesC8* aSnapName(NULL);
	const TDesC8* aIAPName(NULL);
		
	if ( ExtensionParameter(KSIPSnapName,aSnapName) == KErrNone)
		{
			if(ExtensionParameter(KSIPSnapId,iVal) == KErrNone)
			{
				if (iVal != FetchSNAPIDL(*aSnapName))
					res=EFalse;
			}
			else
				res = ETrue;
		}
		
	if ( ExtensionParameter(KSIPAccessPointName,aIAPName) == KErrNone)
		{
			if (IapId() != FetchIAPIDL(*aIAPName))
			res=EFalse;
		}
	// Checking for the case of a Profile created without any Connection related Parameters.
	//In such cases the SNAP Id will be set to a Unique value which will be later used to retrieve the Default SNAP.
	if((ExtensionParameter(KSIPSnapId,iVal)!=KErrNone) && IapId() == 0)
		{
		PROFILE_DEBUG3("CSIPConcreteProfile::ValidateProfileParamsL.. Setting SNAP ID with 25000 as IAP is: ", 0)
		PROFILE_DEBUG3("Exceptional SNAP Entry verifier :", KDefaultSNAPIdentifier)
		SetExtensionParameterL(KSIPSnapId,KDefaultSNAPIdentifier);
		}
	
if ( ExtensionParameter(KBearerType,iVal) == KErrNotFound)
	    { 
PROFILE_DEBUG3("CSIPConcreteProfile::ValidateProfileParamsL.. Setting bearer Id to 0", 0)
        TUint32 defaultId=0;
        SetExtensionParameterL(KBearerType,defaultId);
	    }
	return res;
	}
