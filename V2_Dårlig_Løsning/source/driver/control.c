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
#include "queue.h" 



//Oppstart av heis. Finner først ut hvilken retning heisen skal kjøre 
// og kjører deretter til en definert tilstand (etasje)
// Allokerer og returnerer tilstanden til heisen ved oppstart.
state_t *initialize(){
    int floor = elevio_floorSensor();
    if (floor > 0){
      elevio_motorDirection(DIRN_DOWN); //ned dersom over etasje 0
    }else{
      elevio_motorDirection(DIRN_UP);} //opp dersom under etasje 0

    while (floor == -1){ //kjører helt til en etasje er nådd
        floor = elevio_floorSensor();
    }
    elevio_motorDirection(DIRN_STOP); //stopper når etasjen er nådd

    state_t *current_state = malloc(sizeof(state_t)); //allokerer en state, med etasje og retning
    current_state->direction = DIRN_STOP;
    current_state->floor = floor;
    return current_state;
}



//Oppdaterer tilstanden til heisen. 
void update_state(state_t *current_state, int direction){
  current_state->floor = elevio_floorSensor();
  current_state->direction = direction;
}

//Ved bestilling utenfor heis, med en gang heisen når den etasjen som bestillingen er gjort ifra
//og åpner dørene, er denne bestillingen gjennomført...

//Itererer gjennom alle knapper og lager en ny ordre hvis en knapp er trykket
void check_for_new_order(queue_t *queue, state_t *state){
  for(int floor = 0; floor < N_FLOORS; floor++){ //for alle etasjer
    for(int button = 0; button < N_BUTTONS; button++){ //for alle knapper
      int btnPressed = elevio_callButton(floor, button); //sjekker om gitt knapp er trykket
      elevio_buttonLamp(floor, button, btnPressed); //lyser dersom trykket

      if(btnPressed == 1){ //dersom knappen er trykket
        printf("floor: %d, button: %d\n", floor, button);
        order_t *order = new_order(floor, button, state); //lager en ny ordre
        if (queue_contains(queue, order)){ //sjekker om ordren finnes i køen
          printf("order already in queue\n");
          destroy_order(order); //fjerner ordren hvis den finnes i køen
        }else{
          printf("order not in queue\n");
          queue_add(queue, order); //legger ordren til dersom den ikke finnes i køen
        }
      }           
    }
  }
}




void perform_order(order_t *order_to_perform, state_t *current_state){
  elevio_motorDirection(order_to_perform->direction);
  //if(order_to_perform->floor == current_state->floor){
  //  order_to_perform->completed = 1;
  //}
}

/* void delete_order_on_floor(state_t *current_state){
  order_t *order_up0 = new_order(current_state->floor, 0, DIRN_UP);
  order_t *order_up1 = new_order(current_state->floor, 0, DIRN_UP);
  order_t *order_up2 = new_order(current_state->floor, 0, DIRN_UP);
  order_t *order_down0 = new_order(current_state->floor, 0, DIRN_DOWN);
  order_t *order_down1 = new_order(current_state->floor, 0, DIRN_DOWN);
  order_t *order_down2 = new_order(current_state->floor, 0, DIRN_DOWN); 
  if(queue_contains()){
                
  }
} */

void order_completed(queue_iter_t *queue_iter, order_t *order){
  order_t *tmp_prev = order->prev;
  order_t *tmp_next = order->next;
  
  if(tmp_prev && tmp_next){
    tmp_prev->next = tmp_next;
    tmp_next->prev = tmp_prev;
  }else if(tmp_prev){
    queue_iter->queue->tail = tmp_prev;
    tmp_prev->next = NULL;
  }else if(tmp_next){
    queue_iter->queue->head = tmp_next;
    tmp_next->prev = NULL;
  }

  destroy_order(order);
}


void end_session(queue_t *queue, queue_iter_t *queue_iter, state_t *state){
  queue_destroy(queue);
  queue_destroyiter(queue_iter);
  free(state);
}