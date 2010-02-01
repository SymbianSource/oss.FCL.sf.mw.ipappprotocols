// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : SigCompCompartmentStatesHolder.cpp
// Part of     : SigComp
// SigComp API frontend
// Version     : 1.0
//



// INCLUDE FILES
#include "Sigcomp.h"
#include "StateMgr.h"
#include "SigCompCompartmentStatesHolder.h"


// ============================ MEMBER FUNCTIONS ==============================

CSigCompCompartmentStatesHolder::CSigCompCompartmentStatesHolder(
                        CStateMgr& aStateMgr,
                        CSigComp::TMemorySize aStateMemorySize)
    {

    iStateMgr = &aStateMgr;
    iStateMemorySize = aStateMemorySize;
    iStateMemoryUsed = 0;
    }

// Destructor
EXPORT_C CSigCompCompartmentStatesHolder::~CSigCompCompartmentStatesHolder()
    {

    TInt csiNum = iCompartmentStateItems.Count();
    for (TInt i = 0; i < csiNum; i++)
        {
        TStateItem* si = iCompartmentStateItems[i].iStateItem;
        iStateMgr->ReclaimStateItem(si);
        }

    iCompartmentStateItems.Close();

    iStateMgr = NULL;
    }


// ----------------------------------------------------------------------------
// CSigCompCompartmentStatesHolder::AddStateItemL
// add new state to compartment
// ----------------------------------------------------------------------------
//

TBool CSigCompCompartmentStatesHolder::AddStateItemL(TStateItem* aStateItem,
                                         TUint16 aStateRetentionPriority)
    {

    TInt csiNum = iCompartmentStateItems.Count();

    for (TInt i = 0; i < csiNum; i++)
        {
        if (iCompartmentStateItems[i].iStateItem == aStateItem)
            {
            return EFalse;
            }
        }

    while ((iStateMemoryUsed+aStateItem->iStateLength+64) >
          static_cast<TUint>(iStateMemorySize))
        {
        TInt j = -1;
        csiNum = iCompartmentStateItems.Count();
        for (TInt i = 0; i < csiNum; i++)
            {
            if (j < 0)
                {
                j = i;
                }
            else
                {
                TUint srp1 = iCompartmentStateItems[i].iStateRetentionPriority;
                TUint srp2 = iCompartmentStateItems[j].iStateRetentionPriority;
                if (srp1 < srp2)
                    {
                    j = i;
                    }
                }
            }

        RemoveStateItemL(j);
        }

    TCompartmentStateItem csi;
    csi.iStateItem = aStateItem;
    csi.iStateRetentionPriority = aStateRetentionPriority;

    User::LeaveIfError(iCompartmentStateItems.Append(csi));
    iStateMemoryUsed += (aStateItem->iStateLength + 64);

    return ETrue;
    }


// ----------------------------------------------------------------------------
// CSigCompCompartmentStatesHolder::RemoveStateItemL
// remove state from compartment
// ----------------------------------------------------------------------------
//

void CSigCompCompartmentStatesHolder::RemoveStateItemL(
                                                  TStateItem* aStateItem)
    {

    TInt csiNum = iCompartmentStateItems.Count();
    for (TInt i = 0; i < csiNum; i++)
        {
        if (iCompartmentStateItems[i].iStateItem == aStateItem)
            {
            RemoveStateItemL(i);
            }
        }
    }


void CSigCompCompartmentStatesHolder::RemoveStateItemL(TInt aStateItemIndex)
    {

    TStateItem* si = iCompartmentStateItems[aStateItemIndex].iStateItem;
    iStateMemoryUsed -= (si->iStateLength + 64);
    iStateMgr->ReclaimStateItem(si);
    iCompartmentStateItems.Remove(aStateItemIndex);
    }
