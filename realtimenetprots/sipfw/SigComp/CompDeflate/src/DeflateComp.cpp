// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CDeflateComp.cpp
// Part of     : deflatecomp
// deflate ECOM plugin, CSigCompCompressor implementation
// Version     : 1.4
//



#include <e32base.h>
#include <badesca.h>
#include <hash.h>

#include "Sigcomp.h"
#include "sigcompcompartment.h"
#include "SigCompStateMgr.h"

#include "DeflateComp.h"
#include "Deflate.h"
#include "SigCompDeflateContext.h"
#include "StateMgr.h"
#include "Udvm.h"
#include "sigcompstateitem.h"
#include "MessageWriter.h"


/** SIP dictionary hash */
static const TUint8 KSIPDictionaryHash[] =
    {
    0xfb, 0xe5, 0x07, 0xdf, 0xe5, 0xe6, 0xaa, 0x5a, 0xf2, 0xab,
    0xb9, 0x14, 0xce, 0xaa, 0x05, 0xf9, 0x9c, 0xe6, 0x1b, 0xa5
    };

/** size of the first part of SIP dictionary,
    phrases only. RFC 3485.*/
const TInt KSIPDictionaryPhrasesSize = 0x0d8c;


/** mask to get cpb value from returned parameters */
//static const TInt KReturnedParametersCPBMask = 3;

/** number of bits returned parameters has to be shifted right
    to get cpb value */
static const TInt KReturnedParametersCPBShift = 6;

/** mask to get dms value from returned parameters */
static const TInt KReturnedParametersDMSMask = 7;

/** number of bits returned parameters has to be shifted right
    to get dms value */
static const TInt KReturnedParametersDMSShift = 3;

/** mask to get sms value from returned parameters */
static const TInt KReturnedParametersSMSMask = 7;

/** number of bits returned parameters has to be shifted right
    to get sms value */
static const TInt KReturnedParametersSMSShift = 0;


/** RFC3220 chapter 7. each sigcomp message starts with 5 bits set to 1 */
static const TInt KMessageCommonHeader = (0x1f << 3);

/** T bit in message header */
static const TInt KMessageHeaderTBit (1 << 2);

/** length to be set in message header. 0x1 == 6 bytes of hash id length */
static const TInt KMessageHeaderLen = 0x01;

/** 'requested feedback is present' flag */
static const TInt KRequestedFeedbackPresentFlag = 4;

/** number of bits size of bytecode must be shifted left to get it
    on correct position in header*/
static const TInt KBytecodeLengthShift = 4;

/** number of bits bytecode destination must be shifted right to get
    correct bit representation of destination. note, the bit
    representation is actually (dest >> shift) - 1 */
static const TInt KBytecodeDestinationShift = 6;

/** number of bits cycles must be shifted left to het it on correct position
    in returned parameters field */
//static const TInt KParametersCyclesShift = 6;

/** number of bits memory size must be shifted left to get in on correct
    position in returned parameters field */
//static const TInt KParametersMemorySizeShift = 3;

_LIT8(KZero, "\0");
_LIT8(KEightZeros, "\0\0\0\0\0\0\0\0");

/** minimum circular buffer size */
static const TInt KMinCBSize = (258/2+1);


// ----------------------------------------------------------------------------

CDeflateComp* CDeflateComp::NewL(TAny* aInitParams)
    {
    return new (ELeave)CDeflateComp(aInitParams);  
    }


// ----------------------------------------------------------------------------

CDeflateComp::CDeflateComp(TAny* aInitParams)
    {
    TCompressorInitParams* initParams = static_cast<TCompressorInitParams*>
                                        (aInitParams);
    iStateMgr = initParams->iStateMgr;
    iStateItem = initParams->iStateItem;
    iStateItemLength = initParams->iStateItemLength;
    iUploadState = initParams->iUploadState;

    // SIP dictionary
    if (iStateItem)
        {
        if (!Mem::Compare(iStateItem->iStateIdentifier,
                          sizeof(KSIPDictionaryHash),
                          KSIPDictionaryHash, sizeof(KSIPDictionaryHash)))
            {
            if (iStateItemLength > KSIPDictionaryPhrasesSize)
                iStateItemLength = KSIPDictionaryPhrasesSize;
            }

        }
    }

