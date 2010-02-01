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
// Name        : SigCompCompressionContext.cpp
// Part of     : SigComp
// SigComp compressor ontext
// Version     : 1.0
//



// INCLUDE FILES
#include "SigCompCompressionContext.h"


// ============================ MEMBER FUNCTIONS ==============================
EXPORT_C void CSigCompCompressionContext::SetContext(CBufBase* aContext)
    {
    delete iContext;
    iContext = aContext;
    }

EXPORT_C const CBufBase* CSigCompCompressionContext::Context() const
    {
    return iContext;
    }

EXPORT_C CSigCompCompressionContext::~CSigCompCompressionContext()
    {
    delete iContext;
    }

EXPORT_C CSigCompCompressionContext::CSigCompCompressionContext()
    {
    iContext = NULL;
    }
