/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TSipApplication.inl
* Part of       : SIP / SIP Client Resolver / Resolver
* Version       : SIP 6.0
*
*/



#ifndef TSIPCLIENT_INL
#define TSIPCLIENT_INL

// ---------------------------------------------------------------------------
// TSipClient::TSipClient 
// ---------------------------------------------------------------------------
//
inline TSipClient::TSipClient( const TUid& aUid,
                               TBool aAllowStarting,
                               TBool aRomBased)
: iUid(aUid), 
  iAllowStarting(aAllowStarting),
  iRomBased(aRomBased),
  iImplementationUid( TUid::Null() ),
  iPluginType( EFirstGeneration )
    {
    }

// ---------------------------------------------------------------------------
// TSipClient::Uid 
// ---------------------------------------------------------------------------
//
inline const TUid& TSipClient::Uid() const
    {
    return iUid;
    }
    
// ---------------------------------------------------------------------------
// TSipClient::SetUid 
// ---------------------------------------------------------------------------
//
inline void TSipClient::SetUid( TUid aUid )
    {
    iUid.iUid = aUid.iUid;
    }
    
// ---------------------------------------------------------------------------
// TSipClient::AllowStarting 
// ---------------------------------------------------------------------------
//
inline TBool TSipClient::AllowStarting() const
    {
    return iAllowStarting;
    }

// ---------------------------------------------------------------------------
// TSipClient::SetAllowStarting 
// ---------------------------------------------------------------------------
//
inline void TSipClient::SetAllowStarting( TBool aAllowStarting )
    {
    iAllowStarting = aAllowStarting;
    }

// ---------------------------------------------------------------------------
// TSipClient::RomBased 
// ---------------------------------------------------------------------------
//
inline TBool TSipClient::RomBased() const
    {
    return iRomBased;
    }

// ---------------------------------------------------------------------------
// TSipClient::SetRomBased 
// ---------------------------------------------------------------------------
//
inline void TSipClient::SetRomBased( TBool aRomBased )
    {
    iRomBased = aRomBased;
    }

// ---------------------------------------------------------------------------
// TSipClient::SetPluginType
// ---------------------------------------------------------------------------
//
inline void TSipClient::SetPluginType(TPluginType aPluginType)
    {
    iPluginType = aPluginType;
    }

// ---------------------------------------------------------------------------
// TSipClient::PluginType
// ---------------------------------------------------------------------------
//
inline TSipClient::TPluginType TSipClient::PluginType() const
    {
    return iPluginType;
    }

// ---------------------------------------------------------------------------
// TSipClient::ImplementationUid
// ---------------------------------------------------------------------------
//
inline TUid TSipClient::ImplementationUid() const
	{
	return iImplementationUid;
	}

// ---------------------------------------------------------------------------
// TSipClient::SetImplementationUid
// ---------------------------------------------------------------------------
//
inline void TSipClient::SetImplementationUid( TUid aUid )
	{
	iImplementationUid = aUid;
	}

#endif // TSIPCLIENT_INL

// End of File
