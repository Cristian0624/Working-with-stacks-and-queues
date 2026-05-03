#include "f_for_lab6.h"
// Ensure capacity for new customer
void ensure_memory_u(BankCustomer_u **c, int *size, int *capacity) {
    if (*size < *capacity) return;
    *capacity = (*capacity == 0) ? 2 : *capacity * 2;
    BankCustomer_u *t = realloc(*c, *capacity * sizeof(BankCustomer_u));
    if (!t) {
        printf("No more memory\n");
        exit(1);
    }
    *c = t;
}
// Clear input buffer after scanf
void clear_buffer_u() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Read a line of input safely
void read_line_u(char *s, int size) {
    fgets(s, size, stdin);
    s[strcspn(s, "\n")] = '\0';
}
// Get current date and time into DateTime_u using bit manipulation
void get_current_date_u(DateTime_u *d) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    d->raw = 0;
    dt_set_day  (d, (uint64_t)tm->tm_mday);
    dt_set_month(d, (uint64_t)(tm->tm_mon + 1));
    dt_set_year (d, (uint64_t)(tm->tm_year + 1900));
    dt_set_hour (d, (uint64_t)tm->tm_hour);
    dt_set_min  (d, (uint64_t)tm->tm_min);
}

// 2) Implement print_customer_u function
void print_customer_u(BankCustomer_u *p, int index) {
    if (index >= 0) printf("\nCustomer %d\n", index + 1);
    printf("Name: %s %s\n", bc_name(p), bc_surname(p));
    printf("Account: %u | Rate: %.2f%% | Balance: %.2lf\n",
           *bc_accnum(p), (double)*bc_rate(p), *bc_money(p));
    printf("Opened: %02u/%02u/%u\n",
           (unsigned)dt_get_day(bc_opendate(p)),
           (unsigned)dt_get_month(bc_opendate(p)),
           (unsigned)dt_get_year(bc_opendate(p)));
}

// 1) Logic for duration calculation and sorting
void calculate_duration_u(DateTime_u opening_date, int *years, int *months, int *days) {
    DateTime_u today;
    get_current_date_u(&today);

    *years  = (int)dt_get_year(&today) - (int)dt_get_year(&opening_date);
    *months = (int)dt_get_month(&today) - (int)dt_get_month(&opening_date);
    *days   = (int)dt_get_day(&today) - (int)dt_get_day(&opening_date);

    if (*days < 0) { (*months)--; *days += 30; }
    if (*months < 0) { (*years)--; *months += 12; }
}

void reverse_customers_u(BankCustomer_u *c, int size) {
    for (int i = 0; i < size / 2; i++) {
        swap_customers_u(&c[i], &c[size - 1 - i]);
    }
}

void radix_sort_duration_u(BankCustomer_u *customers, int size) {
    if (size <= 1) return;
    int *keys = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        int y, m, d;
        calculate_duration_u(*bc_opendate(&customers[i]), &y, &m, &d);
        keys[i] = y * 365 + m * 30 + d;
    }

    // Finding max for Radix logic
    int max_key = keys[0];
    for (int i = 1; i < size; i++) if (keys[i] > max_key) max_key = keys[i];

    // Using existing counting_sort_by_digit logic adapted for Union types
    for (int exp = 1; max_key / exp > 0; exp *= 10) {
        BankCustomer_u *output = malloc(size * sizeof(BankCustomer_u));
        int count[10] = {0};
        for (int i = 0; i < size; i++) count[(keys[i] / exp) % 10]++;
        for (int i = 1; i < 10; i++) count[i] += count[i-1];
        for (int i = size - 1; i >= 0; i--) {
            int digit = (keys[i] / exp) % 10;
            output[count[digit] - 1] = customers[i];
            count[digit]--;
        }
        memcpy(customers, output, size * sizeof(BankCustomer_u));
        free(output);
        // Recalculate keys after each pass to maintain alignment
        for (int i = 0; i < size; i++) {
            int y, m, d;
            calculate_duration_u(*bc_opendate(&customers[i]), &y, &m, &d);
            keys[i] = y * 365 + m * 30 + d;
        }
    }
    reverse_customers_u(customers, size);
    free(keys);
}

