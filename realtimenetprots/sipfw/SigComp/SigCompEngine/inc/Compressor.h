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
* Name        : Compressor.h
* Part of     : deflatecomp
* CSigCompCompressor, ecom plugins framework
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIGCOMPCOMPRESSOR_H
#define SIGCOMPCOMPRESSOR_H

#include <ecom/ecom.h>
#include <badesca.h>

class MSigCompStateMgr;
class MSigCompCompartmentContext;
class TStateItem;

const TUid KCompressorInterfaceUid = {0x101F5D38};

class CSigCompCompressor : public CBase
    {
    public:
        class TCompressorInitParams
            {
            public:
                /** statemgr interface */
                MSigCompStateMgr* iStateMgr;
                /** state item user provided when starting sigcomp, dictionary
                *   usually
                */
                TStateItem* iStateItem;
                /** length user provided for the given state */
                TInt iStateItemLength;
                /** upload state? */
                TBool iUploadState;
            };

    public:
        static CSigCompCompressor* NewL(const TDesC8& aMatchString,
                                          MSigCompStateMgr* aStateMgr,
                                          TStateItem* aStateItem,
                                          TInt aStateItemLength,
                                          TBool aUploadState);

        static CSigCompCompressor* NewL(const TDesC8& aMatchString,
                                          MSigCompStateMgr* aStateMgr);

        virtual ~CSigCompCompressor();

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
        * @return complete sigcomp message, the ownership is transferred
        * @leave ECompressionFailure if aMessage.Length() == 0
        */ 
        virtual CBufBase* CompressMessageL(
                              MSigCompCompartmentContext* aCompartmentContext,
                              const TDesC8& aMessage,
                              TBool aForStreamBasedProtocol) = 0;

        /**
        * This method is used to ask compressor whenever it supports
        * dynamic compression or not
        *
        * @return ETrue if it supports dynamic compression, EFalse otherwise
        */
        virtual TBool IsDynamicCompressionSupported() = 0;

    protected:
        inline CSigCompCompressor();

    private:
        /** unique key */
        TUid iUniqueKey; 
    };

#include "Compressor.inl"

#endif 
