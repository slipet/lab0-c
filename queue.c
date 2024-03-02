#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*Compare two values and return the smaller one.*/
#define MIN(a, b)          \
    ({                     \
        typeof(a) _a = a;  \
        typeof(b) _b = b;  \
        _a < _b ? _a : _b; \
    })

#define q_monotonic(head, direction)                                     \
    ({                                                                   \
        if (!head || list_empty(head) || list_is_singular(head))         \
            return q_size(head);                                         \
        element_t *entry = list_entry(head->direction, element_t, list); \
        element_t *safe =                                                \
            list_entry((entry->list).direction, element_t, list);        \
        char *mx = entry->value;                                         \
        while (&entry->list != head) {                                   \
            if (compare(mx, entry->value)) {                             \
                list_del(&entry->list);                                  \
                q_release_element(entry);                                \
            } else {                                                     \
                mx = entry->value;                                       \
            }                                                            \
            entry = safe;                                                \
            safe = list_entry((entry->list).direction, element_t, list); \
        }                                                                \
        q_size(head);                                                    \
    })

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    // Check availability of pointer
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

static inline bool q_insert(struct list_head *head, char *s)
{
    if (head == NULL || s == NULL)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (new_element == NULL)
        return false;
    new_element->value = strdup(s);
    if (new_element->value == NULL) {
        free(new_element);
        return false;
    }
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

static element_t *q_remove(struct list_head *node, char *sp, size_t bufsize)
{
    if (node == NULL || list_empty(node))
        return NULL;
    element_t *del = container_of(node, element_t, list);
    if (sp != NULL && bufsize > 0) {
        size_t len =
            MIN(strlen(del->value),
                bufsize - 1); /* bufsize reserve space for a null terminator*/
        strncpy(sp, del->value, len);
        sp[len] = '\0';
    }
    list_del_init(node);
    return del;
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
    if (head == NULL || list_empty(head))
        return 0;
    size_t len = 0;
    struct list_head *iter;
    list_for_each (iter, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;
    struct list_head **iter = &head;
    struct list_head *tail = head->prev;

    while ((*iter) != tail && (*iter)->next != tail) {
        iter = &(*iter)->next;
        tail = tail->prev;
    }
    struct list_head *del = (*iter)->next;
    list_del(del);
    q_release_element(container_of(del, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return false;
    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head && strcmp(entry->value, safe->value) == 0) {
            element_t *del;
            while (&safe->list != head &&
                   strcmp(entry->value, safe->value) == 0) {
                del = safe;
                safe = list_entry(safe->list.next, element_t, list);
                list_del_init(&(del->list));
                q_release_element(del);
            }
            list_del_init(&entry->list);
            q_release_element(entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *iter = NULL, *next = NULL;
    list_for_each_safe (iter, next, head) {
        list_move(iter, head);
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if(head == NULL || list_empty(head) )
        return;
    struct list_head *safe, *end, *start = head;
    int count = 0;
    list_for_each_safe (end, safe, head) {
        count++;
        if(count == k){
            LIST_HEAD(tmp);
            list_cut_position(&tmp, start, end);
            q_reverse(&tmp);
            list_splice_init(&tmp, start);
            start = safe->prev;
            count = 0;
        }
    }
}
static inline bool compare(char *left, char *right)
{
    /* return 1 -> left>right
     *        0 -> left<=right
     */
    return strcmp(left, right) > 0;
}

static inline void q_merge_two(struct list_head *head,
                               struct list_head *left,
                               struct list_head *right,
                               bool descend)
{
    /* descend = 0 for ascending
     *           1 for descending
     */

    while (!list_empty(left) && !list_empty(right)) {
        if ((compare(list_entry(left->next, element_t, list)->value,
                     list_entry(right->next, element_t, list)->value) &
             descend) ||
            (compare(list_entry(right->next, element_t, list)->value,
                     list_entry(left->next, element_t, list)->value) &
             (!descend))) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }
    }
    if (!list_empty(left)) {
        list_splice_tail_init(left, head);
    } else {
        list_splice_tail_init(right, head);
    }
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head **indirect = &head;
    struct list_head *back = head->prev;
    /* Find the middle of the list. */
    while (*indirect != back && (*indirect)->next != back) {
        indirect = &(*indirect)->next;
        back = back->prev;
    }
    LIST_HEAD(left);
    LIST_HEAD(right);
    list_splice_tail_init(head, &right);
    list_cut_position(&left, &right, back);
    q_sort(&left, descend);
    q_sort(&right, descend);
    q_merge_two(head, &left, &right, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    return q_monotonic(head, next);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    return q_monotonic(head, prev);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (head == NULL || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head, queue_contex_t, chain)->size;

    int queue_size = q_size(head);
    struct list_head *forward, *backward = head->prev;
    while (queue_size > 1) {
        forward = head->next;
        for (int i = 0; i < queue_size / 2; ++i) {
            LIST_HEAD(tmp);
            q_merge_two(&tmp, list_entry(forward, queue_contex_t, chain)->q,
                        list_entry(backward, queue_contex_t, chain)->q,
                        descend);
            list_splice_tail(&tmp,
                             list_entry(forward, queue_contex_t, chain)->q);
            forward = forward->next;
            backward = backward->prev;
        }
        queue_size = (queue_size + 1) / 2;
    }
    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}
