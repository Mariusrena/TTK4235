#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "driver/elevio.h"
#include "driver/control.h"

int main() {

  elevio_init(); 
  
  printf("=============================\n");
  printf("===   Program Starting    ===\n");
  printf("=== Initializing Elevator ===\n");
  printf("=============================\n");
   
  int queue[4][4]; //The floor/button pair array
  int elevator_direction = DIRN_DOWN;
  
  CancelAllOrders(&queue); //Initializes the queue to all zeros
  
  time_t start = time(NULL);
  
  ElevatorInit(); //Drives the elevator to a defined state (Any legal floor)
  
  int old_floor = elevio_floorSensor(); //Sets old floor to the current floor initially

  while(TRUE){
    FloorLight(&old_floor); //Updates light on where the elevator is
       
    QueueUpdate(&queue); //Updates the queue when orders are recieved
      
    LightControl(&queue); //Controlls lights other than floor light
      
    StopOnFloor(&queue, &elevator_direction, &start); //Checks to see if the elevator is to stop when on a floor
      
    DriveHandler(&queue, old_floor, &start, &elevator_direction); //Drives the elevator in the direction of most prioritized order
  
    StopButtonPressed(&queue, old_floor); //Checks id the stop button is pressed
  }
  return 0;
}
