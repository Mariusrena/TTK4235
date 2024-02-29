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
void queue_make(int queue[4][4]){

    int btnPressed, btnPressed_up,btnPressed_down  ;
    for(int f = 0; f < N_FLOORS; f++){
    
            for(int b = 0; b < N_BUTTONS; b++){
                btnPressed = elevio_callButton(f, b);
                elevio_buttonLamp(f, b, btnPressed);
                if (btnPressed && b == 2 )
                {
                    queue[f][skal_heis]=1;
                    queue[f][skal]=1;
                }else if(btnPressed){
                    if (b==1)
                    {
                        queue[f][skal_ned]=1;
                        queue[f][skal]=1;

                        btnPressed_down=1;
                        btnPressed_up=0;  
                    } else if(b==0){
                        queue[f][skal_opp]=1;
                        queue[f][skal]=1;

                        btnPressed_down=0;
                        btnPressed_up=1;  
                    }
                     
                }
                
                
            }
        }
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
}

void lys_control(int queue[4][4]){
    for(int f = 0;f<N_FLOORS;f++){
        if(queue[f][skal] == 1){
            if(queue[f][skal_ned]==1){
            
            elevio_buttonLamp(f, skal_ned-1, 1);}
            if(queue[f][skal_opp]==1){
            elevio_buttonLamp(f, skal_opp-1, 1);}
            if(queue[f][skal_heis]==1){
            elevio_buttonLamp(f, skal_heis-1, 1);}
            
    }}
}
void etasje_lys(int *floor,int *old_floor){
    
    *floor = elevio_floorSensor();
    
     if(*old_floor !=*floor & *floor != -1){//printer etasje
            //printf("%d\n",(*floor+1));
            elevio_floorIndicator(*floor);
            *old_floor=*floor;
        }
    
}

void etasje_stop(int queue[4][4],int *heis_reting,int floor){
int skal_stoppe=0;
for(int f = 0;f<N_FLOORS;f++){

if(queue[f][skal] == 1 && f==floor){
    if(queue[f][skal_heis]==1){
        *heis_reting=DIRN_STOP;
        elevio_motorDirection(DIRN_STOP);
        for (int j = 0 ; j<4;j++){
        queue[f][j] = 0;
        }
        skal_stoppe=1;
    }else if((queue[f][skal_opp]==1 && *heis_reting==DIRN_UP) || (queue[f][skal_ned]==1 && (*heis_reting==DIRN_DOWN)) ){
        elevio_motorDirection(DIRN_STOP);
        *heis_reting=DIRN_STOP;
        for (int j = 0 ; j<4;j++){
        queue[f][j] = 0;
        skal_stoppe=1;
        
        }
    }
}
if(skal_stoppe){sleep(3);}
}}

void start_ved_bestiling(int queue[4][4], int floor,int *heis_reting){
for(int f = 0;f<N_FLOORS;f++){
if(queue[f][0]==1 && floor>=0){
    if(f<floor){
        elevio_motorDirection(DIRN_DOWN);
        *heis_reting =DIRN_DOWN;
    }else if(f>floor){
        elevio_motorDirection(DIRN_UP);
        *heis_reting =DIRN_UP;

    }
}
}
}