void write_output_sorted_u(BankCustomer_u *customers, int size) {
    if (size == 0) {
        printf("\nNo customers to sort!\n");
        return;
    }

    // Create a temporary copy to avoid messing up the main array order
    BankCustomer_u *sorted = malloc(size * sizeof(BankCustomer_u));
    memcpy(sorted, customers, size * sizeof(BankCustomer_u));

    radix_sort_duration_u(sorted, size);

    FILE *fp = fopen("oldest_customers.txt", "w");
    if (!fp) {
        printf("Error opening file!\n");
        free(sorted);
        return;
    }

    fprintf(fp, "=== Customer Duration (Descending - Oldest First) ===\n\n");
    for (int i = 0; i < size; i++) {
        int y, m, d;
        calculate_duration_u(*bc_opendate(&sorted[i]), &y, &m, &d);
        fprintf(fp, "Account #%u - %s %s: %d years, %d months, %d days\n",
                *bc_accnum(&sorted[i]),
                bc_name(&sorted[i]),
                bc_surname(&sorted[i]),
                y, m, d);
    }

    fclose(fp);
    free(sorted);
    printf("Sorted durations saved to oldest_customers.txt\n");
}

void add_bank_customer_u(BankCustomer_u **c, int *size, int *capacity) {
    ensure_memory_u(c, size, capacity);
    BankCustomer_u *p = &(*c)[*size];
    memset(p->raw, 0, BC_SIZE);

    printf("Enter name: ");
    read_line_u(bc_name(p), 50);

    printf("Enter surname: ");
    read_line_u(bc_surname(p), 50);

    uint32_t new_acc;
    printf("Enter account number: ");
    scanf("%u", &new_acc);

    for (int i = 0; i < *size; i++) {
        if (*bc_accnum(&(*c)[i]) == new_acc) {
            printf("Account exists. Oldest record preserved.\n");
            return; // Exit without overwriting or adding duplicate
        }
    }
    *bc_accnum(p) = new_acc;

    printf("Enter deposit rate: ");
    scanf("%f", bc_rate(p));

    char choice;
    printf("Use today as opening date? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
        get_current_date_u(bc_opendate(p)); // Set opening date to current date
        printf("Opening date set: %02u/%02u/%u %02u:%02u\n",
               (unsigned)dt_get_day(bc_opendate(p)),
               (unsigned)dt_get_month(bc_opendate(p)),
               (unsigned)dt_get_year(bc_opendate(p)),
               (unsigned)dt_get_hour(bc_opendate(p)),
               (unsigned)dt_get_min(bc_opendate(p)));
    } else {
        bc_opendate(p)->raw = 0;
        unsigned dd, mm, yy;
        printf("Enter date (DD/MM/YYYY): ");
        get_current_date_u(bc_opendate(p));
        scanf("%u/%u/%u", &dd, &mm, &yy);
        dt_set_day(bc_opendate(p), dd);
        dt_set_month(bc_opendate(p), mm);
        dt_set_year(bc_opendate(p), yy);

    }

    printf("Enter deposit money: ");
    scanf("%lf", bc_money(p));

    get_current_date_u(bc_accdate(p));
    clear_buffer_u();
    (*size)++;
    printf("Customer added!\n");
}

