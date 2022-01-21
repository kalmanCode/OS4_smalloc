//
// Created by student on 1/16/22.
//
#include <cstddef>
#include <unistd.h>
#include <cstring>
struct SMallocMetaData
{
    size_t blockSize;
    bool isFree;
    struct SMallocMetaData* next;
    struct SMallocMetaData* prev;

};



class CAllocator{

    SMallocMetaData* pMetaDataHead;
    SMallocMetaData* pMetaDataTail;

    size_t numOfBlocks;
    size_t numOfBytes;
    size_t numOfFreeBlocks;
    size_t numOfFreeByts;
public:
    const unsigned int MaxSupportedBytsToAlloc;
public:
    CAllocator();
    ~CAllocator() = default;
    CAllocator(CAllocator& c)=delete;
    void* smalloc(size_t size);
    void* scalloc(size_t num , size_t size);

    void sfree(void* p);
    void* srealloc(void* oldp, size_t size);

   void* getDataBlockPtr(SMallocMetaData* start);
   bool existsFreeBlockOfGivenSizeBytes(size_t size,void*& pMemBlock);
   void * getNewMemBlock(size_t size);
   bool bMemoryWasAlloced();
    SMallocMetaData *getPMetaDataTail() const;
   size_t getNumOfFreeBlocks() const;

   size_t getNumOfFreeByts() const;

   size_t getNumOfBlocks() const;

   size_t getNumOfByts() const;
   void increaseNumOfBlocks();
   void decreaseNumOfBlocks();

   void increaseNumOfFreeBlocks();
   void decreaseNumOfFreeBlocks();

   void   increseNumOfBytes(size_t num);
   void   decreaseNumOfBytes(size_t num);

   void   increaseNumOfFreeBytes(size_t num);
   void   decreasNumOfFreeBytes(size_t num);

   unsigned int getMaxSupportedBytsToAlloc() const;

};

CAllocator::CAllocator(): MaxSupportedBytsToAlloc(100000000), numOfBlocks(0), numOfBytes(0), pMetaDataHead(nullptr),
                          pMetaDataTail(nullptr), numOfFreeBlocks(0), numOfFreeByts(0){}

bool CAllocator::existsFreeBlockOfGivenSizeBytes(size_t size, void *&pMemBlock) {
    if(!pMetaDataHead)
    {
        return false;
    }
    SMallocMetaData* runner = pMetaDataHead;
    for(; runner != nullptr; runner = runner->next)
    {
        if(runner->blockSize >= size && runner->isFree)
        {
            decreaseNumOfFreeBlocks();
            decreasNumOfFreeBytes(runner->blockSize);

            runner->isFree = false;
            pMemBlock = static_cast<void*>(runner+1);
            return true;
        }
    }
    return false;

}

bool CAllocator::bMemoryWasAlloced() {
    return (pMetaDataHead != nullptr);
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
        pMetaDataHead=p_sMetadata;
        pMetaDataTail=p_sMetadata;
        p_sMetadata->isFree = false;
        p_sMetadata->blockSize = size;
        p_sMetadata->next= nullptr;
        p_sMetadata->prev= nullptr;

        increaseNumOfBlocks();
        increseNumOfBytes(size);


        return static_cast<void*>(static_cast<char*>(pOldBreak) + sizeof(SMallocMetaData));

    }
    SMallocMetaData* runner = pMetaDataHead;
    while (runner->next)
    {
        runner = runner->next;
    }
    pNewBreak = sbrk(static_cast<intptr_t>(size+sizeof(SMallocMetaData)) );
    if(*static_cast<int*>(pNewBreak) == -1)
    {
        return nullptr;
    }
    auto* p_sMetadata       = static_cast<SMallocMetaData*>(pOldBreak);
    runner->next            = p_sMetadata;
    p_sMetadata->next       = nullptr;
    p_sMetadata->prev       = runner;
    p_sMetadata->isFree     = false;
    p_sMetadata->blockSize  = size;
    pMetaDataTail           = p_sMetadata;
    increaseNumOfBlocks();
    increseNumOfBytes(size);
    return static_cast<void*>(static_cast<char*>(pOldBreak) + sizeof(SMallocMetaData));
}




size_t CAllocator::getNumOfFreeBlocks() const {
    return numOfFreeBlocks;
}

size_t CAllocator::getNumOfFreeByts() const {
    return numOfFreeByts;
}

size_t CAllocator::getNumOfBlocks() const {
    return numOfBlocks;
}

size_t CAllocator::getNumOfByts() const {
    return numOfBytes;
}

unsigned int CAllocator::getMaxSupportedBytsToAlloc() const {
    return MaxSupportedBytsToAlloc;
}

void CAllocator::increaseNumOfBlocks() {
    numOfBlocks ++;
}

void CAllocator::decreaseNumOfBlocks() {
    if (numOfBlocks > 0) {
        numOfBlocks--;
    }
}

void CAllocator::increaseNumOfFreeBlocks() {
      numOfFreeBlocks ++;
}

void CAllocator::decreaseNumOfFreeBlocks() {
    if(numOfFreeBlocks >0)
    {
    numOfFreeBlocks --;
    }
}

void CAllocator::increseNumOfBytes(size_t num) {
    numOfBytes += num;
}

void CAllocator::decreaseNumOfBytes(size_t num) {
    if(num <= numOfFreeBlocks ) {
        numOfBytes -= num;
    } else {
        numOfBytes = 0;
    }
}

