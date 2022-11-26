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

static uint32_t MemoryStress_GenRandNumWithSeed(uint32_t max, uint32_t* seed)
{
    uint32_t x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;
    return x % max;
}

static uint32_t MemoryStress_GenRandNum(MemoryStress_Context_t* context, uint32_t max)
{
    return MemoryStress_GenRandNumWithSeed(max, &context->config.seed);
}

static bool MemoryStress_CheckNode(MemoryStress_Context_t* context, MemoryStress_Node_t* node)
{
    size_t size = node->size;
    uint32_t seed = size;

    /* check data */
    for (size_t i = 0; i < size; i++) {
        uint8_t writeValue = MemoryStress_GenRandNumWithSeed(UINT8_MAX, &seed);
        uint8_t readValue = node->buf[i];

        if (readValue != writeValue) {
            context->error.buf = node->buf;
            context->error.size = node->size;
            context->error.offset = i;
            context->error.readValue = readValue;
            context->error.writeValue = writeValue;
            return false;
        }
    }
    return true;
}

void MemoryStress_Init(MemoryStress_Context_t* context, const MemoryStress_Config_t* config)
{
    memset(context, 0, sizeof(MemoryStress_Context_t));
    context->config = *config;

    /* init node array */
    context->nodeArray = config->mallocFunc(config->nodeLen * sizeof(MemoryStress_Node_t));
    memset(context->nodeArray, 0, config->nodeLen * sizeof(MemoryStress_Node_t));
}

void MemoryStress_Deinit(MemoryStress_Context_t* context)
{
    /* free node array */
    context->config.freeFunc(context->nodeArray);
    context->nodeArray = NULL;
}

void MemoryStress_GetError(MemoryStress_Context_t* context, MemoryStress_Error_t* error)
{
    *error = context->error;
}

bool MemoryStress_Run(MemoryStress_Context_t* context)
{
    /* find ramdom node */
    size_t index = MemoryStress_GenRandNum(context, context->config.nodeLen);
    MemoryStress_Node_t* node = &(context->nodeArray[index]);

    /* check state */
    if (!node->buf) {
        /* generate ramdom size */
        size_t size = MemoryStress_GenRandNum(context, context->config.maxAllocSize);

        /* try alloc memory */
        uint8_t* ptr = context->config.mallocFunc(size);

        /* if alloc failed, continue */
        if (!ptr) {
            return true;
        }

        /* record node */
        node->buf = ptr;
        node->size = size;

        /* fill random data */
        uint32_t seed = size;
        while (size--) {
            *ptr++ = MemoryStress_GenRandNumWithSeed(UINT8_MAX, &seed);
        }

        /* check write OK */
        if (!MemoryStress_CheckNode(context, node)) {
            /* free node */
            context->config.freeFunc(node->buf);
            node->buf = NULL;
            context->error.rwError = MEMORY_STRESS_WRITE_ERROR;
            return false;
        }
    } else {
        /* check read OK */
        if (!MemoryStress_CheckNode(context, node)) {
            context->error.rwError = MEMORY_STRESS_READ_ERROR;
        }

        /* free node */
        context->config.freeFunc(node->buf);
        node->buf = NULL;

        if (context->error.buf) {
            /* stop test */
            return false;
        }
    }
    context->error.cnt++;
    return true;
}
