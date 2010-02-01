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



#ifndef MSRTPCRYPTOHANDLERCONTEXTRTCP_H
#define MSRTPCRYPTOHANDLERCONTEXTRTCP_H

#include <e32std.h>
#include "msrtpcryptohandlercontext.h"


// CLASS DECLARATION
/*
* A context for stream classes for using RTCP-specific crypto handling operations.
*
*  @lib n/a
*/
class MSRTPCryptoHandlerContextRTCP: public MSRTPCryptoHandlerContext
	{	
public: // New functions  
       
       /*
        * Determines the index i
        * of an SRTCP packet by incrementing one on sender's side.         
        * @return void        
        */
	    virtual void CountSenderPacketIndexL() = 0;	    
 
       /*
        * Initial the index i
        * of an SRTCP packet with initial packet index on sender's side.         
        * @return void        
        */		
 		virtual void InitialPacketIndex()=0;
        
       /*
        * Implements pseudocode on RFC 3711 Appendix A
        * for determining the index i of an SRTP packet with given sequence number SEQ
        * and current s_l value in receiver side
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */		
 		virtual void ReceiverPacketIndexL()	=0;    
	};
#endif  //MSRTPCryptoHandlerContextRTCP_H
