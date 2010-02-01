/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipconcreteprofile.h
* Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
* Version     : %version: 2.1.3 %
*
*/



#ifndef CSIPCONCRETEPROFILE_H
#define CSIPCONCRETEPROFILE_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <s32strm.h>
#include <uri8.h>
#include <sipprofiletypeinfo.h>

// FORWARD DECLARATIONS
class CSIPProfileParameter;
class TExtensionIntParam;
class TExtensionBoolParam;
class CExtensionDescrParam;
class CExtensionDescrArrayParam;

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* A container for SIP profile data for one SIP profile. 
* Provides functions for setting and getting profile parameters and
* serializing the profile data to a write stream and 
* reading the data from a read stream.
*
* @lib sipprofile.lib
*/
class CSIPConcreteProfile : public CBase
    {
    public: // Enumerations

        /** Profile states */
        enum TStatus
            {
            /** Unregistered */
            EUnregistered = 1,
            /** Unregistration in progress*/
            EUnregistrationInProgress,
            /** Registration is in progress */
            ERegistrationInProgress,
            /** Registered */
            ERegistered
            };

    public: // Constructors and destructor 

        /**
        * Constructs an object of type CSIPConcreteProfile.
        * @return new instance
        */
        IMPORT_C static CSIPConcreteProfile* NewL();

        /**
        * Constructs an object of type CSIPConcreteProfile 
        * and adds a pointer to it to the cleanup stack.
        * @return new instance
        */
        IMPORT_C static CSIPConcreteProfile* NewLC();

        /**
        * Clones profile. All non-dynamic values except profile id are copied.
        * @return new instance, the ownership is transferred
        */
        IMPORT_C CSIPConcreteProfile* CloneL() const;

        /**
        * Clones profile. All values are copied.
        * @return new instance, the ownership is transferred
        */
		IMPORT_C CSIPConcreteProfile* CloneWithDynamicValuesL() const;

        /**
        * Destructor.
        */
        IMPORT_C ~CSIPConcreteProfile();

    public: // New functions

        /**
        * Sets the profile id
        * @param aProfileId profile id
        */
        IMPORT_C void SetId(TUint32 aProfileId);

        /**
        * Gets the profile Id
        * @return profile Id
        */
        IMPORT_C TUint32 Id() const;

        /*
        * Sets the profile type
        * @param aProfileType the type of the profile
        */
        IMPORT_C void SetProfileType(const TSIPProfileTypeInfo& aProfileType);

        /**
        * Gets profile type.
        * @return profile type
        */
        IMPORT_C const TSIPProfileTypeInfo& ProfileType() const;

        /**
        * Sets the provider's name
        * @param aProviderName provider's name
        */
        IMPORT_C void SetProviderNameL(const TDesC8& aProviderName);

        /**
        * Gets the provider's name
        * @return provider's name, KNullDesC8 if not defined
        */
        IMPORT_C const TDesC8& ProviderName() const;

        /**
        * Sets the IAP ID for the connection
        * @param aIapId a IAP ID
        */
        IMPORT_C void SetIapId(TUint32 aIapId);

        /**
        * Gets the IAP ID used by the profile
        * @return IAP ID
        */
        IMPORT_C TUint32 IapId() const;

        /**
        * Sets the AOR (public user identity) used for the profile
        * @aAOR AORs to bet set
        */
        IMPORT_C void SetAORL(const TDesC8& aAOR);

        /**
        * Gets the AOR (public user identity) used for the profile
        * @return the AOR
        */
        IMPORT_C const TDesC8& AOR() const;

        /**
        * Gets the AOR used for the profile parsed as a URI
        * @return the AOR as a URI
        */
        IMPORT_C const TUriC8& AORUri8() const;

        /**
        * Enables/disables the usage of SigComp
        * @param aOn ETrue enables SigComp for the profile,
        *        EFalse disables it
        */
        IMPORT_C void EnableSigComp(TBool aOn=ETrue);

        /**
        * Checks if the profile uses SigComp
        * @return ETrue if it uses, EFalse if not
        */
        IMPORT_C TBool IsSigCompEnabled() const;

        /**
        * Enables/disables the usage of security negotiation
        * @param aOn ETrue enables security negotiation for the
        *        profile, EFalse disables it.
        */
        IMPORT_C void EnableSecurityNegotiation(TBool aOn=ETrue);

        /**
        * Checks if the profile uses SIP security negotiation
        * @return ETrue if it uses, EFalse if not
        */
        IMPORT_C TBool IsSecurityNegotiationEnabled() const;

        /**
        * Enables/disables auto registration
        * @param aOn ETrue enables profile auto-registration,
        *        EFalse disables it.
        */
        IMPORT_C void SetAutoRegistrationEnabled(TBool aOn=ETrue);

        /**
        * Checks if the profile is marked for auto-registration
        * @return ETrue if marked, EFalse otherwise
        */
        IMPORT_C TBool IsAutoRegistrationEnabled() const;

        /**
        * Sets the server's URI
        * @param aServer the type of the server
        * @param aURI a server address as a URI
        */
        IMPORT_C void SetServerL(
            TUint32 aServer,
            const TDesC8& aURI);

        /**
        * Gets the server's URI
        * @param aServer the type of the server
        * @return server's URI, KNullDesC8 if not defined
        */
        IMPORT_C const TDesC8& Server(TUint32 aServer) const;

        /**
        * Sets a server's parameter
        * @param aServer the type of the server
        * @param aServerParam the type of the parameter
        * @param aValue parameter's value
        */
        IMPORT_C void SetServerParameterL(
            TUint32 aServer,
            TUint32 aServerParam,
            const TDesC8& aValue);

        /**
        * Gets a server's parameter
        * @param aServer the type of the server
        * @param aServerParam the type of the parameter
        * @return parameter's value, KNullDesC8 if not defined
        */
        IMPORT_C const TDesC8& ServerParameter(
            TUint32 aServer,
            TUint32 aServerParam) const;

        /**
        * Sets a server's extension parameter
        * @param aServer the type of the server
        * @param aName the parameter's name
        * @param aValue parameter's value
        */
        IMPORT_C void SetServerExtensionParameterL(
            TUint32 aServer,
            const TDesC8& aName,
            const TDesC8& aValue);

        /**
        * Gets a server's extension parameter
        * @param aServer the type of the server
        * @param aName the parameter's name
        * @return parameter's value, KNullDesC8 if not defined
        */
        IMPORT_C const TDesC8& ServerExtensionParameter(
            TUint32 aServer,
            const TDesC8& aName) const;

        /**
        * Sets Contact-header parameters for the profile.
        * Each array element contains one parameter.
        * @param aParams parameters to set
        */
        IMPORT_C void SetContactHeaderParamsL(const MDesC8Array& aParams);

        /**
        * Gets all the Contact-header params defined for the profile.
        * Each array element contains one parameter.
        * @return array of Contact-header parameters
        */
        IMPORT_C const MDesC8Array& ContactHeaderParams() const;

        /**
        * Sets extra SIP headers for the profile.
        * Each array element contains one header including 
        * the name and the value, for example "HeaderName: value".
        * @param aParams parameters to set
        */
        IMPORT_C void SetSIPHeadersL(const MDesC8Array& aHeaders);
        
        /**
        * Gets all extra SIP headers defined for the profile.
        * Each array element contains one header including 
        * the name and the value, for example "HeaderName: value".
        * @return array of params
        */
        IMPORT_C const MDesC8Array& SIPHeaders() const;

        /**
        * Sets the user's private identity
        * @param aPrivateIdentity user's private identity
        */
        IMPORT_C void SetPrivateIdentityL(
            const TDesC8& aPrivateIdentity);

        /**
        * Gets the user private identity
        * @return user's private identity, KNullDesC8 if not defined
        */
        IMPORT_C const TDesC8& PrivateIdentity() const;

        /**
        * Sets an extension parameter of type TUint32.
        * @param aID parameter identifier
        * @param aValue parameter value
        */
        IMPORT_C void SetExtensionParameterL(
            TUint32 aID,
            TUint32 aValue);

        /**
        * Gets an extension parameter of type TUint32.
        * @param aID parameter identifier
        * @param aParam OUT: parameter's value, if found.
        * @return KErrNone If parameter value was retrieved
        *          KErrNotFound If no such parameter exists
        */
        IMPORT_C TInt ExtensionParameter(
            TUint aID,
            TUint32& aValue) const;

        /**
        * Sets an extension parameter of type TBool.
        * @param aID parameter identifier
        * @param aValue parameter value
        */
        IMPORT_C void SetExtensionParameterL(
            TUint32 aID,
            TBool aValue);
        
        /**
        * Gets an extension parameter of type TBool.
        * @param aID parameter identifier
        * @param aParam OUT: parameter's value, if found.
        * @return KErrNone If parameter value was retrieved
        *          KErrNotFound If no such parameter exists
        */
        IMPORT_C TInt ExtensionParameter(
            TUint aID,
            TBool& aValue) const;

        /**
        * Sets an extension parameter that is of descriptor type.
        * Empty descriptor removes the parameter.
        * @param aID parameter identifier
        * @param aValue parameter value
        */             
        IMPORT_C void SetExtensionParameterL(
            TUint32 aID,
            const TDesC8& aValue);
        
        /**
        * Gets an extension parameter that is of descriptor type.
        * @param aID parameter identifier
        * @param aParam OUT: parameter's value, if found.
        * @return KErrNone If parameter value was retrieved
        *          KErrNotFound If no such parameter exists
        */
        IMPORT_C TInt ExtensionParameter(
            TUint aID,
            TDesC8 const *& aValue) const;

        /**
        * Sets an extension parameters that is of descriptor array type.
        * @param aID parameter identifier
        * @param aValue parameter value
        */             
        IMPORT_C void SetExtensionParameterL(
            TUint32 aID,
            const MDesC8Array& aValue);

        /**
        * Gets an extension parameter that is of descriptor array type.
        * @param aID parameter identifier
        * @param aParam OUT: parameter's value, if found.
        * @return KErrNone If parameter value was retrieved
        *          KErrNotFound If no such parameter exists
        */
        IMPORT_C TInt ExtensionParameter(
            TUint aID,
            MDesC8Array const *& aValue) const;

        /**
        * Deserializes the profile from the stream
        * @param aReadStream stream holding serialized profile
        * @param aAll ETrue, if all values deserialized, 
        *         EFalse, if only non-dynamic values
        * @return new instance, the ownership is transferred
        */
        IMPORT_C static CSIPConcreteProfile* InternalizeL(
            RReadStream& aReadStream, 
            TBool aAll = ETrue);

        /**
        * Gets the size of the serialized profile
        * @param aAll ETrue, if all values serialized, 
        *        EFalse, if only non-dynamic values
        * @return size needed to serialize profile
        */
        IMPORT_C TUint ExternalizedSizeL(TBool aAll = ETrue) const;

        /**
        * Serializes the profile to the stream
        * @param aWriteStream stream where profile is to be serialized
        * @param aAll ETrue, if all values serialized, 
        *        EFalse, if only non-dynamic values
        */
        IMPORT_C void ExternalizeL(
            RWriteStream& aWriteStream, 
            TBool aAll = ETrue) const;

        /**
        * Sets/resets the profile to be the default one.
        * @param aOn ETrue sets the profile as default,
        *        EFalse removes the setting
        */
        IMPORT_C void SetDefault(TBool aOn=ETrue);

        /**
        * Checks if the profile is the default one
        * @return ETrue if the profile is the default one, EFalse if not
        */
        IMPORT_C TBool IsDefault() const;

        /**
        * Sets the negotiated security mechanism in use
        * @param aMechanism negotiated security mechanism
        */
        IMPORT_C void SetNegotiatedSecurityMechanismL(const TDesC8& aMechanism);

        /**
        * Gets the negotiated security mechanism
        * @return negotiated security mechanism
        */
        IMPORT_C const TDesC8& NegotiatedSecurityMechanism() const;

        /**
        * Sets the dynamic proxy address in use.
        * Dynamic proxy address is discovered on run-time, 
        * for eaxmple using DHCP.
        * @param aDynamicProxy proxy address
        */
        IMPORT_C void SetDynamicProxyL(const TDesC8& aDynamicProxy);

        /**
        * Gets the dynamic proxy address in use.
        * Dynamic proxy address is discovered on run-time, 
        * for eaxmple using DHCP.        
        * @return proxy address in use
        */
        IMPORT_C const TDesC8& DynamicProxy() const;

        /**
        * Sets dynamically registered AORs for the profile
        * @param aAORs AORs to set
        */
        IMPORT_C void SetRegisteredAORsL(const MDesC8Array& aAORs);

        /**
        * Gets dynamically registered AORs defined for the profile
        * @return array of AORs
        */
        IMPORT_C const MDesC8Array& RegisteredAORs() const;

        /**
        * Deserialize registered AORs into profile from stream
        * @param aReadStream stream holding serialized profile
        */
        IMPORT_C void InternalizeRegisteredAORsL(RReadStream& aReadStream);

        /**
        * Serialize registered AORs from profile to stream
        * @param aWriteStream stream where profile is to be serialized
        */
        IMPORT_C void ExternalizeRegisteredAORsL(
            RWriteStream& aWriteStream) const;

        /**
        * Get size of serialized registered AORs from profile 
        * @return size needed to serialize profile
        */
        IMPORT_C TUint ExternalizedRegisteredAORsSizeL() const;

        /**
        * Sets the status of the profile
        * @param aStatus status of the profile
        */
        IMPORT_C void SetStatus(CSIPConcreteProfile::TStatus aStatus);

        /**
        * Gets the status of the profile
        * @return status of the profile
        */
        IMPORT_C CSIPConcreteProfile::TStatus Status() const;

        /**
        * Sets the internal registration context id of the profile.
        * In practice this is the value of MSIPRegistrationContext::ContextId()
        * for a CSIPRegistrationBinding related to the profile.
        * @param aId context id
        */
        IMPORT_C void SetContextId(TUint32 aId);

        /**
        * Gets the internal registration context id of the profile.
        * In practice this is the value of MSIPRegistrationContext::ContextId()
        * for a CSIPRegistrationBinding related to the profile.        
        * @return context id
        */
        IMPORT_C TUint32 ContextId() const;

        /**
        * Sets last occurred error during registration
        * @param aError occurred error code
        */
        IMPORT_C void SetLastRegistrationError(TInt aError);

        /**
        * Gets last occurred error during registration
        * @return last occurred error code, KErrNone if no error
        */
        IMPORT_C TInt LastRegistrationError() const;
        
        /**
        * Sets the profile enabled
        * @param aIsEnabled ETrue if profile is enabled
        */
        IMPORT_C void SetEnabled(TBool aIsEnabled);    
        
        /**
        * Checks whether the profile has been enabled by at least one user.
        * @return ETrue if the profile has been enabled by at least one user
        */
        IMPORT_C TBool IsEnabled() const;        
        
    public: // For internal use
    
        /**
        * Sets the storage id used when updating 
        * the profile to permanent storage
        * @param aId storage id
        */
        IMPORT_C void SetStorageId(TUint32 aId);

        /**
        * Gets the storage id used when updating 
        * the profile to permanent storage
        * @return storage id
        */
        IMPORT_C TUint32 StorageId() const;
        
        /**
        * Gets extension parameter count of type MDesC8Array.
        * @return parameter count
        */
		IMPORT_C TInt ExtensionDesArrayParameterCount() const;
		
        /**
        * Gets extension parameter count of type TDesC8.
        * @return parameter count
        */
		IMPORT_C TInt ExtensionDesParameterCount() const;
		
        /**
        * Gets extension parameter count of type TUint32.
        * @return parameter count
        */
        IMPORT_C TInt ExtensionIntParameterCount() const;

		/**
        * Gets the extension parameter
        * @param aIndex parameter's index in the extension array
        * @param aID OUT: parameter's identifier, if found
        * @param aValue OUT: parameter's value, if found
        * @return KErrNone If parameter value was retrieved
        *         KErrNotFound If no such parameter exists
        */
		IMPORT_C TInt ExtensionParameter(
			TInt aIndex,
			TUint& aID,
			TUint32& aValue) const;

        
        /**
        * Gets extension parameter count of type TBool.
        * @return parameter count
        */
        IMPORT_C TInt ExtensionBoolParameterCount() const;
		
		/**
        * Gets the extension parameter
        * @param aIndex parameter's index in the extension array
        * @param aID OUT: parameter's identifier, if found
        * @param aValue OUT: parameter's value, if found
        * @return KErrNone If parameter value was retrieved
        *         KErrNotFound If no such parameter exists
        */
		IMPORT_C TInt ExtensionParameter(
			TInt aIndex,
			TUint& aID,
			TBool& aValue ) const;

		/**
        * Gets the extension parameter
        * @param aIndex parameter's index in the extension array
        * @param aID OUT: parameter's identifier, if found
        * @param aValue OUT: parameter's value, if found
        * @return KErrNone If parameter value was retrieved
        *         KErrNotFound If no such parameter exists
        */
		IMPORT_C TInt ExtensionParameter(
			TInt aIndex,
			TUint& aID,
			MDesC8Array const *& aValue ) const;

		/**
        * Gets the extension parameter
        * @param aIndex parameter's index in the extension array
        * @param aID OUT: parameter's identifier, if found
        * @param aValue OUT: parameter's value, if found
        * @return KErrNone If parameter value was retrieved
        *         KErrNotFound If no such parameter exists
        */
		IMPORT_C TInt ExtensionParameter(
			TInt aIndex,
			TUint& aID,
			TDesC8 const *& aValue ) const;
        
		/**
        * Gets the server extension parameter
		* @param aServer the type of the server
        * @param aIndex parameter's index in the extension array
        * @param aKey OUT: parameter's identifier, if found
        * @param aValue OUT: parameter's value, if found
        * @return KErrNone If parameter value was retrieved
        *         KErrNotFound If no such parameter exists
        */
		IMPORT_C TInt ServerExtensionParameter(
			TUint32 aServer,
			TInt aIndex,
			TDesC8 const *& aKey,
			TDesC8 const *& aValue) const;

		/**
        * Gets server extension parameter count
        * @param aServer the type of the server
        * @return parameter count
        */
		IMPORT_C TInt ServerExtensionParameterCount( TUint32 aServer) const;
		
		/**
        * Gets the server parameter
		* @param aServer the type of the server
        * @param aIndex parameter's index in the extension array
        * @param aID OUT: parameter's identifier, if found
        * @param aValue OUT: parameter's value, if found
        * @return KErrNone If parameter value was retrieved
        *         KErrNotFound If no such parameter exists
        */
		IMPORT_C TInt ServerParameter(
			TUint32 aServer, 
			TInt aIndex,
			TUint& aID,
			TDesC8 const *& aValue) const;

        /**
        * Gets server parameter count
        * @param aServer the type of the server
        * @return server parameter count
        */
        IMPORT_C TInt ServerParameterCount(TUint32 aServer) const;
        
        /**
        * Validate the profile Parameters on being consistent
        * @return ETrue if the profile parameters are consistent.
        */
        IMPORT_C TBool ValidateProfileParamsL();

    public: // For internal use 

        TBool operator==(const CSIPConcreteProfile& aProfile);

        TInt32 LastCompareResult() const;

        static CDesC8ArrayFlat* CopyDesArrayL(const MDesC8Array& aArray);

        static void ExternalizeDesArrayL(const MDesC8Array& aArray, 
                                           RWriteStream& aWriteStream);

        static CDesC8ArrayFlat* InternalizeDesArrayL(RReadStream& aReadStream);
    
        enum TComparison
            {
            EId = 1,
            EName = 2,
            EIAPId = 4,
            ERegistrar = 8,
            ERegistrarParameters = 16,
            ERegistrarExtensionParameters = 32,
            EOutboundProxy = 64,
            EOutboundProxyParameters = 128,
            EOutboundProxyExtensionParameters = 256,
            EPrivateIdentity = 512,
            EAOR = 1024,
            ESigComp = 2048,
            ESecurityNegotiation = 4096,
            EAutoRegistration = 8192,
            ETypeInfo = 16384,
            EExtensionParameters = 32768,
            EProxyResolving = 65536,
            EContactHeaderParams = 131072,
            ESIPHeaders = 262144
            };    

    private: // Constructors

        CSIPConcreteProfile();
        void ConstructL();

    private: // New functions

        void DoInternalizeL(
            RReadStream& aReadStream, 
            TBool aAll = ETrue);

        const TDesC8& Parameter(
            const RPointerArray<CSIPProfileParameter>* aArray,
            const TDesC8& aName) const;
        
        void SetParameterL(
            RPointerArray<CSIPProfileParameter>* aArray,
            const TDesC8& aName,
            const TDesC8& aValue);

        TInt IndexOf(
            const TDesC8& aKey, 
            const RPointerArray<CSIPProfileParameter>* aArray) const;

        void SetOutboundProxyL(const TDesC8& aOutboundProxy);

        void SetRegistrarL(const TDesC8& aRegistrar);
        
        void InternalizeRegistrarL(RReadStream& aReadStream);
        
        void InternalizeProxyL(RReadStream& aReadStream);
        
        void InternalizeExtensionParamsL(RReadStream& aReadStream);

        void CloneExtensionParamsL(CSIPConcreteProfile& aNewObject) const;

        TInt ExtensionIntParamIndex(TUint32 aID) const;
        
        TInt ExtensionBoolParamIndex(TUint32 aID) const;
        
        TInt ExtensionDescrParamIndex(TUint32 aID) const;
        
        TInt ExtensionDescrArrayParamIndex(TUint32 aID) const;

        HBufC8* CreateContactUserNameLC() const;

        TUint8 GetNextBits(
            const TDesC8& aBuf,
            TInt aBits,
            TUint& aCounter) const;
            
        /**
        * Clones profile.
        * @param aWithDynamicValues Should all values be copied
        * @return new instance of the profile, the ownership is transferred
        */
		CSIPConcreteProfile* CloneL(TBool aWithDynamicValues) const;
		
		/**
		*Fetch the Snap ID*
		* @param aValue the SNAP Name
		* @return SNAP ID
		*/
				
		TUint32 FetchSNAPIDL(const TDesC8& aValue);
				
		/**
		*Fetch IAP ID*
		* @param aValue the IAP Name
		* @return IAP ID
		*/
				
		TUint32 FetchIAPIDL(const TDesC8& aValue);



    private: // Data

        // Basic data
        TSIPProfileTypeInfo    iSIPProfileType;
        TUint32    iSIPProfileId;
        TUint32    iStorageId;
        TUint32    iSIPIAPId;
        HBufC8*    iSIPProviderName;
        HBufC8*    iSIPPrivateIdentity;
        CUri8*    iSIPAORUri8;
        TBool iSIPSigComp;
        TBool iSecurityNegotiation;
        TBool iSIPAutoRegistered;        
        CDesC8ArrayFlat* iSIPContactHeaderParams;
        CDesC8ArrayFlat* iSIPHeaders;

        // Outbound proxy related parameters
        HBufC8*    iSIPOutboundProxy;
        RPointerArray<CSIPProfileParameter> iSIPOutboundProxyParameter;
        RPointerArray<CSIPProfileParameter> iSIPOutboundProxyExtParameter;
        
        // Registrar related parameters
        HBufC8*    iSIPRegistrar;
        RPointerArray<CSIPProfileParameter> iSIPRegistrarParameter;
        RPointerArray<CSIPProfileParameter> iSIPRegistrarExtParameter;

        // Extension parameters
        RArray<TExtensionIntParam> iExtensionIntParams;
        RArray<TExtensionBoolParam> iExtensionBoolParams;        
        RPointerArray<CExtensionDescrParam> iExtensionDescrParams;
        RPointerArray<CExtensionDescrArrayParam> iExtensionDescrArrayParams;
        
        // Dynamic data
        TBool iDefaultProfile;
        TBool iIsEnabled; 
        TStatus iStatus;
        TUint32    iContextId;
        TInt iLastRegistrationError;
        TInt32 iLastComparisonResult;
        TBool iDynamicProxyResolving;
        HBufC8*    iDynamicProxy;
        HBufC8*    iSIPNegotiatedSecurityMechanism;
        CDesC8ArrayFlat* iSIPRegisteredAORs;
        
    private: // For testing purposes
#ifdef CPPUNIT_TEST 
        friend class CSIPConcreteProfileTest;     
#endif
    };

#endif // CSIPCONCRETEPROFILE_H
