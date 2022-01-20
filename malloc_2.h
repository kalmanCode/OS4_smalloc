//
// Created by student on 1/20/22.
//

#ifndef HW4_MALLOC_2_H
#define HW4_MALLOC_2_H
#include <cstddef>

void* smalloc(size_t size);
void* scalloc(size_t num , size_t size);

void sfree(void* p);
void* srealloc(void* oldp, size_t size);
#endif //HW4_MALLOC_2_H
