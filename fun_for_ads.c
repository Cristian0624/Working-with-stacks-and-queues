#include "f_for_lab6.h"

// Helpers
void ads_search_acc(Node *head, uint32_t acc) {
    Node *cur = head;
    int found = 0, pos = 1;
    while (cur) {
        if (*bc_accnum(&cur->data) == acc) {
            printf("Found at position %d:\n", pos);
            print_customer_u(&cur->data, -1);
            found = 1;
        }
        cur = cur->next;
        pos++;
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

int list_count(Node *head) {
    int n = 0;
    for (Node *c = head; c; c = c->next) n++;
    return n;
}

void save_list(Node *head, const char *fn, int binary) {
    FILE *f = fopen(fn, binary ? "wb" : "w");
    if (!f) { printf("Cannot open file '%s'.\n", fn); return; }

    int n = list_count(head);

    if (binary) {
        fwrite(&n, sizeof(int), 1, f);
        for (Node *cur = head; cur; cur = cur->next)
            fwrite(cur->data.raw, 1, BC_SIZE, f);
    } else {
        fprintf(f, "%d\n", n);
        for (Node *cur = head; cur; cur = cur->next) {
            BankCustomer_u *p = &cur->data;
            fprintf(f, "%s|%s|%u|%f|%lf|%llu|%llu\n",
                    bc_name(p), bc_surname(p), *bc_accnum(p),
                    *bc_rate(p), *bc_money(p),
                    (unsigned long long)bc_opendate(p)->raw,
                    (unsigned long long)bc_accdate(p)->raw);
        }
    }

    fclose(f);
    printf("Saved %d customer(s) to '%s'.\n", n, fn);
}

// --- STACK FUNCTIONS ---
void stack_init(Stack *s) {
    s->top = NULL;
}
void stack_search(Stack *s, uint32_t acc) {
    ads_search_acc(s->top, acc);
}
void stack_free(Stack *s) {
    ads_free_list(&s->top);
}
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
    printf("\n=== STACK (top → bottom) ===\n");
    display_list_u(s->top);
}
void stack_save(Stack *s, const char *fn, int binary) {
    save_list(s->top, fn, binary);
}

// --- SIMPLE QUEUE IMPLEMENTATIONS ---
void sq_init(SimpleQueue *q) {
    q->front = q->rear = NULL;
}
void sq_search(SimpleQueue *q, uint32_t acc) {
    ads_search_acc(q->front, acc);
}
void sq_free(SimpleQueue *q) {
    ads_free_list(&q->front); q->rear = NULL;
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
void sq_display(SimpleQueue *q) {
    printf("\n=== SIMPLE QUEUE (front → rear) ===\n");
    display_list_u(q->front);
}
void sq_save(SimpleQueue *q, const char *fn, int binary) {
    save_list(q->front, fn, binary);
}

// --- DEQUE IMPLEMENTATIONS ---
void deq_init(Deque *d) {
    d->front = d->rear = NULL;
}
void deq_search(Deque *d, uint32_t acc) {
    ads_search_acc(d->front, acc);
}
void deq_free(Deque *d) {
    ads_free_list(&d->front); d->rear = NULL;
}
void deq_push_front(Deque *d, BankCustomer_u *c) {
    Node *n = malloc(sizeof(Node));
    n->data = *c;
    n->next = d->front;
    d->front = n;
    if (!d->rear) d->rear = n;
}

int deq_pop_front(Deque *d, BankCustomer_u *out) {
    if (!d->front) return 0;
    Node *t = d->front;
    *out = t->data;
    d->front = t->next;
    if (!d->front)
        d->rear = NULL;
    free(t); return 1;
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
int deq_pop_rear(Deque *d, BankCustomer_u *out) {
    if (!d->rear) return 0;
    Node *t = d->rear;
    *out = t->data;
    if (d->front == d->rear) {
        d->front = d->rear = NULL;
    }
    else {
        Node *prev = d->front;
        while (prev->next != d->rear) prev = prev->next;
        prev->next = NULL; d->rear = prev;
    }
    free(t); return 1;
}
void deq_display(Deque *d) {
    printf("\n=== DEQUE (front → rear) ===\n");
    display_list_u(d->front);
}
void deq_save(Deque *d, const char *fn, int binary) {
    save_list(d->front, fn, binary);
}

// --- CIRCULAR QUEUE IMPLEMENTATIONS ---
void cq_init(CircularQueue *q, int cap) {
    q->rear = NULL;
}
void cq_search(CircularQueue *q, uint32_t acc) {
    if (!q->rear) { printf("Circular queue is empty.\n"); return; }
    /* Build a temporary NULL-terminated traversal without modifying pointers */
    Node *front = q->rear->next;
    Node *cur   = front;
    int found   = 0, pos = 1;
    do {
        if (*bc_accnum(&cur->data) == acc) {
            printf("Found at position %d:\n", pos);
            print_customer_u(&cur->data, -1);
            found = 1;
        }
        cur = cur->next;
        pos++;
    } while (cur != front);
    if (!found) printf("Account %u not found.\n", acc);
}
void cq_display(CircularQueue *q) {
    printf("\n=== CIRCULAR QUEUE (front → rear) ===\n");
    if (!q->rear) { printf("Structure is empty.\n"); return; }
    Node *front = q->rear->next;
    Node *cur   = front;
    int   i     = 0;
    do {
        print_customer_u(&cur->data, i++);
        cur = cur->next;
    } while (cur != front);
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
int cq_dequeue(CircularQueue *q, BankCustomer_u *out) {
    if (!q->rear) return 0;
    Node *f = q->rear->next;
    *out = f->data;
    if (f == q->rear)
        q->rear = NULL;
    else
        q->rear->next = f->next;
    free(f); return 1;
}
void cq_free(CircularQueue *q) {
    if (!q->rear) return;
    Node *cur   = q->rear->next;
    q->rear->next = NULL;       // break circle so ads_free_list can walk it
    ads_free_list(&cur);
    q->rear = NULL;
}
void cq_save(CircularQueue *q, const char *fn, int binary) {
    if (!q->rear) {
        // Empty — write an empty file with count 0
        FILE *f = fopen(fn, binary ? "wb" : "w");
        if (f) { if (binary) { int z=0; fwrite(&z,sizeof(int),1,f); } else fprintf(f,"0\n"); fclose(f); }
        printf("Saved 0 customer(s) to '%s'.\n", fn);
        return;
    }
    // Temporarily break the circle so save_list sees a normal list
    Node *front      = q->rear->next;
    q->rear->next    = NULL;   // break circle
    save_list(front, fn, binary);
    q->rear->next    = front;  // restore circle
}

// --- PRIORITY QUEUE IMPLEMENTATIONS ---
void pq_init(PriorityQueue *pq) {
    pq->head = NULL;
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
void pq_search(PriorityQueue *pq, uint32_t acc) {
    ads_search_acc(pq->head, acc);
}
void pq_display(PriorityQueue *pq) {
    printf("\n=== PRIORITY QUEUE (highest deposit rate first) ===\n");
    display_list_u(pq->head);
}
void pq_free(PriorityQueue *pq) {
    ads_free_list(&pq->head);
}
void pq_save(PriorityQueue *pq, const char *fn, int binary) {
    save_list(pq->head, fn, binary);
}

 //  MENU HELPERS
void ads_action_menu(const char *ads_name) {
    printf("\n--- %s Actions ---\n", ads_name);
    printf("  1. Add / Push / Enqueue\n");
    printf("  2. Remove / Pop / Dequeue\n");
    printf("  3. Display\n");
    printf("  4. Search by account number\n");
    printf("  5. Save to file\n");
    printf("  0. Back\n");
    printf("Choice: ");
}

int get_save_mode(void) {
    printf("Save mode: (1) Text  (2) Binary: ");
    int m; scanf("%d", &m);
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return (m == 2) ? 1 : 0;
}

char *get_filename(char *buf, int len) {
    printf("Enter filename: ");
    scanf("%99s", buf);
    buf[len-1] = '\0';
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return buf;
}

uint32_t get_acc(void) {
    uint32_t acc;
    printf("Enter account number: ");
    scanf("%u", &acc);
    int c; while ((c = getchar()) != '\n' && c != EOF);
    return acc;
}

// Pick a customer index from the flat array, with bounds checking.
static int pick_index(BankCustomer_u **arr, int arr_size) {
    if (arr_size == 0) { printf("No customers in flat array. Add one first.\n"); return -1; }
    printf("Which customer? (1-%d): ", arr_size);
    int idx; scanf("%d", &idx);
    int c; while ((c = getchar()) != '\n' && c != EOF);
    if (idx < 1 || idx > arr_size) { printf("Invalid index.\n"); return -1; }
    return idx - 1;
}


 //  STACK MENU
static void menu_stack(Stack *s, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("STACK");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (ch) {
            case 1: {
                int idx = pick_index(arr, arr_size);
                if (idx >= 0) stack_push(s, &(*arr)[idx]);
                break;
            }
            case 2: {
                BankCustomer_u out;
                if (stack_pop(s, &out)) print_customer_u(&out, 0);
                else printf("Stack is empty.\n");
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
            default: printf("Invalid choice.\n");
        }
    } while (ch != 0);
}

 // SIMPLE QUEUE MENU
static void menu_simple_queue(SimpleQueue *q, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("SIMPLE QUEUE");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (ch) {
            case 1: {
                int idx = pick_index(arr, arr_size);
                if (idx >= 0) sq_enqueue(q, &(*arr)[idx]);
                break;
            }
            case 2: {
                BankCustomer_u out;
                if (sq_dequeue(q, &out)) print_customer_u(&out, 0);
                else printf("Queue is empty.\n");
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
            default: printf("Invalid choice.\n");
        }
    } while (ch != 0);
}

 // DEQUE MENU
static void menu_deque(Deque *d, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        printf("\n--- DEQUE Actions ---\n");
        printf("  1. Push to FRONT\n");
        printf("  2. Push to REAR\n");
        printf("  3. Pop from FRONT\n");
        printf("  4. Pop from REAR\n");
        printf("  5. Display\n");
        printf("  6. Search\n");
        printf("  7. Save to file\n");
        printf("  0. Back\nChoice: ");

        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        BankCustomer_u out;
        switch (ch) {
            case 1: {
                int idx = pick_index(arr, arr_size);
                if (idx >= 0) deq_push_front(d, &(*arr)[idx]);
                break;
            }
            case 2: {
                int idx = pick_index(arr, arr_size);
                if (idx >= 0) deq_push_rear(d, &(*arr)[idx]);
                break;
            }
            case 3:
                if (deq_pop_front(d, &out)) print_customer_u(&out, 0);
                else printf("Deque is empty.\n");
                break;
            case 4:
                if (deq_pop_rear(d, &out)) print_customer_u(&out, 0);
                else printf("Deque is empty.\n");
                break;
            case 5: deq_display(d); break;
            case 6: deq_search(d, get_acc()); break;
            case 7: {
                char fn[100]; get_filename(fn, sizeof(fn));
                deq_save(d, fn, get_save_mode());
                break;
            }
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 0);
}

 //  CIRCULAR QUEUE MENU

static void menu_circular(CircularQueue *q, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("CIRCULAR QUEUE");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (ch) {
            case 1: {
                int idx = pick_index(arr, arr_size);
                if (idx >= 0) cq_enqueue(q, &(*arr)[idx]);
                break;
            }
            case 2: {
                BankCustomer_u out;
                if (cq_dequeue(q, &out)) print_customer_u(&out, 0);
                else printf("Circular queue is empty.\n");
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
            default: printf("Invalid choice.\n");
        }
    } while (ch != 0);
}

 //  PRIORITY QUEUE MENU
static void menu_priority(PriorityQueue *pq, BankCustomer_u **arr, int arr_size) {
    int ch;
    do {
        ads_action_menu("PRIORITY QUEUE (max deposit rate)");
        if (scanf("%d", &ch) != 1) ch = -1;
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (ch) {
            case 1: {
                int idx = pick_index(arr, arr_size);
                if (idx >= 0) pq_insert(pq, &(*arr)[idx]);
                break;
            }
            case 2: {
                BankCustomer_u out;
                if (pq_remove(pq, &out)) {
                    printf("Removed (highest rate):\n");
                    print_customer_u(&out, 0);
                } else {
                    printf("Priority queue is empty.\n");
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
            default: printf("Invalid choice.\n");
        }
    } while (ch != 0);
}