// ----------------------------------------------------------------------------

CDeflateComp::~CDeflateComp()
    {
    }



// ----------------------------------------------------------------------------

TUint CDeflateComp::SetDictionary(const TStateItem* aDictionaryState,
                                  CDeflate* aDeflateComp,
                                CSigCompDeflateContext* aDeflateContext) const
    {
    if (aDictionaryState)
        {
        TInt dictLength;
        const TUint8* ptr = aDictionaryState->iStateValue;

        dictLength = (iStateItemLength > aDeflateContext->WindowSize()) ?
                      aDeflateContext->WindowSize() : iStateItemLength;

        ptr += (iStateItemLength - dictLength);

        aDeflateComp->SetDictionary(ptr, dictLength);
        return 1;
        }

    return 0;
    }


// ----------------------------------------------------------------------------

void CDeflateComp::AppendDataL(CMessageWriter* aWriter, TUint8 aValue)
    {
    aWriter->WriteByteL(aValue);
    }


// ----------------------------------------------------------------------------

void CDeflateComp::AppendDataL(CMessageWriter* aWriter, TUint16 aValue)
    {
    TUint8 tmp = static_cast<TUint8>((aValue >> 8) & KMax8BitValueMask);
    aWriter->WriteByteL(tmp);
    tmp = static_cast<TUint8>(aValue & KMax8BitValueMask);
    aWriter->WriteByteL(tmp);
    }


// ----------------------------------------------------------------------------

void CDeflateComp::AppendDataL(CMessageWriter* aWriter, const TDesC8& aData)
    {
    aWriter->WriteBlockL(aData);
    }


// ----------------------------------------------------------------------------

void CDeflateComp::AppendRequestedFeedbackL(CMessageWriter *aWriter,
                                            const TDesC8& aRequestedFeedback)
    {
    if (aRequestedFeedback.Length())
        {
        const TUint8* rfdata = aRequestedFeedback.Ptr();
        if (rfdata[0] & KRequestedFeedbackPresentFlag)
            {
            AppendDataL(aWriter,
                        TPtrC8(&rfdata[1], 
                               aRequestedFeedback.Length() - 1));
            }
        }
    }


// ----------------------------------------------------------------------------

