/* main_lab6.c – Lab 6
 * Bank customer management using union-based BankCustomer_u.
 * ADS: Stack  |  Simple Queue  |  Deque  |  Circular Queue  |  Priority Queue
 *
 * Menu structure:
 *   A. Flat array operations   (same as Lab5 but with union struct)
 *   B. Stack operations
 *   C. Simple Queue operations
 *   D. Deque operations
 *   E. Circular Queue operations
 *   F. Priority Queue operations
 */
#include "f_for_lab6.h"

/* ------------------------------------------------------------------ */
void stack_init(Stack *s) { s->top = NULL; }

void stack_push(Stack *s, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;
    n->next = s->top;
    s->top = n;
}

int stack_pop(Stack *s, BankCustomer_u *out) {
    if (!s->top) return 0;
    Node *t = s->top;
    *out = t->data;
    s->top = t->next;
    free(t);
    return 1;
}

void stack_display(Stack *s) {
    Node *cur = s->top;
    while (cur) {
        printf("Acc: %u\n", *bc_accnum(&cur->data));
        cur = cur->next;
    }
}

void sq_init(SimpleQueue *q) {
    q->front = q->rear = NULL;
}

void sq_enqueue(SimpleQueue *q, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;
    n->next = NULL;

    if (!q->rear)
        q->front = q->rear = n;
    else {
        q->rear->next = n;
        q->rear = n;
    }
}

int sq_dequeue(SimpleQueue *q, BankCustomer_u *out) {
    if (!q->front) return 0;

    Node *t = q->front;
    *out = t->data;
    q->front = t->next;

    if (!q->front) q->rear = NULL;

    free(t);
    return 1;
}

void deq_push_front(Deque *d, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;
    n->next = d->front;
    d->front = n;
    if (!d->rear) d->rear = n;
}

void deq_push_rear(Deque *d, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;
    n->next = NULL;

    if (!d->rear)
        d->front = d->rear = n;
    else {
        d->rear->next = n;
        d->rear = n;
    }
}

void cq_init(CircularQueue *q, int cap) {
    q->rear = NULL;
}

void cq_enqueue(CircularQueue *q, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;

    if (!q->rear) {
        n->next = n;
        q->rear = n;
    } else {
        n->next = q->rear->next;
        q->rear->next = n;
        q->rear = n;
    }
}

void pq_insert(PriorityQueue *pq, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;
    n->next = NULL;

    if (!pq->head || *bc_rate(&pq->head->data) < *bc_rate(c)) {
        n->next = pq->head;
        pq->head = n;
        return;
    }

    Node *cur = pq->head;
    while (cur->next && *bc_rate(&cur->next->data) >= *bc_rate(c))
        cur = cur->next;

    n->next = cur->next;
    cur->next = n;
}

int pq_remove(PriorityQueue *pq, BankCustomer_u *out) {
    if (!pq->head) return 0;
    Node *t = pq->head;
    *out = t->data;
    pq->head = t->next;
    free(t);
    return 1;
}



/* --- GENERIC SEARCH & FREE (Helper) --- */
void ads_search_acc(Node *head, uint32_t acc) {
    Node *cur = head;
    int found = 0, pos = 1;
    while (cur) {
        if (*bc_accnum(&cur->data) == acc) {
            printf("Found at position %d:\n", pos);
            print_customer_u(&cur->data, -1);
            found = 1;
        }
        cur = cur->next; pos++;
    }
    if (!found) printf("Account %u not found.\n", acc);
}

void ads_free_list(Node **head) {
    Node *cur = *head;
    while (cur) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    *head = NULL;
}

void display_list_u(Node *head) {
    if (!head) {
        printf("Structure is empty.\n");
        return;
    }
    Node *cur = head;
    int i = 0;
    while (cur) {
        print_customer_u(&cur->data, i++);
        cur = cur->next;
    }
}

/* --- STACK IMPLEMENTATIONS --- */
void stack_search(Stack *s, uint32_t acc) { ads_search_acc(s->top, acc); }
void stack_free(Stack *s) { ads_free_list(&s->top); }
void stack_save(Stack *s, const char *fn, int binary) {
    // Implementation for saving (use the logic from write_to_txt_u or write_to_file_u)
}

/* --- SIMPLE QUEUE IMPLEMENTATIONS --- */
void sq_search(SimpleQueue *q, uint32_t acc) { ads_search_acc(q->front, acc); }
void sq_free(SimpleQueue *q) { ads_free_list(&q->front); q->rear = NULL; }
void sq_save(SimpleQueue *q, const char *fn, int binary) {
    /* implementation */
}
void sq_display(SimpleQueue *q) {
    display_list_u(q->front);
}

