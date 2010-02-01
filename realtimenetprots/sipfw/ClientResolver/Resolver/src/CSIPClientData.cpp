// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPClientData.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPClientData.h"
#include "CSIPFeatureSet.h"
#include "sdpdocument.h"
#include "sipheaderbase.h"

// -----------------------------------------------------------------------------
// CSIPClientData::NewL
// -----------------------------------------------------------------------------
//
CSIPClientData* CSIPClientData::NewL( const TUid& aImplementationUid,
                                      const TUid& aClientUid,
                                      TBool aRomBased,
		                              TBool aHasDynamicCapabilities )
	{
	CSIPClientData* self = NewLC( aImplementationUid, aClientUid,
	                              aRomBased, aHasDynamicCapabilities);
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::NewLC
// -----------------------------------------------------------------------------
//
CSIPClientData* CSIPClientData::NewLC( const TUid& aImplementationUid,
                                       const TUid& aClientUid,
                                       TBool aRomBased,
		                               TBool aHasDynamicCapabilities )
	{
	CSIPClientData* self = 
	    new( ELeave ) CSIPClientData( aImplementationUid, aClientUid,
	                                  aRomBased, aHasDynamicCapabilities );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::CSIPClientData
// -----------------------------------------------------------------------------
//
CSIPClientData::CSIPClientData( const TUid& aImplementationUid,
                                const TUid& aClientUid,
                                TBool aRomBased,
                                TBool aHasDynamicCapabilities )
    : iImplementationUid( aImplementationUid ),
      iClientUid( aClientUid ),
      iRomBased( aRomBased ),
      iHasDynamicCapabilities( aHasDynamicCapabilities )
	{
	}

// -----------------------------------------------------------------------------
// CSIPClientData::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPClientData::ConstructL()
	{
	iSdpDocument = CSdpDocument::NewL();
	iSIPFeatureSet = CSIPFeatureSet::NewL();
	}

// -----------------------------------------------------------------------------
// CSIPClientData::~CSIPClientData
// -----------------------------------------------------------------------------
//
CSIPClientData::~CSIPClientData()
	{
	iSIPHeaders.ResetAndDestroy();
	delete iSIPFeatureSet;
	delete iSdpDocument;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::ClientUid
// -----------------------------------------------------------------------------
//
const TUid& CSIPClientData::ClientUid() const
	{
	return iClientUid;
	}
	
// -----------------------------------------------------------------------------
// CSIPClientData::RomBased
// -----------------------------------------------------------------------------
//
TBool CSIPClientData::RomBased() const
	{
	return iRomBased;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::HasDynamicCapabilities
// -----------------------------------------------------------------------------
//	
TBool CSIPClientData::HasDynamicCapabilities() const
    {
    return iHasDynamicCapabilities;
    }

// -----------------------------------------------------------------------------
// CSIPClientData::ImplementationUid
// -----------------------------------------------------------------------------
//    
const TUid& CSIPClientData::ImplementationUid() const
    {
    return iImplementationUid;
    }

// -----------------------------------------------------------------------------
// CSIPClientData::AllowStarting
// -----------------------------------------------------------------------------
//
TBool CSIPClientData::AllowStarting() const
	{
	return iAllowStarting;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::SIPHeaders
// -----------------------------------------------------------------------------
//
RPointerArray< CSIPHeaderBase >& CSIPClientData::SIPHeaders()
	{
	return iSIPHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::SIPFeatureSet
// -----------------------------------------------------------------------------
//	
CSIPFeatureSet& CSIPClientData::SIPFeatureSet()
    {
    return *iSIPFeatureSet;
    }

// -----------------------------------------------------------------------------
// CSIPClientData::SdpDocument
// -----------------------------------------------------------------------------
//
CSdpDocument& CSIPClientData::SdpDocument()
	{
	return *iSdpDocument;
	}
	
// -----------------------------------------------------------------------------
// CSIPClientData::SetAllowStarting
// -----------------------------------------------------------------------------
//
void CSIPClientData::SetAllowStarting( TBool aAllowStarting )
	{
	iAllowStarting = aAllowStarting;
	}

// -----------------------------------------------------------------------------
// CSIPClientData::AddMediaFieldL
// -----------------------------------------------------------------------------
//
void CSIPClientData::AddMediaFieldL( CSdpMediaField* aMediaField )
	{
    iSdpDocument->MediaFields().AppendL( aMediaField );
	}

// -----------------------------------------------------------------------------
// CSIPClientData::CloneWithoutCapabilitiesLC
// -----------------------------------------------------------------------------
//	
CSIPClientData* CSIPClientData::CloneWithoutCapabilitiesLC()
    {
    return CSIPClientData::NewLC( iImplementationUid, iClientUid, iRomBased,
                                  iHasDynamicCapabilities );
    }
