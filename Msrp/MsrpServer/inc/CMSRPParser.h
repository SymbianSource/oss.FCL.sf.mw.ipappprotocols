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

#ifndef CMSRPPARSER_H
#define CMSRPPARSER_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MMSRPParser.h"
#include "MMSRPParserObserver.h"
#include "MSRPBuffer.h"
#include "CMSRPMsgParser.h"

class MMSRPParserObserver;
// CLASS DECLARATIONS
/**
 * The socket reader request interface.
 */

class CMSRPParser : public CActive, public MMSRPParser
    {
 public:

     // Constructors and destructor
     static MMSRPParser* NewL(MMSRPParserObserver& aConnection);
     virtual ~CMSRPParser();
      
     virtual void ParseL(RMsrpBuf& aBuf);
     
 protected: // from CActive
     void DoCancel();
     void RunL();
     TInt RunError(TInt aError);
    
 private:
     CMSRPParser(MMSRPParserObserver& aConnection);
     void ConstructL(MMSRPParserObserver& aConnection);     

 private: // data
     MMSRPParserObserver& iConnection;       
     //RPointerArray<RMsrpBuf> iParseBuffers;
     CMSRPMsgParser* iParser;
    };
     
 

#endif // CMSRPPARSER_H

// End of file
