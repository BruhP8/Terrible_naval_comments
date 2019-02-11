#ifndef DARRAY_H
#define DARRAY_H
typedef struct {
	void *begin;
	void *end;
	size_t element_size;
	size_t capacity;
} darray_t;
darray_t *darrayNew(size_t element_size);
void  darrayInsert(darray_t *self, void *pos, void *elem);
void  darrayErase(darray_t *self, void *pos);
void *darrayGet(darray_t *self, size_t idx);
size_t darraySize(darray_t *self);
size_t darrayElemSize(darray_t *self);
void  darrayPushBack(darray_t *self, void *elem);
void  darrayPopBack(darray_t *self);
void  darrayDelete(darray_t *self);
#endif 
