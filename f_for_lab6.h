#ifndef UNTITLED_F_FOR_LAB6_H
#define UNTITLED_F_FOR_LAB6_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

typedef union {
    uint64_t raw; // raw block of memory for the date-time
} DateTime_u;

// Bit positions and masks for DateTime_u
#define DT_DAY_SHIFT 0
#define DT_MONTH_SHIFT 5
#define DT_YEAR_SHIFT 9
#define DT_HOUR_SHIFT 21
#define DT_MIN_SHIFT 26

// Masks for extracting values
#define DT_DAY_MASK 0x1FULL // = 0x1F  = 0001 1111  → 5 bits  → max value 31
#define DT_MONTH_MASK 0x0FULL // = 0x0F  = 0000 1111  → 4 bits  → max value 12
#define DT_YEAR_MASK 0xFFFULL // = 0xFFF = 4095 = 0000 1111 1111 1111 → 12 bits → max value 4095
#define DT_HOUR_MASK 0x1FULL // = 0x1F  = 0001 1111  → 5 bits  → max value 23
#define DT_MIN_MASK 0x3FULL // = 0x3F  = 0011 1111  → 6 bits  → max value 59

#define OFF_NAME 0
#define OFF_SURNAME 50
#define OFF_ACCNUM 100
#define OFF_RATE 104
#define OFF_MONEY 108
#define OFF_OPENDATE 116
#define OFF_ACCDATE 124
#define BC_SIZE 132

static void dt_set_day(DateTime_u *d, uint64_t v) {
    // clear:  punch a 12-bit hole at year's position  | write:  place new value into that hole
    d->raw = (d->raw & ~(DT_DAY_MASK << DT_DAY_SHIFT)) | ((v & DT_DAY_MASK) << DT_DAY_SHIFT);
}
static void dt_set_month(DateTime_u *d, uint64_t v) {
    d->raw = (d->raw & ~(DT_MONTH_MASK << DT_MONTH_SHIFT)) | ((v & DT_MONTH_MASK) << DT_MONTH_SHIFT);
}
static void dt_set_year(DateTime_u *d, uint64_t v) {
    d->raw = (d->raw & ~(DT_YEAR_MASK  << DT_YEAR_SHIFT))  | ((v & DT_YEAR_MASK)  << DT_YEAR_SHIFT);
}
static void dt_set_hour(DateTime_u *d, uint64_t v) {
    d->raw = (d->raw & ~(DT_HOUR_MASK  << DT_HOUR_SHIFT))  | ((v & DT_HOUR_MASK)  << DT_HOUR_SHIFT);
}
static void dt_set_min(DateTime_u *d, uint64_t v) {
    d->raw = (d->raw & ~(DT_MIN_MASK   << DT_MIN_SHIFT))   | ((v & DT_MIN_MASK)   << DT_MIN_SHIFT);
}

static uint64_t dt_get_day(const DateTime_u *d) {
    return (d->raw >> DT_DAY_SHIFT)   & DT_DAY_MASK;
}
static uint64_t dt_get_month(const DateTime_u *d) {
    return (d->raw >> DT_MONTH_SHIFT) & DT_MONTH_MASK;
}
static uint64_t dt_get_year(const DateTime_u *d) {
    return (d->raw >> DT_YEAR_SHIFT)  & DT_YEAR_MASK;
}
static uint64_t dt_get_hour(const DateTime_u *d) {
    return (d->raw >> DT_HOUR_SHIFT)  & DT_HOUR_MASK;
}
static uint64_t dt_get_min(const DateTime_u *d) {
    return (d->raw >> DT_MIN_SHIFT)   & DT_MIN_MASK;
}

typedef union {
    unsigned char raw[BC_SIZE];
} BankCustomer_u;

static char *bc_name(BankCustomer_u *p) {
    return (char*)(p->raw + OFF_NAME);
}
static char *bc_surname(BankCustomer_u *p) {
    return (char*)(p->raw + OFF_SURNAME);
}
static uint32_t *bc_accnum(BankCustomer_u *p) {
    return (uint32_t*)(p->raw + OFF_ACCNUM);
}
static float *bc_rate(BankCustomer_u *p) {
    return (float*)(p->raw + OFF_RATE);
}
static double *bc_money(BankCustomer_u *p) {
    return (double*)(p->raw + OFF_MONEY);
}
static DateTime_u *bc_opendate(BankCustomer_u *p) {
    return (DateTime_u*)(p->raw + OFF_OPENDATE);
}
static DateTime_u *bc_accdate(BankCustomer_u *p) {
    return (DateTime_u*)(p->raw + OFF_ACCDATE);
}

