#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct data {
    char *family_name;
    char *first_name;
    char *last_name;
    char *phone_number;
};

void init_person(struct data *person) {
    (*person).family_name = (char *) malloc(8 * sizeof(char));
    (*person).first_name = (char *) malloc(8 * sizeof(char));
    (*person).last_name = (char *) malloc(8 * sizeof(char));
    (*person).phone_number = (char *) malloc(8 * sizeof(char));
}

struct my_simple_vector {
    struct data **arr;
    int size;
    int arr_size;
};

void init(struct my_simple_vector *vector) {
    (*vector).arr = (struct data **) malloc(8 * sizeof(struct data *));
    (*vector).arr_size = 8;
    (*vector).size = 0;
}

void expand_vector(struct my_simple_vector *vector) {
    (*vector).arr = realloc((*vector).arr, (*vector).arr_size * 2 * sizeof(struct data));
    (*vector).arr_size = (*vector).arr_size * 2;
}

void insert(struct my_simple_vector *vector, struct data *item) {
    if ((*vector).size == (*vector).arr_size) {
        expand_vector(vector);
    }
    *((*vector).arr + (*vector).size++) = item;

}

int read_chunk(FILE *fp, char *chunk) {
    int first_iter = 1;
    int i = 0;
    for (i = 0; first_iter || (chunk[i - 1] != ' ' && chunk[i - 1] != '\n' && chunk[i - 1] != EOF); i++) {
        if (first_iter) {
            first_iter = 0;
        }
        chunk[i] = fgetc(fp);
    }
    if (chunk[i - 1] == EOF) {
        chunk[i - 1] = '\0';
        return 1;
    } else {
        chunk[i - 1] = '\0';
        return 0;
    }
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
        struct data *person = (struct data *) malloc(sizeof(struct data));
        init_person(person);
        read_chunk(fp, (*person).family_name);
        read_chunk(fp, (*person).first_name);
        read_chunk(fp, (*person).last_name);
        if (read_chunk(fp, (*person).phone_number)) {
            insert(vector, person);
            break;
        } else {
            insert(vector, person);
        }
    }
    fclose(fp);
    return vector;
}

void swap(struct data **first, struct data **second) {
    struct data *tmp = *first;
    *first = *second;
    *second = tmp;
}

int comp(struct data *first, struct data *second) {
    int compare_family = strcmp((*first).family_name, (*second).family_name);
    if (compare_family == 0) {
        int compare_name = strcmp((*first).first_name, (*second).first_name);
        if (compare_name == 0) {
            int compare_last = strcmp((*first).last_name, (*second).last_name);
            if (compare_last == 0) {
                return strcmp((*first).phone_number, (*second).phone_number);
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

void insertion_sort(struct my_simple_vector *vector, int l, int h,
                    int (*comparator)(struct data *first, struct data *second)) {
    int i, j;
    struct data *k;
    for (i = l + 1; i <= h; i++) {
        k = (*vector).arr[i];
        j = i - 1;
        while (j >= 0 && comparator((*vector).arr[j], k) > 0) {
            (*vector).arr[j+1] = (*vector).arr[j];
            j = j - 1;
        }
        (*vector).arr[j+1] = k;
    }
}

int partition(struct my_simple_vector *vector, int low, int high,
              int (*comparator)(struct data *first, struct data *second)) {
    struct data *pivot = (*vector).arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (comparator((*vector).arr[j], pivot) < 0) {
            i++;
            swap((*vector).arr + i, (*vector).arr + j);
        }
    }
    swap((*vector).arr + i + 1, (*vector).arr + high);
    return i + 1;
}

void
quick_sort(struct my_simple_vector *vector, int l, int h, int (*comparator)(struct data *first, struct data *second),
           int depth) {
    if (depth > 3500) {
        insertion_sort(vector, l, h, comp);
        return;
    }
    if (l < h) {
        int pivot = partition(vector, l, h, comparator);
        quick_sort(vector, l, pivot - 1, comparator, depth + 1);
        quick_sort(vector, pivot + 1, h, comparator, depth +1);
    }
}

void free_person(struct data **person) {
    free((**person).first_name);
    free((**person).family_name);
    free((**person).last_name);
    free((**person).phone_number);
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
    for (int i = 0; i < (*vector).size; i++) {
        fprintf(fp, "%s ", (*(*vector).arr[i]).family_name);
        fprintf(fp, "%s ", (*(*vector).arr[i]).first_name);
        fprintf(fp, "%s ", (*(*vector).arr[i]).last_name);
        fprintf(fp, "%s\n", (*(*vector).arr[i]).phone_number);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    struct my_simple_vector *input = read_data(argv[2]);
    quick_sort(input, 0, (*input).size - 1, comp, 0);
    write_data(input, argv[3]);
    free_vector(input);
    return 0;
};
