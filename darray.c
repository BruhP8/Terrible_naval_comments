#include "master_head.h"
#define _GNU_SOURCE
#define PTR_ERROR 2
/** Argument : prends un size_t qui précise la taille des éléments que la 
  * structure contiendra \n
  * darray_new créé une nouvelle structure darray_t initialement vide (end = 
  * begin = NULL) et effectue l'allocation mémoire pour cette structure. Elle 
  * contiendra capacity élements de element_size chacun \n
  * Retour : renvoie la structure darray_t nouvellement créée
*/
darray_t *darrayNew(size_t element_size) {
	darray_t *ret;
	ret = (darray_t*)malloc(sizeof(darray_t));
	ret->begin = NULL;
	ret->end = NULL;
	ret->element_size = element_size;
	ret->capacity = 0;
	return ret;
}
/** Aruguments : le darray_t à modifier, la position de l'élément à insérer et l'élément à insérer \n
  * Copie l'actuel darray_t dans un nouveau darray_t en y insérant élem à la position pos. \n
  * En cas de dépassement de la capacité si l'ancien darray était plein, gère la
  * réallocation mémoire \n
  * Retour : rien
*/
void darrayInsert(darray_t *self, void *pos, void *elem) {
	void *new_array;
	char *itr;
	int pos_aux;
	size_t size;
	pos_aux = (char*)pos - (char*)self->begin;
	if(darraySize(self) >= self->capacity) { //si on dépasse sa capacité
		size = darraySize(self); // enregistre la size
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
/** Arguments : le darray_t qu'on veut modifier et la position de l'élément a supprimer \n
  * Supprime l'élément situé à la position pos du darray \n
  * Retour : rien
*/
void darrayErase(darray_t *self, void *pos) {
	memmove(pos, (char *)pos + self->element_size, ((char *)self->end - self->element_size) - (char *)pos);
	self->end = (char*)self->end - self->element_size;
}
/** Argument : Prend le darray_t dont on veut connaitre la taille \n
  * Retour : Renvoie cette size
*/
size_t darraySize(darray_t *self) {
	return ((char*)self->end - (char*)self->begin) / self->element_size;
}
/** Arguments : le darray_t auquel on veut accèder et l'indexe de l'élément \n
  *	Parcours le darray_t passé en argument avec un itérateur idx  \n
  * Retour un pointeur sur cet élément
*/
void *darrayGet(darray_t *self, size_t idx) {
	void *itr;
	itr = (char*)self->begin + idx * self->element_size;
	return itr;
}
/** Arguments : le darray_t dans lequel on veut ajouter un élément, et cet élément \n
  * Se contente d'appeller darrayInsert() avec comme position la fin du darray_t \n
  * Retour : rien
*/
void darrayPushBack(darray_t *self, void *elem) {
	darrayInsert(self, self->end, elem);
}
/** Arguments : le darray_t dont on veut supprimer le dernier élément  \n
  * Se contente d'appeller darrayErase() avec comme position l'avant dernier élément \n
  * Retour : rien
*/
void darrayPopBack(darray_t *self) {
	darrayErase(self, (char*)self->end - self->element_size);
}
/** Argument : Le darray_t a supprimer \n
  * Appelle free sur ce tableau \n
  * Retour : rien
*/
void darrayDelete(darray_t *self) {
	free(self->begin);
	free(self);
}
