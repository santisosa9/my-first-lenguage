#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/info.h"
#include "../headers/utils.h"

void _print_info_base(InfoBase* info);
void _print_info_fn(InfoFunction* info);

InfoBase* as_info_base(Info* info){
    return &info->base;
}

InfoFunction* as_info_fn(Info* info){
    return &info->fn;
}

char* scope_to_str(Scope scope){
    switch (scope){
        case GLOBAL:     return strdup("GLOBAL");        break;
        case LOCAL:      return strdup("LOCAL");         break;
        case PARAM:      return strdup("PARAM");         break;
        default:         return strdup("UNKNOWN_TYPE");  break;
    }
}


Props* new_prop(Type type, int value, char* name, int line, int col, char* file_path){
    Props* prop = (Props*)malloc(sizeof(Props));
    prop->type = type;
    prop->value = value;
    prop->name = name;
    prop->line = line;
    prop->col = col;
    prop->is_fn = false;
    prop->file_path = file_path;
    return prop;
}

void copy_prop(Props* dest, Props* src) {
    if (src == NULL || dest == NULL) return;

    dest->type = src->type;
    dest->value = src->value;
    dest->line = src->line;
    dest->col = src->col;
    dest->is_fn = src->is_fn;
    dest->name = strdup(src->name);
}

Info* new_info(Type type, int value, char* name, int line, int col, int offset, char* file_path){
    Props* props = new_prop(type, value, name, line, col, file_path);
    Info* info = (Info*)malloc(sizeof(Info));

    as_info_base(info)->props = props;

    return info;
}


Info* new_info_fn(Props* props, LinkedList* params, bool is_extern, nat locals){
    Info* info = (Info*)malloc(sizeof(Info));

    as_info_fn(info)->props = props;
    as_info_fn(info)->props->is_fn = true;
    as_info_fn(info)->cant_params = params->size;
    as_info_fn(info)->params = params;
    as_info_fn(info)->is_extern = is_extern;


    return info;
}

Info* copy_info(Tag tag, Info* dest, Info* src){
    if(dest == NULL || src == NULL) return NULL;

    switch (tag) {
        case FN_CALL:
        case FN_DEC:
            memcpy(as_info_fn(dest), as_info_fn(src), sizeof(InfoFunction));
            as_info_fn(dest)->cant_params = as_info_fn(src)->cant_params;
            as_info_fn(dest)->params = as_info_fn(src)->params;
            as_info_fn(dest)->is_extern = as_info_fn(src)->is_extern;
            break;

        default:
            memcpy(as_info_base(dest), as_info_base(src), sizeof(InfoBase));
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
    case FN_CALL:
    case FN_DEC:
        _print_info_fn(as_info_fn(info));
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
    printf("Name: ‘%s’, ", info->props->name);
    printf("Line: %u, ", info->props->line);
    printf("Scope: %s ", scope_to_str(info->scope));
    printf("}\n");
}

void _print_info_fn(InfoFunction* info){
    if(info == NULL) {
        puts("print_info: info is NULL");
        return;
    }
    printf("{");
    printf("Ret type: %s, ", type_to_str(info->props->type));
    printf("Value: %s, ", value_to_str(info->props->value, info->props->type));
    printf("Name: ‘%s’, ", info->props->name);
    printf("Line: %u, ", info->props->line);
    printf("is_extern: %s, ", info->is_extern ? "yes" : "no");
    printf("params: %u [", info->cant_params);
    for (nat i = 0; i < info->cant_params; i++) {
        _print_info_base(lookup(info->params, i));
    }
    printf("]");
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
