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
#include "MemoryStress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void* my_malloc(size_t size)
{
    void* ptr = malloc(size);
    printf("alloc: %zu bytes, ptr = %p\n", size, ptr);
    return ptr;
}

static void my_free(void* ptr)
{
    printf("free: %p\n", ptr);
    free(ptr);
}

int main(int argc, const char* argv[])
{
    MemoryStress_Config_t config;
    memset(&config, 0, sizeof(config));
    config.mallocFunc = my_malloc;
    config.freeFunc = my_free;
    config.maxAllocSize = 1024;
    config.nodeLen = 1024;
    config.seed = 0x114514;

    MemoryStress_Context_t context;
    MemoryStress_Init(&context, &config);

    printf("MemoryStress: testing...\n");

    while (MemoryStress_Run(&context)) {
        usleep(1);
    }

    MemoryStress_Error_t error;
    MemoryStress_GetError(&context, &error);

    printf("MemoryStress: %s ERROR!, "
           "buf = %p, "
           "size = %zu, "
           "offset = %zu(addr = %p), "
           "cnt = %d, "
           "readValue = 0x%x, "
           "writeValue = 0x%x\n",
        error.rwError == MEMORY_STRESS_READ_ERROR ? "READ" : "WRITE",
        error.buf,
        error.size,
        error.offset, error.buf + error.offset,
        error.cnt,
        error.readValue,
        error.writeValue);

    MemoryStress_Deinit(&context);

    return 0;
}
