#include "queue.h"
#include "control.h"
#include "elevio.h"
#include <stdlib.h>
#include <stdio.h>


//Comparison function which is needed in the queue in order to select the next order.
int comparison(const order_t *a, const order_t *b, const state_t *current_state){
  if (current_state->direction == DIRN_UP){
    if(a->floor > current_state->floor && a->floor < b->floor){
      return 1;
    }else if(a->floor > current_state->floor && a->floor > b->floor){
      return 0;
    }else if(a->floor < current_state->floor && a->floor > b->floor){
      return 1;
    }else if(a->floor < current_state->floor && a->floor < b->floor){
      return 0;
    }  
  }
  else if(current_state->direction == DIRN_DOWN){
    if(a->floor < current_state->floor && a->floor > b->floor){
      return 1;
    }else if(a->floor < current_state->floor && a->floor < b->floor){
      return 0;
    }else if(a->floor > current_state->floor && a->floor < b->floor){
      return 1;
    }else if(a->floor > current_state->floor && a->floor > b->floor){
      return 0;
    }  
  }return 1;
  
}

//Sjekker hvilken retning en ny bestilling har.
int order_direction(int floor, int button, state_t *current_state){
  if(button == 0){ //Opp er trykt på utsiden
    return DIRN_UP;
  }else if(button == 1){ //Ned er trykt på utsiden
    return DIRN_DOWN;
  }else if(button == 2){ //Knapp er trykt fra innsiden og anhenger av heisens posisjon
    if (floor > current_state->floor){
      return DIRN_UP;
    }else if (floor < current_state->floor){
      return DIRN_DOWN;
    }
  }else{
    return DIRN_STOP; //Nås i utgangspunktet kun ved feil eller ved bestilling av nåværende etasje
  }
}

//Sier om knappen ble trykt på innsiden av heisen
int order_internal(int button){
    if (button==2){
      return 1;
    }else{
      return 0;
    }
}

//Creates a new order and allocates memory
order_t *new_order(int floor, int button, state_t *state){
  order_t *order = malloc(sizeof(order_t));
  order->floor = floor; //Tildeler etasje
  order->direction = order_direction(floor, button, state); //Tildeler retning
  //order->internal = order_internal(button); //Sier om knappen ble trykt innvendig
  //order->entered = order_internal(button); //Sier om de som trykket knappen har gått inn i heisen
  //order->completed = 0;
  order->next = NULL;
  order->prev = NULL;
  return order;
}

void destroy_order(order_t *order_to_destruction){
  free(order_to_destruction);
}

int equal_order(order_t *a, order_t *b){
  if (a->floor == b->floor
      && a->direction == b->direction
      /*&& a->internal == b->internal
      && a->entered == b->entered*/){
    return 1;
  }
  return 0;
}

//Creates a new queue
queue_t *queue_create() {
  queue_t *queue = malloc(sizeof(queue_t));
  if (queue == NULL){
    return NULL;
  }
  queue->cmpfunc = comparison;
  queue->head = NULL;
  queue->tail = NULL;
  return queue;
}

//deallocates an exisiting queue
void queue_destroy(queue_t *queue){
  order_t *current_order = queue->head;
  while (current_order != NULL){
    order_t *to_destuction = current_order;
    current_order = current_order->next;
    free(to_destuction);
  }
  free(queue);
}


//adds an element to the queue
void queue_add(queue_t *queue, order_t *new_order){
  new_order->next = NULL;
  if (queue->head == NULL){
    queue->head = new_order;
    queue->tail = new_order;
  } else {
    queue->tail->next = new_order;
    new_order->prev = queue->tail;
    queue->tail = new_order;
  }
}


int queue_contains(queue_t *queue, order_t *order_to_check){
  if (queue->head != NULL){
    order_t *current_order = queue->head;
    while(current_order!=NULL){
      if (equal_order(current_order, order_to_check)){
        return 1;
      }
      current_order = current_order->next;
    }
  }
  return 0; 
}

//creates a new queue iterator
queue_iter_t *queue_createiter(queue_t *queue){
  queue_iter_t *queue_iter = malloc(sizeof(queue_iter_t));
  if (queue_iter == NULL){
    return NULL;
  }
  queue_iter->queue = queue;
  queue_iter->next_order = queue->head;
  return queue_iter;
}

//updates the queue iterator to point to queue head
void update_queue_iter(queue_iter_t *queue_iter){
  queue_iter->next_order = queue_iter->queue->head;
}

//destroys an existing queue iterator
void queue_destroyiter(queue_iter_t *queue_iter){
  free(queue_iter);
}

//checks if there is a next element in the queue
int queue_hasnext(queue_iter_t *queue_iter){
  if(queue_iter->next_order != NULL){
    return 1;
  }else{
    return 0;
  }
}

//returns the next element in the queue
order_t *queue_next(queue_iter_t *queue_iter, state_t *current_state){
  if (queue_iter->next_order == NULL){
    return NULL;
  }
  else{
    order_t *next_order = queue_iter->next_order;
    queue_iter->next_order = queue_iter->next_order->next;
    while (queue_hasnext(queue_iter)){
      if (queue_iter->queue->cmpfunc(next_order, queue_iter->next_order, current_state) == 1){
        queue_iter->next_order = queue_iter->next_order->next;
      }else{
        next_order = queue_iter->next_order;
        queue_iter->next_order = queue_iter->next_order->next;
      }  
    }
    
    return next_order;
  }
}

  
