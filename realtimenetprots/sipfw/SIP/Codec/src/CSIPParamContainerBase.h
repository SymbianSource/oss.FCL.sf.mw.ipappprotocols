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
* Name          : CSIPParamContainerBase.h
* Part of       : SIP Codec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPPARAMCONTAINERBASE_H
#define CSIPPARAMCONTAINERBASE_H

#include <e32base.h>
#include "CSIPParam.h"


class CSIPParamContainerBase : public CBase
	{
public:

	~CSIPParamContainerBase ();

	void SetParamL(RStringF aName);
	void SetParamL(RStringF aName, RStringF aValue);
	void SetParamL(RStringF aName, const TDesC8& aValue);
	void SetDesParamL(RStringF aName, const TDesC8& aValue);	
	void SetParamL(RStringF aName, TInt aValue);
	void SetParamL(RStringF aName, TReal aValue);
	void SetParamL(CSIPParam* aParam);
	TBool HasParam(RStringF aParamName) const;
	RStringF ParamValue(RStringF aParamName) const;
	const TDesC8& DesParamValue(RStringF aParamName) const;
	TInt IntParamValue(RStringF aParamName) const;
	TReal RealParamValue(RStringF aParamName) const;
	void DeleteParam(RStringF aParamName);
	TInt ParamCount() const;
	TInt ParamName(TInt aIndex, RStringF& aParamName) const;
	
	void ParseL(const TDesC8& aValue);
	HBufC8* ToTextLC() const;
	void ExternalizeL(RWriteStream& aWriteStream);

protected:

	CSIPParamContainerBase(const TChar& aSeparator);
	
	void ConstructL(const CSIPParamContainerBase& aContainer);
	
	void DoInternalizeL(RReadStream& aReadStream);
	
	TInt FindParamIndex(RStringF aParamName) const;
    
    void CheckGenericParamL(CSIPParam& aParam,
	                        TInt aLeaveCode) const;    
    
    void CheckGenericParamL(RStringF aName,
	                        TBool aHasValue,
	                        const TDesC8& aValue,
	                        TInt aLeaveCode) const;
    
    void RemoveQuotes(TPtrC8& aParamValue) const;

protected:

    virtual TBool AddQuotesWhenEncoding(RStringF aParamName) const;

	virtual void CheckParamL(CSIPParam& aParam) const;
	
	virtual void CheckAndUpdateParamL(RStringF aName,
	                                  TBool aHasValue,
	                                  TPtrC8& aValue) const;
	                                  
protected: // Data

	RPointerArray<CSIPParam> iParams;

private:

	// Needed for cleanup of a RPointerArray<HBufC8>:
	static void ResetAndDestroy (TAny* anArray);
	
	TChar iParamSeparator;
	};

#endif // CSIPPARAMCONTAINERBASE_H

// End of File
