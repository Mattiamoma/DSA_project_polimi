#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 1000


typedef struct node {
    char* name;
    void* data;
    struct node* next;
    
} node_t;

typedef struct storage {
    int quantity;
    int exp;
    struct storage* next;
} storage_t;

typedef struct ingredients {
    char* name;
    int quantity;
    node_t* storage;
    struct ingredients* next;
} ingredients_t;

typedef struct queuePending {
    node_t* recipe;
    int quantity;
    int k;
    struct queuePending* next;
} queuePending_t;

typedef struct queueDone {
    node_t* recipe;
    int quantity;
    int weight;
    int k;
    struct queueDone* next;
} queueDone_t;



typedef struct queueRefDone {
    queueDone_t* head;
    queueDone_t* tail;
} queueRefDone_t;



typedef struct queueRefPending {
    queuePending_t* head;
    queuePending_t* tail;
} queueRefPending_t;



int isCommand(char* string) {
    char remove[16] = "rimuovi_ricetta";
    char add[17] = "aggiungi_ricetta";
    char rif[13] = "rifornimento";
    char order[8] = "ordine";

    if(strcmp(string, add) == 0) {
        return 1;
    } 
    else if(strcmp(string, remove) == 0) {
        return 2;
    } 
    else if(strcmp(string, rif) == 0) {
        return 3;
    } 
    else if(strcmp(string, order) == 0) {
        return 4;
    } 
    else {
        return 0;
    }
}


void queueInsertPending(queueRefPending_t* queueRef, queuePending_t* node) {
    if(queueRef->tail == NULL) {
        queueRef->head = node;
        queueRef->tail = node;
        return;
    }

    queueRef->tail->next = node;
    queueRef->tail = node;
}


void queueInsertDone(queueRefDone_t* queueRef, queueDone_t* node) {
    queueDone_t* ptr;
    queueDone_t* aux;
    node->next = NULL;

    if(queueRef->head == NULL) {
        queueRef->head = node;
        queueRef->tail = node;
        return;
    }
    if(queueRef->head->k > node->k) {
        node->next = queueRef->head;
        queueRef->head = node;
        return;
    }
    if(queueRef->tail->k < node->k) {
        queueRef->tail->next = node;
        queueRef->tail = node;
        return;
    }

    aux = queueRef->head;
    ptr = aux->next;

    while (ptr != NULL && ptr->k < node->k) {
        ptr = ptr->next;
        aux = aux->next;
    }
    aux->next = node;
    node->next = ptr;
    if(ptr == NULL) {
        queueRef->tail = node;
    }
    
}




void printHash(node_t** hashmap) {
    int i;
    node_t* node;
    ingredients_t* ing;
    for(i = 0; i < N; i++) {
        if(hashmap[i] != NULL) {
            node = hashmap[i];
            while (node != NULL) {

                printf("%s: ", node->name);
                ing = node->data;
                while (ing != NULL){

                    printf("%s %d, ", ing->name, ing->quantity);
                    ing = ing->next;
                }
                printf("\n");
                
                node = node->next;
            }
            
        }
    }
}



void printHashStorage(node_t** hashmap) {
    int i;
    node_t* node;
    storage_t* item;
    for(i = 0; i < N; i++) {
        if(hashmap[i] != NULL) {
            node = hashmap[i];
            while (node != NULL) {

                printf("%s: ", node->name);
                item = node->data;
                while (item != NULL){

                    printf("(%d %d) ", item->quantity, item->exp);
                    item = item->next;
                }
                printf("\n");
                
                node = node->next;
            }
            
        }
    }
}



unsigned int indexOf(char* string) {
    int i = 0;
    unsigned int sum = 0;
    while (string[i] != '\0') {

        sum += string[i];
        i++;
    }
    return sum % N;
    
}



void insertHash(node_t** hashmap, node_t* newNode) {

    int index = indexOf(newNode->name);
    newNode->next = NULL;

    if(hashmap[index] == NULL) {
        hashmap[index] = newNode;
    }
    else {
        newNode->next = hashmap[index];
        hashmap[index] = newNode;
    }

}


