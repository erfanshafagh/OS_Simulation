#include "list.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// Static array of list heads and nodes
static List listHeads[LIST_MAX_NUM_HEADS];  
static Node nodes[LIST_MAX_NUM_NODES];

// To track available list heads and nodes
static int nextListHead = 0; 
static int nextFreeNode = 0;

// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create(){
    if (nextListHead >= LIST_MAX_NUM_HEADS) {
        return NULL; // No more list heads available
    }

    for (int i = 0; i < LIST_MAX_NUM_HEADS; i++){
        List* list = &listHeads[nextListHead];
        if (list->valid != true){
            nextListHead++;
            list->head = NULL;
            list->current = NULL;
            list->tail = NULL;
            list->valid = true;
            list->count = 0;
            return list;
        }
    }
    return NULL;
}

// Returns the number of items in pList.
int List_count(List* pList){
    return pList->count;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList){
    if (pList->head == NULL) {
        pList->current = NULL;
        return NULL;
    }
    pList->current = pList->head;
    return pList->current->data;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList){
    if (pList->head == NULL) {
        pList->current = NULL;
        return NULL;
    }
    pList->current = pList->tail;
    return pList->current->data;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList){
    if (pList->current == NULL || pList->current == LIST_OOB_START || pList->current == LIST_OOB_END) { 
        return NULL;
    }
    if(pList->current == pList->tail){
        pList->current = LIST_OOB_END;
        return NULL;
    }
    pList->current = pList->current->next;
    return pList->current->data;  
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList){
    if (pList->current == NULL || pList->current == LIST_OOB_START || pList->current == LIST_OOB_END) { 
        return NULL;
    }
    if(pList->current == pList->head){
        pList->current = LIST_OOB_START;
        return NULL;
    }
    pList->current = pList->current->previous;
    return pList->current->data; 
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList){
    if(pList->current == NULL || pList->current == LIST_OOB_START || pList->current == LIST_OOB_END){
        return NULL;
    }
    return pList->current->data;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_after(List* pList, void* pItem){
    Node* node = CreataNode();
    if(node == NULL){
        return LIST_FAIL;
    }
    pList->count++;

    if(pList->current == LIST_OOB_START || pList->current == NULL){
        node->data = pItem;
        node->next = pList->head;
        pList->head->previous = node;
        node->previous = NULL;
        pList->head = node;
        pList->current = node;
        if(pList->current == NULL){
            pList->tail = node;
        }
        return LIST_SUCCESS;
    }
    else if(pList->current == LIST_OOB_END || pList->current == pList->tail){
        node->data = pItem;
        node->previous = pList->tail;
        pList->tail->next = node;
        node->next = NULL;
        pList->current = node;
        pList->tail = node;
        return LIST_SUCCESS;
    }
    else{
        node->data = pItem;
        Node* Next = pList->current->next;
        pList->current->next = node;
        Next->previous = node;
        node->previous = pList->current;
        node->next = Next;
        pList->current = node;
        return LIST_SUCCESS;
    }
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_before(List* pList, void* pItem){
    Node* node = CreataNode();
    if(node == NULL){
        return LIST_FAIL;
    }
    pList->count++;
    
    if(pList->current == LIST_OOB_START || pList->current == pList->head || pList->current == NULL){
        node->data = pItem;
        node->next = pList->head;
        pList->head->previous = node;
        node->previous = NULL;
        pList->head = node;
        pList->current = node;
        if(pList->current == NULL){
            pList->tail = node;
        }
        return LIST_SUCCESS;
    }
    else if(pList->current == LIST_OOB_END){
        node->data = pItem;
        node->previous = pList->tail;
        pList->tail->next = node;
        node->next = NULL;
        pList->current = node;
        pList->tail = node;
        return LIST_SUCCESS;
    }
    else{
        node->data = pItem;
        pList->current->previous->next = node;
        node->previous = pList->current->previous;
        node->next = pList->current;
        pList->current->previous = node;
        pList->current = node;
        return LIST_SUCCESS;
    }
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem){
    Node* node = CreataNode();
    if(node == NULL){
        return LIST_FAIL;
    }
    pList->count++;

    if(pList->tail == NULL){
        node->data = pItem;
        node->next = NULL;
        node->previous = NULL;
        pList->current = node;
        pList->tail = node;
        pList->head = node;
        return LIST_SUCCESS;
    }else{
        node->data = pItem;
        node->previous = pList->tail;
        pList->tail->next = node;
        node->next = NULL;
        pList->current = node;
        pList->tail = node;
        return LIST_SUCCESS;
    }
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem){
    Node* node = CreataNode();
    if(node == NULL){
        return LIST_FAIL;
    }
    pList->count++;

    if(pList->head == NULL){
        node->data = pItem;
        node->next = NULL;
        node->previous = NULL;
        pList->head = node;
        pList->current = node;
        pList->tail = node;
        return LIST_SUCCESS;
    }else{
        node->data = pItem;
        node->next = pList->head;
        pList->head->previous = node;
        node->previous = NULL;
        pList->head = node;
        pList->current = node;
        return LIST_SUCCESS;
    }
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList){
    if(pList->current == LIST_OOB_START || pList->current == LIST_OOB_END){
        return NULL;
    }
    pList->count--;
    
    if(pList->current == pList->head && pList->current == pList->tail){
        void* target = pList->current->data;
        pList->current->next = NULL;
        pList->current->previous = NULL;
        pList->current->valid = false;
        pList->head = NULL;
        pList->current = NULL;
        pList->tail = NULL;
        return target;
    }else if(pList->current == pList->head){
        void* target = pList->current->data;
        pList->head = pList->current->next;
        pList->current->next = NULL;
        pList->current->previous = NULL;
        pList->current->valid = false;
        pList->head->previous = NULL;
        pList->current = pList->head;
        return target;
    }else if(pList->current == pList->tail){
        void* target = pList->current->data;
        pList->tail = pList->current->previous;
        pList->current->next = NULL;
        pList->current->previous = NULL;
        pList->current->valid = false;
        pList->tail->next = NULL;
        pList->current = pList->tail;
        return target;
    }else{
        void* target = pList->current->data;
        Node* newcurrent = pList->current->next;
        pList->current->previous->next = pList->current->next;
        pList->current->next->previous = pList->current->previous;
        pList->current->next = NULL;
        pList->current->previous = NULL;
        pList->current->valid = false;
        pList->current = newcurrent;
        return target;
    }
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){
    if(pList->head == NULL){
        return NULL;
    }
    pList->count--;
    pList->current = pList->tail;
    void* target = pList->current->data;
    pList->tail = pList->current->previous;
    pList->current->next = NULL;
    pList->current->previous = NULL;
    pList->current->valid = false;
    pList->tail->next = NULL;
    pList->current = pList->tail;
    return target;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2){
    pList1->tail->next = pList2->head;
    pList2->head->previous = pList1->tail;
    pList1->tail = pList2->tail;
    pList1->count = pList1->count + pList2->count;
    pList2->head = NULL;
    pList2->tail = NULL;
    pList2->current = NULL;
    pList2->count = 0;
    pList2->valid = false;
    nextListHead--;
    return;
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn){
    Node* current = pList->head;
    while (current != NULL){
        Node* next = current->next;
        (*pItemFreeFn)(current->data);
        current->next = NULL;
        current->previous = NULL;
        current->valid = false;
        current = next;
    }
    pList->head = NULL;
    pList->current = NULL;
    pList->tail = NULL;
    pList->count = 0;
    pList->valid = false;
    nextListHead--;
    return;
}    

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    Node* current = pList->head;
    if(current == LIST_OOB_END){
        return NULL;
    }
    while (current != NULL) {
        if (pComparator(current->data, pComparisonArg)) {
            pList->current = current;
            return pList->current->data;
        }
        current = current->next;
    }
    pList->current = LIST_OOB_END;
    return NULL; 
}

/*----------ADDED FUNCTIONS----------*/
// returnes a free node pointer to use it in a list
Node* CreataNode(){
    if (nextFreeNode >= LIST_MAX_NUM_NODES) {
        return NULL; // No more nodes available
    }

    for (int i = nextFreeNode; i < LIST_MAX_NUM_NODES; i++){
        Node* node = &nodes[nextFreeNode];
        if (node->valid != true){
            node->valid = true;
            nextFreeNode++;
            return node;
        }
    }
    for (int i = 0; i < nextFreeNode; i++){
        Node* node = &nodes[nextFreeNode];
        if (node->valid != true){
            node->valid = true;
            nextFreeNode++;
            return node;
        }
    }
    return NULL;
}
// print all items in the list
void printALL(List* pList){
    Node* current = pList->head;
    while (current != NULL){
        printf("%s -> ", (char*)current->data);
        current = current->next;
    }
    printf("\n");
}

// End of list.c