#pragma once

//struct for the order struct
typedef struct order order_t;

struct order{
  order_t *next;
  order_t *prev;
  //DEFINE WHAT AN ORDER SHOULD CONTAIN
  int floor;
  int direction;
  //int internal;
  //int entered;
  //int completed;
};

typedef struct state {
  int direction;
  int floor;
} state_t;

//A placeholder for an comparison function which is needed in the queue in order to sort it.
typedef int (*comparison_t)(const order_t *a, const order_t *b, const state_t *state);

//The struct for the queue
typedef struct queue queue_t;

struct queue{
  order_t *head;
  order_t *tail;
  comparison_t cmpfunc;
};


//allocates a new order
order_t *new_order(int floor, int button, state_t *state);

void destroy_order(order_t *order_to_destruction);

int equal_order(order_t *a, order_t *b);

//Creates a new queue
queue_t *queue_create();


//deallocates an exisiting queue
void queue_destroy(queue_t *queue);

//adds an element to the queue
void queue_add(queue_t *queue, order_t *new_order);

int queue_contains(queue_t *queue, order_t *order_to_check);

//struct for the queue iterator
typedef struct queue_iter{
  queue_t *queue;
  order_t *next_order;
} queue_iter_t;


//creates a new queue iterator
queue_iter_t *queue_createiter(queue_t *queue);

//updates the queue iterator to point to queue head
void update_queue_iter(queue_iter_t *queue_iter);

//destroys an existing queue iterator
void queue_destroyiter(queue_iter_t *iter);

//checks if there is a next element in then queue
int queue_hasnext(queue_iter_t *iter);

//returns the next element in the queue
order_t *queue_next(queue_iter_t *queue_iter, state_t *current_state);

