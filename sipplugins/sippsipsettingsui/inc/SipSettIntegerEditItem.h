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
* Description:  SIP Settings view controller
*
*/
#ifndef SIP_SETT_INTEGERED_ITITEM_H
#define SIP_SETT_INTEGERED_ITITEM_H

#include <aknsettingitemlist.h>

// CLASS DECLARATION

class CSipSettIntegerEditItem : public CAknTextSettingItem
    {
    public:
        /**
        * Constructor, passes the parameters to CSipSettIntegerEditItem's
        * constructor
        */
    	CSipSettIntegerEditItem(
    	    TInt aIdentifier, TDes & aInteger);    

        /**
        * Destructor
        */
        virtual ~CSipSettIntegerEditItem();

        /**
        * Updates the list box always after displaying popup list
        */
        void EditItemL( TBool aCalledFromMenu );
    public:
    	/**
    	* Store the TInt value that inputted by end user 
    	*/
    	TInt iValue;
       
    };
#endif /* SIPSETTINTEGEREDITITEM_H_ */
