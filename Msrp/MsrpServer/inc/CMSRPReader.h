/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef CMSRPREADER_H
#define CMSRPREADER_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MMSRPReader.h"
#include "MMSRPReaderObserver.h"
#include "MSRPBuffer.h"

class MMSRPReaderObserver;
// CLASS DECLARATIONS
/**
 * The socket reader request interface.
 */

class CMSRPReader : public CActive, public MMSRPReader
    {
 public:

     // Constructors and destructor
     static MMSRPReader* NewL(RSocket& aSocket, MMSRPReaderObserver& aConnection);
     virtual ~CMSRPReader();
      
     virtual void StartReceivingL(RMsrpBuf aBuf);
     
 protected: // from CActive
     void DoCancel();
     void RunL();
     TInt RunError(TInt aError);
    
 private:
     CMSRPReader(RSocket& aSocket, MMSRPReaderObserver& aConnection);
     void ConstructL();     

 private: // data
     MMSRPReaderObserver& iConnection;  
     RSocket& iSocket;    
     TSockXfrLength iNumberOfBytesRead;
     RMsrpBuf* iBuf;
    };
     
 

#endif // CMSRPREADER_H

// End of file
