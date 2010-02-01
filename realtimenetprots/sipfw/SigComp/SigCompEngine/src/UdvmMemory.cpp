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
// Name        : UdvmMemory.cpp
// Part of     : SigComp / UDVM
// UDVM memory/memory manipulation
// Version     : 1.0
//



#include "Sigcomp.h"
#include "Udvm.h"
#include "UdvmMemory.h"

// ============================ MEMBER FUNCTIONS ==============================

void CUdvmMemory::ConstructL(TUint aMemSize)
    {
    iMemSize = aMemSize;
    iMem = new (ELeave)CArrayFixFlat<TUint8>(8);
    iMem->ResizeL(iMemSize);
    }

CUdvmMemory::CUdvmMemory()
    {
    }

CUdvmMemory* CUdvmMemory::NewLC(TUint aMemSize)
    {
    CUdvmMemory* self= new (ELeave)CUdvmMemory();
    CleanupStack::PushL(self);
    self->ConstructL(aMemSize);
    return self;
    }

CUdvmMemory* CUdvmMemory::NewL(TUint aMemSize)
    {
    CUdvmMemory* self= NewLC(aMemSize);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUdvmMemory::~CUdvmMemory()
    {
    delete iMem;
    }

void CUdvmMemory::CheckMemAccessL(TUint aAddr, TUint aSize) const
    {
    if ((iFreeMemSize < KMaxUdvmMemorySize) &&
        ((aAddr + aSize) > iFreeMemSize))
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }
    }

// ----------------------------------------------------------------------------
// CUdvmMemory::WriteMem8L
// write byte to UDVM memory
// ----------------------------------------------------------------------------
//

void CUdvmMemory::WriteMem8L(TUint aDest, TUint aVal)
    {

    CheckMemAccessL(aDest, 1);

    (*iMem)[aDest] = static_cast<TUint8>(aVal & 0xff);
    }


// ----------------------------------------------------------------------------
// CUdvmMemory::WriteMem16L
// write 16-bit word to UDVM memory
// ----------------------------------------------------------------------------
//

void CUdvmMemory::WriteMem16L(TUint aDest, TUint aVal)
    {

    CheckMemAccessL(aDest, 2);

    // write 16-bit value in big-endian convention
    (*iMem)[aDest] = static_cast<TUint8>((aVal >> 8) & 0xff);
    (*iMem)[(aDest + 1) & KMaxUdvmMemoryMask] = static_cast<TUint8>
                                                           (aVal & 0xff);
    }


// ----------------------------------------------------------------------------
// CUdvmMemory::ReadMem8L
// read byte from UDVM memory
// ----------------------------------------------------------------------------
//

TUint8 CUdvmMemory::ReadMem8L(TUint aSrc) const
    {

    CheckMemAccessL(aSrc, 1);

    return static_cast<TUint8>((*iMem)[aSrc]);
    }


// ----------------------------------------------------------------------------
// CUdvmMemory::ReadMem16L
// read 16-bit word from UDVM memory
// ----------------------------------------------------------------------------
//

TUint16 CUdvmMemory::ReadMem16L(TUint aSrc) const
    {

    CheckMemAccessL(aSrc, 2);

    // read 16-bit value in big-endian convention
    return static_cast<TUint16>(((static_cast<TUint16>((*iMem)[aSrc]) << 8) +
           static_cast<TUint16>((*iMem)[(aSrc + 1) & KMaxUdvmMemoryMask])));
    }


// ----------------------------------------------------------------------------
// CUdvmMemory::SetMemL
// set block of UDVM memory to value
// ----------------------------------------------------------------------------
//

void CUdvmMemory::SetMemL(TUint aDest, TUint8 aVal, TUint aSize)
    {

    CheckMemAccessL(aDest, aSize);

    for (TUint i = 0; i < aSize; i++)
        {
        (*iMem)[aDest] = aVal;
        aDest = (aDest + 1) & KMaxUdvmMemoryMask;
        }
    }


// ----------------------------------------------------------------------------
// CUdvmMemory::CopyToMemL
// copy block of data to UDVM memory
// ----------------------------------------------------------------------------
//

void CUdvmMemory::CopyToMemL(TUint aDest, const TUint8* aSrc, TUint aSize)
    {

    CheckMemAccessL(aDest, aSize);

    for (TUint i = 0; i < aSize; i++)
        {
        (*iMem)[aDest] = *aSrc++;
        aDest = (aDest + 1) & KMaxUdvmMemoryMask;
        }
    }


// ----------------------------------------------------------------------------
// CUdvmMemory::CopyFromMemL
// copy block of data from UDVM memory
// ----------------------------------------------------------------------------
//

void CUdvmMemory::CopyFromMemL(TUint8* aDest, TUint aSrc, TUint aSize) const
    {

    CheckMemAccessL(aSrc, aSize);

    for (TUint i = 0; i < aSize; i++)
        {
        *aDest++ = (*iMem)[aSrc];
        aSrc = (aSrc + 1) & KMaxUdvmMemoryMask;
        }
    }

void CUdvmMemory::InitMemoryL(TUint aMsgSize, TUint aCyclesPerBit)
    {
    if (iMemSize <= aMsgSize)
        {
        User::Leave(CSigComp::EDecompressionFailure);
        }

    iFreeMemSize = iMemSize-aMsgSize;
    if (iFreeMemSize > KMaxUdvmMemorySize)
        {
        iFreeMemSize = KMaxUdvmMemorySize;
        }

    WriteMem16L(CUdvm::EMem_UDVM_memory_size, static_cast<TUint16>
                (iFreeMemSize & KMaxUdvmMemoryMask));
    WriteMem16L(CUdvm::EMem_cycles_per_bit, static_cast<TUint16>
                (aCyclesPerBit));
    WriteMem16L(CUdvm::EMem_SigComp_version, KSigCompVersion);
    SetMemL(CUdvm::EMem_reserved, 0,
            CUdvm::EMem_scratch_pad-CUdvm::EMem_reserved);
    SetMemL(CUdvm::EMem_scratch_pad, 0,
            CUdvm::EMem_registers - CUdvm::EMem_scratch_pad);
    WriteMem16L(CUdvm::EReg_byte_copy_left, 0x0000);
    WriteMem16L(CUdvm::EReg_byte_copy_right, 0x0000);
    WriteMem16L(CUdvm::EReg_input_bit_order, 0x0000);
    WriteMem16L(CUdvm::EReg_stack_location, 0x0000);
    SetMemL(CUdvm::EMem_user_area, 0, iFreeMemSize - CUdvm::EMem_user_area);
    }

TUint8* CUdvmMemory::MemoryPtr()
    {
    return &iMem->At(0);
    }

TUint CUdvmMemory::MemorySize() const
    {
    return iMemSize;
    }

TUint CUdvmMemory::FreeMemorySize() const
    {
    return iFreeMemSize;
    }

