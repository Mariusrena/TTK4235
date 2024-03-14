#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "elevio.h"
#include "con_load.h"
#include "control.h"

void oppstart(){
    int floor = elevio_floorSensor();
    elevio_motorDirection(DIRN_DOWN);
   
    while (floor == -1){ // setter heis til definert tilstand ved oppstart 
        floor = elevio_floorSensor();
    }

    elevio_motorDirection(DIRN_STOP);
}

void queue_make(int queue[4][4]){
    int btnPressed;
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            btnPressed = elevio_callButton(f, b);
            elevio_buttonLamp(f, b, btnPressed);
            if(btnPressed && b==2){
                queue[f][skal_heis]=1;
                queue[f][skal]=1;
            }else if(btnPressed && b==1){
                queue[f][skal_ned]=1;
                queue[f][skal]=1;
            }else if(btnPressed && b==0){
                queue[f][skal_opp]=1;
                queue[f][skal]=1;
            }  
        }
    }
}

void lys_control(int queue[4][4]){
    for(int f = 0;f<N_FLOORS;f++){
        if(queue[f][skal] == 1){
            if(queue[f][skal_ned]==1){
                elevio_buttonLamp(f, skal_ned-1, 1);
            }
            if(queue[f][skal_opp]==1){
                elevio_buttonLamp(f, skal_opp-1, 1);
            }
            if(queue[f][skal_heis]==1){
                elevio_buttonLamp(f, skal_heis-1, 1);
            }  
        }
    }
}

void lys_av(int queue[4][4]){
    for(int f = 0;f<N_FLOORS;f++){
        elevio_buttonLamp(f, skal_ned-1, 0);
        elevio_buttonLamp(f, skal_opp-1, 0);
        elevio_buttonLamp(f, skal_heis-1, 0);
    }
}

void etasje_lys(int *floor,int *old_floor){
    
    *floor = elevio_floorSensor();

    if((*old_floor != *floor) && (*floor != -1)){//printer etasje
            //printf("%d\n",(*floor+1));
            elevio_floorIndicator(*floor);
            *old_floor=*floor;
    }
}


void etasje_stop(int queue[4][4],int *heis_reting,int floor,time_t *start){ //Sjekker om heisen er ankommet en etasje med markert stop
    int skal_stoppe = 0; 
    int under, over;
   
    for(int f = 0;f<N_FLOORS;f++){ //Sjekker alle etasjene
        under = 0;
        over = 0;
        for(int f2 = f+1;f2<N_FLOORS;f2++){ //Sjekker om heisen skal stoppe i en etasje over foreløpig etasjer som blir sjekket
            if(queue[f2][skal] == 1){
                over = 1;
                break;
            }
        }
        for(int f2 = f-1;f2>=0;f2--){ //Sjekker om heisen skal stoppe i en etasje under foreløpig etasjer som blir sjekket
            if(queue[f2][skal] == 1){
                under = 1;
                break; 
            }
        }
        if(queue[f][skal] == 1 && f==floor){ //Dersom heisen skal stoppe i nåværende etasje
            if(queue[f][skal_heis]==1 || //Dersom heisen skal stoppe i etasjen 
              ((queue[f][skal_opp]==1 && (*heis_reting==DIRN_UP || (!under || floor == 0)))) || //eller heisen er på vei opp og --> retningen er opp eller det ikke er noen ordre under
              ((queue[f][skal_ned]==1 && (*heis_reting==DIRN_DOWN || (!over || floor == 3))))){ //eller heisen er på vei ned og --> retningen er ned eller det ikke er noen ordre over
                elevio_motorDirection(DIRN_STOP);
                for (int j = 0 ; j<4;j++){ //Sletter alle ordre i etasjen den har stoppet i
                    queue[f][j] = 0;
                }
                skal_stoppe = 1;
            }
        }
        if(skal_stoppe){*start=time(NULL);} //Starter en timer, slik at heisdøren står åpen 3 sekunder etter at heisen har stoppet
    }
}

void slett_bestillinger(int queue[4][4]){
    for(int f = 0;f<N_FLOORS;f++){    
        for (int j = 0 ; j<4;j++){ //Sletter alle ordre i etasjen den har stoppet i
            queue[f][j] = 0;
        }
    }
}

void start_ved_bestilling(int queue[4][4], double floor,int *heis_reting){
    if(*heis_reting == DIRN_DOWN){
        for(int f = 0;f<N_FLOORS;f++){
            if(queue[f][skal]==1 && floor>=0){
                if(f<floor){
                    *heis_reting = DIRN_DOWN;
                    elevio_motorDirection(DIRN_DOWN);
                    break;
                }else if(f>floor){
                    *heis_reting = DIRN_UP;
                    elevio_motorDirection(DIRN_UP);
                    break;
                }else{
                    elevio_motorDirection(DIRN_STOP);
                    break;
                }
            }
        }
    }
    
    else if (*heis_reting == DIRN_UP){
        for(int f = N_FLOORS-1;f>=0;f--){
            if(queue[f][skal]==1 && floor>=0){
                if(f>floor){
                    *heis_reting = DIRN_UP;
                    elevio_motorDirection(DIRN_UP);
                    break;
                }else if(f<floor){
                    *heis_reting = DIRN_DOWN;
                    elevio_motorDirection(DIRN_DOWN);
                    break;
                }else{
                    elevio_motorDirection(DIRN_STOP);
                    break;
                }
            }
        }
    }else{elevio_motorDirection(DIRN_STOP);}
}

void door_open(int open){
    int was_obstructed = 0;
    elevio_doorOpenLamp(open);
    int floor = elevio_floorSensor();
    while(elevio_obstruction() && open){
        elevio_buttonLamp(floor, skal_ned-1, 0);
        elevio_buttonLamp(floor, skal_opp-1, 0);
        elevio_buttonLamp(floor, skal_heis-1, 0);
        was_obstructed = 1;    
    }
    if(was_obstructed){
        time_t door_time = time(NULL);
        elevio_doorOpenLamp(open);
        while(time(NULL)-door_time <= 3){
            ;
        }
    }
    elevio_doorOpenLamp(0);    
}

void stop_button_pressed(int queue[4][4], int old_floor, int *heis_retning){
    time_t stopp;
    int stop_pressed = 0;
    if(elevio_stopButton()){
        stopp = time(NULL);
        slett_bestillinger(queue);
        lys_av(queue);
        while(elevio_stopButton() || (time(NULL)-stopp) >= 3){
            stop_pressed = 1;
            elevio_motorDirection(DIRN_STOP);
            elevio_stopLamp(1);
            if (elevio_floorSensor()!=-1){
                elevio_stopLamp(1);
               door_open(1);
            }else{
                door_open(0);
            }
            if (!elevio_stopButton()){
                stopp = time(NULL);
                elevio_stopLamp(0);
            }
        }
        door_open(0);
    }
    /* if(stop_pressed){
        stop_pressed=0;
        while (elevio_floorSensor()==-1){
            double between_floors = (double)old_floor;
            heis_retning == DIRN_UP ? between_floors+0.5 : between_floors-0.5;
            start_ved_bestilling(queue,between_floors,heis_retning);
        }
        
    } */
}