void ensure_memory_u(BankCustomer_u **c, int *size, int *capacity);
void clear_buffer_u();
void read_line_u(char *s, int size);
void get_current_date_u(DateTime_u *d);
void add_bank_customer_u(BankCustomer_u **c, int *size, int *capacity);
void display_bank_customers_u(BankCustomer_u *c, int size);
int  compare_u(BankCustomer_u *a, BankCustomer_u *b, int criterion);
void swap_customers_u(BankCustomer_u *a, BankCustomer_u *b);
int  partition_u(BankCustomer_u *c, int low, int high, int criterion);
void quick_sort_u(BankCustomer_u *c, int low, int high, int criterion);
void counting_sort_name_u(BankCustomer_u *c, int size);
void counting_sort_date_desc_u(BankCustomer_u *c, int size);
void modify_customer_u(BankCustomer_u **c, int *size);
void sort_menu_u(BankCustomer_u *c, int size);
void write_to_file_u(BankCustomer_u *c, int size);
void read_from_file_u(BankCustomer_u **c, int *size, int *capacity);

/* ===================== LIST NODE ===================== */
typedef struct Node {
    BankCustomer_u data;
    struct Node *next;
} Node;

/* ===================== STACK ===================== */
typedef struct {
    Node *top;
} Stack;

void stack_init(Stack *s);
void stack_push(Stack *s, BankCustomer_u *c);
int stack_pop(Stack *s, BankCustomer_u *out);
void stack_display(Stack *s);
void stack_search(Stack *s, uint32_t acc);
void stack_save(Stack *s, const char *fn, int binary);
void stack_free(Stack *s);

/* ===================== SIMPLE QUEUE ===================== */
typedef struct {
    Node *front, *rear;
} SimpleQueue;

void sq_init(SimpleQueue *q);
void sq_enqueue(SimpleQueue *q, BankCustomer_u *c);
int sq_dequeue(SimpleQueue *q, BankCustomer_u *out);
void sq_display(SimpleQueue *q);
void sq_search(SimpleQueue *q, uint32_t acc);
void sq_save(SimpleQueue *q, const char *fn, int binary);
void sq_free(SimpleQueue *q);

/* ===================== DEQUE ===================== */
typedef struct {
    Node *front, *rear;
} Deque;

void deq_init(Deque *d);
void deq_push_front(Deque *d, BankCustomer_u *c);
void deq_push_rear(Deque *d, BankCustomer_u *c);
int deq_pop_front(Deque *d, BankCustomer_u *out);
int deq_pop_rear(Deque *d, BankCustomer_u *out);
void deq_display(Deque *d);
void deq_search(Deque *d, uint32_t acc);
void deq_save(Deque *d, const char *fn, int binary);
void deq_free(Deque *d);

/* ===================== CIRCULAR QUEUE ===================== */
typedef struct {
    Node *rear;
} CircularQueue;

void cq_init(CircularQueue *q, int cap);
void cq_enqueue(CircularQueue *q, BankCustomer_u *c);
int  cq_dequeue(CircularQueue *q, BankCustomer_u *out);
void cq_display(CircularQueue *q);
void cq_search(CircularQueue *q, uint32_t acc);
void cq_save(CircularQueue *q, const char *fn, int binary);
void cq_free(CircularQueue *q);

/* ===================== PRIORITY QUEUE ===================== */
typedef struct {
    Node *head;
} PriorityQueue;

void pq_init(PriorityQueue *pq);
void pq_insert(PriorityQueue *pq, BankCustomer_u *c);
int pq_remove(PriorityQueue *pq, BankCustomer_u *out);
void pq_display(PriorityQueue *pq);
void pq_search(PriorityQueue *pq, uint32_t acc);
void pq_save(PriorityQueue *pq, const char *fn, int binary);
void pq_free(PriorityQueue *pq);

#endif //UNTITLED_F_FOR_LAB6_H