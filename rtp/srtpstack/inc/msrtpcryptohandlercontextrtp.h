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



#ifndef MSRTPCRYPTOHANDLERCONTEXTRTP_H
#define MSRTPCRYPTOHANDLERCONTEXTRTP_H

#include <e32std.h>
#include "msrtpcryptohandlercontext.h"

// CLASS DECLARATION
/*
* A context for stream classes for using RTP-specific crypto handling operations.
*
*  @lib n/a
*/
class MSRTPCryptoHandlerContextRTP: public MSRTPCryptoHandlerContext
	{	
    public: // New functions  
        /*
        * Determines the index i
        * of an SRTP packet with given sequence number SEQ on sender's side.         
        * @return void        
        */
	    virtual void CountSenderPacketIndexL() = 0;	    

        /*
        * Implements pseudocode on RFC 3711 Appendix A
        * for determining the index i of an SRTP packet with given sequence number SEQ
        * and current s_l value in receiver side
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */
        virtual void CountReceiverPacketIndexL() = 0;
        
        /*
        * Conditionally update ROC in sender's side:
        * Each time SEQ wraps MOD 2^16, the sender side must 
        * increment ROC by one MOD 2^32.
        * @return void                
        */
        virtual void UpdateROC() = 0;
                
        /*
        * Conditionally update s_l and ROC based on v value. 
        * See RFC 3711 section 3.3.1 for more information
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void                
        */
        virtual void Update_s_l_and_RocL() = 0;

        /*
        * Assign receiver counter s_l to the value of first sequence number. 
        * @return void                
        */
        virtual void SetReceiver_s_l() = 0;
        
	};
#endif  //MSRTPCryptoHandlerContextRTP_H
