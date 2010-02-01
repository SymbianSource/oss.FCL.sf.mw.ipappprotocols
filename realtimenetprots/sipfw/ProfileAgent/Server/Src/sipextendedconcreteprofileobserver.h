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
* Name          : sipextendedconcreteprofileobserver.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPEXTENDEDCONCRETEPROFILEOBSERVER_H
#define MSIPEXTENDEDCONCRETEPROFILEOBSERVER_H

// INCLUDES
#include "sipconcreteprofileobserver.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPProfileCacheItem;

// CLASS DECLARATION
/**
* This class extends the MSIPConcreteProfileObserver by adding callbacks related
* to ALR, in order to pass information about availability and migration of
* various IAPs of a SNAP id.
*
* @lib sipsrofilesrv.exe
*/
class MSIPExtendedConcreteProfileObserver : public MSIPConcreteProfileObserver
    {
	public: // From MSIPConcreteProfileObserver

		virtual void RegistrationStatusChangedL(
            TUint32 aProfileId, 
            TInt aStatus,
            TUint32 aStatusId) = 0;

        virtual void AddedL(TUint32 aProfileId) = 0;

        virtual void RemovedL(TUint32 aProfileId) = 0;

        virtual void UpdatedL(TUint32 aProfileId, TUint aSize) = 0;

        virtual void ErrorOccurredL(
            TUint32 aProfileId,
            TInt aStatus,
            TInt aError) = 0;

	public: // New pure virtual functions

		/**
		* An IAP has become available.
		* @param aProfileId Profile id
		* @param aSnapId SNAP id
		* @param aIapId IAP id
		* @return ETrue Must wait for Profile client to inform if migration to
		*         the IAP is allowed or not.
		*		  EFalse Migration is allowed by default, no need to wait for
		*		         this observer.
        */
		virtual TBool IapAvailableL(TUint32 aProfileId,
								    TUint32 aSnapId,
								    TUint32 aIapId) = 0;

		/**
		* Migration to an IAP has been initiated.
		* @param aProfileId Profile id
		* @param aSnapId SNAP id
		* @param aIapId IAP id
        */
		virtual void MigrationStartedL(TUint32 aProfileId,
								   	   TUint32 aSnapId,
								   	   TUint32 aIapId) = 0;

		/**
		* Migration to an IAP has been completed.
		* @param aProfileId Profile id
		* @param aSnapId SNAP id
		* @param aIapId IAP id
        */
		virtual void MigrationCompletedL(TUint32 aProfileId,
								   	     TUint32 aSnapId,
								   	     TUint32 aIapId) = 0;

		/**
		* ALR related error occurred.
		* @param aProfileId Profile id
		* @param aSnapId SNAP id
		* @param aIapId IAP id
        */
		virtual TInt AlrError(TInt aError,
							  TUint32 aProfileId,
							  TUint32 aSnapId,
							  TUint32 aIapId) = 0;
    };

#endif // MSIPEXTENDEDCONCRETEPROFILEOBSERVER_H
