/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : StateMgr.h
* Part of     : SigComp / state manager
* Interface   : 
* State manager
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef STATEMGR_H
#define STATEMGR_H

//  INCLUDES
#include <e32base.h>
#include <hash.h>
 
#include "SigCompStateMgr.h"

// consts

// RFC3320: For the purpose of calculation, each state item is considered to
// cost (state_length + 64) bytes.
static const TInt KStateCostExtension = 64;

static const TUint KStateIdentifierMinLength = 6; // minimum length of state id
static const TUint KStateIdentifierMaxLength = 20; //maximum length of state id

class TStateItem;

// CLASS DECLARATION

/**
*  State item container for internal use only.
*  
*
*  @lib sigcomp
*/

class TStateItem
    {
    public:
        /** state usage counter */
        TUint iUsageCounter;

        /** RFC defined fields */
        TUint8 iStateIdentifier[KStateIdentifierMaxLength];
        TUint16 iStateLength;
        TUint16 iStateAddress;
        TUint16 iStateInstruction;
        TUint16 iMinimumAccessLength;
        TUint8 iStateValue[1];
    };


/**
*  State manager class.
*  
*
*  @lib sigcomp
*/

class CSigCompCompartment;

class CStateMgr : public CBase, public MSigCompStateMgr
    {

    public:  // Constructors and destructor

        static CStateMgr* NewL();
        static CStateMgr* NewLC();

        /**
         * Destructor.
         */
        virtual ~CStateMgr();


    public: // new functions

       /**
        * allocate TStateItem object and add to state items list
        * 
        * @param aStateLength length of the state value to be created.
        * 
        * @returns just created TStateItem
        */
        TStateItem* CreateStateItemL(TInt aStateLength);

       /**
        * reclaim state item from list and remove if not used
        * 
        * @param aStateItem state item to be reclaimed.
        * 
        */
        void ReclaimStateItem(TStateItem* aStateItem);

       /**
        * Find state item of a given hash.
        * 
        * @param aPartialIdentifier hash ID of the state item we are looking
        *                           for.
        * 
        * @returns state item, or null if no state item was found
        */
        TStateItem* FindStateItem(const TDesC8& aPartialIdentifier);

       /**
        * create state
        * 
        * @param aCompartment compartment this state is associated with.
        *                     NULL means created state will be localy
        *                     available state.
        * @param aStateLength length of the state value
        * @param aStateAddress state address
        * @param aStateInstruction state instruction
        * @param aMinimumAccessLength minimum access length
        * @param aStateValue value of the state
        * @param aStateRetentionPriority retention priority of the state
        * 
        * @returns state item
        */
        TStateItem* CreateStateL(CSigCompCompartment* aCompartment,
                   TUint16 aStateLength, TUint16 aStateAddress,
                   TUint16 aStateInstruction, TUint16 aMinimumAccessLength,
                   const TUint8* aStateValue, TUint16 aStateRetentionPriority);

       /**
        * Free state of a given hash, in the way described in RFC 3220,
        * chapter 6.2
        * 
        * @param aCompartment compartment
        * @param aPartialIdentifier hash ID of the state we want to free.
        * 
        * @returns KErrNone if everything went ok, KErrNotFound if state
        *                   was not found.
        */
        TInt FreeStateL(CSigCompCompartment* aCompartment,
                        const TDesC8& aPartialIdentifier);

        /**
         * set returned feedback in compartment
         *
         * @param aCompartment compartment in which feedback will be saved
         * @param aReturnedFeedback the feedback itself.
         */
        void SetReturnedFeedbackL(CSigCompCompartment& aCompartment,
                                  const TDesC8& aReturnedFeedback) const;
        /**
         * set requested feedback in compartment
         *
         * @param aCompartment compartment in which feedback will be saved
         * @param aRequestedFeedback the feedback itself.
         */
        void SetRequestedFeedbackL(CSigCompCompartment& aCompartment,
                                   const TDesC8& aRequestedFeedback) const;
        /**
         * set returned parameters in compartment
         *
         * @param aCompartment compartment in which parameters will be saved
         * @param aReturnedParameters returned parameters to be saved.
         */
        void SetReturnedParametersL(CSigCompCompartment& aCompartment,
                                    const TDesC8& aReturnedParameters) const;

    private:
        void ConstructL();

        CStateMgr();

        /**
         * Align given value to 4.
         *
         * @param aValue value to be aligned.
         *
         * @returns aligned value
         */
        TInt Align4(TInt aValue) const;

    private:
        /** array containing all state items */
        RPointerArray<TStateItem> iStateItems;

        /** SHA-1 instance */
        CSHA1* iSHA1;

    };

#endif
            
// End of File