node_t* searchHash(node_t** hashmap, char* name) {

    int index = indexOf(name);
    node_t* ptr = hashmap[index];

    while(ptr != NULL && strcmp(ptr->name, name) != 0) {

        ptr = ptr->next;
    }

    return ptr;
}


void deleteHash(node_t** hashmap, node_t* node) {
    int index = indexOf(node->name);
    node_t* ptr = hashmap[index];
    node_t* aux = NULL;


    while(ptr != NULL && ptr != node) {
        aux = ptr;
        ptr = ptr->next;
    }

    if(aux == NULL) {
        hashmap[index] = node->next;
    }
    else {
        aux->next = ptr->next;
    }
    free(node->name);
    free(node);
}



char checkOrder(queuePending_t* order, node_t** storage, queueRefDone_t* done, int k) {

    int quantity;
    int weight;
    queueDone_t* newNode;
    node_t* storageItem;
    storage_t* item;
    ingredients_t* ingredient;

    ingredient = order->recipe->data;
    

    while (ingredient != NULL) {
        
        storageItem = ingredient->storage;
        if(storageItem == NULL || storageItem->data == NULL) {
            return 0;
        }
        quantity = order->quantity * ingredient->quantity;
        item = storageItem->data;

        while (storageItem->data != NULL && item->exp <= k) {
                
            storageItem->data = item->next;
            free(item);
            item = storageItem->data;
        }


        while(item != NULL && quantity > 0) {
            quantity -= item->quantity;
            item = item->next;
        }
        if(quantity > 0) {
            return 0;
        }


        ingredient = ingredient->next;
    }


    ingredient = order->recipe->data;
    weight = 0;

    while(ingredient != NULL) {

        quantity = order->quantity * ingredient->quantity;
        weight += quantity;

        storageItem = ingredient->storage;
        

        while(storageItem->data != NULL) {
            item = storageItem->data;
            quantity -= item->quantity;

            if(quantity < 0) {
                item->quantity = quantity * (-1);
                break;
            }
            else if(quantity == 0) {
                storageItem->data = item->next;
                free(item);
                break;

            }

            storageItem->data = item->next;
            free(item);
            
        }

        ingredient = ingredient->next;
    }
    newNode = malloc(sizeof(queueDone_t));
    newNode->recipe = order->recipe;
    newNode->quantity = order->quantity;
    newNode->weight = weight;
    newNode->k = order->k;
    newNode->next = NULL;

    queueInsertDone(done, newNode);
    

    return 1;

}




char endOfLine() {
    char car;
    car = getchar();
    if(car == '\n' || car == EOF) {
        return 1;
    }
    else {
        ungetc(car, stdin);
        return 0;
    }
}




void aggiungiRicetta(node_t** hashmap, node_t** storage) {
    char name[256];
    node_t* recipe;
    ingredients_t* ingredient;
    

    if(!scanf("%s", name)) {
        return;
    }

    if(searchHash(hashmap, name) != NULL) {
        printf("ignorato\n");
        return;
    }

    recipe = malloc(sizeof(node_t));
    recipe->name = malloc(strlen(name) + 1);
    strcpy(recipe->name, name);
    recipe->next = NULL;
    

    ingredient = malloc(sizeof(ingredients_t));
    if(!scanf(" %s %d", name, &ingredient->quantity)) {
        return;
    }
    ingredient->name = malloc(strlen(name) + 1);
    strcpy(ingredient->name, name);
    recipe->data = ingredient;
    ingredient->storage = searchHash(storage, ingredient->name);
    if(ingredient->storage == NULL) {

        ingredient->storage = calloc(1, sizeof(node_t));
        ingredient->storage->name = malloc(strlen(name) + 1);
        strcpy(ingredient->storage->name, name);
        insertHash(storage, ingredient->storage);
        
    }
    ingredient->next = NULL;

    while (!endOfLine()) {

        ingredient->next = malloc(sizeof(ingredients_t));
        ingredient = ingredient->next;
        if(!scanf(" %s %d", name, &ingredient->quantity)) {
            return;
        }
        ingredient->name = malloc(strlen(name) + 1);
        strcpy(ingredient->name, name);
        ingredient->storage = searchHash(storage, ingredient->name);
        if(ingredient->storage == NULL) {
            ingredient->storage = calloc(1, sizeof(node_t));
            ingredient->storage->name = malloc(strlen(name) + 1);
            strcpy(ingredient->storage->name, name);
            insertHash(storage, ingredient->storage);

        }
        ingredient->next = NULL;

    }
    insertHash(hashmap, recipe);
    printf("aggiunta\n");
    

}




