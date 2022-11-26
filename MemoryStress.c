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
#include <string.h>

typedef struct
{
    uint8_t* buf;
    size_t size;
} MemoryStress_Node_t;

static uint32_t MemoryStress_GenRandNumWithSeed(uint32_t max, uint32_t* seed)
{
    uint32_t x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;
    return x % max;
}

static uint32_t MemoryStress_GenRandNum(uint32_t max)
{
    static uint32_t a = 0x114514; /*Seed*/
    return MemoryStress_GenRandNumWithSeed(max, &a);
}

void MemoryStress_Run(const MemoryStress_Config* config, MemoryStress_Error_t* error)
{
    /* init node array */
    MemoryStress_Node_t* nodeArray = config->mallocFunc(config->nodeLen * sizeof(MemoryStress_Node_t));
    memset(nodeArray, 0, config->nodeLen * sizeof(MemoryStress_Node_t));
    memset(error, 0, sizeof(MemoryStress_Error_t));
    uint32_t cnt = 0;

    /* run test */
    while (1) {

        /* find ramdom node */
        size_t index = MemoryStress_GenRandNum(config->nodeLen);
        MemoryStress_Node_t* node = &nodeArray[index];

        /* check state */
        if (!node->buf) {
            /* generate ramdom size */
            size_t size = MemoryStress_GenRandNum(config->maxAllocSize);

            /* try alloc memory */
            uint8_t* ptr = config->mallocFunc(size);

            /* if alloc failed, continue */
            if (!ptr) {
                continue;
            }

            /* record node */
            node->buf = ptr;
            node->size = size;

            /* fill random data */
            uint32_t seed = size;
            while (size--) {
                *ptr++ = MemoryStress_GenRandNumWithSeed(UINT8_MAX, &seed);
            }
        } else {
            uint32_t size = node->size;
            uint32_t seed = size;

            for (size_t i = 0; i < size; i++) {
                uint8_t realValue = MemoryStress_GenRandNumWithSeed(UINT8_MAX, &seed);
                uint8_t readValue = node->buf[i];
                if (readValue != realValue) {
                    error->buf = node->buf;
                    error->size = node->size;
                    error->offset = i;
                    error->cnt = cnt;
                    error->readValue = readValue;
                    error->realValue = realValue;
                    break;
                }
            }

            /* free buffer */
            config->freeFunc(node->buf);
            node->buf = NULL;

            if (error->buf) {
                /* stop test */
                break;
            }
        }
        cnt++;
    }

    /* free node array */
    config->freeFunc(nodeArray);
}
