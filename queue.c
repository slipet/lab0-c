#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *iter, *safe;
    list_for_each_entry_safe (iter, safe, l, list)
        q_release_element(iter);
    free(l);
}

/* Using q_insert() in case of redefinition of simillar code in q_insert_head()
 * and q_insert_tail() */
static inline bool q_insert(struct list_head *head, char *s)
{
    /*Consider the coditions of head
     *1. head is NULL -> return false
     *2. head had been initialized
     */
    if (!head || !s)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;

    const size_t len = strlen(s) + 1; /*+1 for '\0'*/
    new_element->value = malloc(sizeof(char) * len);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    memcpy(new_element->value, s, len);
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    return q_insert(head, s);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert(head->prev, s);
}

element_t *q_remove(struct list_head *node, char *sp, size_t bufsize)
{
    if (!node || list_empty(node))
        return NULL;
    element_t *element_del = container_of(node, element_t, list);
    if (sp && bufsize) {
        size_t len = strlen(element_del->value);
        len = (len + 1 > bufsize) ? bufsize : len + 1;
        strncpy(sp, element_del->value, bufsize);
        sp[len - 1] = '\0';
    }
    list_del_init(node);
    return element_del;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove(head->next, sp, bufsize);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove(head->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    /* if the queue is not initialized the head is NULL */
    /* using list_empty() for first check queue is if empty.
     * can not use head == prev => may be containing only one element
     */
    if (!head)
        return 0;
    if (list_empty(head))
        return 0;

    int len = 0;
    struct list_head *iter;
    /*iterate the whole list to count the size of queue*/
    list_for_each (iter, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head **indirect = &head;
    struct list_head *back = head->prev;
    /* Indirect pointer start from head and go through whole list along with
     * next pointer. Back pointer start from the end of the list and walk
     * against indirect pointer. They will meet each other in the middle of the
     * list. Time comlexity is O(n/2)
     */
    while (*indirect != back && (*indirect)->next != back) {
        indirect = &(*indirect)->next;
        back = back->prev;
    }

    struct list_head *del = (*indirect)->next;
    list_del(del);
    q_release_element(container_of(del, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    /* Duplication of nodes only occurs if the number of nodes is greater than
     * two.*/
    // if (!head || list_empty(head) || list_is_singular(head))
    // return false;
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
