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




#ifndef MSRTPSTREAMINCONTEXT_H
#define MSRTPSTREAMINCONTEXT_H

#include <e32std.h>


// CLASS DECLARATION
/**
* A context for CSRTPStreamIn states. Defines the states for incoming stream.
*
*  @lib n/a
*/
class MSRTPStreamInContext
	{
	public: // Enumerations
	
        /** Subscription states */
        enum TContextInState 
            {
            /* Not Initialized, no calls made for this stream yet,  */
			ESRTPStreamInUninitialized=0,
			/**  Use "late binding" of SSRC, that is, */
			/** read SSRC from the packet */
			ESRTPStreamInLateBinding=1,			          
            /** Normal, stream initialized, use the SSRC given from user */
			ESRTPStreamInNormal=2,
			/** Not a real state. Upper limit for the number of states */
			EMaxStates=3
            };
	
    public: // New functions
                       
        /*
        * Virtual function for changing state of the current 
        * receiver stream (for RTP)
        * Implemented in CSRTPStreamIn
        * @param aNewState new RTP state which stream is changed to
        */
        virtual void ChangeRTPState(TContextInState aNewState) = 0;

        /*
        * Virtual function for changing state of the current 
        * receiver stream (for RTCP)
        * Implemented in CSRTPStreamIn
        * * @param aNewState new RTCP state which stream is changed to
        */
        virtual void ChangeRTCPState(TContextInState aNewState) = 0;
        
	};
#endif  //MSRTPSTREAMINCONTEXT_H
