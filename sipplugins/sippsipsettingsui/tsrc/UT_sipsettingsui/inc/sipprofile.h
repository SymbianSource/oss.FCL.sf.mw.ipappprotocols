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
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>
#include "sipregistrationcontext.h"
#include "sipprofiletypeinfo.h"

// FORWARD DECLARATIONS
class CSIPConcreteProfile;
class CSIPProfileRegistry;
class CSIPProfileRegistryBase;

/** 
* SIP Profile ID which value is type of TUint32
*/
const TUint32 KSIPProfileId = 1;

/** 
* SIP Provider Name which value is type of TDesC8
*/
const TUint32 KSIPProviderName = 2;

/** 
* Access Point ID which value is type of TUint32
*/
const TUint32 KSIPAccessPointId = 3;

/** 
* Signaling compression (SigComp) which value is type of TBool. 
* ETrue indicates that SigComp is enabled
*/
const TUint32 KSIPSigComp = 4;

/** 
* Security Negotiation which value is type of TBool. ETrue indicates that
* Security Negotiation is enabled
*/
const TUint32 KSIPSecurityNegotiation = 5;

/** 
* Auto Registration which value is type of TBool. ETrue indicates that 
* profile is enabled for auto registration
*/
const TUint32 KSIPAutoRegistration = 6;

/** 
* Defined user's address-of-record which value is type of TDesC8 
*/
const TUint32 KSIPUserAor = 7;

/** 
* Registration which value is type of TBool. ETrue indicates that
* profile is registered 
*/
const TUint32 KSIPProfileRegistered = 8;

/** 
* Uers's registered address-of-records which value is of type MDesC8Array.
* Note that array will contain AORs only in case the profile is registered.
*/
const TUint32 KSIPRegisteredAors = 9;

/** 
* Negotiated security mechanism during registration procedure which value
* is of type TDesC8.
* Note that descriptor will contain value only in case the profile is 
* registered.
*/
const TUint32 KSIPNegotiatedSecurityMechanism = 10;

/** 
* Default profile which value is type of TBool. 
* ETrue indicates that profile is default  
*/
const TUint32 KSIPDefaultProfile = 11;

/** 
* "Contact" header parameters used during registration which value
*is of type MDesC8Array. Array element contains one "Contact" header
* parameter.
*/
const TUint32 KSIPContactHeaderParams = 12;
const TUint32 KSIPRegisteredContact = 13;
const TUint32 KSIPSnapId = 14;
const TUint32 KSIPContactHeaderUser = 15;
const TUint32 KSIPSoIpTOS = 16;

const TUint32 KSIPRegistrar = 130;

/** 
* SIP Outbound Proxy
*/
const TUint32 KSIPOutboundProxy = 131;

/** 
* HTTP Digest user name which value is type of TDesC8 
*/
const TUint32 KSIPDigestUserName = 150;

/** 
* HTTP Digest realm which value is type of TDesC8
*/
const TUint32 KSIPDigestRealm = 151;

/** 
* SIP server's address which value is of type TDesC8
*/
const TUint32 KSIPServerAddress = 152;

