/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class providing profile data of SIP service provider
*
*/


#ifndef CSIPPROFILE_H
#define CSIPPROFILE_H

//  INCLUDES
#include "sipregistrationcontext.h"
#include "sipprofiletypeinfo.h"
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CSIPConcreteProfile;
class CSIPProfileRegistry;
class CSIPProfileRegistryBase;

const TUint32 KSIPProfileId = 1;
const TUint32 KSIPProviderName = 2;
const TUint32 KSIPAccessPointId = 3;
const TUint32 KSIPSigComp = 4;
const TUint32 KSIPSecurityNegotiation = 5;
const TUint32 KSIPAutoRegistration = 6;
const TUint32 KSIPUserAor = 7;
const TUint32 KSIPProfileRegistered = 8;
const TUint32 KSIPRegisteredAors = 9;
const TUint32 KSIPNegotiatedSecurityMechanism = 10;
const TUint32 KSIPDefaultProfile = 11;
const TUint32 KSIPContactHeaderParams = 12;
const TUint32 KSIPRegisteredContact = 13;
const TUint32 KSIPSnapId = 14;
const TUint32 KSIPContactHeaderUser = 15;
const TUint32 KSIPSoIpTOS = 16;
const TUint32 KSIPRegistrar = 130;
const TUint32 KSIPOutboundProxy = 131;
const TUint32 KSIPDigestUserName = 150;
const TUint32 KSIPDigestRealm = 151;
const TUint32 KSIPServerAddress = 152;

class CSIPProfile: public CBase, public MSIPRegistrationContext
    {    
    public:  // Constructors and destructor
        ~CSIPProfile();

    public: // New functions
        const TSIPProfileTypeInfo& Type() const;
        TInt GetParameter(TUint32 aParam, TDesC8 const *& aVal) const; 
        TInt GetParameter(TUint32 aParam, TUint32& aVal) const;
        TInt GetParameter(TUint32 aParam, TBool& aVal) const;
        TInt GetParameter(TUint32 aParam, MDesC8Array const *& aVal) const;
		TInt GetParameter(TUint32 aServerType, TUint32 aParam, TDesC8 const *& aVal) const;

    public: //Functions from base class
        TBool IsContextActive() const;
        TUint32 ContextId() const;
        
    public: //New functions
        static CSIPProfile* NewL(CSIPProfileRegistry* aSIPRegistry);
		static CSIPProfile* NewLC(CSIPProfileRegistry* aSIPRegistry);
        void SetEnabled(TBool aEnabled);
        void SetConcreteProfile(CSIPConcreteProfile* aProfile);
        void ClearConcreteProfile();
        CSIPConcreteProfile& ConcreteProfile();
        const CSIPConcreteProfile& ConcreteProfile() const;
        TBool IsEnabled() const;
        TInt LastRegistrationError() const;
        void SetRegistry(CSIPProfileRegistryBase* aRegistry);
        void ClearRegistry();
        
    protected:
        CSIPProfile(CSIPProfileRegistryBase* aRegistry);
        void ConstructL();

    protected:
        CSIPConcreteProfile* iSIPProfile;
        CSIPProfileRegistryBase* iSIPProfileRegistry;

	public:
		TBuf8<25> iRegistrarPassword;
		TBuf8<25> iProxyPassword;

	protected:
		TBuf8<25> iProviderNameValue;
		TBuf8<25> iUserAorValue;
		TUint iAccessPointIdValue;
		TUint iSnapIdValue;
		TBool iSigCompValue;
		TBool iAutoRegistrationValue;
		TBool iSecurityNegotiationValue;
		TBool iAllowAutoRegistrationOnlyOnHomeNetworkValue;

    private:
    
        TBool iEnabled;
    };

#endif // CSIPPROFILE_H