void CDeflateComp::CalculateBytecodeHashL(
                               CSigCompDeflateContext* aDeflateContext,
                               TDeflateCompBytecodeId aBytecode,
                               TUint8 aReturnedParameters)
    {

    CSHA1* sha1 = CSHA1::NewL();
    CleanupStack::PushL(sha1);
    sha1->Reset();

    // set state header
    TUint start = KDeflateBytecodes[aBytecode].iStateStart;
    TUint instr = KDeflateBytecodes[aBytecode].iBytecodeStart;
    TUint size = KDeflateBytecodes[aBytecode].iStateSize;

    TUint8 stateData[8+11];
    stateData[0] = static_cast<TUint8>((size >> 8) & 0xff);
    stateData[1] = static_cast<TUint8>(size & 0xff);
    stateData[2] = static_cast<TUint8>((start >> 8) & 0xff);
    stateData[3] = static_cast<TUint8>(start & 0xff);
    stateData[4] = static_cast<TUint8>((instr >> 8) & 0xff);
    stateData[5] = static_cast<TUint8>(instr & 0xff);
    stateData[6] = 0;
    stateData[7] = 6;

    // set bytecode values area

    switch (aBytecode)
        {
        case EStaticBytecode:
            {
/*
:state_area_start
:dictionary_id                  pad (6)
:dictionary_phrases_size        pad (2)
:returned_parameters_location   pad (1)
:returned_sigcomp_version       pad (1)
:returned_parameters_zero       pad (1)
*/
            if (iStateItem)
                {
                Mem::Copy(&stateData[8],
                          iStateItem->iStateIdentifier,
                          KMinPartialStateLength);
                stateData[8+6] = static_cast<TUint8>(
                                    (iStateItemLength >> 8) & 0xff);
                stateData[8+7] = static_cast<TUint8>(
                                    iStateItemLength & 0xff);
                }
            else
                {
                Mem::FillZ(&stateData[8], KMinPartialStateLength + 2);
                }
            stateData[8+8] = static_cast<TUint8>(aReturnedParameters);
            stateData[8+9] = static_cast<TUint8>(KSigCompVersion);
            stateData[8+10] = static_cast<TUint8>(0);

            sha1->Hash(TPtrC8(stateData, 8+11));
            break;
            }

        case EStaticUploadBytecode:
            {
            // hash from feedback
            break;
            }

        case EDynamicBytecode:
            {
/*
:state_area_start
:returned_parameters_location   pad (1)
:returned_sigcomp_version       pad (1)
:returned_parameters_zero       pad (1)
*/
            stateData[8+0] = static_cast<TUint8>(aReturnedParameters);
            stateData[8+1] = static_cast<TUint8>(KSigCompVersion);
            stateData[8+2] = static_cast<TUint8>(0);

            sha1->Hash(TPtrC8(stateData, 8+3));
            break;
            }
            
        case EDynamicUploadBytecode:
            {
            // hash from feedback
            break;
            }
        }

    TPtrC8 hash = sha1->Hash(TPtrC8(KDeflateBytecodes[aBytecode].iBytecode,
                               KDeflateBytecodes[aBytecode].iBytecodeSize));

    TInt toZero = KDeflateBytecodes[aBytecode].iStateSize -
                  KDeflateBytecodes[aBytecode].iBytecodeInit +
                  KDeflateBytecodes[aBytecode].iStateStart -
                  KDeflateBytecodes[aBytecode].iBytecodeSize;
    TInt i;
    for(i=0; i<toZero; i++)
        {
// Not covered because current bytecodes not need that
// I left the code there, as it may be usefull for other bytecodes.
        hash.Set(sha1->Hash(KZero));
        }

    aDeflateContext->SetBytecodeHash(hash.Ptr());

    CleanupStack::PopAndDestroy(sha1);
    }


// ----------------------------------------------------------------------------

TInt CDeflateComp::EncodeCyclesPerBit(
                                   CSigComp::TCyclesPerBit aCyclesPerBit) const
    {
    TInt result = 0;
    TUint value = aCyclesPerBit >> 5;

    while (value)
        {
        result++;
        value >>= 1;
        }

    return result;
    }


// ----------------------------------------------------------------------------

TInt CDeflateComp::EncodeMemorySize(CSigComp::TMemorySize aMemorySize) const
    {
    TInt result = 0;
    TUint value = aMemorySize >> 11;

    while (value)
        {
        result++;
        value >>= 1;
        }

    return result;
    }


// ----------------------------------------------------------------------------

CSigComp::TMemorySize CDeflateComp::DecodeMemorySize(TInt aMemorySize)
    {

    TInt result = aMemorySize ? 1 << (10+aMemorySize) : 0;

    return static_cast<CSigComp::TMemorySize>(result);
    }


// ----------------------------------------------------------------------------

TUint8 CDeflateComp::EncodeReturnedParameters(
                               MSigCompCompartmentContext* aCompartmentContext)
    {
    return static_cast<TUint8>(
        (EncodeCyclesPerBit(aCompartmentContext->CyclesPerBit()) <<
            KReturnedParametersCPBShift) |
        (EncodeMemorySize(aCompartmentContext->DecompressionMemorySize()) <<
            KReturnedParametersDMSShift) |
        (EncodeMemorySize(aCompartmentContext->StateMemorySize()) <<
            KReturnedParametersSMSShift));
    }


