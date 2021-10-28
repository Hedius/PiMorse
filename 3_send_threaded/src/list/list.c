/*
 *project morseSend: list.c
 *this list is thread-safe!
 *license: GPLv3
 */
#include "list.h"

/*
* Function: createList
* inits a new list
* returns: pointer to list header
*/
LIST_HEADER * createList(){
    LIST_HEADER * header = malloc(sizeof(LIST_HEADER));
    if(!header) return NULL;

    header->len = 0;
    header->first = header->last = NULL;

    header->status = 1;
    pthread_mutex_init(&header->mutex, NULL);
    return header;
}

/*
* Function: insertFirst
* inserts a new element at the front of the list
* header: LIST_HEADER
* character: ASCII character
* morse: string containing morse code
* returns: pointer to new element
*/
LIST_ELEM * insertFirst(LIST_HEADER * header, char character, char * morse){
    if(!header) return NULL;
    LIST_ELEM * new = malloc(sizeof(LIST_ELEM));
    if(!new) return NULL;
    
    new->prev = NULL;

    pthread_mutex_lock(&header->mutex); //lock

    //first element
    if(header->len == 0)
        header->last = new;
    else
        header->first->prev = new;
    new->next = header->first;
    header->first = new;

    new->character = character;
    new->morse = morse;
    header->len++;

    pthread_mutex_unlock(&header->mutex); //unlock
    return new;
}

/*
* Function: insertLast
* inserts a new element at the back of the list
* header: LIST_HEADER
* character: ASCII character
* morse: string containing morse code
* returns: pointer to new element
*/
LIST_ELEM * insertLast(LIST_HEADER * header, char character, char * morse){
    if(!header) return NULL;
    LIST_ELEM * new = malloc(sizeof(LIST_ELEM));
    if(!new) return NULL;

    new->next = NULL;

    pthread_mutex_lock(&header->mutex); // lock

    // first element
    if(header->len == 0)
        header->first = new;
    else
        header->last->next = new;
    new->prev = header->last;
    header->last = new;

    new->character = character;
    new->morse = morse;
    header->len++;

    pthread_mutex_unlock(&header->mutex); // unlock
    return new;
}

/*
* Function: freeELEM
* deallocates the memory of the given LIST_ELEM
* elem: LIST_ELEM
*/
void freeELEM(LIST_ELEM * elem){
    if(!elem) return;
    free(elem->morse);
    free(elem);
}

/*
* Function: removeFirst
* removes the first element of the given list and returns it
* header: LIST_HEADER
* returns: LIST_ELEM pointer or NULL if empty
*/
LIST_ELEM * removeFirst(LIST_HEADER * header){
    LIST_ELEM * tmp = NULL;
    if(!header) return NULL;

    pthread_mutex_lock(&header->mutex); // lock

    if(header->len > 0){
        tmp = header->first;

        header->first = tmp->next;
        header->len--;
        if(header->len == 0)
            header->last = NULL;
        else
            header->first->prev = NULL;
    }

    pthread_mutex_unlock(&header->mutex); // unlock
    return tmp;
}

/*
* Function: deleteFirst
* removes the first element of the given list and deallocates it
* header: LIST_HEADER
*/
void deleteFirst(LIST_HEADER * header){
    LIST_ELEM * tmp = removeFirst(header);
    if(!tmp) return;
    freeELEM(tmp);
}

/*
* Function: removeLast
* removes the last element of the given list and returns it
* header: LIST_HEADER
* returns: LIST_ELEM pointer or NULL if empty
*/
LIST_ELEM * removeLast(LIST_HEADER * header){
    LIST_ELEM * tmp = NULL;
    if(!header) return NULL;

    pthread_mutex_lock(&header->mutex); // lock

    if(header->len > 0){
        tmp = header->last;

        header->last = tmp->prev;
        header->len--;
        if(header->len == 0)
            header->first = NULL;
        else
            header->last->next = NULL;
    }

    pthread_mutex_unlock(&header->mutex); // unlock
    return tmp;
}

/*
* Function: deleteLast
* removes the last element of the given list and deallocates it
* header: LIST_HEADER
*/
void deleteLast(LIST_HEADER * header){
    LIST_ELEM * tmp = removeLast(header);
    if(!tmp) return;
    freeELEM(tmp);
}

/*
* Function: printList
* prints to content of the given list to fp
* header: LIST_HEADER
* fp: FILE
*/
void printList(LIST_HEADER * header, FILE * fp){
    LIST_ELEM * cur;
    if(!header || !fp) return;
    if(header->len <= 0){
        fprintf(stderr, "List is empty! Cannot print!\n");
        return;
    }

    pthread_mutex_lock(&header->mutex); // lock

    cur = header->first;
    for(int i = 0; i < header->len; i++){
        fprintf(fp, "%i: data: Char: \"%c\" Morse: \"%s\"\n", i, cur->character, cur->morse);
        cur = cur->next;
    }

    pthread_mutex_unlock(&header->mutex); //unlock
}

/*
* Function: printListReverse
* prints to content of the given list to fp (reverse order)
* header: LIST_HEADER
* fp: FILE
*/
void printListReverse(LIST_HEADER * header, FILE * fp){
    LIST_ELEM * cur;
    if(!header || !fp) return;
    if(header->len <= 0){
        fprintf(stderr, "List is empty! Cannot print!\n");
        return;
    }

    pthread_mutex_lock(&header->mutex); // lock

    int i = header->len - 1;
    cur = header->last;
    do{
        fprintf(fp, "%i: data: Char: \"%c\" Morse: \"%s\"\n", i, cur->character, cur->morse);
        i--;
        cur = cur->prev;
    } while(cur);

    pthread_mutex_unlock(&header->mutex);
}


/*
* Function: rmList
* deallocates the full list
* header: LIST_HEADER
*/
void rmList(LIST_HEADER * header){
    if(!header) return;
    while(header->len > 0)
        deleteFirst(header);
    pthread_mutex_destroy(&header->mutex);
    free(header);
}
