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
* Name        : sipprofileagentobserver.h
* Part of     : SIP / SIP Profile Agent
* Version     : %version: 2.1.1 %
*
*/



#ifndef MSIPPROFILEAGENTOBSERVER_H
#define MSIPPROFILEAGENTOBSERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TSIPProfileTypeInfo;
class CSIPConcreteProfile;

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* An interface to be implemented by SIP Profile Agent
* Interface allows receiving events about profile
* registrations/deregistrations/updates and possible errors.
*
* @lib N/A
*/
class MSIPProfileAgentObserver
    {
    public:

        /**
        * Registration status event occurred.
        * @param aProfile the profile 
        * @param aContextId a registration context identifier
        *        equal to CSIPRegistration.ContextId()
        */
        virtual void SIPProfileStatusEvent(
            CSIPConcreteProfile& aProfile,
            TUint32 aContextId) = 0;
        /**
        * Asynchronous error has occured.
        * @param aProfile the profile
        * @param aError an error
        */
        virtual void SIPProfileErrorEvent(
            CSIPConcreteProfile& aProfile,
            TInt aError) = 0;

        /**
        * Checks whether registration should be continued for a profile.
        * For example if the profile has no users, 
        * there is no point in proceeding the registration. 
        * @param aProfileId the profile
        * @param aError latest error
        * @return ETrue, if it is necessary to continue registration; 
        *         otherwise EFalse.
        */
        virtual TBool ProceedRegistration(
            CSIPConcreteProfile& aProfile,
            TInt aError) = 0;
            
        /**
        * Get a list of profiles for which the registration has failed.
        * @param aType profile type to match
        * @param aFailedProfiles on return contains a list of 0..n profiles.
        *        The ownership of contained the profiles is not transferred.
        */
        virtual void GetFailedProfilesL(
            const TSIPProfileTypeInfo& aType,
            RPointerArray<CSIPConcreteProfile>& aFailedProfiles) const = 0;
    };

#endif // MSIPPROFILEAGENTOBSERVER_H
