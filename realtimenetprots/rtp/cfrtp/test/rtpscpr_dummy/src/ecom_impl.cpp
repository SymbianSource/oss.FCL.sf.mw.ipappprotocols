// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ECOM implementation for the Tunnel provider factories
// 
//

/**
 @file
 @internalComponent
*/

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

//RTP SCPR factories
#include "dummy_rtpscprfactory.h"
#include "dummy_rtp_subconparams.h"

//
// ECOM Implementation
//

/*  RTP ECOM Plugin */

const TImplementationProxy ImplementationTable[] =
   {
   IMPLEMENTATION_PROXY_ENTRY(CRtpDummySubConnectionProviderFactory::iUid, CRtpDummySubConnectionProviderFactory::NewL),
   IMPLEMENTATION_PROXY_ENTRY(KDummySubConnRTPParamsImplUid, CSubConDummyRTPParamFactory::NewL),	  
   };
   

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
   {
   aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
   return ImplementationTable;
   }
   
