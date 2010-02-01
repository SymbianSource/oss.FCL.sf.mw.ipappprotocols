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
* Name        : CDeflateCompBytecode.h
* Part of     : deflatecomp / deflatecomp
* This file contains definitions for deflate bytecodes
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CDEFLATECOMPBYTECODE_H
#define CDEFLATECOMPBYTECODE_H


/** bytecode parameters */
class TDeflateCompBytecode
    {
    public:
        const TUint8* iBytecode;
        TUint iBytecodeSize;
        TUint iBytecodeInit;
        TUint iBytecodeStart;
        TUint iStateStart;
        TUint iStateSize;
        TUint iCBufStart;
    };

/** bytecodes ids */
enum TDeflateCompBytecodeId
    {
        EStaticBytecode,
        EStaticUploadBytecode,
        EDynamicBytecode,
        EDynamicUploadBytecode
    };

/** deflate bytecodes */
extern const TDeflateCompBytecode KDeflateBytecodes[];

#endif
