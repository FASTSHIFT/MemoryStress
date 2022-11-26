/*
 * MIT License
 * Copyright (c) 2022 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __MEMORY_STRESS_H
#define __MEMORY_STRESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    void* (*mallocFunc)(size_t size);
    void (*freeFunc)(void* ptr);
    size_t maxAllocSize;
    size_t nodeLen;
    uint32_t seed;
} MemoryStress_Config_t;

typedef enum {
    MEMORY_STRESS_READ_ERROR,
    MEMORY_STRESS_WRITE_ERROR
} MemoryStress_ReadWrite_Error_t;

typedef struct
{
    uint8_t* buf;
    size_t size;
    size_t offset;
    uint32_t cnt;
    uint8_t readValue;
    uint8_t writeValue;
    MemoryStress_ReadWrite_Error_t rwError;
} MemoryStress_Error_t;

typedef struct
{
    uint8_t* buf;
    size_t size;
} MemoryStress_Node_t;

typedef struct
{
    MemoryStress_Node_t* nodeArray;
    MemoryStress_Config_t config;
    MemoryStress_Error_t error;
} MemoryStress_Context_t;

void MemoryStress_Init(MemoryStress_Context_t* context, const MemoryStress_Config_t* config);
void MemoryStress_Deinit(MemoryStress_Context_t* context);
void MemoryStress_GetError(MemoryStress_Context_t* context, MemoryStress_Error_t* error);
bool MemoryStress_Run(MemoryStress_Context_t* context);

#ifdef __cplusplus
}
#endif

#endif
