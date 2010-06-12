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

//  Include Files
#include "MsrpCommon.h"
#include "RMSRP.h"


//  Member Functions

RMSRP::RMSRP() : 
    RSessionBase()
    {
    MSRPLOG("RMSRP.. Ctor");
    }

TInt RMSRP::CreateServerSession()
    {
    MSRPLOG("RMSRP.. ");
    TInt result = StartServer();
    if (( result == KErrNone) || (result == KErrAlreadyExists))
        {
        MSRPLOG("RMSRP.. Create client-server session");
        result = CreateSession( KMSRPServerName, Version() );
        }

    return result;
    }

TVersion RMSRP::Version() const
    {
    
    MSRPLOG("RMSRP.. Get version");
    
    return TVersion(
        KMSRPServerMajorVersionNumber,
        KMSRPServerMinorVersionNumber,
        KMSRPServerBuildVersionNumber );
    }

TInt RMSRP::StartServer( ) const
    {
    
    MSRPLOG("RMSRP.. Start transient server");
    
    RProcess p; // codescanner::resourcenotoncleanupstack
    TRequestStatus status;

    TBuf< KSizeOfProgramPath > serverPath;
    serverPath.Append( KMSRPExe );

    TInt err = p.Create( serverPath, KNullDesC );
    if ( err == KErrAlreadyExists )
        {
        return KErrNone;
        }

    if ( err != KErrNone )
        {
        return err;
        }

    // wait for server
    p.Rendezvous( status );

    if ( status != KRequestPending )
        {
        p.Kill(0); // abort startup
        p.Close();
        return KErrGeneral;
        }
    else
        {
        p.Resume(); // logon OK - start the server
        }

    User::WaitForRequest( status ); // codescanner::userWaitForRequest
    
    if (p.ExitType()==EExitPanic)
        {
        status = KErrServerTerminated;
        }
  
    if ( status != KErrNone )
        {
        p.Close();
        return status.Int();
        }
    p.Close();

    return KErrNone;
    }

