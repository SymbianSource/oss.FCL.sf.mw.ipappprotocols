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

#ifndef CMSRPLISTENERBASE_H_
#define CMSRPLISTENERBASE_H_

// EXTERNAL INCLUDES

// CONSTANTS

// FORWARD DECLARATIONS
class CMSRPSessionImplementation;
class MMSRPServerInterface;

// CLASS DECLARATION
/**
* Class provides a generic base class for all listeners
*/
class CMSRPListenerBase : public CActive
    {
public: // Constructors and destructors

    /**
     * Destructor, deletes the resources of CMSRPListenerBase.
     */
    virtual ~CMSRPListenerBase();

protected: // Constructors

    /**
     * Constructor
     */
    CMSRPListenerBase( CMSRPSessionImplementation& aSessionImpl, 
            MMSRPServerInterface& aServerInterface, 
            TInt aPriority );

protected: // data

    CMSRPSessionImplementation& iSessionImpl;

    // interface to MSRP server
    MMSRPServerInterface& iServerInterface;
    };


#endif /* CMSRPLISTENERBASE_H_ */
