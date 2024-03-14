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
    int heis_reting=DIRN_DOWN;
    for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
        queue[i][j] = 0;
        }
    }

    int floor = 0;
    int old_floor, kjør_i_retning;
    int stop_pressed = 0;
    
    time_t start = time(NULL);
    time_t stopp;

    oppstart();
    
    floor = elevio_floorSensor();
    
    while(1){
        etasje_lys(&floor,&old_floor);
        
        queue_make(&queue);
        
        lys_control(queue);
        
        etasje_stop(queue,&heis_reting,floor,&start);

        if(((time(NULL)-start)) >= 3){
            kjør_i_retning = start_ved_bestilling(queue,floor,&heis_reting);
            
            elevio_motorDirection(kjør_i_retning);
            while(elevio_floorSensor()==-1){
                elevio_motorDirection(kjør_i_retning);
                if (elevio_stopButton()){
                    break;
                }
                
            }
            
            elevio_doorOpenLamp(0);
        }else{ 
            elevio_doorOpenLamp(1);
        }
        

        if(elevio_obstruction()){
            elevio_stopLamp(1);
        }else{
            elevio_stopLamp(0);
        }
        
        if(elevio_stopButton()){
            time_t stopp = time(NULL);
            while(elevio_stopButton() || (time(NULL)-stopp)<3){
                stop_pressed = 1;
                elevio_motorDirection(DIRN_STOP);
                elevio_stopLamp(1);
                if (floor!=-1){
                    elevio_stopLamp(1);
                    elevio_doorOpenLamp(1);
                }
            }
        }
        if(stop_pressed){
            stop_pressed=0;
            etter_stopp(heis_reting);
        }
        //nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    /* for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
            printf("Floor:%d Button:%d -- %d\n",i, j, queue[i][j]);}} */
    return 0;
}
