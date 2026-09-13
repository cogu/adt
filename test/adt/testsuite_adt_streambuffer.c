/*****************************************************************************
* \file      testsuite_adt_streambuffer.c
* \author    Conny Gustafsson
* \date      2026-09-10
* \brief     Unit tests for adt_streambuffer_t
*
* Copyright (c) 2026 Conny Gustafsson
* SPDX-License-Identifier: MIT
* See LICENSE in project root for full license terms.
******************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test_common.h"
#include "adt_streambuffer.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void test_streambuffer_create_destroy(CuTest* tc);
static void test_streambuffer_small_reads(CuTest* tc);
static void test_streambuffer_drain_reset(CuTest* tc);
static void test_streambuffer_rollover_remainder(CuTest* tc);
static void test_streambuffer_jumbo_and_shrink(CuTest* tc);
static void test_streambuffer_direct_write(CuTest* tc);
static void test_streambuffer_stress(CuTest* tc);
static void test_streambuffer_clear(CuTest* tc);
static void test_streambuffer_error_handling(CuTest* tc);
static void test_streambuffer_reserve_in_place(CuTest* tc);
static void test_streambuffer_reserve_compacting(CuTest* tc);
static void test_streambuffer_reserve_128kb_stream(CuTest* tc);

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
CuSuite* testsuite_adt_streambuffer(void)
{
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, test_streambuffer_create_destroy);
   SUITE_ADD_TEST(suite, test_streambuffer_small_reads);
   SUITE_ADD_TEST(suite, test_streambuffer_drain_reset);
   SUITE_ADD_TEST(suite, test_streambuffer_rollover_remainder);
   SUITE_ADD_TEST(suite, test_streambuffer_jumbo_and_shrink);
   SUITE_ADD_TEST(suite, test_streambuffer_direct_write);
   SUITE_ADD_TEST(suite, test_streambuffer_stress);
   SUITE_ADD_TEST(suite, test_streambuffer_clear);
   SUITE_ADD_TEST(suite, test_streambuffer_error_handling);
   SUITE_ADD_TEST(suite, test_streambuffer_reserve_in_place);
   SUITE_ADD_TEST(suite, test_streambuffer_reserve_compacting);
   SUITE_ADD_TEST(suite, test_streambuffer_reserve_128kb_stream);

   return suite;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static void test_streambuffer_create_destroy(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 0u, 0u);

   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_DEFAULT_SLAB_SIZE, sb.default_slab_size);
   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_DEFAULT_MAX_RETAINED_SIZE, sb.max_retained_size);
   CuAssertIntEquals(tc, 0, sb.write_slab_idx);
   CuAssertIntEquals(tc, 0, sb.read_slab_idx);
   CuAssertUIntEquals(tc, 0u, sb.read_pos);
   CuAssertUIntEquals(tc, 0u, adt_streambuffer_size(&sb));
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));
   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_NUM_SLABS * ADT_STREAMBUFFER_DEFAULT_SLAB_SIZE, adt_streambuffer_allocated_bytes(&sb));

   adt_streambuffer_destroy(&sb);

   // Test heap allocation
   adt_streambuffer_t *pHeap = adt_streambuffer_new(1024u, 4096u);
   CuAssertPtrNotNull(tc, pHeap);
   CuAssertUIntEquals(tc, 1024u, pHeap->default_slab_size);
   CuAssertUIntEquals(tc, 4096u, pHeap->max_retained_size);
   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_NUM_SLABS * 1024u, adt_streambuffer_allocated_bytes(pHeap));
   adt_streambuffer_delete(pHeap);

   // Test vdelete
   pHeap = adt_streambuffer_new(512u, 2048u);
   CuAssertPtrNotNull(tc, pHeap);
   adt_streambuffer_vdelete(pHeap);
}

static void test_streambuffer_small_reads(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 4096u, 16384u);

   uint8_t inputData[4000];
   for (uint32_t i = 0; i < 4000u; i++)
   {
      inputData[i] = (uint8_t) (i & 0xFFu);
   }

   adt_error_t err = adt_streambuffer_append(&sb, inputData, 4000u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 4000u, adt_streambuffer_size(&sb));
   CuAssertTrue(tc, !adt_streambuffer_is_empty(&sb));

   // Consume 4 bytes at a time (1000 iterations)
   for (uint32_t iter = 0; iter < 1000u; iter++)
   {
      uint32_t avail = 0u;
      const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
      CuAssertPtrNotNull(tc, pRead);
      CuAssertUIntEquals(tc, 4000u - (iter * 4u), avail);

      for (uint32_t b = 0; b < 4u; b++)
      {
         uint8_t expected = (uint8_t) (((iter * 4u) + b) & 0xFFu);
         CuAssertIntEquals(tc, expected, pRead[b]);
      }

      err = adt_streambuffer_read_commit(&sb, 4u);
      CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   }

   CuAssertUIntEquals(tc, 0u, adt_streambuffer_size(&sb));
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   uint32_t finalAvail = 0u;
   const uint8_t *pFinal = adt_streambuffer_read_begin(&sb, &finalAvail);
   CuAssertPtrEquals(tc, NULL, (void*) pFinal);
   CuAssertUIntEquals(tc, 0u, finalAvail);

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_drain_reset(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 4096u, 16384u);

   uint8_t buf[500];
   memset(buf, 0xAB, sizeof(buf));

   adt_error_t err = adt_streambuffer_append(&sb, buf, sizeof(buf));
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertIntEquals(tc, 0, sb.write_slab_idx);
   CuAssertIntEquals(tc, 0, sb.read_slab_idx);

   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, 500u, avail);

   err = adt_streambuffer_read_commit(&sb, 500u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);

   // Verified in-place drain: read_pos and CurLen reset to 0 without advancing slab index
   CuAssertUIntEquals(tc, 0u, sb.read_pos);
   CuAssertUIntEquals(tc, 0u, sb.slabs[0].u32CurLen);
   CuAssertIntEquals(tc, 0, sb.write_slab_idx);
   CuAssertIntEquals(tc, 0, sb.read_slab_idx);
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   // Ingest again: should still be in slab 0
   err = adt_streambuffer_append(&sb, buf, 300u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertIntEquals(tc, 0, sb.write_slab_idx);
   CuAssertIntEquals(tc, 0, sb.read_slab_idx);
   CuAssertUIntEquals(tc, 300u, adt_streambuffer_size(&sb));

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_rollover_remainder(CuTest* tc)
{
   adt_streambuffer_t sb;
   // Baseline slab size of 128 bytes
   adt_streambuffer_create(&sb, 128u, 512u);

   // Ingest 120 bytes: 24 chunks of 5 bytes
   uint8_t data1[120];
   for (uint32_t i = 0; i < 120u; i++)
   {
      data1[i] = (uint8_t) (i + 1u);
   }
   adt_error_t err = adt_streambuffer_append(&sb, data1, 120u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);

   // Consume 115 bytes (23 chunks of 5 bytes), leaving 5 bytes unparsed
   err = adt_streambuffer_read_commit(&sb, 115u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 5u, adt_streambuffer_size(&sb));
   CuAssertIntEquals(tc, 0, sb.write_slab_idx);
   CuAssertIntEquals(tc, 0, sb.read_slab_idx);

   // Append 20 new bytes: 120 + 20 = 140 > 128 capacity, triggering rollover to slab 1
   uint8_t data2[20];
   for (uint32_t i = 0; i < 20u; i++)
   {
      data2[i] = (uint8_t) (200u + i);
   }
   err = adt_streambuffer_append(&sb, data2, 20u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);

   // Rollover occurred: active slab is now 1
   CuAssertIntEquals(tc, 1, sb.write_slab_idx);
   CuAssertIntEquals(tc, 1, sb.read_slab_idx);
   CuAssertUIntEquals(tc, 0u, sb.read_pos);
   CuAssertUIntEquals(tc, 25u, adt_streambuffer_size(&sb));

   // Slab 0 is retired and cleared
   CuAssertUIntEquals(tc, 0u, sb.slabs[0].u32CurLen);

   // Guaranteed contiguous view: consumer sees all 25 bytes in one contiguous buffer
   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, 25u, avail);

   // Verify the first 5 bytes are the unparsed remainder (data1[115..119])
   for (uint32_t i = 0; i < 5u; i++)
   {
      CuAssertIntEquals(tc, data1[115u + i], pRead[i]);
   }
   // Verify the remaining 20 bytes are data2[0..19]
   for (uint32_t i = 0; i < 20u; i++)
   {
      CuAssertIntEquals(tc, data2[i], pRead[5u + i]);
   }

   err = adt_streambuffer_read_commit(&sb, 25u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_jumbo_and_shrink(CuTest* tc)
{
   adt_streambuffer_t sb;
   const uint32_t baseline = 1024u;
   const uint32_t threshold = 4096u;
   adt_streambuffer_create(&sb, baseline, threshold);

   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_NUM_SLABS * baseline, adt_streambuffer_allocated_bytes(&sb));

   // Write 256 kB payload
   const uint32_t jumboSize = 256u * 1024u;
   uint8_t *jumboData = (uint8_t*) malloc(jumboSize);
   CuAssertPtrNotNull(tc, jumboData);
   assert(jumboData != NULL);
   for (uint32_t i = 0; i < jumboSize; i++)
   {
      jumboData[i] = (uint8_t) (i & 0xFFu);
   }

   adt_error_t err = adt_streambuffer_append(&sb, jumboData, jumboSize);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, jumboSize, adt_streambuffer_size(&sb));

   // Allocated memory must have grown to accommodate >= 256 kB
   CuAssertTrue(tc, adt_streambuffer_allocated_bytes(&sb) >= jumboSize);

   // Read back and verify contiguous buffer
   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, jumboSize, avail);
   CuAssertIntEquals(tc, 0, memcmp(jumboData, pRead, jumboSize));

   // Drain jumbo message completely
   err = adt_streambuffer_read_commit(&sb, jumboSize);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   // Shrink-on-drain verification: slab must have downsized back to baseline
   CuAssertUIntEquals(tc, baseline, sb.slabs[sb.read_slab_idx].u32AllocLen);
   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_NUM_SLABS * baseline, adt_streambuffer_allocated_bytes(&sb));

   // Also test shrink on rollover
   err = adt_streambuffer_append(&sb, jumboData, jumboSize);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   // Consume almost all, leaving 10 bytes unparsed
   err = adt_streambuffer_read_commit(&sb, jumboSize - 10u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 10u, adt_streambuffer_size(&sb));

   // Appending 100 bytes triggers rollover because jumbo slab is at capacity
   uint8_t smallBuf[100];
   memset(smallBuf, 0xEE, sizeof(smallBuf));
   err = adt_streambuffer_append(&sb, smallBuf, sizeof(smallBuf));
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);

   // The retired jumbo slab must have shrunk back to baseline!
   uint8_t oldSlabIdx = (sb.write_slab_idx + ADT_STREAMBUFFER_NUM_SLABS - 1u) % ADT_STREAMBUFFER_NUM_SLABS;
   CuAssertUIntEquals(tc, baseline, sb.slabs[oldSlabIdx].u32AllocLen);

   free(jumboData);
   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_direct_write(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 1024u, 4096u);

   uint32_t avail = 0u;
   uint8_t *pWrite = adt_streambuffer_write_begin(&sb, 200u, &avail);
   CuAssertPtrNotNull(tc, pWrite);
   CuAssertTrue(tc, avail >= 200u);

   for (uint32_t i = 0; i < 200u; i++)
   {
      pWrite[i] = (uint8_t) (i * 2u);
   }

   adt_error_t err = adt_streambuffer_write_commit(&sb, 200u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 200u, adt_streambuffer_size(&sb));

   uint32_t readAvail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &readAvail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, 200u, readAvail);
   for (uint32_t i = 0; i < 200u; i++)
   {
      CuAssertIntEquals(tc, (uint8_t)(i * 2u), pRead[i]);
   }

   err = adt_streambuffer_read_commit(&sb, 200u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   // Test committing 0 bytes does not corrupt state
   pWrite = adt_streambuffer_write_begin(&sb, 50u, &avail);
   CuAssertPtrNotNull(tc, pWrite);
   err = adt_streambuffer_write_commit(&sb, 0u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 0u, adt_streambuffer_size(&sb));

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_stress(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 512u, 2048u);

   uint32_t writeSeq = 0u;
   uint32_t readSeq = 0u;

   // Stream 50,000 bytes with varying chunk and framing sizes
   const uint32_t totalBytes = 50000u;

   while (readSeq < totalBytes)
   {
      // Producer phase
      if (writeSeq < totalBytes)
      {
         // Pseudo-random chunk size between 1 and 300
         uint32_t chunkSize = 1u + ((writeSeq * 17u + 13u) % 300u);
         if (writeSeq + chunkSize > totalBytes)
         {
            chunkSize = totalBytes - writeSeq;
         }

         uint8_t writeBuf[300] = {0};
         for (uint32_t i = 0; i < chunkSize; i++)
         {
            writeBuf[i] = (uint8_t) ((writeSeq + i) & 0xFFu);
         }

         adt_error_t err = adt_streambuffer_append(&sb, writeBuf, chunkSize);
         CuAssertIntEquals(tc, ADT_NO_ERROR, err);
         writeSeq += chunkSize;
      }

      // Consumer phase: consume some or all available bytes
      uint32_t avail = 0u;
      const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
      if ((pRead != NULL) && (avail > 0u))
      {
         // Pseudo-random framing parse size between 1 and 150
         uint32_t parseSize = 1u + ((readSeq * 31u + 7u) % 150u);
         if (parseSize > avail)
         {
            parseSize = avail;
         }

         for (uint32_t i = 0; i < parseSize; i++)
         {
            uint8_t expected = (uint8_t) ((readSeq + i) & 0xFFu);
            CuAssertIntEquals(tc, expected, pRead[i]);
         }

         adt_error_t err = adt_streambuffer_read_commit(&sb, parseSize);
         CuAssertIntEquals(tc, ADT_NO_ERROR, err);
         readSeq += parseSize;
      }
   }

   CuAssertUIntEquals(tc, totalBytes, readSeq);
   CuAssertUIntEquals(tc, 0u, adt_streambuffer_size(&sb));
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_clear(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 1024u, 4096u);

   uint8_t data[2000];
   memset(data, 0x77, sizeof(data));
   adt_error_t err = adt_streambuffer_append(&sb, data, sizeof(data));
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 2000u, adt_streambuffer_size(&sb));

   adt_streambuffer_clear(&sb);

   CuAssertUIntEquals(tc, 0u, adt_streambuffer_size(&sb));
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));
   CuAssertIntEquals(tc, 0, sb.write_slab_idx);
   CuAssertIntEquals(tc, 0, sb.read_slab_idx);
   CuAssertUIntEquals(tc, 0u, sb.read_pos);

   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrEquals(tc, NULL, (void*) pRead);
   CuAssertUIntEquals(tc, 0u, avail);

   // Clear after oversized allocation downsizes back to default
   const uint32_t bigSize = 32u * 1024u;
   uint8_t *bigBuf = (uint8_t*) malloc(bigSize);
   CuAssertPtrNotNull(tc, bigBuf);
   assert(bigBuf != NULL);
   memset(bigBuf, 0x33, bigSize);
   err = adt_streambuffer_append(&sb, bigBuf, bigSize);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertTrue(tc, adt_streambuffer_allocated_bytes(&sb) >= bigSize);

   adt_streambuffer_clear(&sb);
   CuAssertUIntEquals(tc, ADT_STREAMBUFFER_NUM_SLABS * 1024u, adt_streambuffer_allocated_bytes(&sb));

   free(bigBuf);
   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_error_handling(CuTest* tc)
{
   // NULL instance checks
   CuAssertUIntEquals(tc, 0u, adt_streambuffer_size(NULL));
   CuAssertUIntEquals(tc, 0u, adt_streambuffer_allocated_bytes(NULL));
   CuAssertTrue(tc, adt_streambuffer_is_empty(NULL));
   CuAssertPtrEquals(tc, NULL, (void*) adt_streambuffer_read_begin(NULL, NULL));
   CuAssertPtrEquals(tc, NULL, adt_streambuffer_write_begin(NULL, 100u, NULL));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_streambuffer_read_commit(NULL, 10u));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_streambuffer_write_commit(NULL, 10u));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_streambuffer_append(NULL, (const uint8_t*)"abc", 3u));
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_streambuffer_reserve(NULL, 1024u));

   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 1024u, 4096u);

   // Reserve 0 bytes succeeds
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_reserve(&sb, 0u));

   // Append NULL pointer with >0 len
   CuAssertIntEquals(tc, ADT_INVALID_ARGUMENT_ERROR, adt_streambuffer_append(&sb, NULL, 10u));

   // Append 0 bytes succeeds
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_append(&sb, NULL, 0u));

   // Ingest 50 bytes
   uint8_t buf[50];
   memset(buf, 0x11, sizeof(buf));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_append(&sb, buf, 50u));

   // read_commit with more bytes than available returns ADT_LENGTH_ERROR
   CuAssertIntEquals(tc, ADT_LENGTH_ERROR, adt_streambuffer_read_commit(&sb, 51u));

   // write_commit with more bytes than available returns ADT_LENGTH_ERROR
   uint32_t avail = 0u;
   adt_streambuffer_write_begin(&sb, 10u, &avail);
   CuAssertIntEquals(tc, ADT_LENGTH_ERROR, adt_streambuffer_write_commit(&sb, avail + 1u));

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_reserve_in_place(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 4096u, 16384u);

   // Ingest 4096 bytes
   uint8_t payload[4096];
   for (uint32_t i = 0u; i < sizeof(payload); i++)
   {
      payload[i] = (uint8_t)(i & 0xFFu);
   }
   adt_error_t err = adt_streambuffer_append(&sb, payload, sizeof(payload));
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertUIntEquals(tc, 0u, sb.write_slab_idx);
   CuAssertUIntEquals(tc, 0u, sb.read_pos);

   // Consumer inspects header and reserves 128KB in place
   err = adt_streambuffer_reserve(&sb, 128u * 1024u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);

   // Should remain on slab 0 with read_pos 0, and expanded capacity
   CuAssertUIntEquals(tc, 0u, sb.write_slab_idx);
   CuAssertUIntEquals(tc, 0u, sb.read_pos);
   CuAssertTrue(tc, sb.slabs[0].u32AllocLen >= 128u * 1024u);
   CuAssertUIntEquals(tc, 4096u, sb.slabs[0].u32CurLen);
   CuAssertUIntEquals(tc, 4096u, sb.slabs[1].u32AllocLen);

   // Data integrity check
   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, 4096u, avail);
   CuAssertIntEquals(tc, 0, memcmp(pRead, payload, sizeof(payload)));

   // Subsequent reserve of smaller size is a no-op
   err = adt_streambuffer_reserve(&sb, 50000u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);
   CuAssertTrue(tc, sb.slabs[0].u32AllocLen >= 128u * 1024u);

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_reserve_compacting(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 4096u, 16384u);

   // Ingest Message A (100 bytes) + Message B start (2000 bytes) - both fit in 4096 baseline slab
   uint8_t msgA[100];
   uint8_t msgB[2000];
   memset(msgA, 0xAA, sizeof(msgA));
   for (uint32_t i = 0u; i < sizeof(msgB); i++)
   {
      msgB[i] = (uint8_t)((i + 7u) & 0xFFu);
   }

   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_append(&sb, msgA, sizeof(msgA)));
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_append(&sb, msgB, sizeof(msgB)));

   // Consumer parses and commits Message A
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_read_commit(&sb, sizeof(msgA)));
   CuAssertUIntEquals(tc, sizeof(msgA), sb.read_pos);
   CuAssertUIntEquals(tc, 0u, sb.write_slab_idx);

   // Consumer reads Message B header and reserves 64KB
   adt_error_t err = adt_streambuffer_reserve(&sb, 64u * 1024u);
   CuAssertIntEquals(tc, ADT_NO_ERROR, err);

   // Compacted to slab 1 at offset 0
   CuAssertUIntEquals(tc, 1u, sb.write_slab_idx);
   CuAssertUIntEquals(tc, 1u, sb.read_slab_idx);
   CuAssertUIntEquals(tc, 0u, sb.read_pos);
   CuAssertUIntEquals(tc, sizeof(msgB), sb.slabs[1].u32CurLen);
   CuAssertTrue(tc, sb.slabs[1].u32AllocLen >= 64u * 1024u);

   // Verify Message B data was preserved
   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, sizeof(msgB), avail);
   CuAssertIntEquals(tc, 0, memcmp(pRead, msgB, sizeof(msgB)));

   adt_streambuffer_destroy(&sb);
}

static void test_streambuffer_reserve_128kb_stream(CuTest* tc)
{
   adt_streambuffer_t sb;
   adt_streambuffer_create(&sb, 0u, 0u); // Defaults: 4KB slab, 16KB max retained

   uint8_t chunk[4096];
   for (uint32_t i = 0u; i < sizeof(chunk); i++)
   {
      chunk[i] = (uint8_t)(i & 0xFFu);
   }

   // Write 1: 4096 bytes
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_append(&sb, chunk, sizeof(chunk)));

   // Consumer reads header and reserves full 128KB
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_reserve(&sb, 128u * 1024u));

   // Producer writes remaining 31 chunks
   uint8_t initial_active_slab = sb.write_slab_idx;
   uint32_t swap_count = 0u;
   uint8_t last_idx = initial_active_slab;

   for (uint32_t i = 2u; i <= 32u; i++)
   {
      uint32_t avail = 0u;
      uint8_t *pWrite = adt_streambuffer_write_begin(&sb, sizeof(chunk), &avail);
      CuAssertPtrNotNull(tc, pWrite);
      CuAssertTrue(tc, avail >= sizeof(chunk));

      if (sb.write_slab_idx != last_idx)
      {
         swap_count++;
         last_idx = sb.write_slab_idx;
      }

      memcpy(pWrite, chunk, sizeof(chunk));
      CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_write_commit(&sb, sizeof(chunk)));
   }

   // Zero slab swaps occurred during the remaining 31 writes!
   CuAssertUIntEquals(tc, 0u, swap_count);
   CuAssertUIntEquals(tc, initial_active_slab, sb.write_slab_idx);
   CuAssertUIntEquals(tc, 128u * 1024u, adt_streambuffer_size(&sb));

   // Consumer reads the entire contiguous 128KB
   uint32_t avail = 0u;
   const uint8_t *pRead = adt_streambuffer_read_begin(&sb, &avail);
   CuAssertPtrNotNull(tc, pRead);
   CuAssertUIntEquals(tc, 128u * 1024u, avail);

   // Verify each chunk content
   for (uint32_t i = 0u; i < 32u; i++)
   {
      CuAssertIntEquals(tc, 0, memcmp(pRead + ((size_t) i * 4096u), chunk, sizeof(chunk)));
   }

   // Consumer consumes full message
   CuAssertIntEquals(tc, ADT_NO_ERROR, adt_streambuffer_read_commit(&sb, 128u * 1024u));
   CuAssertTrue(tc, adt_streambuffer_is_empty(&sb));

   // Shrink-on-drain: active slab should have shrunk back to default 4KB
   CuAssertUIntEquals(tc, 4096u, sb.slabs[sb.read_slab_idx].u32AllocLen);

   adt_streambuffer_destroy(&sb);
}
