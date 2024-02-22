#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>

#include "elevio.h"
#include "con_load.h"
#include "control.h"

void oppstart(){
    int floor = elevio_floorSensor();
    elevio_motorDirection(DIRN_DOWN);
    while (floor == -1) // setter heis til definert tilstand ved oppstart 
    {
        floor = elevio_floorSensor();
    }
    elevio_motorDirection(DIRN_STOP);
}

void queue_make(int *queue){

    int btnPressed ;
    for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                btnPressed = elevio_callButton(f, b);
                elevio_buttonLamp(f, b, btnPressed);
                if (btnPressed & b == 2 )
                {
                    queue[f]=skal_heis;
                }else if(btnPressed & queue[f]!=skal_heis){
                    if (b==1)
                    {
                        queue[f]=skal_ned;
                    } else if(b==0){
                        queue[f]=skal_opp;
                    }
                     
                }
                
                
            }
        }
}

void lys_control(int *queue){
    for(int f = 0;f<N_FLOORS;f++){
        if(queue[f] != skal_ikke){
    elevio_buttonLamp(f, queue[f]-1, 1);
    }}//funker ikke hvis du tryker på opp/ned i en etasje du alerede skal til  
}