void CAllocator::increaseNumOfFreeBytes(size_t num) {
    numOfFreeByts += num;
}

void CAllocator::decreasNumOfFreeBytes(size_t num) {
    if (num <= numOfFreeByts) {
        numOfFreeByts -= num;
    }
    else {
        numOfFreeByts = 0;
    }
}


SMallocMetaData *CAllocator::getPMetaDataTail() const {
    return pMetaDataTail;
}

void *CAllocator::smalloc(size_t size) {
    if(size == 0    || size > MaxSupportedBytsToAlloc ) { return nullptr;}
    void* pMemBlock(nullptr);
    if(existsFreeBlockOfGivenSizeBytes(size,pMemBlock) )
    {
        return pMemBlock;
    }
    pMemBlock = getNewMemBlock(size);
    return pMemBlock;
}

void *CAllocator::scalloc(size_t num, size_t size) {
    if(num == 0 || num*size > MaxSupportedBytsToAlloc)
    {
        return nullptr;
    }
    void* pDataBlock = smalloc(num*size);
    if(!pDataBlock)
    {
        return nullptr;
    }
    std::memset(pDataBlock,0,num*size);
    return pDataBlock;
}

void CAllocator::sfree(void *p) {
    if(!p )
    {
        return;
    }
    auto pTemp = static_cast<char*>(p);
    pTemp -= sizeof(SMallocMetaData);
    auto pMetadata =static_cast<SMallocMetaData*>(static_cast<void*>(pTemp));
    if(pMetadata->isFree)
    {
        return;
    }
    pMetadata->isFree = true;
    increaseNumOfFreeBytes(pMetadata->blockSize);
    increaseNumOfFreeBlocks();
}

void *CAllocator::srealloc(void *oldp, size_t size) {
    if( size == 0 || size > MaxSupportedBytsToAlloc )
    {
        return nullptr;
    }

    if(!oldp)
    {
        return smalloc(size);
    }
    char* pBlock = static_cast<char*>(oldp);
    pBlock -= sizeof(SMallocMetaData);
    void* ptemp = static_cast<void*>(pBlock);
    auto pMetaData = static_cast<SMallocMetaData*>(ptemp);
    if(pMetaData->blockSize >= size)
    {
        return oldp;
    }
    // else block is too small .
    // 1)find suiteable block .
    // 2) copy data to new block .
    // 3) update metedata.
    // 4) update CAllocator block&mem data

    // cAllocator.realloc(oldp,size);
    for (SMallocMetaData* runner = pMetaDataHead; runner ; runner=runner->next)
    {
        if(runner->isFree && runner->blockSize >=size)
        {
            increaseNumOfFreeBytes(pMetaData->blockSize);
            pMetaData->isFree = true;

            runner->isFree = false;
            decreasNumOfFreeBytes(runner->blockSize);
            auto temp = static_cast<void*>(runner);
            auto pChDatablk = static_cast<char*>(temp)+ sizeof(SMallocMetaData);
            auto pDataBlk = static_cast<void*>(pChDatablk);

            std::memcpy(pDataBlk,oldp,pMetaData->blockSize);
            return pDataBlk;
        }
    }

    SMallocMetaData* pOldTail = getPMetaDataTail();
    void* pOldBreak = sbrk(static_cast<intptr_t >(size + sizeof(SMallocMetaData)));
    if(*static_cast<int*>(pOldBreak) == -1)
    {
        return oldp;
    }
    auto* pCurrentMeta = static_cast<SMallocMetaData*>(pOldBreak);
    pCurrentMeta->isFree = false;
    pCurrentMeta->blockSize = size;
    pCurrentMeta->next = nullptr;
    pCurrentMeta->prev = pMetaDataTail;
    pMetaDataTail->next = pCurrentMeta;
    pMetaDataTail       = pCurrentMeta;
    increaseNumOfBlocks();
    increaseNumOfFreeBlocks();
    increseNumOfBytes(size);
    void* pNewMemBlk =static_cast<void*>( static_cast<char*>(pOldBreak)+sizeof(SMallocMetaData) ) ;
    std::memcpy(pNewMemBlk,oldp,size);
    pMetaData->isFree = true;
    increaseNumOfFreeBytes(pMetaData->blockSize);
    return  pNewMemBlk;
}


static CAllocator cAllocator;
void* smalloc(size_t size)
{
    return cAllocator.smalloc(size);
}

void* scalloc(size_t num, size_t size)
{
    return cAllocator.scalloc(num,size);
}

void sfree(void* p)
{
   cAllocator.sfree(p);
}





void* srealloc(void* oldp, size_t size)
{
    return cAllocator.srealloc(oldp,size);

}










size_t _num_free_blocks()
{
    return cAllocator.getNumOfFreeBlocks();
}

size_t _num_free_bytes()
{
    return cAllocator.getNumOfFreeByts();
}

size_t _num_allocated_blocks()
{
    return cAllocator.getNumOfBlocks();
}


size_t _num_allocated_bytes()
{
    return  cAllocator.getNumOfByts();
}

size_t _num_meta_data_bytes()
{
    return sizeof(SMallocMetaData)*cAllocator.getNumOfBlocks();
}


size_t _size_meta_data()
{
    return sizeof(SMallocMetaData);
}