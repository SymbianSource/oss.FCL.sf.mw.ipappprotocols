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

#ifndef CMSRPWRITER_H
#define CMSRPWRITER_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MMSRPWriter.h"
#include "MMSRPWriterObserver.h"


class MMSRPWriterObserver;
class MMSRPWriterErrorObserver;

// CLASS DECLARATIONS
/**
 * The socket reader request interface.
 */

class CMSRPWriter : public CActive, public MMSRPWriter
    {
 public:

     // Constructors and destructor
     static MMSRPWriter* NewL(RSocket& aSocket, MMSRPWriterErrorObserver& aConnection );
     virtual ~CMSRPWriter();
      
     virtual void RequestSendL(MMSRPWriterObserver& aMsg);
     
 protected: // from CActive
     void DoCancel();
     void RunL();
     TInt RunError(TInt aError);
    
 private:
     CMSRPWriter(RSocket& aSocket, MMSRPWriterErrorObserver& aConnection);
     void ConstructL();
     void SendL();

 private: // data
     MMSRPWriterErrorObserver& iConnection;  
     RSocket& iSocket;     
     RPointerArray<MMSRPWriterObserver> iSendQueue;
     TBool iWriteIssued;
    };     

#endif // CMSRPWRITER_H

// End of file
