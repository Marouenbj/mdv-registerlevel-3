#include "../include/mdv.h"

extern GPIO_t *GPIOA;
extern TIMgp_t *TIM2;
extern NVIC_t *NVICC;
extern DMA_t *DMA1;
extern USART_t *USART2;
extern ADC_t *ADC1;
extern STK_t *STK;

extern fsm_t *fsm;

void
TIM2_IRQHandler (void)
{
  if (TIM2->SR.UIF)
    {
      TIM2->SR.UIF = 0;
    }
}

void
ADC1_2_IRQHandler (void)
{
  if (ADC1->ISR.EOC)
    {
      fsm->data.adc_k = __atomic_load_n (
          &ADC1->DR.RDATA, memory_order_seq_cst);
      fsm->state.sampling_instant = true;
    }
}

void
USART2_IRQHandler (void)
{
  if (USART2->ISR.RTOF)
    {
      USART2->ICR.RTOCF = 1;
      if (DMA1->CNDTR6.NDT != 100)
        {
          fsm->state.rto_detected = true;
          DMA1->CCR6.EN = 0;
          DMA1->CNDTR6.NDT = 100;
          DMA1->CCR6.EN = 1;
        }
    }
}

void
SysTick_Handler (void)
{
  if (STK->CTRL.COUNTFLAG)
    {
      LED_TGL_GREEN;
      STK->CTRL.ENABLE = 1;
    }
}

void
DMA1_Channel6_IRQHandler (void)
{
  if (DMA1->ISR.TCIF6)
    {
      DMA1->IFCR.CTCIF6 = 1;
    }
  if (DMA1->ISR.TEIF6)
    {
      DMA1->IFCR.CTEIF6 = 1;
    }
}

void
DMA1_Channel7_IRQHandler (void)
{
  if (DMA1->ISR.TCIF7)
    {
      LED_TGL_YELLOW;
      fsm->state.dmat_finished = true;
      DMA1->IFCR.CTCIF7 = 1;
      DMA1->CCR7.EN = 0;
    }
  if (DMA1->ISR.TEIF7)
    {
      DMA1->IFCR.CTEIF7 = 1;
    }
}
