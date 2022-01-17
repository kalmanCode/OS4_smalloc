//
// Created by student on 1/16/22.
//

//#include "malloc_1.h"
#include <cstddef>

#include <unistd.h>
const unsigned int MaxSupportedBytsToAlloc(100000000);

void *smalloc(size_t byteSize)
{
    if(byteSize == 0){ return nullptr; }
    if(byteSize > MaxSupportedBytsToAlloc)  { return nullptr; }
    void* p_memAlloced = sbrk(byteSize);
    if(!p_memAlloced)   { return nullptr; }
    if(*static_cast<int*>(p_memAlloced) == -1 )
    {
        return nullptr;
    }
    return p_memAlloced;
}