void display_bank_customers_u(BankCustomer_u *c, int size) {
    if (size == 0) {
        printf("\nNo customers!\n");
        return;
    }
    printf("\n================ CUSTOMERS LIST ==================\n");
    for (int i = 0; i < size; i++) {
        BankCustomer_u *p = &c[i];
        printf("\nCustomer %d\n", i + 1);
        printf("Name: %s\n", bc_name(p));
        printf("Surname: %s\n", bc_surname(p));
        printf("Account number: %u\n", *bc_accnum(p));
        printf("Deposit rate: %.2f%%\n",(double)*bc_rate(p));
        printf("Deposit money: %.2lf\n",*bc_money(p));
        printf("Opening date: %02u/%02u/%u %02u:%02u\n",
                (unsigned)dt_get_day(bc_opendate(p)),
                (unsigned)dt_get_month(bc_opendate(p)),
                (unsigned)dt_get_year(bc_opendate(p)),
                (unsigned)dt_get_hour(bc_opendate(p)),
                (unsigned)dt_get_min(bc_opendate(p)));
        printf("Last access:  %02u/%02u/%u %02u:%02u\n",
                (unsigned)dt_get_day(bc_accdate(p)),
                (unsigned)dt_get_month(bc_accdate(p)),
                (unsigned)dt_get_year(bc_accdate(p)),
                (unsigned)dt_get_hour(bc_accdate(p)),
                (unsigned)dt_get_min(bc_accdate(p)));
        printf("\n----------------------------------------------\n");
    }
    printf("===============================================\n\n");
}
// Compare two customers based on criterion: 1 for name, 2 for last access date
int compare_u(BankCustomer_u *a, BankCustomer_u *b, int criterion) {
    if (criterion == 1)
        return strcmp(bc_name(a), bc_name(b));

    DateTime_u *da = bc_accdate(a);
    DateTime_u *db = bc_accdate(b);
    if (dt_get_year(da) != dt_get_year(db))
        return (int)dt_get_year(da) - (int)dt_get_year(db);
    if (dt_get_month(da) != dt_get_month(db))
        return (int)dt_get_month(da) - (int)dt_get_month(db);
    if (dt_get_day(da) != dt_get_day(db))
        return (int)dt_get_day(da) - (int)dt_get_day(db);
    if (dt_get_hour(da) != dt_get_hour(db))
        return (int)dt_get_hour(da) - (int)dt_get_hour(db);
    return (int)dt_get_min(da) - (int)dt_get_min(db);
}
// Swap two customers by copying their raw data
void swap_customers_u(BankCustomer_u *a, BankCustomer_u *b) {
    unsigned char tmp[BC_SIZE];
    memcpy(tmp, a->raw, BC_SIZE);
    memcpy(a->raw, b->raw, BC_SIZE);
    memcpy(b->raw, tmp, BC_SIZE);
}

int partition_u(BankCustomer_u *c, int low, int high, int criterion) {
    BankCustomer_u *pivot = &c[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (compare_u(&c[j], pivot, criterion) < 0) {
            i++;
            swap_customers_u(&c[i], &c[j]);
        }
    }
    swap_customers_u(&c[i+1], &c[high]);
    return i + 1;
}

void quick_sort_u(BankCustomer_u *c, int low, int high, int criterion) {
    if (low < high) {
        int pi = partition_u(c, low, high, criterion);
        quick_sort_u(c, low, pi-1, criterion);
        quick_sort_u(c, pi+1, high, criterion);
    }
}

void counting_sort_name_u(BankCustomer_u *c, int size) {
    if (size <= 1)
        return;
    int max_len = 0;
    for (int i = 0; i < size; i++) {
        int l = (int)strlen(bc_name(&c[i]));
        if (l > max_len) max_len = l;
    }
    BankCustomer_u *out = malloc(size * sizeof(BankCustomer_u));
    if (!out) {
        printf("No more memory\n");
        exit(1);
    }

    for (int pos = max_len - 1; pos >= 0; pos--) {
        int count[257] = {0};
        for (int i = 0; i < size; i++) {
            char *nm = bc_name(&c[i]);
            unsigned char ch = (pos < (int)strlen(nm)) ? (unsigned char)nm[pos] : 0;
            count[ch + 1]++;
        }
        for (int i = 255; i >= 0; i--) count[i] += count[i+1];
        for (int i = size-1; i >= 0; i--) {
            char *nm = bc_name(&c[i]);
            unsigned char ch = (pos < (int)strlen(nm)) ? (unsigned char)nm[pos] : 0;
            out[count[ch + 1] - 1] = c[i];
            count[ch + 1]--;
        }
        memcpy(c, out, size * sizeof(BankCustomer_u));
    }
    free(out);
}

