#pragma once

void oppstart();

typedef enum { // hvem som vil at den skal til etajen/hva de vil  
    skal_ikke   = 0,
    skal_opp      = 1,
    skal_ned    = 2,
    skal_heis          = 3
} stop_type;

void queue_make(int *queue);
void lys_control(int *queue);