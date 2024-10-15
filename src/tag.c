#include "../headers/tag.h"
#include "../headers/utils.h"

char* tag_to_str(Tag tag){
    switch (tag){
        case ID:        return strdup("ID");            break;
        case PROGRAM:   return strdup("PROGRAM");       break;
        case RET:       return strdup("RET");           break;
        case ASIG:      return strdup("ASIG");          break;
        case TYPE:      return strdup("TYPE");          break;
        case SEMICOLON: return strdup("SEMICOLON");     break;
        case BLOCK:     return strdup("BLOCK");         break;
        case DEC:       return strdup("DEC");           break;
        case NOT:       return strdup("NOT");           break;
        case OR:        return strdup("OR");            break;
        case AND:       return strdup("AND");           break;
        case ADD:       return strdup("ADD");           break;
        case SUB:       return strdup("SUB");           break;
        case UNSUB:     return strdup("UNSUB");         break;
        case DIV:       return strdup("DIV");           break;
        case MOD:       return strdup("MOD");           break;
        case EQUALS:    return strdup("EQUALS");        break;
        case MINOR:     return strdup("MINOR");         break;
        case BIGGER:    return strdup("BIGGER");        break;
        case MUL:       return strdup("MUL");           break;
        case VALUE:     return strdup("VALUE");         break;
        case IF:        return strdup("IF");            break;
        case WHILE:     return strdup("WHILE");         break;
        case T_E:       return strdup("T_E");           break;
        case DEC_FN:    return strdup("DEC_FN");        break;
        case CALL_FN:   return strdup("CALL_FN");       break;
        case PARAM:     return strdup("PARAM");         break;
        default:        return strdup("UNKNOWN_TAG");   break;
    }
}
