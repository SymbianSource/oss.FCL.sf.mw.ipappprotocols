/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header file for definitions related to RTP
*              : (Real-time Transport Protocol) library
*
*/




#ifndef __RTPASSIGNUNIQUEID_H
#define __RTPASSIGNUNIQUEID_H

//  INCLUDES

#include <e32std.h>
#include <in_sock.h>
#include <es_sock.h>
#include  "rtpdef.h"

class MRtpAsignUniqueID
	{
	public:
	    
	    /**
	    * Get Unique StreamID from RTPmanager
	    * @param None
	    */
	    virtual TRtpId AssignUniqueID()= 0;
	    
	    /**
	    * Used when creating Rx stream in session, call RtpManager to add stream
	    * To session
	    * @param None
	    */
	    virtual TInt AddStreamToSession(TRtpId aSessionId, TRtpId aStreamId)=0;
	};

#endif /* __RTPASSIGNUNIQUEID_H*/
