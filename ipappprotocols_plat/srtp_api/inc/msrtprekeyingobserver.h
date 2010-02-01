/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Provides a callback for re-keying events
*
*/




#ifndef __MSRTP_REKEYING_OBS_H__
#define __MSRTP_REKEYING_OBS_H__

#include "srtpmasterkey.h"

class CSRTPStream;
class CSRTPSession;

/**
* Provides a callback clients.
*/
class MSRTPReKeyingObserver
    {
    public:
       /**
        * This function is called by SRTP Stream initiated with 
        * MSRTPReKeyingObserver when a master key is stale and needs 
        * to be refreshed.  
        * Re-Key will be triggered when SRTP index is (2^48)-1 and
        * SRTCP index is (2^31)-1
        * @param aStream Stream which master key has to be refreshed
        */
        virtual void SRTPMasterKeyStaleEvent(const CSRTPStream& aStream)=0;
        
       /**
        * This function is called by SRTPSRTP Stream initiated with 
        * CSRTPSession  when a master key is stale and
        * needs to be refreshed. 
        * * @param aSession Session which master key has to be refreshed
        */
        virtual void SRTPMasterKeyStaleEvent(const CSRTPSession& aSession)=0;
        
    };

#endif // __MSRTP_REKEYING_OBS_H__
