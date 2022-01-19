//
// Created by student on 1/16/22.
//
#include <cstddef>
#include <unistd.h>
#include <cstring>
struct SMallocMetaData
{
    size_t blockSize;
    size_t actualOccupiedBytes;
    bool isFree;
    struct SMallocMetaData* next;
    struct SMallocMetaData* prev;

    SMallocMetaData(size_t blockSize ,size_t actualOccupiedBytes,bool isFree ,SMallocMetaData* next ,
                    SMallocMetaData* prev );

};


SMallocMetaData::SMallocMetaData(size_t blockSize,size_t actualOccupiedBytes,bool isFree, SMallocMetaData* next,
                                 SMallocMetaData* prev):
        blockSize(blockSize), actualOccupiedBytes(actualOccupiedBytes), isFree(isFree), next(next), prev(prev) {}


class CAllocator{

    SMallocMetaData* pMetaDataHead;
public:
    SMallocMetaData *getPMetaDataHead() const;

private:
    SMallocMetaData* pMetaDataTail;
public:
    SMallocMetaData *getPMetaDataTail() const;

private:
    size_t numOfBlocks;
    size_t numOfBytes;
    size_t numOfFreeBlocks;
    size_t numOfFreeByts;
public:
    const unsigned int MaxSupportedBytsToAlloc;
public:
    CAllocator();
    ~CAllocator() = default;
   void* getDataBlockPtr(SMallocMetaData* start);
   bool existsFreeBlockOfGivenSizeBytes(size_t size,void*& pMemBlock);
   void * getNewMemBlock(size_t size);
   bool bMemoryWasAlloced();

   size_t getNumOfFreeBlocks() const;

   size_t getNumOfFreeByts() const;

   size_t getNumOfBlocks() const;

   size_t getNumOfByts() const;
   void   increaseNumOfBlocks(size_t num);
   void  decreaseNumOfBlocks(size_t num);

   void  increaseNumOfFreeBlocks(size_t num);
   void  decreaseNumOfFreeBlocks(size_t num);

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
            runner->actualOccupiedBytes=size;
            numOfBlocks++;
            numOfFreeByts -= size;
            numOfFreeBlocks--;

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
        numOfFreeBlocks++;
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
        increaseNumOfBlocks(1);
        increseNumOfBytes(static_cast<int>(size));


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
    increaseNumOfBlocks(1);
    increseNumOfBytes(static_cast<int>(size));
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

void CAllocator::increaseNumOfBlocks(size_t num) {
    numOfBlocks +=num;
}

void CAllocator::decreaseNumOfBlocks(size_t num) {
    numOfBlocks -= num;
}

void CAllocator::increaseNumOfFreeBlocks(size_t num) {
    numOfFreeBlocks += num;
}

void CAllocator::decreaseNumOfFreeBlocks(size_t num) {
    numOfFreeBlocks -= num;
}

void CAllocator::increseNumOfBytes(size_t num) {
    numOfBytes += num;
}

void CAllocator::decreaseNumOfBytes(size_t num) {
    numOfBytes -= num;
}

void CAllocator::increaseNumOfFreeBytes(size_t num) {
    numOfFreeByts += num;
}

void CAllocator::decreasNumOfFreeBytes(size_t num) {
    numOfFreeByts -= num;
}

SMallocMetaData *CAllocator::getPMetaDataHead() const {
    return pMetaDataHead;
}

SMallocMetaData *CAllocator::getPMetaDataTail() const {
    return pMetaDataTail;
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

void* scalloc(size_t num, size_t size)
{
    if(num == 0 || num*size > cAllocator.MaxSupportedBytsToAlloc)
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

void sfree(void* p)
{
    if(!p )
    {
        return;
    }
    auto pTemp = static_cast<char*>(p);
    pTemp -= sizeof(SMallocMetaData);
    auto pMetadata =static_cast<SMallocMetaData*>(static_cast<void*>(pTemp));
    pMetadata->isFree = true;
    cAllocator.decreasNumOfFreeBytes(pMetadata->blockSize);
    cAllocator.decreaseNumOfFreeBlocks(1);
}





void* srealloc(void* oldp, size_t size)
{
    if( size == 0 || size > cAllocator.MaxSupportedBytsToAlloc )
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
    for (SMallocMetaData* runner = cAllocator.getPMetaDataHead(); runner ; runner=runner->next)
    {
        if(runner->isFree && runner->blockSize >=size)
        {
            cAllocator.increaseNumOfFreeBytes(pMetaData->blockSize);
            pMetaData->isFree = true;

            runner->isFree = false;
            cAllocator.decreasNumOfFreeBytes(size);
            auto temp = static_cast<void*>(runner);
            auto pChDatablk = static_cast<char*>(temp)+ sizeof(SMallocMetaData);
            auto pDataBlk = static_cast<void*>(pChDatablk);

            std::memcpy(pDataBlk,oldp,pMetaData->blockSize);
            return pDataBlk;
        }
    }

    SMallocMetaData* pOldTail = cAllocator.getPMetaDataTail();


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