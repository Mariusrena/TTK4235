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


/* Initializes the elevator, driving it to a defined stated.
  Does so by driving downwards until it finds itself on any legal floor. */
void ElevatorInit () {
  elevio_motorDirection(DIRN_DOWN);
  
  int floor = elevio_floorSensor();
  while (floor == -1) { 
    floor = elevio_floorSensor();
  }

  elevio_motorDirection(DIRN_STOP);
}

/* Updates the state of orders in the queue (an array)
  Iterates through all possible floor/button combinations,
  if a given button is pressed the state for the floor/button
  combination is set to TRUE in the array, 
  representing an order added to the queue */
void QueueUpdate (int (*queue)[4][4]) {
  int btnPressed;
  for (int floor_number = 0; floor_number < N_FLOORS; floor_number++) {
    for (int button_number = 0; button_number < N_BUTTONS; button_number++) {
      
      btnPressed = elevio_callButton(floor_number, button_number);
      elevio_buttonLamp(floor_number, button_number, btnPressed);

      if (btnPressed && (button_number == 2) ) {
        (*queue)[floor_number][kInsideElevator] = TRUE;
        (*queue)[floor_number][kGoingHere] = TRUE;
      } else if (btnPressed && (button_number == 1) ) {
        (*queue)[floor_number][kGoingDown] = TRUE;
        (*queue)[floor_number][kGoingHere] = TRUE;
      } else if (btnPressed && (button_number == 0) ) {
        (*queue)[floor_number][kGoingUp] = TRUE;
        (*queue)[floor_number][kGoingHere] = TRUE;
      }  
    }
  }
}

/* Drives the elevator in the prioritized direction. 
  If the elevator is going down, orders going down are prioritized,
  and vice versa going upwards. Has a safety check if the elevator direction
  is set to stop, which in theory should never happen */
void StartOnOrder (int (*queue)[4][4], double current_floor, int *elevator_direction) {
  if (*elevator_direction == DIRN_DOWN) {
    for (int floor_number = 0; floor_number < N_FLOORS; floor_number++) {
      if ((*queue)[floor_number][kGoingHere] == TRUE && current_floor >= 0) {
        if (floor_number < current_floor) {
          *elevator_direction = DIRN_DOWN;
          elevio_motorDirection(DIRN_DOWN);
          break;
        } else if (floor_number > current_floor) {
          *elevator_direction = DIRN_UP;
          elevio_motorDirection(DIRN_UP);
          break;
        } else {
          elevio_motorDirection(DIRN_STOP);
          break;
        }
      }
    }
  }
  
  else if (*elevator_direction == DIRN_UP) {
    for (int floor_number = N_FLOORS-1; floor_number >= 0; floor_number--) {
      if ((*queue)[floor_number][kGoingHere] == TRUE && current_floor >= 0) {
        if (floor_number > current_floor) {
          *elevator_direction = DIRN_UP;
          elevio_motorDirection(DIRN_UP);
          break;
        } else if (floor_number < current_floor) {
          *elevator_direction = DIRN_DOWN;
          elevio_motorDirection(DIRN_DOWN);
          break;
        } else {
          elevio_motorDirection(DIRN_STOP);
          break;
        }
      }
    }
  } else {elevio_motorDirection(DIRN_STOP);}
}

/* If the elevator reaches a floor with an order, and certain condidtions are met, the elevator stops */
void StopOnFloor (int (*queue)[4][4], int *elevator_direction, time_t *start_timer) {
  if (elevio_floorSensor() != -1) {
    int stopping = FALSE; 
    int under, over;
    
    for (int floor_number = 0; floor_number < N_FLOORS; floor_number++) {
      under = FALSE;
      over = FALSE;
      
      /* Checks if there are orders above the current floor */
      for (int floor_above = floor_number+1; floor_above < N_FLOORS; floor_above++) {
        if ((*queue)[floor_above][kGoingHere] == TRUE) {
          over = 1;
          break;
        }
      }

      /* Checks if there are orders under the current floor */
      for (int floor_below = floor_number-1; floor_below >= 0; floor_below--) {
        if ((*queue)[floor_below][kGoingHere] == TRUE) {
          under = TRUE;
          break; 
        }
      }

      /* The elevator stops if there is an order on the floor the elevator currently resides on AND:
        1. An order is placed from the inside of the elevator
        2. An order is placed for going up AND (the direction of the elevator is upwards OR (there are no orders below the elevator OR is resides at the lowest floor))
        3. An order is placed for going down AND (the direction of the elevator is downwards OR (there are no orders above the elevator OR is resides at the highest floor))
        
        If the elevator stops, it considers all orders on that floor completed and sets all floor/button pairs on that floor to FALSE
        it also start a timer, which is used to ensure the door stays open for 3 seconds after a stop */
      int current_floor = elevio_floorSensor();
      if ((*queue)[floor_number][kGoingHere] == TRUE && floor_number == current_floor) {
        if ( ((*queue)[floor_number][kInsideElevator] == TRUE) ||  
            ((*queue)[floor_number][kGoingUp] == TRUE && (*elevator_direction == DIRN_UP || (!under || current_floor == 0) ) ) ||
            ((*queue)[floor_number][kGoingDown] == TRUE && (*elevator_direction == DIRN_DOWN || (!over || current_floor == 3) ) ) ) { 
          elevio_motorDirection(DIRN_STOP);
          for (int floor_state = 0 ; floor_state < 4; floor_state++) { 
            (*queue)[floor_number][floor_state] = FALSE;
          }
          stopping = TRUE;
        }
      } if (stopping) {*start_timer=time(NULL);}
    }
  }
}

