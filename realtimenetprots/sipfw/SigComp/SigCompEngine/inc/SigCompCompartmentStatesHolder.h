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
* Name        : SigCompCompartmentStatesHolder.h
* Part of     : SigComp / compartment
* Interface   : 
* sigcomp compartment states holder
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIGCOMPCOMPARTMENTSTATESHOLDER_H
#define SIGCOMPCOMPARTMENTSTATESHOLDER_H

//  INCLUDES
#include <e32base.h>
 
// CLASS DECLARATION


/**
 * @brief Class for storage state item per compartment
 * 
 * This class is used for storage state item per compartment
 *
 */

class TStateItem;

class TCompartmentStateItem
    {
    public:
        /** compartment state item */
        TStateItem* iStateItem;
        /** state retention priority */
        TUint16 iStateRetentionPriority;
    };


/**
*  Compartment states holder.
*  
*
*  @lib sigcomp
*/

class CStateMgr;

class CSigCompCompartmentStatesHolder : public CBase
    {
    public:

        /**
         * Constructor.
         *
         * @param aStateMgr the instance of StateMgr
         *
         * @param aStateMemorySize number of bytes offered for a compartment
         *                         for the state creation.
         */

        CSigCompCompartmentStatesHolder(
                        CStateMgr& aStateMgr,
                        CSigComp::TMemorySize aStateMemorySize);

        /**
         * Destructor
         */

        IMPORT_C ~CSigCompCompartmentStatesHolder();


        /**
         * Add state item to compartment, invoked from state manager
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aStateItem a state item
         * @param aStateRetentionPriority a state retention priority
         * 
         * @returns ETrue if success, EFalse if state exist
         */

        TBool AddStateItemL(TStateItem* aStateItem,
                            TUint16 aStateRetentionPriority);


        /**
         * Remove state item from compartment and reclaim from state manager
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aStateItem a state item
         */

        void RemoveStateItemL(TStateItem* aStateItem);


        /**
         * Remove state item from compartment and reclaim from state manager
         * Internal use only
         * Can leave if system resourcess missing (no memory for egz.)
         *
         * @param aStateItem a state item index
         */

        void RemoveStateItemL(TInt aStateItemIndex);


    private:

        /** state manager */
        CStateMgr* iStateMgr;

        /** state memory size (in bytes) */
        TUint iStateMemorySize;

        /** current state memory usage (in bytes) */
        TUint iStateMemoryUsed;

        /** state items */
        RArray<TCompartmentStateItem> iCompartmentStateItems;

    };

#endif
            
// End of File