// ----------------------------------------------------------------------------

void CDeflateComp::ParseReturnedParameters(const TDesC8& aReturnedParameters,
                                       CSigCompDeflateContext* aDeflateContext)
    {

    if (aReturnedParameters.Length())
        {
        const TUint8* retpdata = aReturnedParameters.Ptr();
        TUint8 rp = *retpdata;

        aDeflateContext->SetDms(
            DecodeMemorySize((rp >> KReturnedParametersDMSShift) &
                KReturnedParametersDMSMask));
        aDeflateContext->SetSms(
            DecodeMemorySize((rp >> KReturnedParametersSMSShift) &
                KReturnedParametersSMSMask));
        }
    }


// ----------------------------------------------------------------------------

void CDeflateComp::AppendDataHeaderL(CMessageWriter* aWriter,
                                     TDeflateCompBytecodeId aBytecode,
                                     TUint8 aReturnedParameters,
                                     TInt aUdvmSize,
                                     TBool aFirstMessage,
                                     CSigCompDeflateContext* aDeflateContext)
    {

    switch (aBytecode)
        {
        case EStaticBytecode:
            {
/*
    dictionary_id:                6 bytes (1st message only)
    dictionary_phrases_size:      2 bytes (1st message only)
    returned_parameters_location: 1 byte  (1st message only)
    returned_sigcomp_version:     1 byte  (1st message only)
    udvm_memory_size:             2 bytes
*/
            if (aFirstMessage)
                {
                if (iStateItem)
                    {
                    AppendDataL(aWriter,
                                TPtrC8(iStateItem->iStateIdentifier,
                                       KMinPartialStateLength));
                    AppendDataL(aWriter,
                                static_cast<TUint16>(iStateItemLength));
                    }
                else
                    {
                    AppendDataL(aWriter, KEightZeros);
                    }

                AppendDataL(aWriter, aReturnedParameters);
                AppendDataL(aWriter, static_cast<TUint8>(KSigCompVersion));
                }

            AppendDataL(aWriter, static_cast<TUint16>(aUdvmSize));

            break;
            }

        case EStaticUploadBytecode:
            {
/*
    udvm_memory_size:             2 bytes
    state_length                  2 bytes (1st message only)
    state value                   state_length bytes (1st message only)
    returned_parameters_location: 1 byte  (1st message only)
    returned_sigcomp_version:     1 byte  (1st message only)
*/
            AppendDataL(aWriter, static_cast<TUint16>(aUdvmSize));

            if (aFirstMessage)
                {
                AppendDataL(aWriter, static_cast<TUint16>(iStateItemLength));
                AppendDataL(aWriter, TPtrC8(iStateItem->iStateValue,
                                            iStateItemLength));
                AppendDataL(aWriter, aReturnedParameters);
                AppendDataL(aWriter, static_cast<TUint8>(KSigCompVersion));
                }

            break;
            }

        case EDynamicBytecode:
            {
/*
    returned_parameters_location: 1 byte  (1st message only)
    returned_sigcomp_version:     1 byte  (1st message only)
    udvm_memory_size:             2 bytes
    dictionary_phrases_size:      2 bytes
    dictionary_id:                6 bytes (if dictionary_phrases_size!=0)
    cbstate_size:                 2 bytes
    cbstate_id:                   6 bytes (if cbstate_size!=0)
*/
            if (aFirstMessage)
                {
                AppendDataL(aWriter, aReturnedParameters);
                AppendDataL(aWriter, static_cast<TUint8>(KSigCompVersion));
                }

            AppendDataL(aWriter, static_cast<TUint16>(aUdvmSize));

            if (aDeflateContext->CBStateConfirmed())
                {
                AppendDataL(aWriter, static_cast<TUint16>(0));
                AppendDataL(aWriter, static_cast<TUint16>(
                                aDeflateContext->CBStateLength()));
                AppendDataL(aWriter, TPtrC8(aDeflateContext->CBStateHash(),
                                            KMinPartialStateLength));
                }
            else
                {
                if (iStateItem)
                    {
                    AppendDataL(aWriter, static_cast<TUint16>(
                                                iStateItemLength));
                    AppendDataL(aWriter, TPtrC8(iStateItem->iStateIdentifier,
                                                KMinPartialStateLength));
                    AppendDataL(aWriter, static_cast<TUint16>(0));
                    }
                else
                    {
                    AppendDataL(aWriter, static_cast<TUint16>(0));
                    AppendDataL(aWriter, static_cast<TUint16>(0));
                    }
                }
            break;
            }

        case EDynamicUploadBytecode:
            {
/*
    udvm_memory_size:             2 bytes
    state_length                  2 bytes (1st message only)
    state value                   state_length bytes (1st message only)
    returned_parameters_location: 1 byte  (1st message only)
    returned_sigcomp_version:     1 byte  (1st message only)
    cbstate_size:                 2 bytes (nth message only)
    cbstate_id:                   6 bytes (if cbstate_size!=0)
*/
            AppendDataL(aWriter, static_cast<TUint16>(aUdvmSize));

            if (aFirstMessage)
                {
                AppendDataL(aWriter, static_cast<TUint16>(iStateItemLength));
                AppendDataL(aWriter, TPtrC8(iStateItem->iStateValue,
                                            iStateItemLength));
                AppendDataL(aWriter, aReturnedParameters);
                AppendDataL(aWriter, static_cast<TUint8>(KSigCompVersion));
                }
            else
                {
                if (aDeflateContext->CBStateConfirmed())
                    {
                    AppendDataL(aWriter, static_cast<TUint16>(
                                    aDeflateContext->CBStateLength()));
                    AppendDataL(aWriter, TPtrC8(aDeflateContext->CBStateHash(),
                                                KMinPartialStateLength));
                    }
                else
                    {
                    AppendDataL(aWriter, static_cast<TUint16>(0));
                    }
                }

            break;
            }
        }
    }

    
