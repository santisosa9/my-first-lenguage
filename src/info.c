#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/info.h"
#include "../headers/utils.h"

void _print_info_base(InfoBase* info);
void _print_info_fn(InfoFunction* info);
void _print_info_cf(InfoControlFlow* info);

InfoBase* as_info_base(Info* info){
    return &info->base;
}

InfoFunction* as_info_fn(Info* info){
    return &info->fn;
}

InfoControlFlow* as_info_cf(Info* info){
    return &info->cf;
}

Props* new_prop(Type type, int value, char* name, int line, int col){
    Props* prop = (Props*)malloc(sizeof(Props));
    prop->type = type;
    prop->value = value;
    prop->name = name;
    prop->line = line;
    prop->col = col;
    return prop;
}

Info* new_info(Type type, int value, char* name, int line, int col){
    Props* props = new_prop(type, value, name, line, col);
    Info* info = (Info*)malloc(sizeof(Info));

    as_info_base(info)->props = props;

    return info;
}


Info* new_info_fn(Props* props, nat cant_params, InfoBase* params){
    Info* info = (Info*)malloc(sizeof(Info));

    as_info_fn(info)->props = props;
    as_info_fn(info)->cant_params = cant_params;
    as_info_fn(info)->params = params;

    return info;
}

Info* copy_info(Tag tag, Info* dest, Info* src){
    if(dest == NULL || src == NULL) return NULL;

    switch (tag) {
        case FN:
            as_info_fn(dest)->props = as_info_fn(src)->props;
            as_info_fn(dest)->cant_params = as_info_fn(src)->cant_params;
            as_info_fn(dest)->params = as_info_fn(src)->params;
            break;

        case CF:
            as_info_cf(dest)->props = as_info_cf(src)->props;
            break;

        default:
            as_info_base(dest)->props = as_info_base(src)->props;
            break;
    }

    return dest;
}



void update_value(Info* dest, int value) {
    if(dest != NULL) as_info_base(dest)->props->value = value;
}

void print_info(Tag tag, Info* info){
    switch (tag)
    {
    case FN:
        _print_info_fn(as_info_fn(info));
        break;

    case CF:
        _print_info_cf(as_info_cf(info));
        break;

    default:
        _print_info_base(as_info_base(info));
        break;
    }
}

void _print_info_base(InfoBase* info){
    if(info == NULL) {
        puts("");
        return;
    }
    printf("{");
    printf("Type: %s, ", type_to_str(info->props->type));
    printf("Value: %s, ", value_to_str(info->props->value, info->props->type));
    printf("Name: %s, ", info->props->name);
    printf("Line: %d", info->props->line);
    printf("}\n");
}

void _print_info_fn(InfoFunction* info){
    if(info == NULL) {
        puts("print_info: info is NULL");
        return;
    }
    printf("{");
    printf("Return_type: %s", type_to_str(info->props->type));
    printf("Value: %s, ", value_to_str(info->props->value, info->props->type));
    printf("Name: %s, ", info->props->name);
    printf("Line: %d", info->props->line);
    printf("Cant_params: %d, ", info->cant_params);
    printf("Params: [");
    for (nat i = 0; i < info->cant_params; i++) {
        _print_info_base(&info->params[i]);
    }
    printf("]");
    printf("}\n");
}

void _print_info_cf(InfoControlFlow* info){
    if(info == NULL) {
        puts("print_info: info is NULL");
        return;
    }
    printf("{");
    printf("Type: %s, ", type_to_str(info->props->type));
    printf("Value: %s, ", value_to_str(info->props->value, info->props->type));
    printf("Name: %s, ", info->props->name);
    printf("Line: %d", info->props->line);
    printf("}\n");
}

char* type_to_str(Type type){
    switch (type){
        case INT:       return strdup("INT");           break;
        case BOOL:      return strdup("BOOL");          break;
        case VOID:      return strdup("VOID");          break;
        case NO_TYPED:  return strdup("NO_TYPED");      break;
        case ANY:       return strdup("ANY");           break;
        default:        return strdup("UNKNOWN_TYPE");  break;
    }
}

char* value_to_str(int value, Type type){
    switch (type) {
        case BOOL:  return value == 0 ? strdup("false") : strdup("true");
        default:    return itos(value);
    }
}
