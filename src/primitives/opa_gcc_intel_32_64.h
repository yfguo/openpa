/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef OPA_GCC_INTEL_32_64_H_INCLUDED
#define OPA_GCC_INTEL_32_64_H_INCLUDED

#define MPIDU_ATOMIC_UNIVERSAL_PRIMITIVE MPIDU_ATOMIC_CAS

/* XXX DJG FIXME do we need to align these? */
typedef struct { volatile int v;  } OPA_t;
typedef struct { int * volatile v; } OPA_ptr_t;

/* Aligned loads and stores are atomic on x86(-64). */
static inline int OPA_load(OPA_t *ptr)
{
    return ptr->v;
}

/* Aligned loads and stores are atomic on x86(-64). */
static inline void OPA_store(OPA_t *ptr, int val)
{
    ptr->v = val;
}

/* Aligned loads and stores are atomic on x86(-64). */
static inline void *OPA_load_ptr(OPA_ptr_t *ptr)
{
    return ptr->v;
}

/* Aligned loads and stores are atomic on x86(-64). */
static inline void OPA_store_ptr(OPA_ptr_t *ptr, void *val)
{
    ptr->v = val;
}

static inline void OPA_add(OPA_t *ptr, int val)
{
    __asm__ __volatile__ ("lock ; add %1,%0"
                          :"=m" (ptr->v)
                          :"ir" (val), "m" (ptr->v));
}

static inline void OPA_incr(OPA_t *ptr)
{
    switch(sizeof(ptr->v))
    {
    case 4:
        __asm__ __volatile__ ("lock ; incl %0"
                              :"=m" (ptr->v)
                              :"m" (ptr->v));
        break;
    case 8:
        __asm__ __volatile__ ("lock ; incq %0"
                              :"=m" (ptr->v)
                              :"m" (ptr->v));
        break;
    default:
        /* int is not 64 or 32 bits  */
        OPA_assert(0);
    }
    return;
}

static inline void OPA_decr(OPA_t *ptr)
{
    switch(sizeof(ptr->v))
    {
    case 4:
        __asm__ __volatile__ ("lock ; decl %0"
                              :"=m" (ptr->v)
                              :"m" (ptr->v));
        break;
    case 8:
        __asm__ __volatile__ ("lock ; decq %0"
                              :"=m" (ptr->v)
                              :"m" (ptr->v));
        break;
    default:
        /* int is not 64 or 32 bits  */
        OPA_assert(0);
    }
    return;
}


static inline int OPA_decr_and_test(OPA_t *ptr)
{
    int result;
    switch(sizeof(ptr->v))
    {
    case 4:
        __asm__ __volatile__ ("lock ; decl %0; setz %1"
                              :"=m" (ptr->v), "=q" (result)
                              :"m" (ptr->v));
        break;
    case 8:
        __asm__ __volatile__ ("lock ; decq %0; setz %1"
                              :"=m" (ptr->v), "=q" (result)
                              :"m" (ptr->v));
        break;
    default:
        /* int is not 64 or 32 bits  */
        OPA_assert(0);
    }
    return result;
}

static inline int OPA_fetch_and_add(OPA_t *ptr, int val)
{
    __asm__ __volatile__ ("lock ; xadd %0,%1"
                          : "=r" (val), "=m" (ptr->v)
                          :  "0" (val),  "m" (ptr->v));
    return val;
}

#define OPA_fetch_and_decr_by_faa OPA_fetch_and_decr 
#define OPA_fetch_and_incr_by_faa OPA_fetch_and_incr 


static inline int *OPA_cas_ptr(OPA_ptr_t *ptr, void *oldv, void *newv)
{
    int *prev;
    __asm__ __volatile__ ("lock ; cmpxchg %2,%3"
                          : "=a" (prev), "=m" (ptr->v)
                          : "q" (newv), "m" (ptr->v), "0" (oldv));
    return prev;
}

static inline int OPA_cas_int(OPA_t *ptr, int oldv, int newv)
{
    int prev;
    __asm__ __volatile__ ("lock ; cmpxchg %2,%3"
                          : "=a" (prev), "=m" (ptr->v)
                          : "q" (newv), "m" (ptr->v), "0" (oldv)
                          : "memory");
    return prev;
}

static inline int *OPA_swap_ptr(OPA_ptr_t *ptr, void *val)
{
    __asm__ __volatile__ ("xchg %0,%1"
                          :"=r" (val), "=m" (ptr->v)
                          : "0" (val),  "m" (ptr->v));
    return val;
}

static inline int OPA_swap_int(OPA_t *ptr, int val)
{
    __asm__ __volatile__ ("xchg %0,%1"
                          :"=r" (val), "=m" (ptr->v)
                          : "0" (val),  "m" (ptr->v));
    return val;
}

#include"opa_emulated.h"

#endif /* OPA_GCC_INTEL_32_64_H_INCLUDED */