// Copyright (c) 2006-2010 Filip Wasilewski <http://filipwasilewski.pl/>
// See COPYING for license details.

// $Id$

// Common constants, typedefs and functions

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <math.h>
#include <memory.h>


///////////////////////////////////////////////////////////////////////////////
// Typedefs

#ifdef PY_EXTENSION
    #include "Python.h"
    typedef Py_ssize_t index_t;

    // using Python's memory manager
    #define wtmalloc(size)      PyMem_Malloc(size)
    #define wtfree(ptr)         PyMem_Free(ptr)
    void *wtcalloc(size_t, size_t);

    #if PY_VERSION_HEX >= 0x03000000
        static int PywtCapsule_Check(PyObject *ptr)
        {
            return PyCapsule_CheckExact(ptr);
        }
        static void * PywtCapsule_AsVoidPtr(PyObject *obj)
        {
            void *ret = PyCapsule_GetPointer(obj, NULL);
            if (ret == NULL) {
                PyErr_Clear();
            }
            return ret;
        }
    #else
        static int PywtCapsule_Check(PyObject *ptr)
        {
            return PyCObject_Check(ptr);
        }
        static void * PywtCapsule_AsVoidPtr(PyObject *ptr)
        {
            return PyCObject_AsVoidPtr(ptr);
        }
    #endif
#else
    typedef int index_t; 
    // standard c memory management
    #define wtmalloc(size)      malloc(size)
    #define wtfree(ptr)         free(ptr)
    #define wtcalloc(len, size) calloc(len, size)
#endif


typedef const index_t const_index_t;


// Signal extension modes
typedef enum {
       MODE_INVALID = -1,
       MODE_ZEROPAD = 0,   // default, signal extended with zeros
       MODE_SYMMETRIC,     // signal extended symmetrically (mirror)
       MODE_CONSTANT_EDGE, // signal extended with the border value
       MODE_SMOOTH,        // linear extrapolation (first derivative)
       MODE_PERIODIC,      // signal is treated as being periodic
       MODE_PERIODIZATION, // signal is treated as being periodic, minimal output lenght
       MODE_MAX,
       MODE_ASYMMETRIC // TODO
} MODE;


///////////////////////////////////////////////////////////////////////////////
// Calculating buffer lengths for various operations

// Length of DWT coeffs for specified input data length, filter length and
// signal extension mode 

index_t dwt_buffer_length(index_t input_len, index_t filter_len, MODE mode);

// Length of reconstructed signal for specified input coeffs length and filter
// length. It is used for direct reconstruction from coefficients (normal
// convolution of upsampled coeffs with filter).

index_t reconstruction_buffer_length(index_t coeffs_len, index_t filter_len);

// Length of IDWT reconstructed signal for specified input coeffs length, filter
// length and extension mode.

index_t idwt_buffer_length(index_t coeffs_len, index_t filter_len, MODE mode);

// Length of SWT coefficients for specified input signal length.
// Equals to input_len

index_t swt_buffer_length(index_t input_len);


///////////////////////////////////////////////////////////////////////////////
// Maximum useful level of DWT decomposition.

int dwt_max_level(index_t input_len, index_t filter_len);


///////////////////////////////////////////////////////////////////////////////
// Maximum useful level of SWT decomposition.

int swt_max_level(index_t input_len);


#endif //_COMMON_H_
