#include "master_head.h"
#define _GNU_SOURCE
#define PTR_ERROR 2
/** Argument : prends un size_t qui précise la taille des éléments que la 
  * structure contiendra
  * Retour : renvoie la structure darray_t nouvellement créée
  * darray_new créé une nouvelle structure darray_t initialement vide (end = 
  * begin = NULL) et effectue l'allocation mémoire pour cette structure. Elle 
  * contiendra capacity élements de element_size chacun
*/
darray_t *darrayNew(size_t element_size) {
	darray_t *ret;
	ret = (darray_t*)malloc(sizeof(darray_t)); // Du coup... Indépendant de element_size ??
	ret->begin = NULL;
	ret->end = ret->begin; // Donc NULL... pourquoi ne pas mettre null ?
	ret->element_size = element_size;
	ret->capacity = 0;
	return ret;
}

void darrayInsert(darray_t *self, void *pos, void *elem) {
	void *new_array;
	char *itr;
	int pos_aux;
	size_t size;
	pos_aux = (char*)pos - (char*)self->begin;
	if(darraySize(self) >= self->capacity) {
		size = darraySize(self);
		new_array = realloc(self->begin, (darraySize(self) + 1) * self->element_size);
		if (!new_array) {
			fprintf(stderr, "Failed memory reallocation at %s:%d\nAborting...", __FILE__,
						 __LINE__ - 2);
			exit(PTR_ERROR);
		}
		self->begin = new_array;
		++self->capacity;
		self->end = (char*)self->begin + size * self->element_size;
	}
	itr = (char*)self->begin + pos_aux;
	memmove(itr + self->element_size, itr, (char*)self->end - (char*)itr);
	memcpy(itr, elem, self->element_size);
	self->end = (char*)self->end + self->element_size;
}
void darrayErase(darray_t *self, void *pos) {
	memmove(pos, (char *)pos + self->element_size,
					((char *)self->end - self->element_size) - (char *)pos);
	self->end = (char*)self->end - self->element_size;
}
size_t darraySize(darray_t *self) {
	return ((char*)self->end - (char*)self->begin) / self->element_size;
}
void *darrayGet(darray_t *self, size_t idx) {
	void *itr;
	itr = (char*)self->begin + idx * self->element_size;
	return itr;
}
void darrayPushBack(darray_t *self, void *elem) {
	darrayInsert(self, self->end, elem);
}
void darrayPopBack(darray_t *self) {
	darrayErase(self, (char*)self->end - self->element_size);
}
void darrayDelete(darray_t *self) {
	free(self->begin);
	free(self);
}