void counting_sort_date_desc_u(BankCustomer_u *c, int size) {
    if (size <= 1)
        return;
    long long *keys = malloc(size * sizeof(long long));
    if (!keys) {
        printf("No more memory\n"); exit(1);
    }
                                //DDMMYYYYHHMM
    long long max_k = -1, min_k = 999999999999LL;
    for (int i = 0; i < size; i++) {
        DateTime_u *d = bc_accdate(&c[i]);
        keys[i] = (long long)dt_get_year(d) * 100000000LL
                + (long long)dt_get_month(d) * 1000000LL
                + (long long)dt_get_day(d) * 10000LL
                + (long long)dt_get_hour(d) * 100LL
                + (long long)dt_get_min(d);
        if (keys[i] > max_k) max_k = keys[i];
        if (keys[i] < min_k) min_k = keys[i];
    }

    long long range_ll = max_k - min_k + 1;
    if (range_ll > 1000000) {
        printf("Date range too large for counting sort.\n");
        free(keys); return;
    }
    int range = (int)range_ll;
    int *count = calloc(range, sizeof(int));
    BankCustomer_u *out = malloc(size * sizeof(BankCustomer_u));
    if (!count || !out) {
        printf("No more memory\n");
        free(keys); free(count); free(out);
        exit(1);
    }

    for (int i = 0; i < size; i++)
        count[keys[i]-min_k]++;
    for (int i = range-2; i >= 0; i--)
        count[i] += count[i+1];
    for (int i = size-1; i >= 0; i--) {
        out[count[keys[i]-min_k]-1] = c[i];
        count[keys[i]-min_k]--;
    }
    memcpy(c, out, size * sizeof(BankCustomer_u)); // Copy sorted data back
    free(keys); free(count); free(out);
}

void modify_customer_u(BankCustomer_u **c, int *size) {
    if (*size == 0) {
        printf("\nNo customers available to modify or delete.\n");
        return;
    }

    uint32_t acc;
    printf("\nEnter the Account Number of the customer: ");
    scanf("%u", &acc);
    clear_buffer_u();

    for (int i = 0; i < *size; i++) {
        BankCustomer_u *p = &(*c)[i];
        if (*bc_accnum(p) != acc)
            continue;

        printf("\nCustomer Found:%s %s\n", bc_name(p), bc_surname(p));
        printf("--- Action Menu ---\n");
        printf("(1) Modify Name\n(2) Modify Surname\n(3) Modify Deposit Rate\n"
               "(4) Modify Deposit Money\n(5) Delete this customer\nChoice: ");
        int ch;
        scanf("%d", &ch);
        clear_buffer_u();

        switch (ch) {
            case 1:
                printf("Enter new name: ");
                read_line_u(bc_name(p), 50);
                break;
            case 2:
                printf("Enter new surname: ");
                read_line_u(bc_surname(p), 50);
                break;
            case 3:
                printf("Enter new deposit rate: ");
                scanf("%f", bc_rate(p));
                break;
            case 4:
                printf("Enter new deposit money: ");
                scanf("%lf", bc_money(p));
                break;
            case 5:
                for (int j = i; j < (*size)-1; j++)
                    (*c)[j] = (*c)[j+1];
                (*size)--;
                printf("Customer record deleted successfully.\n");
                return;
            default:
                printf("Invalid choice.\n");
                return;
        }
        get_current_date_u(bc_accdate(p));
        printf("Record successfully updated!\n");
        return;
    }
    printf("Account %u not found.\n", (unsigned)acc);
}

