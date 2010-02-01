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
// Name        : StateMgr.cpp
// Part of     : SigComp / state manager
// State manager
// Version     : 1.0
//




// INCLUDE FILES
#include "StateMgr.h"
#include "sigcompcompartment.h"
#include "SigCompCompartmentStatesHolder.h"


// ============================ MEMBER FUNCTIONS ==============================

void CStateMgr::ConstructL()
    {
    iSHA1 = CSHA1::NewL();
    }

CStateMgr::CStateMgr()
    {
    }

CStateMgr* CStateMgr::NewLC()
    {
    CStateMgr* self= new (ELeave) CStateMgr();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CStateMgr* CStateMgr::NewL()
    {
    CStateMgr* self= NewLC();
    CleanupStack::Pop();
    return self;
    }

    
// Destructor
CStateMgr::~CStateMgr()
    {

    iStateItems.ResetAndDestroy();

    delete iSHA1;
    }

TInt CStateMgr::Align4(TInt aValue) const
    {
    // align value to boundary of 4
    return (((aValue) + 3) & (~3));
    }

// ----------------------------------------------------------------------------
// CStateMgr::CreateStateItemL
// allocate TStateItem object and add to state items list
// ----------------------------------------------------------------------------
//
TStateItem* CStateMgr::CreateStateItemL(TInt aStateLength)
    {
    TStateItem* si = reinterpret_cast<TStateItem*>
                                      (User::AllocL(Align4(sizeof(TStateItem) -
                                      1 + aStateLength)));
    CleanupStack::PushL(si);
    User::LeaveIfError(iStateItems.Append(si));
    CleanupStack::Pop();
    return si;
    }


// ----------------------------------------------------------------------------
// CStateMgr::ReclaimStateItem
// reclaim state item from list and remove if not used
// ----------------------------------------------------------------------------
//

void CStateMgr::ReclaimStateItem(TStateItem* aStateItem)
    {

    if (aStateItem)
        {
        if (aStateItem->iUsageCounter > 0)
            {
            aStateItem->iUsageCounter--;
            }

        if (aStateItem->iUsageCounter == 0)
            {
            TInt i = iStateItems.Find(aStateItem);
            if (i >= 0)
                {
                iStateItems.Remove(i);
                User::Free(aStateItem);
                }
            }
        }
    }


// ----------------------------------------------------------------------------
// CStateMgr::FindStateItem
// search state item on list
// ----------------------------------------------------------------------------
//

TStateItem* CStateMgr::FindStateItem(const TDesC8& aPartialIdentifier)
    {

    TInt piLen = aPartialIdentifier.Size();

    TStateItem* si = NULL;

    TInt siNum = iStateItems.Count();
    for (TInt i = 0; i < siNum; i++)
        {
        if (Mem::Compare(iStateItems[i]->iStateIdentifier, piLen,
                                        aPartialIdentifier.Ptr(), piLen) == 0)
            {
            if (si != NULL)
                {
                return NULL;
                }

            si = iStateItems[i];
            }
        }

    return si;
    }


// ----------------------------------------------------------------------------
// CStateMgr::CreateStateL
// create state
// ----------------------------------------------------------------------------
//

TStateItem* CStateMgr::CreateStateL(CSigCompCompartment* aCompartment,
                                    TUint16 aStateLength,
                                    TUint16 aStateAddress,
                                    TUint16 aStateInstruction,
                                    TUint16 aMinimumAccessLength,
                                    const TUint8* aStateValue,
                                    TUint16 aStateRetentionPriority)
    {

    if (aCompartment)
        {
        TUint sms = aCompartment->StateMemorySize();
        if (sms == CSigComp::E0)
            {
            // empty compartment
            return NULL;
            }

        if (sms < (static_cast<TUint>(aStateLength) + KStateCostExtension))
            {
            aStateLength = static_cast<TUint16>((sms - KStateCostExtension));
            }
        }

    TUint8 stateData[8];

    // extract values in big endian convention
    stateData[0] = static_cast<TUint8>((aStateLength >> 8) & 0xff);
    stateData[1] = static_cast<TUint8>(aStateLength & 0xff);
    stateData[2] = static_cast<TUint8>((aStateAddress >> 8) & 0xff);
    stateData[3] = static_cast<TUint8>(aStateAddress & 0xff);
    stateData[4] = static_cast<TUint8>((aStateInstruction >> 8) & 0xff);
    stateData[5] = static_cast<TUint8>(aStateInstruction & 0xff);
    stateData[6] = static_cast<TUint8>((aMinimumAccessLength >> 8) & 0xff);
    stateData[7] = static_cast<TUint8>(aMinimumAccessLength & 0xff);

    TPtrC8 data = TPtrC8(stateData, 8);
    TPtrC8 value = TPtrC8(aStateValue, aStateLength);

    iSHA1->Reset();
    iSHA1->Hash(data);
    TPtrC8 hash = iSHA1->Hash(value);

    const TUint8* stateIdentifier = hash.Ptr();

    TStateItem* si = FindStateItem(hash);
    if (si &&
         ((si->iStateLength != aStateLength) ||
            (si->iStateAddress != aStateAddress) ||
            (si->iStateInstruction != aStateInstruction) ||
            (si->iMinimumAccessLength != aMinimumAccessLength) ||
            (Mem::Compare(si->iStateValue,
                          si->iStateLength,
                          aStateValue,
                          aStateLength) != 0))) User::Leave(KErrInUse);
/* NOT COVERED !!! there would have to be 2 different data sets which
produce exactly the same SHA1 hash sum */

    if (si == NULL)
        {
        si = CreateStateItemL(aStateLength);
        Mem::Copy(si->iStateIdentifier, stateIdentifier,
                  KStateIdentifierMaxLength);
        si->iStateLength = aStateLength;
        si->iStateAddress = aStateAddress;
        si->iStateInstruction = aStateInstruction;
        si->iMinimumAccessLength = aMinimumAccessLength;
        Mem::Copy(si->iStateValue, aStateValue, aStateLength);
        si->iUsageCounter = 0;
        }

    if (aCompartment)
        {
        CSigCompCompartmentStatesHolder* sh =
                                  const_cast<CSigCompCompartmentStatesHolder*>(
                                                 aCompartment->StatesHolder());

        if (sh->AddStateItemL(si,
                              aStateRetentionPriority))
            {
            si->iUsageCounter++;
            }
        }
    else
        {
        si->iUsageCounter++;
        }

    return si;
    }


// ----------------------------------------------------------------------------
// CStateMgr::FreeStateL
// free state
// ----------------------------------------------------------------------------
//

TInt CStateMgr::FreeStateL(CSigCompCompartment* aCompartment,
                           const TDesC8& aPartialIdentifier)
    {

    TStateItem* si = FindStateItem(aPartialIdentifier);
    if (si)
        {
        if (aCompartment)
            {
            CSigCompCompartmentStatesHolder* sh =
                                  const_cast<CSigCompCompartmentStatesHolder*>(
                                                 aCompartment->StatesHolder());

            sh->RemoveStateItemL(si);
            return KErrNone;
            }
        }

    return KErrNotFound;
    }


// ----------------------------------------------------------------------------
// CStateMgr::SetReturnedFeedbackL
// set returned feedback in compartment
// ----------------------------------------------------------------------------
//

void CStateMgr::SetReturnedFeedbackL(CSigCompCompartment& aCompartment,
                                     const TDesC8& aReturnedFeedback) const
    {
    aCompartment.SetReturnedFeedbackL(aReturnedFeedback);
    }


// ----------------------------------------------------------------------------
// CStateMgr::SetRequestedFeedbackL
// set requested feedback in compartment
// ----------------------------------------------------------------------------
//

void CStateMgr::SetRequestedFeedbackL(CSigCompCompartment& aCompartment,
                                      const TDesC8& aRequestedFeedback) const
    {
    aCompartment.SetRequestedFeedbackL(aRequestedFeedback);
    }


// ----------------------------------------------------------------------------
// CStateMgr::SetReturnedParametersL
// set returned parameters in compartment
// ----------------------------------------------------------------------------
//

void CStateMgr::SetReturnedParametersL(CSigCompCompartment& aCompartment,
                                       const TDesC8& aReturnedParameters) const
    {
    aCompartment.SetReturnedParametersL(aReturnedParameters);
    }
