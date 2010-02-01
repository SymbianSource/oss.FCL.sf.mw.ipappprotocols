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
// RTP CfMessages Definitions.
// 
//

/**
 @file
 @internalComponent
*/


#ifndef __RTP_CFMESSAGES__H
#define __RTP_CFMESSAGES__H

#include <elements/nm_common.h>
#include <elements/nm_signatures.h>
#include "rtp_uid.h"

class TRtpMessages
/**
RTP message realm (messages specific to RTP layer)

@internalComponent
*/
	{	
  public:
    enum { ERealmId = KRtpScprFactoryImplementationUid }; //RTP SCPR Implementation UID
    
  	enum 
  	    {	
        ERtpMessageReadyForBind = Messages::KNullMessageId + 1,
        ERtpMessageCreateRtp,
        ERtpMessageCreateRtcp,
	    };
	
  public:
	
	typedef Messages::TMessageSigNumber<ERtpMessageReadyForBind, TRtpMessages::ERealmId> TRtpMessageReadyForBind;
	};

#endif //__RTP_CFMESSAGES__H
