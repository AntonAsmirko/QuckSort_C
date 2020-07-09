#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) (a > b? a: b)

typedef struct stack {
    size_t size;
    size_t arr_size;
    int *arr;
} stack;

int make_stack(stack *my_stack) {
    if ((my_stack->arr = (int *) malloc(sizeof(int))) == NULL)
        return 1;
    my_stack->arr_size = 1;
    my_stack->size = 0;
    return 0;
}

int expand_stack(stack *my_stack) {
    if ((my_stack->arr = realloc(my_stack->arr, 2 * my_stack->arr_size)) == NULL)return 1;
    my_stack->arr_size *= 2;
    return 0;
}

int push(stack *my_stack, int item) {
    int error = 0;
    if (my_stack->size == my_stack->arr_size) error = expand_stack(my_stack);
    my_stack->arr[(my_stack->size)++] = item;
    return error;
}

int shrink_stack(stack *my_stack) {
    if ((my_stack->arr = realloc(my_stack->arr, MAX((my_stack->arr_size / 2), 1))) == NULL)return 1;
    my_stack->arr_size = MAX(my_stack->arr_size / 2, 1);
    return 0;
}

int pop(stack *my_stack, int *item) {
    int error = 0;
    *item = my_stack->arr[--(my_stack->size)];
    if (my_stack->size == my_stack->arr_size / 2) error = shrink_stack(my_stack);
    return error;
}

typedef struct data {
    char *family_name;
    char *first_name;
    char *last_name;
    int phone_number;
} data;

void init_person(data *person) {
    person->family_name = (char *) malloc(21 * sizeof(char));
    person->first_name = (char *) malloc(21 * sizeof(char));
    person->last_name = (char *) malloc(21 * sizeof(char));
}

struct my_simple_vector {
    data **arr;
    int size;
    int arr_size;
};

void init(struct my_simple_vector *vector) {
    vector->arr = (data **) malloc(8 * sizeof(data *));
    vector->arr_size = 8;
    vector->size = 0;
}

void expand_vector(struct my_simple_vector *vector) {
    vector->arr = realloc((*vector).arr, (*vector).arr_size * 2 * sizeof(data));
    vector->arr_size = (*vector).arr_size * 2;
}

void insert(struct my_simple_vector *vector, data *item) {
    if (vector->size == vector->arr_size) expand_vector(vector);
    *(vector->arr + (*vector).size++) = item;
}

struct my_simple_vector *read_data(char *file) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("%s", "can not open input file");
        exit(1);
    }
    struct my_simple_vector *vector = (struct my_simple_vector *) malloc(sizeof(struct my_simple_vector));
    init(vector);
    while (1) {
        data *person = (data *) malloc(sizeof(data));
        init_person(person);
        if (fscanf(fp, "%s", person->family_name) == EOF) {
            free(person);
            break;
        }
        fscanf(fp, "%s", person->first_name);
        fscanf(fp, "%s", person->last_name);
        fscanf(fp, "%d", &(person->phone_number));
        insert(vector, person);
    }
    fclose(fp);
    return vector;
}

void swap(data **first, data **second) {
    data *tmp = *first;
    *first = *second;
    *second = tmp;
}

int comp(data *first, data *second) {
    int compare_family = strcmp(first->family_name, second->family_name);
    if (compare_family == 0) {
        int compare_name = strcmp(first->first_name, second->first_name);
        if (compare_name == 0) {
            int compare_last = strcmp(first->last_name, second->last_name);
            if (compare_last == 0) {
                return first->phone_number - second->phone_number;
            } else {
                return compare_last;
            }
        } else {
            return compare_name;
        }
    } else {
        return compare_family;
    }
}

int partition(struct my_simple_vector *vector, int low, int high,
              int (*comparator)(data *first, data *second)) {
    data *pivot = vector->arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (comparator(vector->arr[j], pivot) < 0) {
            i++;
            swap(vector->arr + i, vector->arr + j);
        }
    }
    swap(vector->arr + i + 1, vector->arr + high);
    return i + 1;
}

int iterative_quick_sort(struct my_simple_vector *vector, int l, int h,
                         int (*comparator)(data *first, data *second)) {
    stack *my_stack;
    if ((my_stack = (stack *) malloc(sizeof(stack))) == NULL) return 1;
    if (make_stack(my_stack)) {
        free(my_stack);
        return 1;
    }
    if (push(my_stack, l)) {
        free(my_stack);
        return 1;
    }
    if (push(my_stack, h)) {
        free(my_stack);
        return 1;
    }
    while (my_stack->size > 0) {
        if (pop(my_stack, &h)) {
            free(my_stack);
            return 1;
        }
        if (pop(my_stack, &l)) {
            free(my_stack);
            return 1;
        }
        int p = partition(vector, l, h, comparator);
        if (p - 1 > l) {
            if (push(my_stack, l)) {
                free(my_stack);
                return 1;
            }
            if (push(my_stack, p - 1)) {
                free(my_stack);
                return 1;
            }
        }
        if (p + 1 < h) {
            if (push(my_stack, p + 1)) {
                free(my_stack);
                return 1;
            }
            if (push(my_stack, h)) {
                free(my_stack);
                return 1;
            }
        }
    }
    return 0;
}

void free_person(data **person) {
    free((**person).first_name);
    free((**person).family_name);
    free((**person).last_name);
    free(*person);
}

void free_vector(struct my_simple_vector *vector) {
    for (int i = 0; i < (*vector).size; i++) {
        free_person((*vector).arr + i);
    }
    free((&(*vector).arr));
}

void write_data(struct my_simple_vector *vector, char *output) {
    FILE *fp = fopen(output, "w");
    for (int i = 0; i < vector->size; i++) {
        fprintf(fp, "%s ", vector->arr[i]->family_name);
        fprintf(fp, "%s ", vector->arr[i]->first_name);
        fprintf(fp, "%s ", vector->arr[i]->last_name);
        fprintf(fp, "%d\n", vector->arr[i]->phone_number);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("%s", "too few arguments");
        return 228;
    } else if (argc > 3) {
        printf("%s", "too many arguments");
        return 337;
    }
    struct my_simple_vector *input = read_data(argv[1]);
    if (iterative_quick_sort(input, 0, input->size - 1, comp)) {
        free_vector(input);
        return 1;
    }
    write_data(input, argv[2]);
    free_vector(input);
    return 0;
};