void sort_menu_u(BankCustomer_u *c, int size) {
    if (size <= 1) {
        printf("Not enough customers.\n");
        return;
    }
    printf("\n--- Sorting Options ---\n");
    printf("(1) Name (Ascending - Quick Sort)\n");
    printf("(2) Name (Descending - Counting Sort)\n");
    printf("(3) Last day accessed (Ascending - Quick Sort)\n");
    printf("(4) Last day accessed (Descending - Counting Sort)\n");
    printf("(5) Sort by duration of the account (Descending - Radix Sort)\n");
    printf("Choice: ");
    int ch;
    scanf("%d", &ch);
    clear_buffer_u();
    switch (ch) {
        case 1: {
            quick_sort_u(c, 0, size-1, 1);
            printf("Sorted name ascending.\n");
            break;
        }
        case 2: {
            counting_sort_name_u(c, size);
            printf("Sorted name descending.\n");
            break;
        }
        case 3: {
            quick_sort_u(c, 0, size-1, 2);
            printf("Sorted date ascending.\n");
            break;
        }
        case 4: {
            counting_sort_date_desc_u(c,size);
            printf("Sorted date descending.\n");
            break;
        }
        case 5: {
            write_output_sorted_u(c, size);
            printf("Sorted by duration (Oldest accounts first).\n");
            break;
        }
        default:
            printf("Invalid choice.\n");
            return;
    }
    printf("\n");
}
// Save customers to file in binary format
void write_to_file_u(BankCustomer_u *c, int size) {
    char fn[100];
    printf("File name to write: ");
    scanf("%99s", fn);
    FILE *f = fopen(fn, "wb");
    if (!f) {
        printf("Cannot open file.\n");
        return;
    }
    fwrite(&size, sizeof(int), 1, f);
    for (int i = 0; i < size; i++)
        fwrite(c[i].raw, 1, BC_SIZE, f);
    fclose(f);
    printf("Saved %d customers.\n", size);
}
// Load customers from file in binary format
void read_from_file_u(BankCustomer_u **c, int *size, int *cap) {
    char fn[100];
    printf("File name to read: ");
    scanf("%99s", fn);
    FILE *f = fopen(fn, "rb");
    if (!f) {
        printf("File not found.\n");
        return;
    }
    int count;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        return;
    }

    *size = 0; *cap = count;
    BankCustomer_u *t = realloc(*c, count * sizeof(BankCustomer_u));
    if (!t) {
        printf("No more memory\n");
        fclose(f);
        return;
    }
    *c = t;
    for (int i = 0; i < count; i++) {
        memset((*c)[i].raw, 0, BC_SIZE);
        fread((*c)[i].raw, 1, BC_SIZE, f);
        (*size)++;
    }
    fclose(f);
    printf("Loaded %d customers.\n", *size);
}

// 3) TXT File Operations
void write_to_txt_u(BankCustomer_u *c, int size) {
    char fn[100]; printf("Enter .txt filename: "); scanf("%99s", fn);
    FILE *f = fopen(fn, "w");
    if (!f) return;
    fprintf(f, "%d\n", size);
    for (int i = 0; i < size; i++) {
        fprintf(f, "%s|%s|%u|%f|%lf|%llu|%llu\n",
                bc_name(&c[i]), bc_surname(&c[i]), *bc_accnum(&c[i]),
                *bc_rate(&c[i]), *bc_money(&c[i]),
                bc_opendate(&c[i])->raw, bc_accdate(&c[i])->raw);
    }
    printf("Saved %d customers.\n", size);
    fclose(f);
}

void read_from_txt_u(BankCustomer_u **c, int *size, int *cap) {
    char fn[100]; printf("Enter .txt filename: "); scanf("%99s", fn);
    FILE *f = fopen(fn, "r");
    if (!f) return;
    int count; fscanf(f, "%d\n", &count);
    *size = 0; *cap = count;
    *c = realloc(*c, count * sizeof(BankCustomer_u));
    for (int i = 0; i < count; i++) {
        BankCustomer_u *p = &(*c)[i];
        memset(p->raw, 0, BC_SIZE);
        fscanf(f, "%[^|]|%[^|]|%u|%f|%lf|%llu|%llu\n",
               bc_name(p), bc_surname(p), bc_accnum(p),
               bc_rate(p), bc_money(p), &bc_opendate(p)->raw, &bc_accdate(p)->raw);
        (*size)++;
    }
    printf("Loaded %d customers.\n", *size);
    fclose(f);
}