/**
* cogu 2017-02-19: This is a slighty modified version of CuTest.c v1.5 (http://cutest.sourceforge.net)
* I have fixed a memory leak in the framework as well as adding test macro for unsigned integer equality (CuAssertUIntEquals).
* cogu 2026-09-05: Resolve Clang-tidy issues.
*
*/

#ifndef CU_TEST_H
#define CU_TEST_H

#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#define CUTEST_VERSION  "CuTest 1.5_COGU_PATCH_1"

/* CuString */

char* CuStrAlloc(int size);
char* CuStrCopy(const char* old);

#define CU_ALLOC(TYPE)		((TYPE*) malloc(sizeof(TYPE)))

#define HUGE_STRING_LEN	8192
#define STRING_MAX		256
#define STRING_INC		256

/* cogu 2026-09-05: Added cross-platform string functions */
#ifdef _MSC_VER
#define STRLCPY(dest, src, size) strcpy_s((dest), (size), (src))
#define STRLCAT(dest, src, size) strcat_s((dest), (size), (src))
#else
#define STRLCPY(dest, src, size) strlcpy((dest), (src), (size))
#define STRLCAT(dest, src, size) strlcat((dest), (src), (size))
#endif

/* cogu 2026-09-05: Added noreturn macro for MSVC and GCC/Clang */
#if defined(_MSC_VER)
#define CU_NORETURN __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
#define CU_NORETURN __attribute__((noreturn))
#else
#define CU_NORETURN
#endif

typedef struct
{
    int length;
    int size;
    char* buffer;
} CuString;

void CuStringInit(CuString* str);
CuString* CuStringNew(void);
void CuStringRead(CuString* str, const char* path);
void CuStringAppend(CuString* str, const char* text);
void CuStringAppendChar(CuString* str, char ch);
void CuStringAppendFormat(CuString* str, const char* format, ...);
void CuStringInsert(CuString* str, const char* text, int pos);
void CuStringResize(CuString* str, int newSize);
void CuStringDelete(CuString* str);

/* CuTest */

typedef struct CuTest CuTest;

typedef void (*TestFunction)(CuTest *);
typedef void (*CuFnPtr)(void);

struct CuTest
{
    char* name;
    TestFunction function;
    int failed;
    int ran;
    const char* message;
    jmp_buf *jumpBuf;
};

void CuTestInit(CuTest* t, const char* name, TestFunction function);
CuTest* CuTestNew(const char* name, TestFunction function);
void CuTestRun(CuTest* tc);
void CuTestDelete(CuTest *t);

/* Internal versions of assert functions -- use the public versions */
CU_NORETURN void CuFail_Line(CuTest* tc, const char* file, int line, const char* message2, const char* message);
void CuAssert_Line(CuTest* tc, const char* file, int line, const char* message, int condition);
void CuAssertStrEquals_LineMsg(CuTest* tc,
    const char* file, int line, const char* message,
    const char* expected, const char* actual);
void CuAssertIntEquals_LineMsg(CuTest* tc,
    const char* file, int line, const char* message,
    int expected, int actual);
void CuAssertUIntEquals_LineMsg(CuTest* tc,
   const char* file, int line, const char* message,
   unsigned int expected, unsigned int actual);
void CuAssertULIntEquals_LineMsg(CuTest* tc,
   const char* file, int line, const char* message,
   unsigned long long int expected, unsigned long long int actual);
void CuAssertDblEquals_LineMsg(CuTest* tc,
    const char* file, int line, const char* message,
    double expected, double actual, double delta);
void CuAssertPtrEquals_LineMsg(CuTest* tc,
    const char* file, int line, const char* message,
    void* expected, void* actual);
void CuAssertConstPtrEquals_LineMsg(CuTest* tc,
   const char* file, int line, const char* message,
   const void* expected, const void* actual);
void CuAssertFnPtrEquals_LineMsg(CuTest* tc,
   const char* file, int line, const char* message,
   CuFnPtr expected, CuFnPtr actual);


