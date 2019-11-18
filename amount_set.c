#include <stdio.h>
//Hello my name is arkadi//
#include <stdbool.h>
#include <stdlib.h>
#include "amount_set.h"
#include <assert.h>
typedef struct AS_node_t *ASNode;
struct AS_node_t {
    ASElement e;
    double amount;

    ASNode next;
    ASNode prev;

};

struct AmountSet_t {
    CopyASElement copyElement;
    FreeASElement freeElement;
    CompareASElements compareElements;

    ASNode head;
    ASNode iterator;
};

AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements)
{
    if(!copyElement||!freeElement||!compareElements){return NULL;}
    AmountSet target = (AmountSet)malloc(sizeof(*target));
    if(!target) return NULL;
    target->copyElement = copyElement;
    target->freeElement = freeElement;
    target->compareElements = compareElements;
    target->head = NULL;
    target->iterator = NULL;
    return target;
}

void asDestroy(AmountSet set){
    if (set==NULL) return;
    if(set->head==NULL)
    {
        free(set);
        return;
    }
    if ((set->head)->next==NULL){
        set->freeElement((set->head)->e);
        return;
    }
    set->freeElement((set->head)->e);
    ASNode temp = (set->head)->next;
    free(set->head);
    set->head=temp;
    asDestroy(set);
}

AmountSet asCopy(AmountSet set){
    if (!set) return NULL;
    AmountSet target = asCreate(set->copyElement, set->freeElement, set->compareElements);
    if(!target) return NULL;
    ASNode currentNode = set->head;
    AS_FOREACH(ASNode,iterator,set){
        if(asRegister(target, iterator->e)!=AS_SUCCESS)
        {
            asDestroy(target);
            return NULL;
        }
        if(asChangeAmount(target, iterator->e, iterator->amount)!=AS_SUCCESS)
        {
            asDestroy(target);
            return NULL;
        }
    }
    return target;
}

int asGetSize(AmountSet set){
    int size = 0;
    if(set==NULL) return -1;
    ASNode currentNode = set->head;
    while(currentNode){
        size++;
        currentNode = currentNode->next;
    }
    return size;
}
//HELLO GIT HUB!!
bool asContains(AmountSet set, ASElement element) {
    if (set == NULL || element == NULL ) return false;
    ASNode currentNode = set->head;
    while(currentNode){
        if (set->compareElements(element,currentNode->e)==0)
            return true;
    }
    return false;
}

AmountSetResult asGetAmount(AmountSet set, ASElement element, double *outAmount){return 1;}

AmountSetResult asRegister(AmountSet set, ASElement element){return 1;}

AmountSetResult asChangeAmount(AmountSet set, ASElement element, const double amount){return 1;}

AmountSetResult asDelete(AmountSet set, ASElement element){return 1;}

AmountSetResult asClear(AmountSet set){return 1;}

ASElement asGetFirst(AmountSet set){return NULL;}

ASElement asGetNext(AmountSet set){return NULL;}
