#include "../headers/tag.h"

char* tag_to_str(Tag tag){
    switch (tag){
        case ID:        return strdup("ID");            break;
        case MAIN:      return strdup("MAIN");          break;
        case RET:       return strdup("RET");           break;
        case ASIG:      return strdup("ASIG");          break;
        case TYPE:      return strdup("TYPE");          break;
        case SEMICOLON: return strdup("SEMICOLON");     break;
        case DEC:       return strdup("DEC");           break;
        case NOT:       return strdup("NOT");           break;
        case OR:        return strdup("OR");            break;
        case AND:       return strdup("AND");           break;
        case ADD:       return strdup("ADD");           break;
        case MUL:       return strdup("MUL");           break;
        case VALUE:     return strdup("VALUE");         break;
        default:        return strdup("UNKNOWN_TAG");   break;
    }
}
