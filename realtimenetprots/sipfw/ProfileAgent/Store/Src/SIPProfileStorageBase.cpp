// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : sipprofilestoragebase
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



//  INCLUDE FILES
#include "SIPProfileStorageBase.h"
#include "SIPProfileStorageIndex.h"
#include "sipconcreteprofile.h"
#include <centralrepository.h>
#include <sipprofile.h>
#include <sipmanagedprofile.h>
#include "SIPProfileStoragePrivateCRKeys.h"
#include "SipProfileLog.h"

_LIT(KFullNameOfFileStore,"C:\\private\\101F413C\\sipprofiles.dat");
_LIT(KFullNameOfFileStorePath,"\\private\\101F413C\\");
_LIT(KFullNameOfFileStoreFile,"sipprofiles.dat");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::CSIPProfileStorageBase
// -----------------------------------------------------------------------------
//
CSIPProfileStorageBase::CSIPProfileStorageBase(RFs& aFs):iFs(aFs)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::BaseConstructL()
    {
    // Cenrep initialization
    iSipCenRep = CRepository::NewL(KCrUidSipProfileSrv);
    iProfileIndex = CSIPProfileStorageIndex::NewL(*iSipCenRep);    
           
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::~CSIPProfileStorageBase
// -----------------------------------------------------------------------------
//
CSIPProfileStorageBase::~CSIPProfileStorageBase()
    {
    delete iProfileIndex;
    delete iSipCenRep;
    if (iStore)
    	{
    	delete iStore;
    	iStore = 0;
    	iFs.Delete(KFullNameOfFileStore);
    	}    
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::InitializeFileStoreL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::InitializeFileStoreL()
	{
	PROFILE_DEBUG1("CSIPProfileStorageBase::InitializeFileStoreL : Start")
	// File store initialization
    TParse filestore;
    TBool fileOK = EFalse;

	User::LeaveIfError(iFs.Parse(KFullNameOfFileStore,filestore));
    TFindFile filefinder(iFs);
    TInt err =  
        filefinder.FindByDir(KFullNameOfFileStoreFile,KFullNameOfFileStorePath);
    if (err == KErrNone)
		{
		TEntry entry;
		User::LeaveIfError(iFs.Entry(KFullNameOfFileStore, entry));
		if (entry.iSize != 0)
			{
			fileOK = ETrue;
			}
		}
		if (err != KErrNone || !fileOK)
		{
			// Need to check about the possiblity of different errors
			return; 
		}
		else
		{
			iStore = 
            CPermanentFileStore::OpenL(iFs,filestore.FullName(),EFileWrite);
		}
		
		__ASSERT_ALWAYS (iStore->Root() != 0, User::Leave (KErrCorrupt));
		iProfileIndex->SetPermanentStoreObjectL(iStore); 
	PROFILE_DEBUG1("CSIPProfileStorageBase::InitializeFileStoreL : End")
	}


// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::SIPProfilesL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPConcreteProfile>* CSIPProfileStorageBase::SIPProfilesL(TBool FromStore)
    {
	
    if (!FromStore)
    	{
		PROFILE_DEBUG1("Getting Profiles From CenRep")
    	// Gett the profile from cenrep file
    	const RArray<TUint32>& profiles = iProfileIndex->Profiles();
    	RPointerArray<CSIPConcreteProfile>* array = 
        	new (ELeave) RPointerArray<CSIPConcreteProfile>(1);
    
	    CleanupStack::PushL(array);
    	CleanupStack::PushL(TCleanupItem(ResetAndDestroyProfile,array));

    	for (TInt i = 0; i < profiles.Count(); i++)
        	{
        	CSIPConcreteProfile* profile = SIPProfileL(profiles[i]);
        	CleanupStack::PushL(profile);
        	array->AppendL(profile);
        	CleanupStack::Pop(profile);
        	}

    	CleanupStack::Pop(2); //cleanup & array
    	return array;
    	}
    	else
    	{
    	// Get the profiles from sipprofiles.dat file
		PROFILE_DEBUG1("Getting Profiles From SipProfiles.Dat")
    	InitializeFileStoreL();
    
    	const RArray<TStreamId>& streams = iProfileIndex->Streams(); 
    	
    	RPointerArray<CSIPConcreteProfile>* array1 = 
        	new (ELeave) RPointerArray<CSIPConcreteProfile>(1);   

    	for (TInt i = 0; i < streams.Count(); i++)
        	{
        	CSIPConcreteProfile* profile = SIPProfileL(streams[i]);        
        	CleanupStack::PushL(profile);
        
        	// Put the profiles in cenrep
        	profile->SetId(0);
        	profile->SetStorageId(0);
        	this->AddSIPProfileL(*profile);        
                
        	array1->AppendL(profile);
        	CleanupStack::Pop(profile);
        	}
        
    	// Remove the sipprofiles.dat since the profiles has been picked and stored
    	// in cenrep
    	TFindFile filefinder(iFs);
    	TInt err =  
        	filefinder.FindByDir(KFullNameOfFileStoreFile,KFullNameOfFileStorePath);
    	if (err == KErrNone)
			{
			delete iStore;
			iStore = 0;
			iFs.Delete(KFullNameOfFileStore);    
			}
			return array1;	
    	}
   
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::DefaultProfile
// -----------------------------------------------------------------------------
//
TUint32 CSIPProfileStorageBase::DefaultProfile()
	{
	return iProfileIndex->DefaultProfile();
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::DeleteAllProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::DeleteAllProfilesL()
	{
	PROFILE_DEBUG1("CSIPProfileStorageBase::DeleteAllProfilesL: Start")
    CRepository* sipCenRep = CRepository::NewL(KCrUidSipProfileSrv);
	CleanupStack::PushL(sipCenRep);    
    User::LeaveIfError(sipCenRep->Reset());      
    CSIPProfileStorageIndex *PrflStorageIndex = CSIPProfileStorageIndex::NewL(*sipCenRep);            
    PrflStorageIndex->DeleteAllProfilesIds();
    delete PrflStorageIndex;
    CleanupStack::PopAndDestroy(sipCenRep);   
PROFILE_DEBUG1("CSIPProfileStorageBase::DeleteAllProfilesL: End")	
    }
    
// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::DeleteAllStreamProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::DeleteAllStreamProfilesL()
	{	
	PROFILE_DEBUG1("CSIPProfileStorageBase::DeleteAllStreamProfilesL: Start")		
    // Delete Stream IDs for all profiles
    iProfileIndex->DeleteAllStreamIds();
    // Delete the dat file
    RFs session;
	CleanupClosePushL(session);
	User::LeaveIfError(session.Connect());
	TInt err = session.Delete(KFullNameOfFileStore);
	CleanupStack::PopAndDestroy();
	if (err != KErrNotFound && err != KErrNone)
		{
		User::LeaveIfError(err);
		}
	PROFILE_DEBUG1("CSIPProfileStorageBase::DeleteAllStreamProfilesL: End")		 
    }
    

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::ResetAndDestroyProfile
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::ResetAndDestroyProfile(TAny* aArray)
    {
	(static_cast<RPointerArray<CSIPConcreteProfile>*>
		(aArray))->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::SIPProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileStorageBase::SIPProfileL(TUint32 aProfileId)
    {
	PROFILE_DEBUG3("CSIPProfileStorageBase::SIPProfileL: Profile ID : ",aProfileId)		
    __ASSERT_ALWAYS(iProfileIndex->ProfileExists(aProfileId),
	                User::Leave (KErrNotFound)); 
    TUint32 partial = aProfileId << KProfileIdShift;
    RArray<TUint32> foundProfileKeys;
    iSipCenRep->FindL(partial,KProfileIdMask,foundProfileKeys);
    CleanupClosePushL(foundProfileKeys);
    TInt itemCount = foundProfileKeys.Count();
    PROFILE_DEBUG3("CSIPProfileStorageBase::SIPProfileL:Total Number of Profile Specific Keys :",itemCount)		
    CSIPConcreteProfile* profile = CSIPConcreteProfile::NewL();
    CleanupStack::PushL(profile);
    TUint32 profileKey;
    TVariableType variableType(EInteger);
    for (TInt i = 0; i < itemCount; i++)
        {
        profileKey = foundProfileKeys[i];
		
		variableType = VariableTypeL(profileKey);
		switch(variableType)
            {
            case EInteger:
            case EBoolean:
            case EDescriptor:
                SetVariableL(profileKey, *profile, variableType);
            	break;
                       
            case EDescArray:
            case EBoolArray:
            case EIntArray:
            case EDescArrayArray:
                SetArrayVariableL(i, foundProfileKeys, *profile);
            	break;
             
            default:
                User::Leave( KErrNotSupported );                      
            }
        } 
    CleanupStack::Pop(profile);
    CleanupStack::PopAndDestroy();
	
    return profile;    
    }    
// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::VariableTypeL
// -----------------------------------------------------------------------------
//
CSIPProfileStorageBase::TVariableType 
    CSIPProfileStorageBase::VariableTypeL(TUint32 aProfileKey)
    {
	
    TVariableType variableType(EInteger);
    TInt variableId = (aProfileKey & KVariableIdMask) >> KVariableIdShift;
    switch(variableId) 
        {
        case KSIPProfileId:    
        case KSIPAccessPointId:
        case KSIPProfileClass:
        case KSIPProfileStorageId:
        case KSIPContextId:
        case KSIPAllowIMSRegistration:
            variableType = EInteger;
        	break;
 
        case KSIPSigComp:
        case KSIPSecurityNegotiation:
        case KSIPAutoRegistration:
        case KSIPDefaultProfile:
        case KSIPIsEnabled:
        case KSIPProfileLocked:
            variableType = EBoolean;
        	break;

        case KSIPProxyDigestUserName:
        case KSIPProxyDigestRealm: 
        case KSIPProxyDigestPassword:
        case KSIPOutboundProxy:
        case KSIPRegistrarDigestUserName:
        case KSIPRegistrarDigestRealm: 
        case KSIPRegistrarDigestPassword:
        case KSIPRegistrar:             
        case KSIPProviderName:
        case KSIPUserAor:
        case KSIPNegotiatedSecurityMechanism:
        case KSIPPrivateIdentity:
        case KSIPDynamicProxy:     
        case KSIPProfileName:
            variableType = EDescriptor;
        	break;
    
        case KSIPContactHeaderParams:        
        case KSIPRegisteredAors:   
        case KSIPHeaders:
        case KSIPExtProxyParam:           
        case KSIPRegistrarExtParam:
        case KSIPExtensionDescParam:        
            variableType = EDescArray;
        	break;

        case KSIPExtensionDescArrayParam:
            variableType = EDescArrayArray;
        	break;
            
        case KSipExtensionBoolParam:
            variableType = EBoolArray;
        	break;       

        case KSIPExtensionIntParam:
            variableType = EIntArray;
        	break;
                            
        default:
            User::Leave(KErrNotSupported);
        }
	
    return variableType;
    }
// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::SetVariableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::SetVariableL(TUint32 aProfileKey, 
                                          CSIPConcreteProfile& aProfile,
                                          TVariableType aVarType)
    {
    TInt    numValue(0);
    TInt    variableId(0);
    HBufC8* bufValue = HBufC8::NewL(KMaxBufLength);
    CleanupStack::PushL(bufValue);   
    TPtr8   bufValuePtr = bufValue->Des();
    
    switch(aVarType)
        {
        case EInteger:
        case EBoolean:
            User::LeaveIfError(iSipCenRep->Get(aProfileKey, numValue));    
	        break;

        case EDescriptor:
            User::LeaveIfError(iSipCenRep->Get(aProfileKey, bufValuePtr));
    	    break;
    	    
		default:
			User::Leave(KErrCorrupt);
        }

    variableId = (aProfileKey & KVariableIdMask) >> KVariableIdShift;     

    switch(variableId)
        { 
        case KSIPProfileLocked:
            aProfile.SetExtensionParameterL(
                KSIPProfileLocked,
                (TBool)numValue);
            break;
            
        case KSIPAllowIMSRegistration:
            aProfile.SetExtensionParameterL(
                KSIPAllowIMSRegistration,
                (TUint32)numValue);
            break;
              
        case KSIPProfileId:
            aProfile.SetId(numValue);
        	aProfile.SetStorageId(numValue);
        	break;
          
		case KSIPProfileStorageId:
            aProfile.SetStorageId(numValue);
        	break;
        
        case KSIPAccessPointId:
            aProfile.SetIapId(numValue); 
        	break;
            
        case KSIPSigComp:
            aProfile.EnableSigComp(numValue);
        	break;
            
        case KSIPSecurityNegotiation:
            aProfile.EnableSecurityNegotiation(numValue); 
        	break;
            
        case KSIPAutoRegistration:
            aProfile.SetAutoRegistrationEnabled(numValue);
        	break;
            
        case KSIPDefaultProfile:
            aProfile.SetDefault(numValue);
        	break;

        case KSIPProfileName:
            iProfileInfo.iSIPProfileName = bufValuePtr;
            aProfile.SetProfileType(iProfileInfo);
        	break;
            
        case KSIPProfileClass:
            iProfileInfo.iSIPProfileClass = 
                static_cast<TSIPProfileTypeInfo::TSIPProfileClass>(numValue);
            aProfile.SetProfileType(iProfileInfo);
        	break;           

        case KSIPProviderName:
            aProfile.SetProviderNameL(bufValuePtr);
        	break;
            
        case KSIPUserAor:
            aProfile.SetAORL(bufValuePtr);
        	break;
            
        case KSIPNegotiatedSecurityMechanism:
            aProfile.SetNegotiatedSecurityMechanismL(bufValuePtr);
        	break; 

        case KSIPPrivateIdentity:
            aProfile.SetPrivateIdentityL(bufValuePtr);
        	break; 

        case KSIPDynamicProxy:
            aProfile.SetDynamicProxyL(bufValuePtr);
        	break; 

        case KSIPProxyDigestUserName: 
            aProfile.SetServerParameterL(KSIPOutboundProxy, 
                                         KSIPDigestUserName, 
                                         bufValuePtr);
        	break;
                 
        case KSIPProxyDigestRealm:
            aProfile.SetServerParameterL(KSIPOutboundProxy, 
                                         KSIPDigestRealm, 
                                         bufValuePtr);            
        	break;
                    
        case KSIPProxyDigestPassword:
            aProfile.SetServerParameterL(KSIPOutboundProxy, 
                                         KSIPDigestPassword, 
                                         bufValuePtr);
        	break;        
            
        case KSIPOutboundProxy:
            aProfile.SetServerL(KSIPOutboundProxy, bufValuePtr);
        	break;        
            
        case KSIPRegistrarDigestUserName:
            aProfile.SetServerParameterL(KSIPRegistrar, 
                                         KSIPDigestUserName, 
                                         bufValuePtr);
        	break;
                    
        case KSIPRegistrarDigestRealm:
            aProfile.SetServerParameterL(KSIPRegistrar, 
                                         KSIPDigestRealm, 
                                         bufValuePtr);
        	break;
                    
        case KSIPRegistrarDigestPassword:
            aProfile.SetServerParameterL(KSIPRegistrar, 
                                         KSIPDigestPassword, 
                                         bufValuePtr);
        	break;    
              
        case KSIPRegistrar:
            aProfile.SetServerL(KSIPRegistrar, bufValuePtr);
        	break;   
        
        case KSIPContextId:
            aProfile.SetContextId(numValue);
        	break;

        case KSIPIsEnabled:
           aProfile.SetEnabled(numValue);
        	break;
               
        default:
            User::Leave(KErrNotSupported);
        }
    CleanupStack::PopAndDestroy(bufValue);
    }
// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::SetArrayVariableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::SetArrayVariableL(TInt& aIndex, 
                                               RArray<TUint32>& aKeysArray,
                                               CSIPConcreteProfile& aProfile)
    {
     __ASSERT_ALWAYS((aKeysArray[aIndex] & KArrayIndexMask) == 0,
		User::Leave (KErrCorrupt));
    	
    TUint32 profileKey = aKeysArray[aIndex];
    
    TInt variableId = (profileKey & KVariableIdMask) >> KVariableIdShift;
	CDesC8ArrayFlat* valueArray = new (ELeave) CDesC8ArrayFlat(1);
    CleanupStack::PushL(valueArray);

    switch(variableId)
        {     
        case KSIPContactHeaderParams:
            FillValueArrayL(
            	aProfile,
				valueArray,
				aIndex, 
                aKeysArray,
                variableId);
            aProfile.SetContactHeaderParamsL(*valueArray);
        	break;
                
        case KSIPRegisteredAors:
            FillValueArrayL(
	            aProfile,
	            valueArray,
	            aIndex, 
				aKeysArray,
				variableId);
            aProfile.SetRegisteredAORsL(*valueArray);
        	break;
            
        case KSIPHeaders:
            FillValueArrayL(
            	aProfile,
            	valueArray,
            	aIndex, 
				aKeysArray,
				variableId);
            aProfile.SetSIPHeadersL(*valueArray);
        	break;
                
        case KSIPExtProxyParam:
            SetServerExtParametersL(
            	KSIPOutboundProxy,
            	aProfile, 
				aIndex,
				aKeysArray);
        	break;  
            
        case KSIPRegistrarExtParam:
            SetServerExtParametersL(
            	KSIPRegistrar,
            	aProfile,
            	aIndex,
            	aKeysArray);
        	break; 
                   
        case KSIPExtensionIntParam:
        case KSipExtensionBoolParam:
        case KSIPExtensionDescParam:
        case KSIPExtensionDescArrayParam:       
            FillValueArrayL(
            	aProfile,
            	valueArray,
            	aIndex, 
				aKeysArray,
				variableId);
        	break;
                        
        default:
            User::Leave(KErrNotSupported);
        }
         
    CleanupStack::Pop(valueArray);
    valueArray->Reset();
	delete valueArray;
    }     
// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::FillValueArrayL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::FillValueArrayL(CSIPConcreteProfile& aProfile,
                                             CDesC8ArrayFlat* aValueArray,
                                             TInt& aIndex,
                                             RArray<TUint32>& aKeysArray,
                                             TUint32 aVarId)
    {
    TInt    arrayLength;
    TInt    subArrayLength;
    TInt    arrayId;
    TBool   isExtension = EFalse;
    TInt    keyValue;
    TInt    numValue;
    
    HBufC8*  bufValue = HBufC8::NewL(KMaxBufLength);
    CleanupStack::PushL(bufValue);
    TPtr8    bufValuePtr = bufValue->Des();
    
    
    if (aVarId == KSIPExtensionIntParam ||
        aVarId == KSipExtensionBoolParam ||
        aVarId == KSIPExtensionDescParam ||
        aVarId == KSIPExtensionDescArrayParam)
        {
        isExtension = ETrue;
        }
    
    TUint32 profileKey = aKeysArray[aIndex];
    User::LeaveIfError(iSipCenRep->Get(profileKey, arrayLength));
        
    while (arrayLength--)
		{
        if (isExtension)
            {
            if (aVarId == KSIPExtensionDescArrayParam)
                {
                // get subarray length
                profileKey = aKeysArray[++aIndex]; 
                User::LeaveIfError(iSipCenRep->Get(profileKey, subArrayLength));
            	
                // get array id
                 profileKey = aKeysArray[++aIndex];
                User::LeaveIfError(iSipCenRep->Get(profileKey, arrayId));
                --arrayLength;
                for (TInt k=0;k<subArrayLength;k++)
                    {
                    profileKey = aKeysArray[++aIndex];
                    User::LeaveIfError(iSipCenRep->Get(profileKey, bufValuePtr));
                    --arrayLength;
                    aValueArray->AppendL(bufValuePtr);
                    }
                aProfile.SetExtensionParameterL(arrayId, *aValueArray);
                aValueArray->Reset();               
                }
             else // KSIPExtensionDescParam || KSIPExtensionIntParam || KSIPExtensionBoolParam
                {
                //get key (always an integer)
                profileKey = aKeysArray[++aIndex];
                User::LeaveIfError(iSipCenRep->Get(profileKey, keyValue));
                // get value
                profileKey = aKeysArray[++aIndex];
                if(aVarId == KSIPExtensionDescParam)
                    {
                    User::LeaveIfError(iSipCenRep->Get(profileKey, bufValuePtr));    
                    }
                else // KSIPExtensionIntParam || KSIPExtensionBoolParam
                    {
                    User::LeaveIfError(iSipCenRep->Get(profileKey, numValue));
                    }
                
                if(aVarId == KSIPExtensionDescParam)
                    {  
                    aProfile.SetExtensionParameterL(keyValue,bufValuePtr);      
                    }
                else if(aVarId == KSipExtensionBoolParam)
                    {
                    aProfile.SetExtensionParameterL(keyValue,(TBool)numValue);
                    }
                else// aVarId == KSIPExtensionIntParam
                    {
                    aProfile.SetExtensionParameterL(keyValue,(TUint32)numValue);
                    }
                }
            }
        else // just a plain value array
            {
            profileKey = aKeysArray[++aIndex];
            User::LeaveIfError(iSipCenRep->Get(profileKey, bufValuePtr));
            aValueArray->AppendL(bufValuePtr);            
            }
        } 
                  
    CleanupStack::PopAndDestroy(bufValue);
    bufValue = NULL;    
    }
// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::SetServerExtParametersL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageBase::SetServerExtParametersL(
	TUint32 aServer, 
	CSIPConcreteProfile& aProfile,
	TInt& aIndex, 
	RArray<TUint32>& aKeysArray )
    {
    __ASSERT_ALWAYS((aKeysArray[aIndex] & KArrayIndexMask) == 0,
		User::Leave (KErrCorrupt));

    TUint32 profileKey = aKeysArray[aIndex];
    HBufC8* bufValue = HBufC8::NewL(KMaxBufLength);
    CleanupStack::PushL(bufValue);
    TPtr8   bufValPtr = bufValue->Des();
    HBufC8* bufKey = HBufC8::NewL(KMaxBufLength);
    CleanupStack::PushL(bufKey);
    TPtr8   bufKeyPtr = bufKey->Des();    
    TInt    arrayLength;

    User::LeaveIfError(iSipCenRep->Get(profileKey, arrayLength));
    
    for (TInt j=0; j < arrayLength; j++)
        {
        profileKey = aKeysArray[++aIndex];
        User::LeaveIfError(iSipCenRep->Get(profileKey, bufKeyPtr));
        profileKey = aKeysArray[++aIndex];
        User::LeaveIfError(iSipCenRep->Get(profileKey, bufValPtr));
        aProfile.SetServerExtensionParameterL(aServer, bufKeyPtr, bufValPtr);          
        }
    CleanupStack::PopAndDestroy(bufKey);
    CleanupStack::PopAndDestroy(bufValue);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::SIPProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileStorageBase::SIPProfileL(TStreamId aStreamId)
    {
	__ASSERT_ALWAYS(iProfileIndex->ProfileExists(aStreamId),
	                User::Leave (KErrNotFound));
    RStoreReadStream stream;
    stream.OpenLC(*iStore,aStreamId);
    CSIPConcreteProfile* profile = 
        CSIPConcreteProfile::InternalizeL(stream,EFalse);
    profile->SetStorageId(aStreamId.Value());
    CleanupStack::PopAndDestroy(); // stream
    return profile;
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageBase::GetProfileIndexObject
// -----------------------------------------------------------------------------
//
CSIPProfileStorageIndex* CSIPProfileStorageBase::GetProfileStorageIndexObject()
    {
    return iProfileIndex;    
    }
