#pragma once

#include <stdbool.h>
#include "tag.h"
#include "utils.h"
#include "linked_list.h"

typedef enum Type
{
    ANY,
    INT,
    BOOL,
    VOID,
    NO_TYPED,
} Type;


typedef struct Props
{
    Type type;
    int value;
    char* name;
    nat line;
    nat col;
    bool is_fn;
} Props;

typedef struct InfoBase
{
    Props* props;
} InfoBase;

typedef InfoBase Param;

typedef struct InfoFunction
{
    Props* props;
    nat cant_params;
    LinkedList* params; // Param[]
    bool is_extern;
} InfoFunction;

typedef union Info {
    InfoFunction fn;
    InfoBase base;
} Info;

/* Convert the info to its corresponding type. */
InfoBase* as_info_base(Info* info);
InfoFunction* as_info_fn(Info* info);

/* Create a new prop. */
Props* new_props(Type type, int value, char* name, int line, int col);

/* Create a new info. */
Info* new_info(Type type, int value, char* name, int line, int col);
Info* new_info_fn(Props* props, LinkedList* params, bool is_extern);
Info* new_info_base(Props* props);

/* Print the info. */
void print_info(Tag tag, Info* info);

/* Copy the info. */
Info* copy_info(Tag tag, Info* dest, Info* src);

/* Update the value of the info. */
void update_value(Info* dest, int value);

/* Converts a value to its corresponding string representation. */
char* value_to_str(int value, Type type);

/* Converts a type to its corresponding string representation. */
char* type_to_str(Type type);
