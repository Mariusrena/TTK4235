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
    int heis_reting=0;
    for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
        queue[i][j] = 0;
        }
    }
    int floor = 0;
    int old_floor;
    
    time_t start = time(NULL);
    
    oppstart();
    
    floor = elevio_floorSensor();
    
    
    
    while(1){
        etasje_lys(&floor,&old_floor);
        
        queue_make(&queue);
        
        lys_control(queue);
        
        printf("Floor: %d\n",floor);
        etasje_stop(queue,&heis_reting,floor,&start);

        if(((time(NULL)-start)) >= 3){
            start_ved_bestiling(queue,floor,&heis_reting);
            elevio_doorOpenLamp(0);
        }else{
            elevio_doorOpenLamp(1);
        }
        

        if(elevio_obstruction()){
            elevio_stopLamp(1);
        }else{
            elevio_stopLamp(0);
        }
        
        while(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            elevio_stopLamp(1);
            if (floor!=-1){
                elevio_stopLamp(1);
                elevio_doorOpenLamp(1);
            }
            if(!elevio_stopButton()){
                elevio_stopLamp(0);
                sleep(3);
            }
        }
        
        
        //nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    /* for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
            printf("Floor:%d Button:%d -- %d\n",i, j, queue[i][j]);}} */
    return 0;
}
