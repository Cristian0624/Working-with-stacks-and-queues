#include "fun_for_ads.c"

int main(void) {
    /* Flat array */
    BankCustomer_u *customers = NULL;
    int size = 0, capacity = 0;

    /* ADS structures */
    Stack         stk;   stack_init(&stk);
    SimpleQueue   sq;    sq_init(&sq);
    Deque         deq;   deq_init(&deq);
    CircularQueue cq;    cq_init(&cq, 4);  /* start capacity 4, auto-grows */
    PriorityQueue pq;    pq_init(&pq);

    int choice;
    do {
        print_main_menu();
        if (scanf("%d", &choice) != 1) { choice = -1; }
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (choice) {
            case 1:  add_bank_customer_u(&customers, &size, &capacity); break;
            case 2:  display_bank_customers_u(customers, size);         break;
            case 3:  modify_customer_u(&customers, &size);              break;
            case 4:  sort_menu_u(customers, size);                      break;
            case 5:  write_to_file_u(customers, size);                  break;
            case 6:  read_from_file_u(&customers, &size, &capacity);    break;
            case 7:  menu_stack      (&stk, &customers, size);          break;
            case 8:  menu_simple_queue(&sq, &customers, size);          break;
            case 9:  menu_deque      (&deq, &customers, size);          break;
            case 10: menu_circular   (&cq,  &customers, size);          break;
            case 11: menu_priority   (&pq,  &customers, size);          break;
            case 0:  printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    /* Cleanup */
    free(customers);
    stack_free(&stk);
    sq_free(&sq);
    deq_free(&deq);
    cq_free(&cq);
    pq_free(&pq);

    return 0;
}
