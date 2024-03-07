#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"
#include "driver/control.h"
#include "driver/queue.h"


int main(){
    elevio_init();
    
    printf("=== Intializing Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
    
    state_t *current_state = initialize();
    queue_t *queue = queue_create();
    queue_iter_t *queue_iter = queue_createiter(queue);

    while(1){            
        check_for_new_order(queue, current_state);
        update_queue_iter(queue_iter);
                
        if(queue_hasnext(queue_iter)){
            order_t *next_order = queue_next(queue_iter, current_state);
            printf("next order: %d, %d, %d, %d\n", next_order->floor, &next_order->direction, next_order->next, next_order->prev);
            elevio_motorDirection(next_order->direction);
            
        }
        
        
        /* 
        //perform_order(next_order, current_state);
        if (next_order){
            printf("next order: %d, %d\n", next_order->floor, next_order->direction);
            elevio_motorDirection(next_order->direction);
            
            update_state(current_state, next_order->direction);

            if (next_order->floor==current_state->floor){
                order_completed(queue_iter, next_order);
            }
        
            if (current_state->floor==(0||1||2||3)){
                
            }
        }
        
        */

        
        


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

    end_session(queue, queue_iter, current_state);
    return 0;
}