// CLASS DECLARATION
/**
* @publishedAll
* @released
*
* Class contains profile information of particular SIP service
* provider. Class provides functions for quering profile parameter
* values.
*  @lib sipprofilecli.lib
*/
class CSIPProfile: public CBase,public MSIPRegistrationContext
    {    
    public:  // Constructors and destructor
        /**
        * Destructor.
        * @capability NetworkServices
        */
        IMPORT_C ~CSIPProfile();

    public: // New functions
        /**
        * Gets profile type information
        * @return profile type information
        */
        IMPORT_C const TSIPProfileTypeInfo& Type() const;

        /** 
        * Gets profile parameter
        * @param aParam a parameter to get
        * @param aVal on return will contain parameter value
        * @return KErrNotFound if parameter was not found, KErrNone otherwise
        */
        IMPORT_C TInt GetParameter(TUint32 aParam, TDesC8 const *& aVal) const; 

        /** 
        * Gets profile parameter
        * @param aParam a parameter to get
        * @param aVal on return will contain parameter value
        * @return KErrNotFound if parameter was not found, KErrNone otherwise
        */
        IMPORT_C TInt GetParameter(TUint32 aParam, TUint32& aVal) const;

        /** 
        * Gets profile parameter
        * @param aParam a parameter to get
        * @param aVal on return will contain parameter value
        * @return KErrNotFound if parameter was not found, KErrNone otherwise
        */
        IMPORT_C TInt GetParameter(TUint32 aParam, TBool& aVal) const;

        /** 
        * Gets profile parameter
        * @param aParam a parameter to get
        * @param aVal on return will contain parameter value
        * @return KErrNotFound if parameter was not found, KErrNone otherwise
        */
        IMPORT_C TInt GetParameter(TUint32 aParam, MDesC8Array const *& aVal) const;

        /** 
        * Gets profile parameter that is defined for a particular SIP server
        * @param aServer a SIP server type
        * @param aParam a parameter to get
        * @param aVal on return will contain parameter value
        * @return KErrNotFound if parameter was not found, KErrNone otherwise
        */
        IMPORT_C TInt GetParameter(TUint32 aServerType, TUint32 aParam, TDesC8 const *& aVal) const;

    public: //Functions from base class
        /**
        * Tests if the registration context can be
        * used for creating SIP messages/dialogs
        * Profile can be used, when CSIPProfileRegistry::IsEnabled() == ETrue
        * and Status() == ERegistered.
        * @return ETrue if can be used, EFalse otherwise
        */
        TBool IsContextActive() const;

        /**
        * 
        * For internal use only
        * @return context id
        */
        TUint32 ContextId() const;
        

    public: //New functions

        /**
        * Two-phased constructor.
        * @param aSIPRegistry a SIP profile client providing connection to SIP 
        *         profile server.
        */

        static CSIPProfile* NewL(CSIPProfileRegistry* aSIPRegistry);

        /**
        * Two-phased constructor.
        * @param aSIPRegistry a SIP profile client providing connection to SIP 
        *         profile server.
        * Constructs an object and adds the pointer to the cleanup stack;
        */

        static CSIPProfile* NewLC(CSIPProfileRegistry* aSIPRegistry);

        /**
        * Sets profiles enabled state
        * @param aEnabled ETrue if enabled
        */

        void SetEnabled(TBool aEnabled);

        /**
        * Sets concrete profile that holds actual data 
        * @param aProfile profile instance containing data
        */

        void SetConcreteProfile(CSIPConcreteProfile* aProfile);

        /**
        * Clears concrete profile that holds actual data 
        */

        void ClearConcreteProfile();

        /**
        * Gets concrete profile that holds actual data
        * @return concrete profile holding data
        */

        CSIPConcreteProfile& ConcreteProfile();

        /**
        * Gets concrete profile that holds actual data
        * @return concrete profile holding data
        */

        const CSIPConcreteProfile& ConcreteProfile() const;

        /**
        * Checks if the profile is enabled
        * @return ETrue if enabled, EFalse otherwise
        */

        TBool IsEnabled() const;

        /**
        * Gets last occurred error during registration
        * @return last occurred error code, KErrNone if no error
        */
        TInt LastRegistrationError() const;

        /**
        * Checks if two profiles are equal
        * @return ETrue if equal
        */
        TBool operator==(const CSIPProfile& 
                                 aProfile) const;

        /**
        * sets pointer to registry for profile
        */
        void SetRegistry(CSIPProfileRegistryBase* aRegistry);
        
        /**
        * Clears pointer to registry from profile
        */
        void ClearRegistry();
        
    protected:

        /**
        * Constructor. 
        */                
        CSIPProfile(CSIPProfileRegistryBase* aRegistry);
        
        /**
        * 2nd phase constructor. 
        */                
        void ConstructL();

    protected:
    
        CSIPConcreteProfile* iSIPProfile;
        CSIPProfileRegistryBase* iSIPProfileRegistry;

    // Stub data

    public:
		TBuf8<25> iRegistrarPassword;
		TBuf8<25> iProxyPassword;
		TBuf8<25> iProviderNameValue;
		TBuf8<25> iUserAorValue;
		TUint iAccessPointIdValue;
		TUint iSnapIdValue;
		TBool iSigCompValue;
		TBool iAutoRegistrationValue;
		TBool iSecurityNegotiationValue;
		TBool iAllowAutoRegistrationOnlyOnHomeNetworkValue;
    
        TBool iEnabled;
        TBool iIsDefaultProfile;
        
        TSIPProfileTypeInfo iTypeInfo;
        
        // Value iTUint32Value is set to aVal when calling
        // GetParameter(TUint32 aParam, TUint32& aVal)
        // if iTUint32ValueError == KErrNone. 
        // iTUint32ValueError is returned in any case
        TUint32 iTUint32Value;
        TInt iTUint32ValueError;
        
        // Value iTBoolValue is set to aVal when calling
        // GetParameter(TUint32 aParam, TBool& aVal)
        // if iTBoolValueError == KErrNone. 
        // iTBoolValueError is returned in any case
        TBool iTBoolValue;
        TInt iTBoolValueError;
        
        CDesC8Array* iArray;
        
        //
    };

#endif // CSIPPROFILE_H
