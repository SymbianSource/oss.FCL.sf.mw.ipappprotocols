/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilequeueitem.inl
* Part of     : sip profile agent
* inline functions
* Version     : 1.0
* ============================ MEMBER FUNCTIONS ===============================
* TSIPProfileQueueItem::TSIPProfileQueueItem
*
*/



inline TSIPProfileQueueItem::TSIPProfileQueueItem(
	CSIPConcreteProfile& aConcreteProfile,
	TBool aRetryRegistration) :
    iConcreteProfile(aConcreteProfile),
    iRetryRegistration(aRetryRegistration)
	{
	}