/* Deletes all orders in the queue by setting all floor/button pairs to FALSE */
void CancelAllOrders (int (*queue)[4][4]) {
  for(int floor_number = 0; floor_number < N_FLOORS; floor_number++){    
    for (int floor_state = 0; floor_state < 4; floor_state++){
      ((*queue))[floor_number][floor_state] = FALSE;
    }
  }
}

/* Sets the floor light of the elevator by keeping track of the last visited floor
  When entering a new floor, the old floor variable is set equal to the new floor
  and the lighting is switched from old to new */
void FloorLight (int *old_floor) {
  int current_floor = elevio_floorSensor();
  if ( (*old_floor != current_floor) && (current_floor != -1) ) {
    elevio_floorIndicator(current_floor);
    *old_floor = current_floor;
  }
}

/* Sets a light to ON when a button is pressed
  Iterates through the order array and checks whether the button is pressed */
void LightControl (int (*queue)[4][4]) {
  for (int floor_number = 0; floor_number < N_FLOORS; floor_number++) {
    if ((*queue)[floor_number][kGoingHere] == TRUE) {
      if ((*queue)[floor_number][kGoingDown] == TRUE) {
        elevio_buttonLamp(floor_number, kGoingDown-1, ON);
      }
      if ((*queue)[floor_number][kGoingUp] == TRUE) {
        elevio_buttonLamp(floor_number, kGoingUp-1, ON);
      }
      if ((*queue)[floor_number][kInsideElevator] == TRUE) {
        elevio_buttonLamp(floor_number, kInsideElevator-1, ON);
      }  
    }
  }
}

/* Turns OFF all lights on all buttons */
void LightsOff (int (*queue)[4][4]) {
  for (int floor_number = 0; floor_number < N_FLOORS; floor_number++) {
    elevio_buttonLamp(floor_number, kGoingDown-1, OFF);
    elevio_buttonLamp(floor_number, kGoingUp-1, OFF);
    elevio_buttonLamp(floor_number, kInsideElevator-1, OFF);
  }
}

/* If the door of the elevtor is open, set the door lamp to ON 
  If the door is obstructed, keep it open until not obstructed then wait 3 seconds before closing */
void DoorOpen (int open) {
  int was_obstructed = FALSE;
  elevio_doorOpenLamp(open);
  int floor = elevio_floorSensor();
  while (elevio_obstruction() && open) {
    elevio_buttonLamp(floor, kGoingDown-1, OFF);
    elevio_buttonLamp(floor, kGoingUp-1, OFF);
    elevio_buttonLamp(floor, kInsideElevator-1, OFF);
    was_obstructed = TRUE;    
  } if (was_obstructed) {
    time_t time_door_opened = time(NULL);
    elevio_doorOpenLamp(open);
    while ((time(NULL)-time_door_opened) <= 3) {
      ;
    }
  }
  elevio_doorOpenLamp(FALSE);    
}

/* If the stop button is pressed -> for as long as it is pressed:
  1. Cancel all orders
  2. Turn of all button lights
  3. Stop the elevator
  4. Turn of stop lamp
  
  If the elevator is at a floor -> Open the door
  If the stop button is unpressed, wait 3 seconds before moving on if the door is open
  If pressed again during door wait, go back to previous loop */
void StopButtonPressed (int (*queue)[4][4], int old_floor) {
  if (elevio_stopButton()) {
    
    time_t stop_pressed_time;
    CancelAllOrders(queue);
    LightsOff(queue);
  
    StopPressed:
    while (elevio_stopButton()) {
      stop_pressed_time = time(NULL);
      elevio_motorDirection(DIRN_STOP);
      elevio_stopLamp(ON);
  
      if (elevio_floorSensor() != -1) {
        DoorOpen(TRUE);
      } else {
        DoorOpen(FALSE);
      } 
    }

    while ( ( (time(NULL)-stop_pressed_time) >= 3) && (elevio_floorSensor() != -1) ) {
      elevio_stopLamp(ON);
      
      if (elevio_floorSensor() != -1) {
        DoorOpen(TRUE);
      } else {
        DoorOpen(FALSE);
      }

      if (elevio_stopButton()) {
        goto StopPressed;
      }
    }
    elevio_stopLamp(OFF);
    DoorOpen(FALSE);
  }
}

/* Handles the movement of the elevator on orders.
  As long as the door has been open for 3 seconds after the previous order, start a new order
  Keeps track of half floors incase the stop button is pressed between floors. */
void DriveHandler (int (*queue)[4][4], int old_floor, time_t *start, int *elevator_direction) {
  double between_floors = (double) old_floor;
  if ( (time(NULL)-*start) >= 3) {
    if (elevio_floorSensor() == -1) {
      *elevator_direction == DIRN_UP ? between_floors+0.5 : between_floors-0.5;
    }
    StartOnOrder(queue, between_floors, elevator_direction);
    LightControl(queue);
    DoorOpen(FALSE);
  } else if (elevio_floorSensor() != -1) { 
    LightControl(queue);
    DoorOpen(TRUE);
  }
}