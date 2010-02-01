/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements MSIPProfileRegistryObserver interface.
*
*/


#ifndef CSIPPROFILEREGISTRYOBSERVER_H
#define CSIPPROFILEREGISTRYOBSERVER_H

//  INCLUDES
#include <sipprofileregistryobserver.h>

// CLASS DECLARATION

/**
 * Class implements MSIPProfileRegistryObserver interface. 
 * Observs changes in the SIP profile registry.
 * @lib wpsipadapter.lib
 * @since 2.0
 */
class CSIPProfileRegistryObserver : public CBase, 
                                    public MSIPProfileRegistryObserver
    {
    public:

        static CSIPProfileRegistryObserver* NewLC();
        static CSIPProfileRegistryObserver* NewL();
        virtual ~CSIPProfileRegistryObserver();

        /** 
         * An event related to SIP Profile has accorred
         * @param aProfileId a profile Id
         * @param aEvent an occurred event
         */
        void ProfileRegistryEventOccurred( TUint32 aProfileId, 
            TEvent aEvent );

        /**
         * An asynchronous error has occurred related to SIP profile
         * Event is send to those observers, who have the
         * corresponding profile instantiated.
         * @param aProfileId the id of failed profile 
         * @param aError an occurred error
         */
        void ProfileRegistryErrorOccurred( TUint32 aProfileId, TInt aError );

    private:
        CSIPProfileRegistryObserver(); 
        void ConstructL();

    private:    // Friend classes
        //friend class UT_CWPSIPAdapter; // For testing purpose

    };

#endif // CSIPPROFILEREGISTRYOBSERVER_H

// End of file.