// ----------------------------------------------------------------------------

TInt CDeflateComp::CalcCBSizeL(TInt& aUdvmSize,
                                TDeflateCompBytecodeId aBytecode,
                                CSigCompDeflateContext* aDeflateContext)
    {

    TInt cbSize = aUdvmSize - KDeflateBytecodes[aBytecode].iCBufStart;

    // no memory for bytecode and circular buffer
    if (cbSize < 0)
        return cbSize;

    switch (aBytecode)
        {
        case EStaticBytecode:
            {
            break;
            }

        case EStaticUploadBytecode:
            {
            break;
            }

        case EDynamicBytecode:
            {
            TInt availSms = aDeflateContext->Sms() -
                            KDeflateBytecodes[aBytecode].iStateSize -
                            KStateCostExtension;

            if (availSms <= 0)
                {
                return cbSize;
                }

            if ((cbSize+2+KStateCostExtension) > availSms)
                {
                cbSize = availSms - 2 - KStateCostExtension;
                aUdvmSize = cbSize + KDeflateBytecodes[aBytecode].iCBufStart;
                }

            break;
            }
            
        case EDynamicUploadBytecode:
            {
            TInt availSms = aDeflateContext->Sms() -
                            KDeflateBytecodes[aBytecode].iStateSize -
                            KStateCostExtension -
                            iStateItemLength -
                            KStateCostExtension;

            if (availSms <= 0)
                {
                return cbSize;
                }

            if ((cbSize+2+KStateCostExtension) > availSms)
                {
                cbSize = availSms - 2 - KStateCostExtension;
                aUdvmSize = cbSize + KDeflateBytecodes[aBytecode].iCBufStart;
                }

            break;
            }
        }

    return cbSize;
    }

// ----------------------------------------------------------------------------

