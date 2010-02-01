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




#ifndef MSRTPSTREAMOUTCONTEXT_H
#define MSRTPSTREAMOUTCONTEXT_H

#include <e32std.h>


// CLASS DECLARATION
/**
* A context for CSRTPStreamOut states. Defines the states for outgoing stream.
*
*  @lib n/a
*/
class MSRTPStreamOutContext
	{
	public: // Enumerations
	
       enum TContextOutState 
            {
            /* Not initialized, no calls made for this stream yet,  */         
			ESRTPStreamOutUninitialized=0,
            /** Normal, stream initialized, use the SSRC given from user */
			ESRTPStreamOutNormal=1,
			/** Not a real state. Upper limit for the number of states */
			EMaxStates=2
          	};
	
    public: // New functions    
                
        /*
        * Virtual function for changing state of the current 
        * sender stream (for RTP)
        * Implemented in CSRTPStreamOut
        * @param aNewState new RTP state which stream is changed to
        */
        virtual void ChangeRTPState(TContextOutState aNewState) = 0;

        /*
        * Virtual function for changing state of the current 
        * sender stream (for RTCP)
        * Implemented in CSRTPStreamOut
        * * @param aNewState new RTCP state which stream is changed to
        */
        virtual void ChangeRTCPState(TContextOutState aNewState) = 0;
        
	};
#endif  //MSRTPStreamOutContext_H
