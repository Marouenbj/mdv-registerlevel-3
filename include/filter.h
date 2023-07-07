#pragma once
#ifndef FILTER_H
#define FILTER_H

#include "string.h"

#include "filter_coeffs.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

#include "cmsis_gcc.h"
#include "stdatomic.h"

/* @brief filter_t struct is initialised in main.c */
typedef struct filter_t
{
  size_t numTaps;
  volatile int16_t state[FIR_LENGTH];
  const int16_t coeffs[FIR_LENGTH];
  struct
  {
    size_t fillcnt;
    size_t active_buffer;
    size_t passive_buffer;
    volatile int16_t buffer[2][FIR_LENGTH];
  } output;
  struct
  {
    size_t decimator;
    size_t decim_reload;
  } status;
} filter_t;

/**
 * @brief Shift filter state one element to right
 * @important use void* memcpy (void *restrict dest, const
 * void *restrict src, size_t n) for performance benefits.
 */
__STATIC_FORCEINLINE void
filter_shift_right (filter_t *filt)
{
  // TODO
}

/**
 * @brief Implements a decimating filter
 * @important Decimation factor is loaded in
 * filt->status.decim_reload, decrement and compare
 * filt->status.decimator with null for performance
 * benefits
 * @return true: not decimated
 * @return false: decimated
 */
__STATIC_FORCEINLINE bool
filter_decim (filter_t *filt)
{
  // Decrement filt->status.decimator
  // TODO

  // Check if null
  if (/* TODO */)
    {
      // Reload decimator
      // TODO
      return true;
    }

  return false;
}

/**
 * @brief Prepend new data to filter state
 * @important Use @func filter_shift_right followed by
 * prepending the data
 * @param filt: filter structure
 * @param uint16_t: most current data
 */
__STATIC_FORCEINLINE void
filter_push (filter_t *filt, uint16_t data)
{
  // TODO
}

/**
 * @brief Calculate filter output and switch buffers if
 * active buffer is full
 * @important filt->output.fillcnt tracks the fill counter
 * for active buffer
 * @important Convert and cast the filter output to the
 * correct fixed-point representation before appending the
 * result to the active buffer
 * @param filt: filter structure
 * @return true: active buffer switched
 * @return false: active buffer not switched
 */
__STATIC_FORCEINLINE bool
filter_calc (filter_t *filt)
{
  int32_t res = 0;
  // Calculate filter output

  // TODO

  // Convert (or bitshift) and cast filter output to fit
  // active buffer, append the result to the active buffer,
  // increment fill counter
  // TODO

  // Check if active buffer is full
  // if true reset counter, switch buffer, set transfer
  // flag (return true)
  // else return false
  if (/* TODO */)
    {
      // TODO
      return true;
    }
  return false;
}

#endif
