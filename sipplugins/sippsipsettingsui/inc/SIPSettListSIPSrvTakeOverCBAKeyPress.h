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
* Description:  The proxy setting view
*
*/



#ifndef SIP_SETT_LIST_SIP_SRV_TAKE_OVER_CBA_KEY_PRESS 
#define SIP_SETT_LIST_SIP_SRV_TAKE_OVER_CBA_KEY_PRESS
#include <coecntrl.h>

class CSIPSettListSIPSrvTakeOverCBAKeyPress : public CCoeControl
	{
	public:	// Constructors and destructor
	
        /**
        * Two-phased constructor. 
        */
		static CSIPSettListSIPSrvTakeOverCBAKeyPress * NewL();

        /**
        * Two-phased constructor. Instance is placed on the cleanup stack
        * @param aHandler Profile handler for model
        */
        static CSIPSettListSIPSrvTakeOverCBAKeyPress * NewLC();

        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvTakeOverCBAKeyPress ();

	public:		// Own functions

        /**
        * Returns information is right soft key pressed
        * @return Is right soft key pressed
        */
		TBool BackCalled();

	protected: // for CCoeControl				

        /**
         * From CCoeControl, receives key events and passes them
         * to list box
         */
		virtual TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

	private:

        /**
        * C++ default constructor.        
        */
		CSIPSettListSIPSrvTakeOverCBAKeyPress ();
	
        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();

	private: // Data

        /**
         * Is right soft key pressed
         */
		TBool iBackCalled;
	
	/**
	* For testing purposes
	*/        
    friend class UT_CSIPSettListSIPSrvTakeOverCBAKeyPress;

	};	

#endif      // SIP_SETT_LIST_SIP_SRV_TAKE_OVER_CBA_KEY_PRESS
            
// End of File