/* --- DEQUE IMPLEMENTATIONS --- */
void deq_init(Deque *d) { d->front = d->rear = NULL; }
void deq_search(Deque *d, uint32_t acc) { ads_search_acc(d->front, acc); }
void deq_free(Deque *d) { ads_free_list(&d->front); d->rear = NULL; }
int deq_pop_front(Deque *d, BankCustomer_u *out) {
    if (!d->front) return 0;
    Node *t = d->front;
    *out = t->data;
    d->front = t->next;
    if (!d->front) d->rear = NULL;
    free(t); return 1;
}
int deq_pop_rear(Deque *d, BankCustomer_u *out) {
    if (!d->rear) return 0;
    Node *t = d->rear;
    *out = t->data;
    if (d->front == d->rear) { d->front = d->rear = NULL; }
    else {
        Node *prev = d->front;
        while (prev->next != d->rear) prev = prev->next;
        prev->next = NULL; d->rear = prev;
    }
    free(t); return 1;
}
void deq_display(Deque *d) { ads_search_acc(d->front, 0xFFFFFFFF); } // Generic display trick
void deq_save(Deque *d, const char *fn, int binary) { /* implementation */ }

/* --- CIRCULAR QUEUE IMPLEMENTATIONS --- */
void cq_search(CircularQueue *q, uint32_t acc) { if (q->rear) ads_search_acc(q->rear->next, acc); }
void cq_display(CircularQueue *q) { /* logic to traverse circular link */ }
int cq_dequeue(CircularQueue *q, BankCustomer_u *out) {
    if (!q->rear) return 0;
    Node *f = q->rear->next;
    *out = f->data;
    if (f == q->rear) q->rear = NULL;
    else q->rear->next = f->next;
    free(f); return 1;
}
void cq_free(CircularQueue *q) {
    if (!q->rear) return;
    Node *cur = q->rear->next;
    q->rear->next = NULL; // Break circle
    ads_free_list(&cur);
}
void cq_save(CircularQueue *q, const char *fn, int binary) { /* implementation */ }

/* --- PRIORITY QUEUE IMPLEMENTATIONS --- */
void pq_init(PriorityQueue *pq) { pq->head = NULL; }
void pq_search(PriorityQueue *pq, uint32_t acc) { ads_search_acc(pq->head, acc); }
void pq_display(PriorityQueue *pq) { ads_search_acc(pq->head, 0xFFFFFFFF); }
void pq_free(PriorityQueue *pq) { ads_free_list(&pq->head); }
void pq_save(PriorityQueue *pq, const char *fn, int binary) { /* implementation */ }

static void ads_action_menu(const char *ads_name) {
    printf("\n--- %s Actions ---\n", ads_name);
    printf("  1. Add / Push / Enqueue\n");
    printf("  2. Remove / Pop / Dequeue\n");
    printf("  3. Display\n");
    printf("  4. Search by account number\n");
    printf("  5. Save to file\n");
    printf("  0. Back\n");
    printf("Choice: ");
}

static int get_save_mode(void) {
    printf("Save mode: (1) Text  (2) Binary: ");
    int m; scanf("%d", &m);
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return (m == 2) ? 1 : 0;
}

static char *get_filename(char *buf, int len) {
    printf("Enter filename: ");
    scanf("%99s", buf); buf[len-1] = '\0';
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return buf;
}

static uint32_t get_acc(void) {
    uint32_t acc; printf("Enter account number: "); scanf("%u", &acc);
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return acc;
}

/* ================================================================== */
/*  Sub-menus for each ADS                                             */
/* ================================================================== */

