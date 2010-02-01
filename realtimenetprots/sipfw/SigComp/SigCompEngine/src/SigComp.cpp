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
// Name        : SigComp.cpp
// Part of     : SigComp
// SigComp API frontend
// Version     : 1.0
//



// INCLUDE FILES
#include "Sigcomp.h"
#include "sigcompstateitem.h"
#include "Udvm.h"
#include "StateMgr.h"
#include "Compressor.h"
#include "MessageReader.h"

// ============================ MEMBER FUNCTIONS ==============================

CSigComp::CSigComp()
    {
    }

void CSigComp::ConstructL(TMemorySize aDecompressionMemSize,
                          TCyclesPerBit aCyclesPerBit,
                          TMemorySize aStateMemorySize,
                          TBool aUploadState)
    {

    if (aDecompressionMemSize == E0)
        {
        User::Leave(KErrArgument);
        }

    iMemSize = aDecompressionMemSize;
    iCyclesPerBit = aCyclesPerBit;
    iStateMemorySize = aStateMemorySize;
    iUploadState = aUploadState;

    iStateMgr = CStateMgr::NewL();
    }

void CSigComp::ConstructL(TMemorySize aDecompressionMemSize,
                          TCyclesPerBit aCyclesPerBit,
                          TMemorySize aStateMemorySize,
                          const TDesC8& aAlgorithm)
    {

    ConstructL(aDecompressionMemSize,
               aCyclesPerBit,
               aStateMemorySize,
               EFalse);

    iCompressor = CSigCompCompressor::NewL(aAlgorithm, iStateMgr);
    }

void CSigComp::ConstructL(TMemorySize aDecompressionMemSize,
                          TCyclesPerBit aCyclesPerBit,
                          TMemorySize aStateMemorySize,
                          const TDesC8& aAlgorithm,
                          const TSigCompStateItem& aStateItem,
                          TBool aUploadState)
    {
    ConstructL(aDecompressionMemSize,
               aCyclesPerBit,
               aStateMemorySize,
               aUploadState);

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

#if !defined(__WINS__) && !defined(__WINSCW__)
    TParsePtrC parser(RProcess().FileName());
    User::LeaveIfError(rfs.SetSessionPath(parser.Drive()));
#endif    

    TInt stateLen = 0;
    RFile file;
    User::LeaveIfError(file.Open(rfs, aStateItem.iStateValue, EFileRead));
    CleanupClosePushL(file);

    file.Size(stateLen);
    HBufC8* valueBuf = HBufC8::NewL(stateLen);
    TPtr8 valueBufDes = valueBuf->Des();
    file.Read(valueBufDes, stateLen);
    CleanupStack::PopAndDestroy(2); // file, rfs
    CleanupStack::PushL(valueBuf);

    TStateItem* stateItem = iStateMgr->CreateStateL(NULL,
                                         (TInt16)stateLen,
                                         aStateItem.iStateAddress,
                                         aStateItem.iStateInstruction,
                                         aStateItem.iMinimumAccessLength,
                                         valueBuf->Ptr(), 0xffff);

    if (aStateItem.iStateIdentifier.Size() > 0)
        {
        if (aStateItem.iStateIdentifier.Compare(TPtrC8(
                                            stateItem->iStateIdentifier, 20)))
            {
            User::Leave(KErrNotFound);
            }
        }

    iCompressor = CSigCompCompressor::NewL(aAlgorithm,
                                             iStateMgr,
                                             stateItem,
                                             aStateItem.iStateLength,
                                             aUploadState);
    CleanupStack::PopAndDestroy(valueBuf);
    }


EXPORT_C CSigComp* CSigComp::NewL(TMemorySize aDecompressionMemSize,
                                  TCyclesPerBit aCyclesPerBit,
                                  TMemorySize aStateMemorySize,
                                  const TDesC8& aAlgorithm)
    {

    CSigComp* self = NewLC(aDecompressionMemSize,
                           aCyclesPerBit,
                           aStateMemorySize,
                           aAlgorithm);
    CleanupStack::Pop();

    return self;
    }

EXPORT_C CSigComp* CSigComp::NewL(TMemorySize aDecompressionMemSize,
                                  TCyclesPerBit aCyclesPerBit,
                                  TMemorySize aStateMemorySize,
                                  const TDesC8& aAlgorithm,
                                  const TSigCompStateItem& aStateItem,
                                  TBool aUploadState)
    {

    CSigComp* self = NewLC(aDecompressionMemSize,
                           aCyclesPerBit,
                           aStateMemorySize,
                           aAlgorithm,
                           aStateItem,
                           aUploadState);
    CleanupStack::Pop();

    return self;
    }

EXPORT_C CSigComp* CSigComp::NewLC(TMemorySize aDecompressionMemSize,
                                   TCyclesPerBit aCyclesPerBit,
                                   TMemorySize aStateMemorySize,
                                   const TDesC8& aAlgorithm)
    {

    CSigComp* self = new (ELeave)CSigComp;
    
    CleanupStack::PushL(self);
    self->ConstructL(aDecompressionMemSize,
                     aCyclesPerBit,
                     aStateMemorySize,
                     aAlgorithm);

    return self;
    }

