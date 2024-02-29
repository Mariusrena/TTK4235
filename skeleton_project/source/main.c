#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "driver/control.h"



int main(){
    elevio_init();
    //int btnPressed ,btnPressed_up,btnPressed_down;

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
    int queue[4][4];
    int heis_reting=0;
    for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
        queue[i][j] = 0;
        }
    }
    int floor = 0;
    int old_floor;


    oppstart();
    
    floor = elevio_floorSensor();
    
    /*
    while (0)//knap test
    {
        
    
            btnPressed_up = elevio_callButton(1, 0);
            elevio_buttonLamp(1, 0, btnPressed);
            if (btnPressed_up)
            {
                elevio_motorDirection(DIRN_UP);
            }
            btnPressed_down = elevio_callButton(1, 1);
            elevio_buttonLamp(1, 1, btnPressed);
            if (btnPressed_down)
            {
                elevio_motorDirection(DIRN_DOWN);
            }
            
            if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        }
        
    }
    */
    while(1){
        
        etasje_lys(&floor,&old_floor);
                    



        queue_make(&queue);
        lys_control(queue);
        etasje_stop(queue,&heis_reting,floor);
        start_ved_bestiling(queue, floor,&heis_reting);
        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }
        
        if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        }
        
      
        
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

/*printer queue
    for (int i = 0 ; i<4;i++){
        for (int j = 0 ; j<4;j++){
        printf("%d\n",queue[i][j]);
        }
    }*/
    return 0;
}
