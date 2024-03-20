#pragma once

typedef enum { 
  kGoingHere      = 0, //The elevator has recieved an order on this floor
  kGoingUp        = 1, //The elevator has recieved an order for going up
  kGoingDown      = 2, //The elevator has recieved an order for going down
  kInsideElevator  = 3  //The elevator has recieved an order from inside the elevator
} StopType;

typedef enum { 
  TRUE  = 1,
  FALSE = 0,
} Booleans;

typedef enum { 
  ON  = 1,
  OFF = 0,
} StateOf;

void ElevatorInit();

void QueueUpdate(int (*queue)[4][4]);
void StartOnOrder(int (*queue)[4][4], double current_floor, int *elevator_direction);
void StopOnFloor(int (*queue)[4][4], int *elevator_direction, time_t *start);
void CancelAllOrders(int (*queue)[4][4]);

void FloorLight(int *old_floor);
void LightControl(int (*queue)[4][4]);
void LightsOff(int (*queue)[4][4]);
void DoorOpen(int open);
void StopButtonPressed(int (*queue)[4][4], int old_floor);
void DriveHandler(int (*queue)[4][4], int old_floor, time_t *start, int *elevator_direction);
