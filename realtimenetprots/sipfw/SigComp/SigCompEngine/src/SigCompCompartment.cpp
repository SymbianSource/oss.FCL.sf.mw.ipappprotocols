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
// Name        : SigCompCompartment.cpp
// Part of     : SigComp
// SigComp API frontend
// Version     : 1.0
//



// INCLUDE FILES
#include "Sigcomp.h"
#include "Compressor.h"
#include "sigcompstateitem.h"
#include "StateMgr.h"
#include "SigCompCompartmentStatesHolder.h"
#include "SigCompCompressionContext.h"
#include "sigcompcompartment.h"

// ============================ MEMBER FUNCTIONS ==============================

CSigCompCompartment::CSigCompCompartment()
    {
    }

void CSigCompCompartment::ConstructL(const CSigComp& aSigComp,
                                     TBool aDynamicCompression)
    {

    ConstructL(aSigComp, aSigComp.StateMemorySize(), aDynamicCompression);
    }

void CSigCompCompartment::ConstructL(const CSigComp& aSigComp,
                                     CSigComp::TMemorySize aStateMemorySize,
                                     TBool aDynamicCompression)
    {

    iSigComp = &aSigComp;
    iStateMemorySize = aStateMemorySize;
    iDynamicCompression = aDynamicCompression;

    if(aDynamicCompression)
        {
        CSigCompCompressor* ci = const_cast<CSigCompCompressor*>(
                                                       iSigComp->Compressor());
        if(!ci->IsDynamicCompressionSupported())
            {
            User::Leave(KErrNotSupported);
            }
        }

    iStateMgr = const_cast<CStateMgr*>(aSigComp.StateMgr());
    iStatesHolder = new (ELeave)CSigCompCompartmentStatesHolder(*iStateMgr,
                                                          iStateMemorySize);
    iReturnedFeedback = HBufC8::NewL(0);
    iRequestedFeedback = HBufC8::NewL(0);
    iReturnedParameters = HBufC8::NewL(0);
    }

EXPORT_C CSigCompCompartment* CSigCompCompartment::NewL(
                                                   const CSigComp& aSigComp,
                                                   TBool aDynamicCompression)
    {
    CSigCompCompartment* self = NewLC(aSigComp, aDynamicCompression);
    CleanupStack::Pop();

    return self;
    }

EXPORT_C CSigCompCompartment* CSigCompCompartment::NewL(
                                        const CSigComp& aSigComp,
                                        CSigComp::TMemorySize aStateMemorySize,
                                        TBool aDynamicCompression)
    {

    CSigCompCompartment* self = NewLC(aSigComp,
                                      aStateMemorySize,
                                      aDynamicCompression);
    CleanupStack::Pop();

    return self;
    }


EXPORT_C CSigCompCompartment* CSigCompCompartment::NewLC(
                                                   const CSigComp& aSigComp,
                                                   TBool aDynamicCompression)
    {
    CSigCompCompartment* self = new (ELeave)CSigCompCompartment;
    
    CleanupStack::PushL(self);
    self->ConstructL(aSigComp, aDynamicCompression);

    return self;
    }

EXPORT_C CSigCompCompartment* CSigCompCompartment::NewLC(
                                        const CSigComp& aSigComp,
                                        CSigComp::TMemorySize aStateMemorySize,
                                        TBool aDynamicCompression)
    {
    CSigCompCompartment* self = new (ELeave)CSigCompCompartment;
    
    CleanupStack::PushL(self);
    self->ConstructL(aSigComp, aStateMemorySize, aDynamicCompression);

    return self;
    }


// Destructor
EXPORT_C CSigCompCompartment::~CSigCompCompartment()
    {

    delete iCompressionContext;

    delete iStatesHolder;

    delete iReturnedFeedback;
    delete iRequestedFeedback;
    delete iReturnedParameters;
    }


// ----------------------------------------------------------------------------
// CSigCompCompartment::CompressL
// 
// ----------------------------------------------------------------------------
//

