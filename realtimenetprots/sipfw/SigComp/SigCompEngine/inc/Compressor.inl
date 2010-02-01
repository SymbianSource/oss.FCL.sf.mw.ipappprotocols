/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : Compressor.inl
* Part of     : deflatecomp
* inline functions for compressor plugins.
* Version     : 1.0
*
*/



inline CSigCompCompressor::CSigCompCompressor()
    {
    }

inline CSigCompCompressor::~CSigCompCompressor()
    {
    REComSession::DestroyedImplementation(iUniqueKey);
    }

inline CSigCompCompressor* CSigCompCompressor::NewL(
                                                   const TDesC8& aMatchString,
                                                   MSigCompStateMgr* aStateMgr,
                                                   TStateItem* aStateItem,
                                                   TInt aStateItemLength,
                                                   TBool aUploadState)
    {
    TCompressorInitParams initParams;
    initParams.iStateMgr = aStateMgr;
    initParams.iStateItem = aStateItem;
    initParams.iStateItemLength = aStateItemLength;
    initParams.iUploadState = aUploadState;

    /* Set up the interface find for the default resolver. */
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatchString);
    resolverParams.SetWildcardMatch(ETrue);
    return reinterpret_cast<CSigCompCompressor*>
        (REComSession::CreateImplementationL(
            KCompressorInterfaceUid,
            _FOFF(CSigCompCompressor, iUniqueKey), 
            &initParams,
            resolverParams,
            KRomOnlyResolverUid));
    }

inline CSigCompCompressor* CSigCompCompressor::NewL(
                                                   const TDesC8& aMatchString,
                                                   MSigCompStateMgr* aStateMgr)
    {
    TCompressorInitParams initParams;
    initParams.iStateMgr = aStateMgr;
    initParams.iStateItem = NULL;
    initParams.iStateItemLength = 0;
    initParams.iUploadState = EFalse;

    /* Set up the interface find for the default resolver. */
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatchString);
    resolverParams.SetWildcardMatch(ETrue);
    return reinterpret_cast<CSigCompCompressor*>
        (REComSession::CreateImplementationL(
            KCompressorInterfaceUid,
            _FOFF(CSigCompCompressor, iUniqueKey), 
            &initParams,
            resolverParams,
            KRomOnlyResolverUid));
    }
