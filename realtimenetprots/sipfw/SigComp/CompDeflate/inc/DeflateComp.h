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
* Name        : CDeflateComp.h
* Part of     : deflatecomp / deflatecomp
* This ifle is main part of deflatecomp,
* implements CSigCompCompressor.
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CDEFLATECOMP_H
#define CDEFLATECOMP_H

#include "Compressor.h"
#include "Sigcomp.h"
#include "DeflateCompBytecode.h"

class CDeflate;
class TStateItem;
class CSigCompDeflateContext;
class CMessageWriter;

/** minimum state hash length */
const TInt KMinPartialStateLength = 6;

class CDeflateComp : public CSigCompCompressor
    {
    public:
        /**
        * aInitParams are in fact TCompressorInitParams, as defined
        * in Compressor.h
        */
        static CDeflateComp* NewL(TAny* aInitParams);
        ~CDeflateComp();

        /**
        * This function compresses given message, and outputs complete
        * sigcomp message.
        * 
        * @param aCompartmentContext compartment context
        * @param aMessage Message to compress.
        * @param aForStreamBasedProtocol indicates what kind of
        *                       protocol is used (a delimeter at
        *                       the end of message
        *                       is added based on this parameter).
        *
        * @returns complete sigcomp message, the ownership is transferred
        */ 
        CBufBase* CompressMessageL(
                              MSigCompCompartmentContext* aCompartmentContext,
                              const TDesC8& aMessage,
                              TBool aForStreamBasedProtocol);

        /**
        * This method is used to ask compressor whenever it supports
        * dynamic compression or not
        *
        * @returns ETrue if it supports dynamic compression, EFalse otherwise
        */
        TBool IsDynamicCompressionSupported();

    private:
        /** constructor */
        CDeflateComp(TAny* aInitParams);

        /** 
        * Preload given dictionary for gived CDeflate object
        *
        * @param aDictionaryState dictionary state, NULL if its not available
        * @param aDeflateComp CDeflate object for which dictionary should be
        *                     preloaded.
        * @param aDeflateContext deflate context
        *
        * @returns 1 if dictionary was preloaded, 0 if not. this value is send
        *          to the other side in compressed message header.
        *
        */
        TUint SetDictionary(const TStateItem* aDictionaryState,
                            CDeflate* aDeflateComp,
                            CSigCompDeflateContext* aDeflateContext) const;

        /** 
        * Calculate memory size in bytes based on bit representation.
        *
        * @param aMemorySize bit representation of memory size
        *                    
        * @returns TMemorySize, memory size in bytes
        */
        CSigComp::TMemorySize DecodeMemorySize(TInt aMemorySize);
    
        /** 
        * Calculate bits representation of given cycles per bit
        *
        * @param aCyclesPerBit cycles per bit (in cycles)
        *                    
        * @returns bit represenation of given cycles per bit value
        */
        TInt EncodeCyclesPerBit(CSigComp::TCyclesPerBit aCyclesPerBit) const;
    
        /** 
        * Calculate bits representation of given memory size value
        *
        * @param aMemorySize memory size (in bytes)
        *                    
        * @returns bit represenation of given memory size
        */
        TInt EncodeMemorySize(CSigComp::TMemorySize aMemorySize) const;

        /** 
        * Append given value at the end of output buffer
        *
        * @param aWriter writer to which value will be appended
        * @param aValue value to be appended
        *                    
        */
        void AppendDataL(CMessageWriter* aWriter, TUint8 aValue);

        /** 
        * Append given value at the end of output buffer
        *
        * @param aWriter writer to which value will be appended
        * @param aValue value to be appended
        *                    
        */
        void AppendDataL(CMessageWriter* aWriter, TUint16 aValue);

        /** 
        * Append given descriptor at the end of output buffer
        *
        * @param aWriter writer to which value will be appended
        * @param aValue descriptor to be appended
        *                    
        */
        void AppendDataL(CMessageWriter* aWriter, const TDesC8& aData);

        /** 
        * Append requested feedback at the end of given buffer.
        * Feedback may be NULL.
        *
        * @param aWriter writer to which value will be appended
        * @param aRequestedFeedback feedback. may be NULL.
        *                    
        */
        void AppendRequestedFeedbackL(CMessageWriter* aWriter,
                                      const TDesC8& aRequestedFeedback);

        /** 
        * Encode local parameters to returned parameters format.
        *
        * @param aCompartmentContext compartment context
        * @returns encoded parameters
        *                    
        */
        TUint8 EncodeReturnedParameters(
                              MSigCompCompartmentContext* aCompartmentContext);

        /** 
        * Decode remote returned parameters.
        *
        * @param aReturnedParameters parameters to decode
        * @param aDeflateContext deflate context
        *                    
        */
        void ParseReturnedParameters(const TDesC8& aReturnedParameters,
                                     CSigCompDeflateContext* aDeflateContext);

        /**
        * Get deflate context from compartment, allocate new and set it
        * in compartment if needed
        *
        * @param aCompartmentContext compartment context.
        *
        * @returns deflate context
        */
        CSigCompDeflateContext* DeflateContextL(
                              MSigCompCompartmentContext* aCompartmentContext);

        /** 
        * Calculate hash of bytecode state (SHA-1).
        *
        * @param aDeflateContext deflate context
        * @param aBytecode bytecode id
        * @param aReturnedParameters local encoded parameters
        *                    
        */
        void CalculateBytecodeHashL(CSigCompDeflateContext* aDeflateContext,
                                    TDeflateCompBytecodeId aBytecode,
                                    TUint8 aReturnedParameters);

        /** 
        * Append header before compressed data.
        *
        * @param aWriter writer to which value will be appended
        * @param aBytecode bytecode id
        * @param aReturnedParameters local encoded parameters
        * @param aUdvmSize udvm memory size
        * @param aFirstMessage is first message
        * @param aDeflateContext deflate context
        *                    
        */
        void AppendDataHeaderL(CMessageWriter* aWriter,
                               TDeflateCompBytecodeId aBytecode,
                               TUint8 aReturnedParameters,
                               TInt aUdvmSize,
                               TBool aFirstMessage,
                               CSigCompDeflateContext* aDeflateContext);

        /** 
        * Calculate circular buffer size.
        *
        * @param aUdvmSize udvm memory size
        * @param aBytecode bytecode id
        * @param aDeflateContext deflate context
        *
        * @returns calculated size
        *                    
        */
        TInt CalcCBSizeL(TInt& aUdvmSize,
                         TDeflateCompBytecodeId aBytecode,
                         CSigCompDeflateContext* aDeflateContext);

    private:
        /** StateMgr Interace */
        MSigCompStateMgr* iStateMgr;

        /** user provided state item, dictionaru usually, may be NULL*/
        TStateItem* iStateItem;

        /** length of the user provided state item */
        TInt iStateItemLength;

        /** upload state? */
        TBool iUploadState;

    };  

#endif
