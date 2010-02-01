/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name          : MSipCellularDataUsageObserver.h
* Part of       : ConnectionMgr
* See class definition below.
*/

/**
 @internalComponent
*/

#ifndef MSIPCELLULARDATAUSAGEOBSERVER_H
#define MSIPCELLULARDATAUSAGEOBSERVER_H

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION
/**
*/
class MSipCellularDataUsageObserver
    {
    public: // Abstract methods
    enum TCellularDataUsageStatus
            {
            EDataUsageAllowed,
            EDataUsageNotAllowed		     
            };

        /**
         * Called when the settings for Cellular Data usage is Updated.
         */
     virtual void CellularDataUsageChangedL(MSipCellularDataUsageObserver::TCellularDataUsageStatus aStatus) = 0;
    };

#endif MSIPCELLULARDATAUSAGEOBSERVER_H