CSigCompDeflateContext* CDeflateComp::DeflateContextL(
                               MSigCompCompartmentContext* aCompartmentContext)
    {
    CSigCompCompressionContext* tmpContext = 
                                   const_cast<CSigCompCompressionContext*>
                                   (aCompartmentContext->CompressionContext());

    CSigCompDeflateContext* deflateContext =
                                           static_cast<CSigCompDeflateContext*>
                                           (tmpContext);

    if (deflateContext == NULL)
        {
        deflateContext = new (ELeave)CSigCompDeflateContext();
        aCompartmentContext->SetCompressionContext(
                             static_cast<CSigCompCompressionContext*>
                             (deflateContext));
        deflateContext->SetUploadState(iUploadState);
        deflateContext->SetStateItem(iStateItem, iStateItemLength);
        }
    else
        {
        iUploadState = deflateContext->UploadState();
        iStateItem = deflateContext->StateItem();
        iStateItemLength = deflateContext->StateItemLength();
        }

    return deflateContext;
    }


// ----------------------------------------------------------------------------

CBufBase* CDeflateComp::CompressMessageL(
                              MSigCompCompartmentContext* aCompartmentContext,
                              const TDesC8& aMessage,
                              TBool aForStreamBasedProtocol)
    {

    if (aMessage.Length() == 0)
        {
        User::Leave(CSigComp::ECompressionFailure);
        }

    TBool dynamic = aCompartmentContext->IsDynamicCompression();

    CSigCompDeflateContext* deflateContext =
                                      DeflateContextL(aCompartmentContext);

    ParseReturnedParameters(aCompartmentContext->ReturnedParameters(),
                            deflateContext);

    // select bytecode
    TDeflateCompBytecodeId bytecode;

    if (iUploadState)
        {
        if (dynamic)
            bytecode = EDynamicUploadBytecode;
        else
            bytecode = EStaticUploadBytecode;
        }
    else
        {
        if (dynamic)
            bytecode = EDynamicBytecode;
        else
            bytecode = EStaticBytecode;
        }

    CBufFlat* outputBuffer = CBufFlat::NewL(32);
    CleanupStack::PushL(outputBuffer);
    outputBuffer->SetReserveL(32);

    CMessageWriter* messageWriter = new (ELeave)CMessageWriter(outputBuffer,
                                                      aForStreamBasedProtocol);
    CleanupStack::PushL(messageWriter);

    TInt msgPos = 0;
    while (msgPos < aMessage.Size())
        {
        TInt msgSize = aMessage.Size() - msgPos;

        if (aForStreamBasedProtocol)
            {
            if (msgSize > KMaxOutputSize)
                {
                msgSize = KMaxOutputSize;
                }
            }
        else
            {
            if (msgSize > KMaxOutputSize)
                {
                User::Leave(CSigComp::ECompressionFailure);
                }
            }

        TPtrC8 message(aMessage.Mid(msgPos, msgSize));

        const TDesC8& requestedFeedback =
                            aCompartmentContext->RequestedFeedback();
        const TDesC8& returnedFeedback =
                            aCompartmentContext->ReturnedFeedback();

        CDeflate* deflateComp = CDeflate::NewL(dynamic, deflateContext);
        CleanupStack::PushL(deflateComp);

        TUint8 returnedParameters =
                            EncodeReturnedParameters(aCompartmentContext);

        if (!deflateContext->BytecodeStateConfirmed())
            {
            if (returnedFeedback.Length())
                {
                if(deflateContext->UploadState())
                    {
                    deflateContext->SetBytecodeHash(returnedFeedback.Ptr() +
                                                    dynamic*8);
                    deflateContext->SetBytecodeStateConfirmed(ETrue);
                    }
                else
                    {
                    CalculateBytecodeHashL(deflateContext, bytecode,
                                           returnedParameters);
                    deflateContext->SetBytecodeStateConfirmed(ETrue);
                    }
                }
            }

        if (dynamic)
            {
            if (returnedFeedback.Length())
                {
                deflateContext->SetCBStateHash(returnedFeedback.Ptr());
                TInt cbLen = (returnedFeedback.Ptr()[6]<<8) +
                             returnedFeedback.Ptr()[7];
                deflateContext->SetCBStateLength(cbLen);
                deflateContext->SetCBStateConfirmed(ETrue);
                }
            else
                {
                deflateContext->SetCBStateConfirmed(EFalse);
                deflateComp->Reset();
                }
            }

        TUint8 header = KMessageCommonHeader;

        if (requestedFeedback.Length())
            {
            header |= KMessageHeaderTBit;
            }

        if(!deflateContext->BytecodeStateConfirmed())
            {
            AppendDataL(messageWriter, header);
            AppendRequestedFeedbackL(messageWriter, requestedFeedback);

            TUint16 lendest = static_cast<TUint16>(
                                (KDeflateBytecodes[bytecode].iBytecodeSize <<
                                    KBytecodeLengthShift) |
                                ((KDeflateBytecodes[bytecode].iBytecodeInit >>
                                    KBytecodeDestinationShift) - 1));

            AppendDataL(messageWriter, lendest);
            AppendDataL(messageWriter,
                        TPtrC8(KDeflateBytecodes[bytecode].iBytecode,
                               KDeflateBytecodes[bytecode].iBytecodeSize));
            }
        else
            {
            header |= KMessageHeaderLen;
            AppendDataL(messageWriter, header);
            AppendRequestedFeedbackL(messageWriter, requestedFeedback);

            AppendDataL(messageWriter,
                        TPtrC8(deflateContext->BytecodeHash(),
                               KMinPartialStateLength));
            }

        // calculate available memory on remote side
        TInt udvmSize = -1;
        TInt cbSize = -1;
        if (deflateContext->BytecodeStateConfirmed())
            {
            udvmSize = deflateContext->UdvmSize();
            cbSize = deflateContext->CBSize();
            }
        else
            {
            TInt dms = deflateContext->Dms();
            for (; (udvmSize < 0) || (cbSize < KMinCBSize); dms<<=1)
                {
                if (aForStreamBasedProtocol)
                    {
                    udvmSize = dms / 2;
                    }
                else
                    {
                    // simulate compressed message size
                    TInt msgSize = outputBuffer->Size() + 2 +
                                   message.Length() + 50;
                    if(iUploadState &&
                       !deflateContext->BytecodeStateConfirmed())
                        msgSize += iStateItemLength;

                    udvmSize = dms - msgSize;
                    }

                if (udvmSize > 0)
                    {
                    if (udvmSize > static_cast<TInt>(KMaxUdvmMemorySize))
                        {
                        udvmSize = KMaxUdvmMemorySize;
                        }

                    cbSize = CalcCBSizeL(udvmSize, bytecode, deflateContext);
                    }
                }
                deflateContext->SetUdvmSize(udvmSize);
                deflateContext->SetCBSize(cbSize);
            }

        AppendDataHeaderL(messageWriter,
                          bytecode, returnedParameters, udvmSize,
                          !deflateContext->BytecodeStateConfirmed(),
                          deflateContext);

        //configure deflate compressor
        deflateComp->ChangeWindowSizeL(cbSize);

        if ((!dynamic) || (!deflateContext->CBStateConfirmed()))
            {
            SetDictionary(deflateContext->StateItem(),
                          deflateComp,
                          deflateContext);
            }

        //compress actual data
        deflateComp->CompressL(message, messageWriter);
        CleanupStack::PopAndDestroy(deflateComp);

        messageWriter->FlushAndTerminateL();

        // clean feedbacks
        aCompartmentContext->SetRequestedFeedbackL(KNullDesC8);
        aCompartmentContext->SetReturnedFeedbackL(KNullDesC8);

        msgPos += msgSize;
        }
    outputBuffer->Compress();
    CleanupStack::PopAndDestroy(messageWriter);
    CleanupStack::Pop(outputBuffer);

    return outputBuffer;
    }

TBool CDeflateComp::IsDynamicCompressionSupported()
    {
    return ETrue;
    }
