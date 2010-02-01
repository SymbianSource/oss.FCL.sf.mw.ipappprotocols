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
// Name        : CSigCompDeflateContext.cpp
// Part of     : CDeflateComp
// Interface   : 
// compression context
// Version     : 1.0
//




#include <e32base.h>
#include <badesca.h>
#include "SigCompDeflateContext.h"
#include "DeflateComp.h"

CSigCompDeflateContext::CSigCompDeflateContext()
    {
    iUploadState = EFalse;
    iBytecodeStateConfirmed = EFalse;
    iCBStateConfirmed = EFalse;
    iDecompressionMemorySize = 2048;
    iStateMemorySize = 2048;
    iUdvmSize = 0;
    iCBSize = 0;
    }

CSigCompDeflateContext::~CSigCompDeflateContext()
    {
    delete iWindow;
    }

void CSigCompDeflateContext::SetWindowSize(TInt aWindowSize)
    {
    iWindowSize = aWindowSize;
    }

TInt CSigCompDeflateContext::WindowSize()
    {
    return iWindowSize;
    }

void CSigCompDeflateContext::SetCurrentPosition(TInt aCurrentPosition)
    {
    iCurrentPosition = aCurrentPosition;
    }

TInt CSigCompDeflateContext::CurrentPosition()
    {
    return iCurrentPosition;
    }

void CSigCompDeflateContext::SetWindow(CArrayFixFlat<TUint8>* aWindow)
    {
    delete iWindow;
    iWindow = aWindow;
    }

CArrayFixFlat<TUint8>* CSigCompDeflateContext::Window()
    {
    return iWindow;
    }

void CSigCompDeflateContext::SetCBStateHash(const TUint8* aHash)
    {
    Mem::Copy(iCBStateHash, aHash, KMinPartialStateLength);
    }

void CSigCompDeflateContext::SetBytecodeHash(const TUint8* aHash)
    {
    Mem::Copy(iBytecodeStateHash, aHash, KMinPartialStateLength);
    }

const TUint8* CSigCompDeflateContext::CBStateHash() const
    {
    return iCBStateHash;
    }

const TUint8* CSigCompDeflateContext::BytecodeHash() const
    {
    return iBytecodeStateHash;
    }

void CSigCompDeflateContext::SetCBStateLength(TInt aLength)
    {
    iCBStateLength = aLength;
    }

TInt CSigCompDeflateContext::CBStateLength() const
    {
    return iCBStateLength;
    }

void CSigCompDeflateContext::SetUploadState(TBool aUploadState)
    {
    iUploadState = aUploadState;
    }

TBool CSigCompDeflateContext::UploadState()
    {
    return iUploadState;
    }

void CSigCompDeflateContext::SetStateItem(TStateItem* aStateItem,
                                          TInt aStateItemLength)
    {
    iStateItem = aStateItem;
    iStateItemLength = aStateItemLength;
    }

TStateItem* CSigCompDeflateContext::StateItem()
    {
    return iStateItem;
    }

TInt CSigCompDeflateContext::StateItemLength()
    {
    return iStateItemLength;
    }

TBool CSigCompDeflateContext::CBStateConfirmed() const
    {
    return iCBStateConfirmed;
    }

void CSigCompDeflateContext::SetCBStateConfirmed(TBool aStateConfirmed)
    {
    iCBStateConfirmed = aStateConfirmed;
    }

TBool CSigCompDeflateContext::BytecodeStateConfirmed() const
    {
    return iBytecodeStateConfirmed;
    }

void CSigCompDeflateContext::SetBytecodeStateConfirmed(
                                    TBool aBytecodeStateConfirmed)
    {
    iBytecodeStateConfirmed = aBytecodeStateConfirmed;
    }

TInt CSigCompDeflateContext::Sms() const
    {
    return iStateMemorySize;
    }

TInt CSigCompDeflateContext::Dms() const
    {
    return iDecompressionMemorySize;
    }

void CSigCompDeflateContext::SetSms(TInt aSms)
    {
    iStateMemorySize = aSms;
    }

void CSigCompDeflateContext::SetDms(TInt aDms)
    {
    iDecompressionMemorySize = aDms;
    }

TInt CSigCompDeflateContext::UdvmSize() const
    {
    return iUdvmSize;
    }

void CSigCompDeflateContext::SetUdvmSize(TInt aUdvmSize)
    {
    iUdvmSize = aUdvmSize;
    }

TInt CSigCompDeflateContext::CBSize() const
    {
    return iCBSize;
    }

void CSigCompDeflateContext::SetCBSize(TInt aCBSize)
    {
    iCBSize = aCBSize;
    }
