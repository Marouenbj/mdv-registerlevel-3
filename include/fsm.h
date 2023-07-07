#pragma once
#ifndef FSM_H
#define FSM_H

#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

#include "stdatomic.h"

#include "filter.h"

typedef union adc_data
{
  volatile uint16_t hw;
  volatile uint8_t b[sizeof (uint16_t)];
} adc_data;

typedef struct fsm_t
{
  struct
  {
    volatile uint16_t psc;
    volatile uint32_t arr;
    volatile uint32_t count;
    volatile int16_t filter;
    volatile int16_t decim;
  } cmd;
  struct
  {
    volatile atomic_bool rto_detected;
    volatile atomic_bool coeff_acquired;
    volatile atomic_bool cmd_valid;
    volatile atomic_bool sampling_instant;
    volatile atomic_bool processing;
    volatile atomic_bool buffer_switch;
    volatile atomic_bool dmat_finished;
    volatile atomic_bool filter_active;
    volatile atomic_bool reset;
    volatile size_t counter;
  } state;
  struct
  {
    uint8_t *const dmabuf_ptr;
    uint8_t *const mainbuf_ptr;
  } buf;
  struct
  {
    volatile adc_data fir_k;
    volatile uint16_t adc_k;
  } data;
  filter_t *const filter;
} fsm_t;

#endif