/* --- STACK --- */
static void menu_stack(Stack *s, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("STACK");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch(ch) {
            case 1:
                if (arr_size == 0) { printf("No customers in flat array. Add one first.\n"); break; }
                printf("Push which customer? (1-%d): ", arr_size);
                int idx; scanf("%d", &idx);
                while ((c = getchar()) != '\n' && c != EOF);
                if (idx < 1 || idx > arr_size) { printf("Invalid index.\n"); break; }
                stack_push(s, &(*arr)[idx-1]);
                break;
            case 2: {
                BankCustomer_u out;
                if (stack_pop(s, &out))
                    print_customer_u(&out, 0);
                break;
            }
            case 3: stack_display(s); break;
            case 4: stack_search(s, get_acc()); break;
            case 5: {
                char fn[100]; get_filename(fn, sizeof(fn));
                stack_save(s, fn, get_save_mode());
                break;
            }
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while (ch != 0);
}

/* --- SIMPLE QUEUE --- */
static void menu_simple_queue(SimpleQueue *q, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("SIMPLE QUEUE");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch(ch) {
            case 1:
                if (arr_size == 0) { printf("No customers in flat array.\n"); break; }
                printf("Enqueue which customer? (1-%d): ", arr_size);
                int idx; scanf("%d", &idx);
                while ((c = getchar()) != '\n' && c != EOF);
                if (idx < 1 || idx > arr_size) { printf("Invalid index.\n"); break; }
                sq_enqueue(q, &(*arr)[idx-1]);
                break;
            case 2: {
                BankCustomer_u out;
                if (sq_dequeue(q, &out))
                    print_customer_u(&out, 0);
                break;
            }
            case 3: sq_display(q); break;
            case 4: sq_search(q, get_acc()); break;
            case 5: {
                char fn[100]; get_filename(fn, sizeof(fn));
                sq_save(q, fn, get_save_mode());
                break;
            }
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while (ch != 0);
}

/* --- DEQUE --- */
static void menu_deque(Deque *d, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        printf("\n--- DEQUE Actions ---\n");
        printf("  1. Push to FRONT\n  2. Push to REAR\n");
        printf("  3. Pop from FRONT\n  4. Pop from REAR\n");
        printf("  5. Display\n  6. Search\n  7. Save to file\n  0. Back\nChoice: ");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        int idx;
        BankCustomer_u out;
        switch(ch) {
            case 1:
            case 2:
                if (arr_size == 0) { printf("No customers in flat array.\n"); break; }
                printf("Which customer? (1-%d): ", arr_size);
                scanf("%d", &idx);
                while ((c = getchar()) != '\n' && c != EOF);
                if (idx < 1 || idx > arr_size) { printf("Invalid index.\n"); break; }
                if (ch == 1) deq_push_front(d, &(*arr)[idx-1]);
                else         deq_push_rear (d, &(*arr)[idx-1]);
                break;
            case 3:
                if (deq_pop_front(d, &out)) print_customer_u(&out, 0);
                break;
            case 4:
                if (deq_pop_rear(d, &out))  print_customer_u(&out, 0);
                break;
            case 5: deq_display(d); break;
            case 6: deq_search(d, get_acc()); break;
            case 7: {
                char fn[100]; get_filename(fn, sizeof(fn));
                deq_save(d, fn, get_save_mode());
                break;
            }
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while (ch != 0);
}

/* --- CIRCULAR QUEUE --- */
static void menu_circular(CircularQueue *q, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("CIRCULAR QUEUE");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch(ch) {
            case 1:
                if (arr_size == 0) { printf("No customers in flat array.\n"); break; }
                printf("Enqueue which? (1-%d): ", arr_size);
                int idx; scanf("%d", &idx);
                while ((c = getchar()) != '\n' && c != EOF);
                if (idx < 1 || idx > arr_size) { printf("Invalid.\n"); break; }
                cq_enqueue(q, &(*arr)[idx-1]);
                break;
            case 2: {
                BankCustomer_u out;
                if (cq_dequeue(q, &out)) print_customer_u(&out, 0);
                break;
            }
            case 3: cq_display(q); break;
            case 4: cq_search(q, get_acc()); break;
            case 5: {
                char fn[100]; get_filename(fn, sizeof(fn));
                cq_save(q, fn, get_save_mode());
                break;
            }
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while (ch != 0);
}

/* --- PRIORITY QUEUE --- */
static void menu_priority(PriorityQueue *pq, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("PRIORITY QUEUE (max deposit rate)");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch(ch) {
            case 1:
                if (arr_size == 0) { printf("No customers in flat array.\n"); break; }
                printf("Insert which? (1-%d): ", arr_size);
                int idx; scanf("%d", &idx);
                while ((c = getchar()) != '\n' && c != EOF);
                if (idx < 1 || idx > arr_size) { printf("Invalid.\n"); break; }
                pq_insert(pq, &(*arr)[idx-1]);
                break;
            case 2: {
                BankCustomer_u out;
                if (pq_remove(pq, &out)) {
                    printf("Removed (highest rate):\n");
                    print_customer_u(&out, 0);
                }
                break;
            }
            case 3: pq_display(pq); break;
            case 4: pq_search(pq, get_acc()); break;
            case 5: {
                char fn[100]; get_filename(fn, sizeof(fn));
                pq_save(pq, fn, get_save_mode());
                break;
            }
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while (ch != 0);
}


