#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "driver/control.h"



int main(){
    elevio_init();

    printf("=============================\n");
    printf("===   Program Starting    ===\n");
    printf("=== Initializing Elevator ===\n");
    printf("=============================\n");
    
    int queue[4][4];
    int heis_retning=DIRN_DOWN;
    for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
        queue[i][j] = 0;
        }
    }

    int floor = 0;
    int old_floor, kjor_i_retning;
    
    time_t start = time(NULL);

    oppstart();
    
    floor = elevio_floorSensor();
    
    while(1){
        etasje_lys(&floor,&old_floor);
        
        queue_make(&queue);
        
        lys_control(queue);
        
        etasje_stop(queue,&heis_retning,floor,&start);

        double between_floors = (double)old_floor;
        if(((time(NULL)-start)) >= 3){
            if(elevio_floorSensor()==-1){
                heis_retning == DIRN_UP ? between_floors+0.5 : between_floors-0.5;
            }
            start_ved_bestilling(queue, between_floors,&heis_retning);
            lys_control(queue);
            door_open(0);
        }else if (elevio_floorSensor()!=-1){ 
            lys_control(queue);
            door_open(1);
        }
        
        stop_button_pressed(&queue, old_floor, &heis_retning);
        
    }
    return 0;
}
