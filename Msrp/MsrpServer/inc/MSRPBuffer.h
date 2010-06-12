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

#ifndef __MMSRPBUFFER_H
#define __MMSRPBUFFER_H

// INCLUDES
#include <e32base.h>
#include "MsrpCommon.h"

// CLASS DECLARATIONS
class RMsrpBuf;
class CMSRPBuffer;
class CMSRPBufPool;
/**
 * pool of buffers
 */


class CMSRPBufPool : public CBase
    {
    friend class CMSRPBuffer;     
public: 
    inline RMsrpBuf ProvideBufferL(TInt aSize = KBufSize);
    //inline RMsrpBuf ProvideBufferL(TInt aSize);
        
    //InitializePool; //dummy
    //void ReleasePool(); //force free all buffers
    inline virtual ~CMSRPBufPool();
        
private:
    inline void TakeBackBufferL(CMSRPBuffer* aBuffer); //append to pool

private:    
    RPointerArray<CMSRPBuffer> iFreeBufPool; //free buf pool
    RPointerArray<CMSRPBuffer> iUsedBufPool; //free buf pool
    };


/**
* The buffer class
*/
class CMSRPBuffer : public CBase
    {
    
friend class RMsrpBuf;  

    public:
        inline static CMSRPBuffer* NewL(TInt aSize, CMSRPBufPool& aPool );        
        inline virtual ~CMSRPBuffer();
    
    private:
        inline void ConstructL(TInt aSize);
        inline CMSRPBuffer(CMSRPBufPool& aPool);
        TPtr8 MsrpDes(TInt aOffset);
        TPtr8 MsrpDes();

        inline void IncRef(void);           
        inline void DecRef(void);

    private:
        TUint iRef;
        HBufC8* iBuf;
        CMSRPBufPool& iBufPool;
                
    };

class RMsrpBuf
    {
    public:    
        
    inline RMsrpBuf(CMSRPBuffer& aBuf);
            
    inline RMsrpBuf(RMsrpBuf& aClone, TPtr8 aPtr);
    
    //get a R ref on the C class using the R class //MidTPtr
    inline RMsrpBuf(const RMsrpBuf& aCopy);
    
    //makes the rmsrpbuf point to the remaining buffer after the length  
    TInt MsrpRightExtract();
        
    //if same pool then append to our Rmsrpbuf and return true
    //no continuity check, not required as of now
    inline TBool Collate(RMsrpBuf& aSuffix);
    
    inline void MsrpRightTPtr(TInt length);
    
    inline void MsrpLeftTPtr(TInt length);
    
    inline void MsrpMidTPtr(TInt offset);
            
    inline void MsrpMidTPtr(TInt offset, TInt length);
    
    inline void Zero();
    
    inline void Append(TDesC8& aDes);
        
    
    /*implicit TPtr conv, temp only*/
    inline operator TPtr8() const;
    
    inline operator TPtr8&();
    
    /**
     * if TPtrs are passed to and stored in some class obj
     * then the passer must call close on rbuf after the obj has cleaned up itself or 
     *  released the reference 
     */
    inline TPtr8 Ptr();
    
    inline virtual ~RMsrpBuf();
        
    private:
        //assignment
        RMsrpBuf& operator=(const RMsrpBuf&);
        
    private:
        CMSRPBuffer& iBuf;
        TPtr8 iPtr;    
    };



    inline RMsrpBuf CMSRPBufPool::ProvideBufferL(TInt aSize) //remove head element of pool or allocate and provide
        {
        CMSRPBuffer* buf = NULL;
        if (iFreeBufPool.Count())
            {
            buf = iFreeBufPool[0];
            iFreeBufPool.Remove(0);
            }
        else
            {
            buf = CMSRPBuffer::NewL(aSize, *this);
            }
        CleanupStack::PushL(buf);
        iUsedBufPool.AppendL(buf); 
        CleanupStack::Pop();
        RMsrpBuf ptr(*buf);
        return ptr;
        }
   
    inline CMSRPBufPool::~CMSRPBufPool()
        {
        iFreeBufPool.ResetAndDestroy();
        iFreeBufPool.Close();
        iUsedBufPool.ResetAndDestroy();
        iUsedBufPool.Close();
        }

    inline void CMSRPBufPool::TakeBackBufferL(CMSRPBuffer* aBuffer) //append to pool
        {
        TInt index = iUsedBufPool.Find(aBuffer);    
        iUsedBufPool.Remove(index);
        CleanupStack::PushL(aBuffer);
        iFreeBufPool.AppendL(aBuffer);
        CleanupStack::Pop(aBuffer);
        }


