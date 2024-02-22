#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "driver/control.h"



int main(){
    elevio_init();
    int floor = elevio_floorSensor();
    //int btnPressed ,btnPressed_up,btnPressed_down;

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
    int queue[4];
    for (int i = 0 ; i<4;i++){
        queue[i] = 0;
    }
    int old_floor = 0;
    oppstart();
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
    
        floor = elevio_floorSensor();
        if(old_floor != floor & floor != -1){//printer etasje
            printf("%d\n",floor+1);
        }
        old_floor = floor;
        if(floor == 0){
            elevio_motorDirection(DIRN_UP);
            
        }

        if(floor == N_FLOORS-1){
            elevio_motorDirection(DIRN_DOWN);
                  
        }


        queue_make(queue);
        lys_control(queue);
        
        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }
        
        if(elevio_stopButton()){
            elevio_motorDirection(DIRN_STOP);
            break;
        }
        /*
        {//hvis du tryker opp eller ned knapen på etasjepanel vil den gå opp eller ned 
        btnPressed_up = elevio_callButton(1, 0);
        elevio_buttonLamp(1, 0, btnPressed_up);
        if (btnPressed_up)
        {
            elevio_motorDirection(DIRN_UP);
        }
        btnPressed_down = elevio_callButton(1, 1);
        elevio_buttonLamp(1, 1, btnPressed_down);
        if (btnPressed_down)
        {
            elevio_motorDirection(DIRN_DOWN);
        }
        }
        */
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    for (int i = 0 ; i<4;i++){
        printf("%d\n",queue[i]);
    }
    return 0;
}
