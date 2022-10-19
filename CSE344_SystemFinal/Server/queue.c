#include "queue.h"

void offer_int(Queue **q, int e){
    if(q==NULL) return;

    if((*q)==NULL) {
        (*q) = (Queue*)malloc(sizeof(Queue));
        (*q)->head = NULL;
        (*q)->tail =NULL;
    }

    if((*q)->tail == NULL){
        (*q)->head = (Node*)malloc(sizeof(Node));
        (*q)->tail = (*q)->head;

        (*q)->head->data = e;
        (*q)->head->next = NULL;

        return;
    }

    (*q)->tail->next = (Node*)malloc(sizeof(Node));
    (*q)->tail->next->data = e;
    (*q)->tail->next->next = NULL;
    
    (*q)->tail = (*q)->tail->next;
}

int poll_int(Queue **q){
    int ret_val = -1;

    if(q==NULL || (*q)==NULL || (*q)->head==NULL) {
        return ret_val;
    }

    Node* temp = (*q)->head;
    ret_val = temp->data;
    (*q)->head=(*q)->head->next;

    if((*q)->head == NULL){
        (*q)->tail = NULL;
    }

    free(temp);

    return ret_val;
}

void free_queue(Queue **q){
    if(q==NULL || (*q)==NULL) return;
    Node *temp;
    Node *iter=(*q)->head;
    while(iter != NULL){
        temp = iter;
        iter = iter->next;
        temp->next = NULL;
        free(temp);
    }

    free(*q);
}

int is_empty(Queue **q){
    if(q==NULL || (*q)==NULL || (*q)->head==NULL) return 1;
    
    return 0;
}

