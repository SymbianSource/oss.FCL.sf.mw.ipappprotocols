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
* Name        : sipprofileagent.h
* Part of     : SIP / SIP Profile Agent
* Version     : %version: 2.1.1 %
*
*/



#ifndef CSIPPROFILEAGENT_H
#define CSIPPROFILEAGENT_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include "sipprofileagentinitparams.h"
#include "sipprofiletypeinfo.h"
#include "sipconcreteprofile.h"

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* This an abstract class that must be implemented by
* the actual profile ECom plug-in of the particular type.
* Class supplies instantiation & destruction by using
* the ECom framework and SIP profile related functional services.
*
* @lib N/A
*/
class CSIPProfileAgent : public CBase
    {
    public: // Constructors and destructor    

        /** 
        * Creates an implementation for the given profile type
        * @param aProfileType a profile type
        * @param aSIPProfileAgentInitParams
        * @returns implementation
        */
        static CSIPProfileAgent* NewL(
            const TDesC8& aProfileType,                      
            TSIPProfileAgentInitParams& aSIPProfileAgentInitParams);

        /**
        * Destructor
        */
        virtual ~CSIPProfileAgent();
        
    public: // New functions

        /**
        * Returns all implementations conformant to this interface
        * @param aImpInfoArray on return will contain the list of the implementations
        */
        static void ListAllImplementationsL(
            RImplInfoPtrArray& aImplInfoArray);

        /**
        * Gets profile agent type
        * @return profile agent type
        */
        virtual const TSIPProfileTypeInfo& Type() const = 0;

        /**
        * Creates new profile prefilled with default values.
        * Ownership is transferred.
        * @return new instance of CSIPConcreteProfile
        */
        virtual CSIPConcreteProfile* CreateL() = 0;

        /**
        * Registers SIP profile and maintains the registration. 
        * Takes actions needed for the registration
        * according to the profile type.
        * @param aSIPProfile SIP profile to register
        */
        virtual void RegisterL(
            CSIPConcreteProfile& aSIPProfile) = 0;

        /**
        * Updates SIP profile registration.
        * @param aNewProfile SIP profile to update
        * @param aOldProfile old SIP profile
        */
        virtual void UpdateL(
            CSIPConcreteProfile& aNewProfile, 
            CSIPConcreteProfile& aOldProfile) = 0;

        /**
        * Deregisters SIP profile according to the profile type.
        * @param aSIPProfile SIP profile to deregister
        */
        virtual void DeregisterL(
            CSIPConcreteProfile& aSIPProfile) = 0;

        /**
        * Gets the profile state
        * @param aState a returned state of the profile 
        * @param aSIPProfile a SIP profile 
        * @return KErrNotFound if not found, otherwise KErrNone
        */
        virtual TInt GetProfileState(
            CSIPConcreteProfile::TStatus& aState,
            CSIPConcreteProfile& aSIPProfile) const = 0;

        /**
        * Checks if the plug-in is idle i.e. 
        * does not handle any registration and can be unloaded. 
        */
        virtual TBool IsIdle() = 0;

        /**
        * Checks whether a registration is pending for the profile.
        * In other words a REGISTER has been sent 
        * but a final response has not yet been received.
        * @param aSIPProfile SIP profile
        */        
        virtual TBool RegisterPending( 
            CSIPConcreteProfile& aSIPProfile) const = 0;

        /**
        * Terminates abnormally profile handling i.e.
        * cleans up the reserved resources for the profile and forgets it.
        * @param aSIPProfile a SIP profile
        * @return KErrNone if successful; 
        *         KErrNotFound if the profile is not found
        */
        virtual TInt TerminateHandling(
            CSIPConcreteProfile& aSIPProfile) = 0;
            
        /**
        * For future extensions to avoid source compatibility breaks
        * @param aOperationCode operation code
        * @param aParams parameters that depend on the operation code
        * @return KErrNone, if successful, KErrNotSupported, if not supported,
        *         otherwise a system wide error code
        */     
        virtual TInt Extension(
            TInt aOperationCode,
            TAny* aParams) = 0;      
                           
    protected:

        inline CSIPProfileAgent();

    private:

        /// Unique instance identifier key
        TUid iDtor_ID_Key;
    };
    
#include "sipprofileagent.inl"

#endif // CSIPPROFILEAGENT_H
