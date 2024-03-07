#pragma once
#include "queue.h"


state_t *initialize();

void check_for_new_order(queue_t *queue, state_t *state);

void update_state(state_t *current_state, int direction);

void perform_order(order_t *order_to_perform, state_t *current_state);

void end_session(queue_t *queue, queue_iter_t *queue_iter, state_t *state);