void rimuoviRicetta(node_t** hashmap, queueRefPending_t* pending, queueRefDone_t* done) {
    char name[256];
    node_t* node;
    ingredients_t* ingredient;  
    queuePending_t* ptr; 
    queueDone_t* ptr2;

    if(!scanf("%s", name)) {
        return;
    }


    node = searchHash(hashmap, name);
    

    if(node == NULL) {
        printf("non presente\n");
        return;
    }

    ptr = pending->head;
    while (ptr != NULL) {
        if(strcmp(ptr->recipe->name, name) == 0) {
            printf("ordini in sospeso\n");
            return;
        }
        ptr = ptr->next;
    }

    ptr2 = done->head;
    while (ptr2 != NULL) {
        if(strcmp(ptr2->recipe->name, name) == 0) {
            printf("ordini in sospeso\n");
            return;
        }
        ptr2 = ptr2->next;
    }


    ingredient = node->data;
    while (ingredient != NULL) {
        node->data = ingredient->next;
        free(ingredient->name);
        free(ingredient);
        ingredient = node->data;
    }
    
    
    deleteHash(hashmap, node);
    printf("rimossa\n");

}


void rifornimento(node_t** hashmap, queueRefPending_t* pendingRef, queueRefDone_t* doneRef, int k) {

    char name[256];
    node_t* storage;
    storage_t* newNode;
    storage_t* item;
    storage_t* aux;
    queuePending_t* ptr;
    queuePending_t* prec;

    printf("rifornito\n");

    while (!endOfLine()) {

        newNode = malloc(sizeof(storage_t));
        newNode->next = NULL;
        
        if(!scanf(" %s %d %d", name, &newNode->quantity, &newNode->exp)) {
            return;
        }
        storage = searchHash(hashmap, name);

        if(storage == NULL) {
            storage = malloc(sizeof(node_t));
            storage->next = NULL;
            storage->name = malloc(strlen(name) + 1);
            strcpy(storage->name, name);
            storage->data = newNode;
            insertHash(hashmap, storage);
        }

        else {  
            item = storage->data;
            
            while (item != NULL && item->exp <= k) {
                
                storage->data = item->next;
                free(item);
                item = storage->data;
            }
            

            if(storage->data == NULL) {
                storage->data = newNode;
            }
            else if(item->exp > newNode->exp) {

                newNode->next = item;
                storage->data = newNode;
            }
            else if(item->exp == newNode->exp) {
                item->quantity += newNode->quantity;
                free(newNode);
            }
            else {

                aux = storage->data;
                item = aux->next;
                while (item != NULL) {
                
                    if(item->exp > newNode->exp) {
                        aux->next = newNode;
                        newNode->next = item;
                        break;
                    }
                    else if(item->exp == newNode->exp) {
                        item->quantity += newNode->quantity;
                        free(newNode);
                        break;
                    }

                    item = item->next;
                    aux = aux->next;
                }

                if(item == NULL) {
                    aux->next = newNode;
                    newNode->next = NULL;
                }

            }
        }

    }
    

    //printHashStorage(hashmap);

    ptr = pendingRef->head;
    prec = NULL;
    while (ptr != NULL) {
        
        
        if(checkOrder(ptr, hashmap, doneRef, k) == 1) {
            if(prec == NULL) {
                pendingRef->head = ptr->next;
                ptr->next = NULL;
                free(ptr);
                ptr = pendingRef->head;
            }
            else {
                prec->next = ptr->next;
                ptr->next = NULL;
                free(ptr);
                ptr = prec->next;
            }

        }

        else {
            prec = ptr;
            ptr = ptr->next;
        }
    }
    
    if(pendingRef->head == NULL) {
        pendingRef->tail = NULL;
    }
    else {
        pendingRef->tail = prec;
    }

}

