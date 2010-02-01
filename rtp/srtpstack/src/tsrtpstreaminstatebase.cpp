/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    implementation
*
*/




#include "tsrtpstreaminstatebase.h"
#include "msrtpstreamincontext.h"

// ----------------------------------------------------------------------------
// TSRTPStreamInStateBase::TSRTPStreamInStateBase
// ----------------------------------------------------------------------------
//
TSRTPStreamInStateBase::TSRTPStreamInStateBase(
    MSRTPStreamInContext& aStreamContext,
    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext)
    : TSRTPStreamStateBase(aCryptoHandlerRTPContext, aCryptoHandlerRTCPContext),
      iStreamContext(aStreamContext)
    {
    }
    
