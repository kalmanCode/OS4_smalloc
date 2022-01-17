//
// Created by student on 1/16/22.
//
#include <cstddef>
#include <unistd.h>

struct SMallocMetaData
{
    size_t blockSize;
    bool isFree;
    struct SMallocMetaData* next;
    struct SMallocMetaData* prev;

    SMallocMetaData(size_t blockSize ,bool isFree ,SMallocMetaData* next ,
                    SMallocMetaData* prev );

};


SMallocMetaData::SMallocMetaData(size_t blockSize, bool isFree, SMallocMetaData* next, SMallocMetaData* prev):
blockSize(blockSize), isFree(isFree),next(next),prev(prev) {}


class CAllocator{

    SMallocMetaData* pMetaDataHead;
    SMallocMetaData* pMetaDataTail;

public:
    const unsigned int MaxSupportedBytsToAlloc;
public:
    CAllocator();
    ~CAllocator();
   void* getDataBlockPtr(SMallocMetaData* start);
   bool existsFreeBlockOfGivenSizeBytes(size_t size,void*& pMemBlock);
   void * getNewMemBlock(size_t size);
   bool bMemoryWasAlloced();
};

CAllocator::CAllocator():MaxSupportedBytsToAlloc(100000000),pMetaDataHead(nullptr),pMetaDataTail(nullptr) {}

bool CAllocator::existsFreeBlockOfGivenSizeBytes(size_t size, void *&pMemBlock) {
    if(!pMetaDataHead)
    {
        return false;
    }
    SMallocMetaData* runner = pMetaDataHead;
    for(; runner != nullptr; runner = runner->next)
    {
        if(runner->blockSize == size && runner->isFree)
        {
            runner->isFree = false;
            pMemBlock = static_cast<void*>(runner+sizeof(SMallocMetaData));
            return true;
        }
    }
    return false;

}

bool CAllocator::bMemoryWasAlloced() {
    return (pMetaDataHead == nullptr);
}


void * CAllocator::getNewMemBlock(size_t size) {
    void* pOldBreak(sbrk(0));
    void* pNewBreak(nullptr);
    if(!bMemoryWasAlloced())
    {
        pNewBreak = sbrk(static_cast<intptr_t>(size+sizeof(SMallocMetaData)) );
        if(*static_cast<int*>(pNewBreak) == -1)
        {
            return nullptr;
        }
        auto* p_sMetadata = static_cast<SMallocMetaData*>(pOldBreak);

    }
}


CAllocator cAllocator;
void* smalloc(size_t size)
{
    if(size == 0    || size > cAllocator.MaxSupportedBytsToAlloc ) { return nullptr;}
    void* pMemBlock(nullptr);
    if(cAllocator.existsFreeBlockOfGivenSizeBytes(size,pMemBlock) )
    {
        return pMemBlock;
    }
    pMemBlock = cAllocator.getNewMemBlock(size);
    return pMemBlock;
}