/* public assert functions */

#define CuFail(tc, ms)                        CuFail_Line(  (tc), __FILE__, __LINE__, NULL, (ms))
/* cogu 2026-09-05: Inlined assertion condition checks to avoid false positive null dereference warnings in static analysis */
#define CuAssert(tc, ms, cond) \
    do { \
        if (!(cond)) { \
            CuFail_Line((tc), __FILE__, __LINE__, NULL, (ms)); \
        } \
    } while(0)
#define CuAssertTrue(tc, cond) \
    do { \
        if (!(cond)) { \
            CuFail_Line((tc), __FILE__, __LINE__, NULL, "assert failed"); \
        } \
    } while(0)

#define CuAssertStrEquals(tc,ex,ac)           CuAssertStrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CuAssertStrEquals_Msg(tc,ms,ex,ac)    CuAssertStrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CuAssertIntEquals(tc,ex,ac)           CuAssertIntEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CuAssertIntEquals_Msg(tc,ms,ex,ac)    CuAssertIntEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CuAssertUIntEquals(tc,ex,ac)          CuAssertUIntEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CuAssertUIntEquals_Msg(tc,ms,ex,ac)   CuAssertUIntEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CuAssertULIntEquals(tc,ex,ac)         CuAssertULIntEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CuAssertULIntEquals_Msg(tc,ms,ex,ac)  CuAssertULIntEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CuAssertDblEquals(tc,ex,ac,dl)        CuAssertDblEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac),(dl))
#define CuAssertDblEquals_Msg(tc,ms,ex,ac,dl) CuAssertDblEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac),(dl))
#define CuAssertPtrEquals(tc,ex,ac)           CuAssertPtrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CuAssertPtrEquals_Msg(tc,ms,ex,ac)    CuAssertPtrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
#define CuAssertConstPtrEquals(tc,ex,ac)           CuAssertConstPtrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(ex),(ac))
#define CuAssertConstPtrEquals_Msg(tc,ms,ex,ac)    CuAssertConstPtrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(ex),(ac))
/* cogu 2026-09-05: Added below helper functions to solve a compiler warning issue */
#define CuAssertFnPtrEquals(tc,ex,ac)              CuAssertFnPtrEquals_LineMsg((tc),__FILE__,__LINE__,NULL,(CuFnPtr)(ex),(CuFnPtr)(ac))
#define CuAssertFnPtrEquals_Msg(tc,ms,ex,ac)       CuAssertFnPtrEquals_LineMsg((tc),__FILE__,__LINE__,(ms),(CuFnPtr)(ex),(CuFnPtr)(ac))


#define CuAssertPtrNotNull(tc,p) \
    do { \
        if ((p) == NULL) { \
            CuFail_Line((tc),__FILE__,__LINE__,NULL,"null pointer unexpected"); \
        } \
    } while(0)
#define CuAssertPtrNotNullMsg(tc,msg,p) \
    do { \
        if ((p) == NULL) { \
            CuFail_Line((tc),__FILE__,__LINE__,(msg),"null pointer unexpected"); \
        } \
    } while(0)

/* CuSuite */

#define MAX_TEST_CASES	1024

#define SUITE_ADD_TEST(SUITE,TEST)	CuSuiteAdd(SUITE, CuTestNew(#TEST, TEST))

typedef struct
{
    int count;
    CuTest* list[MAX_TEST_CASES];
    int failCount;

} CuSuite;


void CuSuiteInit(CuSuite* testSuite);
CuSuite* CuSuiteNew(void);
void CuSuiteDelete(CuSuite *testSuite);
void CuSuiteAdd(CuSuite* testSuite, CuTest *testCase);
void CuSuiteAddSuite(CuSuite* testSuite, CuSuite* testSuite2);
void CuSuiteRun(CuSuite* testSuite);
void CuSuiteSummary(CuSuite* testSuite, CuString* summary);
void CuSuiteDetails(CuSuite* testSuite, CuString* details);

#endif /* CU_TEST_H */
