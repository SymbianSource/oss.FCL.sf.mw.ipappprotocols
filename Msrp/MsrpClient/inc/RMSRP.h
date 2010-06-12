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

#ifndef RMSRP_H_
#define RMSRP_H_


// Include Files

#include <e32base.h>
#include <e32std.h>

// Forward Declarations

// Class declaration

class RMSRP : public RSessionBase
    {
public:
    /**
      * constructor.
      */
    RMSRP();
    
public:
    /**
      * Opens a Session to MSRP Server
      * @return Status of the operation
      */
    TInt CreateServerSession();

    /**
      * The client API version.
      */
    TVersion Version() const;
    
private:
    /**
      * Starts the MSRP server
      * @return Status indicating whether server was started
      */
    TInt StartServer() const;
    
private:
    
    };


#endif /* RMSRP_H_ */
