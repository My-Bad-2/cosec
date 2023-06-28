#pragma once

#include <stdlib.h>
#include <string.h>
#include <cstddef>

template <typename T>
struct node {
    T data;
    bool taken = false;
};

template <typename T>
struct list {
   public:
    size_t length;
    node<T>* data;

    list(int length) : length(length) {
        data = (node<T>*)malloc((length + 1) * sizeof(node<T>));
        memset(data, 0, length * sizeof(node<T>));
    }

    ~list() { free(data); }

    template <typename U>
    node<T>* find(bool (*function)(U, node<T>*), U arg) {
        for (size_t i = 0; i < length; i++) {
            if (data[i].taken) {
                if (function(arg, &data[i])) {
                    return &data[i];
                }
            }
        }

        return nullptr;
    }

    void remove(node<T>* n) {
        for (size_t i = 0; i < length; i++) {
            if (&data[i] == n) {
                memset(&data[i], 0, sizeof(node<T>));
            }
        }
    }

    node<T>* add(T value) {
    retry:
        for (size_t i = 0; i < length; i++) {
            if (!data[i].taken) {
                data[i].data = value;
                data[i].taken = true;

                return &data[i];
            }
        }

        data = (node<T>*)realloc(data, (length + 2) * sizeof(node<T>));
        memset(&data[length], 0, sizeof(node<T>));
        length++;

        goto retry;
    }
};