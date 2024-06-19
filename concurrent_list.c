#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
    int value;
    struct node* next;
    pthread_mutex_t lock;
};

struct list {
    node* head;
};

void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
    printf("%d ", node->value);
  }
}

list* create_list()
{
    list* l = (list*)malloc(sizeof(list));
    l->head = NULL;
    return l;
}

void delete_list(list* list)
{
    if (list == NULL) return;
    node* current = list->head;
    while (current != NULL) {
        node* temp = current;
        current = current->next;
        pthread_mutex_destroy(&temp->lock);
        free(temp);
    }
    free(list);
}

void insert_value(list* list, int value)
{
    if (list == NULL) return;
    node* new_node = (node*)malloc(sizeof(node));
    new_node->value = value;
    new_node->next = NULL;
    pthread_mutex_init(&new_node->lock, NULL);
    pthread_mutex_t* lock_to_unlock = NULL;
    if (list->head == NULL || list->head->value > value) {
        pthread_mutex_lock(&new_node->lock);
        new_node->next = list->head;
        list->head = new_node;
        pthread_mutex_unlock(&new_node->lock);
        return;
    }
    node* prev = list->head;
    pthread_mutex_lock(&prev->lock);
    node* current = prev->next;
    while (current != NULL && current->value < value) {
        pthread_mutex_lock(&current->lock);
        pthread_mutex_unlock(&prev->lock);
        prev = current;
        current = current->next;
    }
    pthread_mutex_lock(&new_node->lock);
    new_node->next = current;
    prev->next = new_node;
    pthread_mutex_unlock(&new_node->lock);
    if (current != NULL) {
        pthread_mutex_unlock(&current->lock);
    }
    pthread_mutex_unlock(&prev->lock);
}

void remove_value(list* list, int value)
{
    if (list == NULL) return;
    struct node* prev = NULL;
    struct node* current = list->head;
    if (current != NULL) {
        pthread_mutex_lock(&current->lock);
    }
    while (current != NULL && current->value != value) {
        if (prev != NULL) {
            pthread_mutex_unlock(&prev->lock);
        }
        prev = current;
        current = current->next;
        if (current != NULL) {
            pthread_mutex_lock(&current->lock);
        }
    }
    if (current != NULL && current->value == value) {
        if (prev != NULL) {
            prev->next = current->next;
            pthread_mutex_unlock(&prev->lock);
        }
        else {
            list->head = current->next;
        }
        pthread_mutex_unlock(&current->lock);
        pthread_mutex_destroy(&current->lock);
        free(current);
    }
    else if (current != NULL) {
        pthread_mutex_unlock(&current->lock);
    }
}

void print_list(list* list)
{
    if (list == NULL) {
        printf("\n");
        return;
    }
    struct node* current = list->head;
    while (current != NULL) {
        printf("%d ", current->value);
        current = current->next;
    }
    printf("\n"); // DO NOT DELETE
}

void count_list(list* list, int (*predicate)(int))
{
    if (list == NULL) return;
    int count = 0; // DO NOT DELETE
    struct node* current = list->head;

    if (current != NULL) {
        pthread_mutex_lock(&current->lock);
    }

    while (current != NULL) {
        if (predicate(current->value)) {
            count++;
        }
        struct node* next = current->next;
        if (next != NULL) {
            pthread_mutex_lock(&next->lock);
        }
        pthread_mutex_unlock(&current->lock);
        current = next;
    }
    printf("%d items were counted\n", count); // DO NOT DELETE
}
