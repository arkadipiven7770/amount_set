#include <stdio.h>
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
    ASNode currNode = set->head;
    while(currNode) {
        ASNode removeNode = currNode;
        currNode = currNode->next;
        set->freeElement(removeNode->e);
        free(removeNode);
    }
    free(set);
}

AmountSet asCopy(AmountSet set){
    if (set==NULL) return NULL;
    AmountSet target = asCreate(set->copyElement, set->freeElement, set->compareElements);
    if(target==NULL) return NULL;
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

bool asContains(AmountSet set, ASElement element) {
    if (set == NULL || element == NULL ) return false;
    ASNode currentNode = set->head;
    while(currentNode){
        if (set->compareElements(element,currentNode->e)==0)
            return true;
        currentNode=currentNode->next;
    }
    return false;
}

AmountSetResult asGetAmount(AmountSet set, ASElement element, double *outAmount){
   if(set==NULL||element==0||outAmount==0){
       return AS_NULL_ARGUMENT;
   }
   ASNode currNode = set->head;
   while(currNode!=NULL){
       if(set->compareElements(currNode->e,element)==0){
           *outAmount=currNode->amount;
           return AS_SUCCESS;
       }
       currNode = currNode->next;
   }
   return AS_ITEM_DOES_NOT_EXIST;
}

AmountSetResult asRegister(AmountSet set, ASElement element){
    if (set==NULL||element==NULL){
        return AS_NULL_ARGUMENT;
    }
    if(asContains(set,element)){
        return AS_ITEM_ALREADY_EXISTS;
    }
    ASNode newElement = (ASNode)malloc(sizeof(*newElement));
    if(newElement==NULL){
        return AS_OUT_OF_MEMORY;
    }
    newElement->amount = 0;
    newElement->e = set->copyElement(element);
    newElement->next = NULL;
    newElement->prev = NULL;
    if(set->head==NULL){
        set->head= newElement;
        return AS_SUCCESS;
    }
    ASNode currNode = set->head;
    while(currNode->next != NULL){
        currNode = currNode->next;
    }
    currNode->next = newElement;
    newElement->prev = currNode;
return AS_SUCCESS;
}

AmountSetResult asChangeAmount(AmountSet set, ASElement element, const double amount){
    if (set==NULL||element==NULL){
        return AS_NULL_ARGUMENT;
    }
    AS_FOREACH(ASNode, iterator, set){
        if(set->compareElements(iterator->e,element)==0){
            if(iterator->amount+amount<0){
                return AS_INSUFFICIENT_AMOUNT;
            }
            iterator->amount=iterator->amount+amount;
            return AS_SUCCESS;
        }
    }
    return AS_ITEM_DOES_NOT_EXIST;
}

AmountSetResult asDelete(AmountSet set, ASElement element){
    if(set==NULL||element==NULL) return AS_NULL_ARGUMENT;
    ASNode currNode = set->head;
    while(currNode){
        if(set->compareElements(currNode->e,element)==0){
            (currNode->next)->prev = currNode->prev;
            (currNode->prev)->next = currNode->next;
            set->freeElement(currNode->e);
            free(currNode);
            return AS_SUCCESS;
        }
        currNode = currNode->next;
    }
    return AS_ITEM_DOES_NOT_EXIST;
}

AmountSetResult asClear(AmountSet set){
    if(set==NULL){
        return AS_NULL_ARGUMENT;
    }
    ASNode currNode = set->head;
    while(currNode) {
        ASNode removeNode = currNode;
        currNode = currNode->next;
        set->freeElement(removeNode->e);
        free(removeNode);
    }
    set->head = NULL;
    return AS_SUCCESS;
}

ASElement asGetFirst(AmountSet set){
    if (set==NULL||set->head==NULL){
        return NULL;
    }
    ASNode First = set->head;
    ASNode currNode = set->head;
    while(currNode!=NULL){
        if (set->compareElements(First->e,currNode->e)>0){
            First = currNode;
        }
        currNode = currNode->next;
    }
    set->iterator = First;
    return First;
}

ASElement asGetNext(AmountSet set){
    if (set==NULL||set->iterator==NULL){
        return NULL;
    }
    ASNode NEXT = set->head;
    ASNode currNode = set->head;
    bool Changed = false;
    while(NEXT){
        if(set->compareElements((set->iterator)->e,NEXT->e)<0){
            Changed = true;
            break;
        }
        NEXT=NEXT->next;
    }
    if(!Changed) return NULL;
    while(currNode!=NULL){
        if(set->compareElements(currNode->e,(set->iterator)->e)>0&&
                        set->compareElements(currNode->e,NEXT->e)<0){
            NEXT=currNode;
        }
        currNode = currNode->next;
    }
    set->iterator=NEXT;
    return NEXT;
}