/**
* The buffer class
*/
        inline CMSRPBuffer* CMSRPBuffer::NewL(TInt aSize, CMSRPBufPool& aPool )
            {
            CMSRPBuffer* self = new (ELeave) CMSRPBuffer(aPool );
            CleanupStack::PushL(self);
            self->ConstructL(aSize);
            CleanupStack::Pop(self);
            return self;
            }
            
        inline CMSRPBuffer::CMSRPBuffer(CMSRPBufPool& aPool ): iBufPool(aPool)
            {

            }
        
        inline void CMSRPBuffer::ConstructL(TInt aSize)
           {
           iBuf = HBufC8::NewL(aSize);
           }
        
        inline CMSRPBuffer::~CMSRPBuffer()
            {
            delete iBuf;
            }      

        inline TPtr8 CMSRPBuffer::MsrpDes(TInt aOffset) //MsrpRef
            {
            TPtr8 mPtr(iBuf->Des());
            mPtr.Set(mPtr.MidTPtr(aOffset));
            return mPtr;
            }
        
        inline TPtr8 CMSRPBuffer::MsrpDes() //MsrpRef
            {
            return iBuf->Des();            
            }

        inline void CMSRPBuffer::IncRef(void)
            {
            iRef++;
            }
        inline void CMSRPBuffer::DecRef(void)
            {
            iRef--;
            if (iRef == 0)
                iBufPool.TakeBackBufferL(this);
                //delete this;                
            }


    inline RMsrpBuf::RMsrpBuf(CMSRPBuffer& aBuf):iBuf(aBuf), iPtr(aBuf.MsrpDes())    
        {
        iBuf.IncRef();                 
        }
            
    inline RMsrpBuf::RMsrpBuf(RMsrpBuf& aClone, TPtr8 aPtr):iBuf(aClone.iBuf), iPtr(aPtr) 
        {
        //check TPtr belongs to buf
        //if(iBuf.IsInBuf(aPtr))//DEBUG
        iBuf.IncRef();
        //iPtr = aPtr;         
        }
    //get a R ref on the C class using the R class //MidTPtr
    inline RMsrpBuf::RMsrpBuf(const RMsrpBuf& aCopy):iBuf(aCopy.iBuf), iPtr(aCopy.iPtr)
        {        
        iBuf.IncRef();
        }
    
    inline TInt RMsrpBuf::MsrpRightExtract()
        {
        TInt length = iPtr.Length();
        iPtr.SetMax();
        iPtr.Set(iPtr.MidTPtr(length));     
        iPtr.Zero();
        return iPtr.MaxLength();
        }
    
    inline TBool RMsrpBuf::Collate(RMsrpBuf& aSuffix)
        {
        if (&aSuffix.iBuf != &iBuf)
            return FALSE;
                
        iPtr.SetLength(iPtr.Length() + aSuffix.iPtr.Length());      
        return TRUE;
        }
    
    inline void RMsrpBuf::MsrpRightTPtr(TInt length)
        {
        iPtr.Set(iPtr.RightTPtr(length));    
        }
    
    inline void RMsrpBuf::MsrpLeftTPtr(TInt length)
        {
        iPtr.Set(iPtr.LeftTPtr(length));    
        }
    
    inline void RMsrpBuf::MsrpMidTPtr(TInt offset)
        {
        TInt length = iPtr.Length();
        iPtr.SetMax();
        iPtr.Set(iPtr.MidTPtr(offset));     
        iPtr.SetLength(length - offset);  
        }
            
    inline void RMsrpBuf::MsrpMidTPtr(TInt offset, TInt length)
        {
        iPtr.Set(iPtr.MidTPtr(offset, length));    
        }
    
    inline void RMsrpBuf::Zero()
        {
        iPtr.Zero();    
        }
    
    inline void RMsrpBuf::Append(TDesC8& aDes)
        {
        iPtr.Append(aDes);    
        }
    
    /*implicit TPtr conv, temp only*/
    inline RMsrpBuf::operator TPtr8() const
        {
        return iPtr;        
        }
    
    inline RMsrpBuf::operator TPtr8&()
        {
        return iPtr;        
        }
    
    /**
     * if TPtrs are passed to and stored in some class obj
     * then the passer must call close on rbuf after the obj has cleaned up itself or 
     *  released the reference 
     */
    inline TPtr8 RMsrpBuf::Ptr()
        {
        return iPtr;
        }
    
    inline RMsrpBuf::~RMsrpBuf()
    //inline void Close()
        {
        iBuf.DecRef();
        iPtr.Zero();
        //iBuf = 0;
        }
   

#endif // __MMSRPBUFFER_H

// End of file