void ordine(node_t** hashRecipes, node_t** hashStorage, queueRefDone_t* doneRef, queueRefPending_t* pendingRef,int k) {

    char name[256];
    int quantity;
    queuePending_t* newNode;
    node_t* recipe;

    if(!scanf(" %s %d", name, &quantity)) {
        return;
    }

    recipe = searchHash(hashRecipes, name);
    if(recipe == NULL) {
        printf("rifiutato\n");
        return;
    }
    
    newNode = malloc(sizeof(queuePending_t));
    newNode->k = k;
    newNode->recipe = recipe;
    newNode->quantity = quantity;
    newNode->next = NULL;
    

    if(checkOrder(newNode, hashStorage, doneRef, k) == 1) {
        
        free(newNode);
        //printf("Preparato %s\n", newNode->recipe->name);
    }

    else {
        queueInsertPending(pendingRef, newNode);
    }
    printf("accettato\n");
    /*printf("Pending: ");
    printQueue(pendingRef->head);
    printf("Done: ");
    printQueue(doneRef->head);*/


}


void corriere(queueRefDone_t* doneRef, int cap) {
    
    int totalWeight = 0;
    queueDone_t* head;
    queueDone_t* ptr;
    queueDone_t* prec;
    queueDone_t* max;
    queueDone_t* maxPrec;

    ptr = doneRef->head;
    prec = NULL;

    //printQueue(doneRef->head);

    while(ptr != NULL && (totalWeight + ptr->weight) <= cap) {
        
        totalWeight += ptr->weight;

        prec = ptr;
        ptr = ptr->next;
    }

    if(totalWeight == 0) {
        printf("camioncino vuoto\n");
        return;
    }
    
    prec->next = NULL;
    head = doneRef->head;
    doneRef->head = ptr;
    ptr = head;



    while(head != NULL) {
        max = head;
        maxPrec = NULL;
        prec = head;
        ptr = head->next;
        while(ptr != NULL) {
            if(max->weight < ptr->weight) {
                max = ptr;
                maxPrec = prec;
            }
            prec = prec->next;
            ptr = ptr->next;
        }
        printf("%d %s %d\n", max->k, max->recipe->name, max->quantity);

        if(maxPrec == NULL) {
            head = max->next;
            free(max);
        }
        else {
            maxPrec->next = max->next;
            free(max);
        }
    }
}








int main() {

    int freq, cap;
    int k = 0;
    int command;
    char input[256];
    node_t* recipes[N] = {NULL};
    node_t* storage[N] = {NULL};
    queueRefDone_t* doneRef = calloc(1, sizeof(queueRefDone_t));
    queueRefPending_t* pendingRef = calloc(1, sizeof(queueRefPending_t));

    if(!scanf(" %d %d", &freq, &cap)) {
        return 0;
    }

    
    while(scanf(" %s", input) != EOF) {
        
        command = isCommand(input);

        switch (command) {

        case 1:
            aggiungiRicetta(recipes, storage);
            break;
        case 2:
            rimuoviRicetta(recipes, pendingRef, doneRef);
            break;
        case 3:
            rifornimento(storage, pendingRef, doneRef, k);
            break;
        case 4:
            ordine(recipes, storage, doneRef, pendingRef, k);
            break;
        
        default:
            continue;
        }

        
        k++;

        if(k % freq == 0) {
            corriere(doneRef, cap);
        }
           
    }   

    return 0;

}