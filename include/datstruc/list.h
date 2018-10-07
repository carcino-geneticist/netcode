#ifndef datstruc_list_h_
#define datstruc_list_h_

struct list_t {
	void *data;
	struct list_t *next;
};


// stack operations
int list_push(struct list_t **head, void *data);
int list_pop(struct list_t **head, void **data);
void *list_rpop(struct list_t **head); // return the data pointer
int list_fpop(struct list_t **head); // call free() on data ptr as you pop

// misc
int list_append(struct list_t *head, void *data);
int list_delete(struct list_t **head);

// non-mutating, won't return errors 
struct list_t *list_tail(struct list_t *head);

#endif
