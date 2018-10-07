#ifndef queue_h_
#define queue_h_

#ifndef datstruc_list_h
#include "list.h"
#endif

struct queue_t {
	struct list_t *list;	
};

int queue_push(struct queue_t *q, void *data);
int queue_pop(struct queue_t *q, void **data);
void *queue_rpop(struct queue_t *q);
int queue_delete(struct queue_t *q);

#endif
