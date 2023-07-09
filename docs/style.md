# Code Style and Best Practices
This is not an exhaustive list, so please try to familiarize yourself with the general coding style of this project when contributing :)

Your IDE can help automatically format code for you by using the formatting specified in the included `.clang-format` file.

## General Formatting
* Use the included `.clang-format` in your IDE for automatic formatting.
* Use tabs for indentation.
* Break at 80 columns
  - If a declaration, function call, etc, goes over 80 columns limit, split each argument into its own line and align to the opening parenthesis.

## Naming Conventions
* All names should be in `snake_case`.

## Pointers and References
Add a space after (but not before) `*` and `&`. In other words, attack `*` and `&` to the type, not the name. This helps make it clear whether the variable is a reference, pointer, or not.

###### Example:
```cpp
int& foo;
void* bar;
char* baz[];
```

## Classes and Structs
As a general rule of thumb, use classes for anything with complex functionality and structs for bits of data that need to be conveniently grouped together.

Data members do not need to be prefixed.

Typically, it's good practice to make member variables in classes private, and to add public getters and setters (however applicable) for them.

###### Example:
```cpp
class my_class {
   public:
    void set_foo(int foo) {
        this->foo = foo;
    }

    int get_foo() {
        return this->foo;
    }

   private:
    int foo;
};
```

## `using` statements
Please do not put `using namespace` statements in header files.

## Omission of braces in control flow
Please avoid omitting braces from control flow statements.

###### Okay:
```cpp
if(foo) {
    bar();
}
```

###### Wrong:
```cpp
if(foo)
    bar();
```

## TODO
When something needs to be finished at some point in the future, simply leave a comment that starts with `FIXME:` or `TODO:` and describe what needs to be done or fixed.

###### Example:
```cpp
// TODO: Write a better implementation
int sum(int a, int b) {
    int ret = a + b;
    return ret;
}
```

## Header files
All header files must start with `#pragma once`. It's okay to start `libc` header files in the format:

```c
#ifndef _<FOLDER_NAME>_<FILENAME>
#define _<FOLDER_NAME>_<FILENAME>

#endif
```