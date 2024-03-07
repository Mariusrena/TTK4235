#pragma once

void oppstart();

typedef enum { // hvem som vil at den skal til etajen/hva de vil  
    skal        = 0,
    skal_opp    = 1,
    skal_ned    = 2,
    skal_heis   = 3
} stop_type;

void queue_make(int queue[4][4]);
void lys_control(int queue[4][4]);
void etasje_lys(int *floor,int *old_floor);
void etasje_stop(int queue[4][4],int *heis_reting, int floor, clock_t *start);
void start_ved_bestiling(int queue[4][4], int floor,int *heis_reting);
