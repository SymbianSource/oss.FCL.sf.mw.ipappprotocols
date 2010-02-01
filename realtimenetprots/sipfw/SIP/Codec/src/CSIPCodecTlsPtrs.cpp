// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPCodecTlsPtrs.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "CSIPCodecTlsPtrs.h"
#include "CSIPStrings.h"
#include "CSIPHeaderLookupTable.h"

// ----------------------------------------------------------------------------
// CSIPCodecTlsPtrs::CSIPCodecTlsPtrs
// ----------------------------------------------------------------------------
//
CSIPCodecTlsPtrs::CSIPCodecTlsPtrs(CSIPStrings* aStrings)
 : iStrings(aStrings)
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPCodecTlsPtrs::CSIPCodecTlsPtrs
// ----------------------------------------------------------------------------
//	
CSIPCodecTlsPtrs::CSIPCodecTlsPtrs(CSIPHeaderLookupTable* aHeaderLookup) 
 : iHeaderLookup(aHeaderLookup)
 	{
 	}	

// ----------------------------------------------------------------------------
// CSIPCodecTlsPtrs::~CSIPCodecTlsPtrs
// ----------------------------------------------------------------------------
//
CSIPCodecTlsPtrs::~CSIPCodecTlsPtrs()
	{
	delete iStrings;
	delete iHeaderLookup;
	}

// ----------------------------------------------------------------------------
// CSIPCodecTlsPtrs::DestroyStrings
// ----------------------------------------------------------------------------
//	
void CSIPCodecTlsPtrs::DestroyStrings()
	{
	delete iStrings;
	iStrings = NULL;	
	}
	
// ----------------------------------------------------------------------------
// CSIPCodecTlsPtrs::DestroyHeaderLookup
// ----------------------------------------------------------------------------
//	
void CSIPCodecTlsPtrs::DestroyHeaderLookup()
	{
	delete iHeaderLookup;
	iHeaderLookup = NULL;	
	}