EXPORT_C CSigComp* CSigComp::NewLC(TMemorySize aDecompressionMemSize,
                                   TCyclesPerBit aCyclesPerBit,
                                   TMemorySize aStateMemorySize,
                                   const TDesC8& aAlgorithm,
                                   const TSigCompStateItem& aStateItem,
                                   TBool aUploadState)
    {

    CSigComp* self = new (ELeave)CSigComp;
    
    CleanupStack::PushL(self);
    self->ConstructL(aDecompressionMemSize,
                     aCyclesPerBit,
                     aStateMemorySize,
                     aAlgorithm,
                     aStateItem,
                     aUploadState);

    return self;
    }


// Destructor
EXPORT_C CSigComp::~CSigComp()
    {

    delete iCompressor;
    delete iStateMgr;
    delete iUdvm;
    }


// ----------------------------------------------------------------------------
// CSigComp::DecompressL
// Decompress message
// ----------------------------------------------------------------------------
//

EXPORT_C CBufBase* CSigComp::DecompressL(const TDesC8& aMessage,
                                         TUint& aBytesConsumed,
                                         TBool aStreamBasedProtocol)
    {
    delete iUdvm;
    iUdvm = NULL;
    iUdvm = CUdvm::NewL(iStateMgr,
                        iMemSize,
                        iCyclesPerBit);
    return iUdvm->DecompressL(aMessage, aBytesConsumed, aStreamBasedProtocol);
    }


// ----------------------------------------------------------------------------
// CSigComp::AllowL
// Allow state create/free
// ----------------------------------------------------------------------------
//

EXPORT_C void CSigComp::AllowL(CSigCompCompartment& aSigCompCompartment)
    {

    if (iUdvm)
        {
        iUdvm->AllowStateOperationsL(&aSigCompCompartment);
        delete iUdvm;
        iUdvm = NULL;
        }
    }


// ----------------------------------------------------------------------------
// CSigComp::Deny
// Deny states
// ----------------------------------------------------------------------------
//

EXPORT_C void CSigComp::Deny()
    {

    if (iUdvm)
        {
        delete iUdvm;
        iUdvm = NULL;
        }
    }


// ----------------------------------------------------------------------------
// CSigComp::IsSigCompMsg
// Is SigComp message?
// ----------------------------------------------------------------------------
//

EXPORT_C TBool CSigComp::IsSigCompMsg(const TDesC8& aMessage) const
    {

    TInt pos = 0;

    // skip delimiters

    TBool skip = ETrue;

    while (skip)
        {
        if (pos < aMessage.Length())
            {
            if (aMessage[pos] == 0xff)
                {
                if ((pos+1) < aMessage.Length())
                    {
                    if (aMessage[pos+1] == 0xff)
                        {
                        pos += 2;
                        }
                    else
                        {
                        skip = EFalse;  // no more delimiters
                        }
                    }
                else
                    {
                    return EFalse;  // message too short
                    }
                }
            else
                {
                skip = EFalse;  // no more delimiters
                }
            }
        else
            {
                return EFalse;  // message too short
            }
        }

    //

    TUint byte;

    byte = aMessage[pos];

    // if byte == 0xff then quoted bytes, but byte is still 0xff

    return ((byte & KSigCompHeaderMask) == KSigCompHeaderMask);
    }


// ----------------------------------------------------------------------------
// CSigComp::IsCompleteL
// Is stream based message complete?
// ----------------------------------------------------------------------------
//

EXPORT_C TBool CSigComp::IsCompleteL(const TDesC8& aMessage) const
    {

    if (aMessage.Length() > 1)
        {
        CMessageReader* msgReader = new (ELeave)CMessageReader(aMessage, 
                                                               ETrue);
        CleanupStack::PushL(msgReader);

        msgReader->SkipDelimiters();

        TUint byte;
        TInt retval;

        while ((retval = msgReader->ReadByte(byte)) == KErrNone)
            {
            }
        if (retval == KErrEof)
            {
            CleanupStack::PopAndDestroy(msgReader);
            return ETrue;
            }
        else
            {
            CleanupStack::PopAndDestroy(msgReader);
            return EFalse;
            }
        }

    return EFalse;
    }


// ----------------------------------------------------------------------------
// CSigComp::DecompressionMemorySize
// get DMS
// ----------------------------------------------------------------------------
//

CSigComp::TMemorySize CSigComp::DecompressionMemorySize() const
    {
    return static_cast<CSigComp::TMemorySize>(iMemSize);
    };


// ----------------------------------------------------------------------------
// CSigComp::CyclesPerBit
// Get CPB
// ----------------------------------------------------------------------------
//

CSigComp::TCyclesPerBit CSigComp::CyclesPerBit() const
    {
    return iCyclesPerBit;
    };


// ----------------------------------------------------------------------------
// CSigComp::StateMemorySize
// Get SMS
// ----------------------------------------------------------------------------
//

CSigComp::TMemorySize CSigComp::StateMemorySize() const
    {
    return iStateMemorySize;
    };


// ----------------------------------------------------------------------------
// CSigComp::StateMgr
// Get state menager
// ----------------------------------------------------------------------------
//

const CStateMgr* CSigComp::StateMgr() const
    {
    return iStateMgr;
    }

// ----------------------------------------------------------------------------
// CSigComp::Compressor
// Get compressor
// ----------------------------------------------------------------------------
//

const CSigCompCompressor* CSigComp::Compressor() const
    {
    return iCompressor;
    }

// ----------------------------------------------------------------------------
// CSigComp::CyclesConsumed
// get consumed cycles number
// ----------------------------------------------------------------------------
//

#if defined(SIGCOMP_DEBUG)

EXPORT_C TUint CSigComp::CyclesConsumed() const
    {
    return iUdvm->CyclesConsumed();
    }

#endif