EXPORT_C CBufBase* CSigCompCompartment::CompressL(const TDesC8& aMessage,
                                                 TBool aForStreamBasedProtocol)
    {
    CSigCompCompressor* compressor = const_cast<CSigCompCompressor*>(
                                                       iSigComp->Compressor());

    CBufBase* msg = compressor->CompressMessageL(this, aMessage,
                                           aForStreamBasedProtocol);
    return msg;
    }


// ----------------------------------------------------------------------------
// CSigCompCompartment::SetReturnedFeedbackL
// set returned feedback in compartment
// ----------------------------------------------------------------------------
//

void CSigCompCompartment::SetReturnedFeedbackL(const TDesC8& aReturnedFeedback)
    {

    delete iReturnedFeedback;
    iReturnedFeedback = NULL;
    iReturnedFeedback = aReturnedFeedback.AllocL();
    }


// ----------------------------------------------------------------------------
// CSigCompCompartment::SetRequestedFeedbackL
// set requested feedback in compartment
// ----------------------------------------------------------------------------
//

void CSigCompCompartment::SetRequestedFeedbackL(
                                              const TDesC8& aRequestedFeedback)
    {

    delete iRequestedFeedback;
    iRequestedFeedback = NULL;
    iRequestedFeedback = aRequestedFeedback.AllocL();
    }


// ----------------------------------------------------------------------------
// CSigCompCompartment::SetReturnedParametersL
// set returned parameters in compartment
// ----------------------------------------------------------------------------
//

void CSigCompCompartment::SetReturnedParametersL(
                                            const TDesC8& aReturnedParameters)
    {

    delete iReturnedParameters;
    iReturnedParameters = NULL;
    iReturnedParameters = aReturnedParameters.AllocL();
    }

// ----------------------------------------------------------------------------
// CSigCompCompartment::StateMemorySize
// get state memory size
// ----------------------------------------------------------------------------
//

CSigComp::TMemorySize CSigCompCompartment::StateMemorySize() const
    {
    return iStateMemorySize;
    };


// ----------------------------------------------------------------------------
// CSigCompCompartment::ReturnedFeedback
// get returned feedback from compartment
// ----------------------------------------------------------------------------
//

#if defined(SIGCOMP_DEBUG)
EXPORT_C
#endif
const TDesC8& CSigCompCompartment::ReturnedFeedback() const
    {
    return *iReturnedFeedback;
    }


// ----------------------------------------------------------------------------
// CSigCompCompartment::RequestedFeedback
// set requested feedback in compartment
// ----------------------------------------------------------------------------
//

#if defined(SIGCOMP_DEBUG)
EXPORT_C
#endif
const TDesC8& CSigCompCompartment::RequestedFeedback() const
    {
    return *iRequestedFeedback;
    }


// ----------------------------------------------------------------------------
// CSigCompCompartment::ReturnedParameters
// get returned parameters from compartment
// ----------------------------------------------------------------------------
//

#if defined(SIGCOMP_DEBUG)
EXPORT_C
#endif
const TDesC8& CSigCompCompartment::ReturnedParameters() const
    {
    return *iReturnedParameters;
    }

// ----------------------------------------------------------------------------
// CSigCompCompartment::StatesHolder
// get states holder
// ----------------------------------------------------------------------------
//

const CSigCompCompartmentStatesHolder*
                                      CSigCompCompartment::StatesHolder() const
    {
    return iStatesHolder;
    }

const CSigCompCompressionContext*
                                CSigCompCompartment::CompressionContext() const
    {
    return iCompressionContext;
    }

void CSigCompCompartment::SetCompressionContext(
                          CSigCompCompressionContext* aContext)
    {
    iCompressionContext = aContext;
    }

TBool CSigCompCompartment::IsDynamicCompression() const
    {
    return iDynamicCompression;
    }
        
CSigComp::TCyclesPerBit CSigCompCompartment::CyclesPerBit() const
    {
    return iSigComp->CyclesPerBit();
    }


CSigComp::TMemorySize CSigCompCompartment::DecompressionMemorySize() const
    {
    return iSigComp->DecompressionMemorySize();
    }
