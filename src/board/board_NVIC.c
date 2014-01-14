

#include "board_NVIC.h"


void NVIC_init(void)
{
    /* NVIC_PriorityGroup,: specifies the priority grouping bits length. */
    NVIC_PriorityGroupConfig( NVIC_PRIORITY_GPOUP );
}