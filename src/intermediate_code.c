#include <stdio.h>
#include <stdlib.h>
#include "../headers/ast.h"
// #include "../headers/info.h"
#include "../headers/intermediate_code.h"
#include "../headers/quadruple.h"

int temp_counter = 0;

char* getTempName() {
    char* temp_name = (char*)malloc(10 * sizeof(char));
    sprintf(temp_name, "t%d", temp_counter++);
    return temp_name;
}

int label_counter = 0;

char* getLabelName() {
    char* label_name = (char*)malloc(10 * sizeof(char));
    sprintf(label_name, "l%d", label_counter++);
    return label_name;
}

void generate_intermediate_code(AST* tree, LinkedList* ic) {
    if (ic == NULL) {
        fprintf(stderr, "Error: generate_intermediate_code: Invalid call error.\n");
        fprintf(stderr, "Precondition fault: 'ic' must not be NULL.\n");
        exit(EXIT_FAILURE);
    }

    if (tree == NULL) {
        return;
    }

    Info* arg1 = NULL;
    Info* arg2 = NULL;
    Info* result = NULL;
    Tag tag = tree->tag;

    switch (tag) {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case LESS:
        case GREATER:
        case EQUAL:
        case AND:
        case OR: {
            // Primero genero código intermedio para los hijos (posorder).
            generate_intermediate_code(tree->left, ic);
            generate_intermediate_code(tree->right, ic);

            if (tree->left != NULL && tree->right != NULL) {
                arg1 = tree->left->info;
                arg2 = tree->right->info;

                as_info_base(tree->info)->props->name = getTempName();
                result = tree->info;

                Quadruple* quad = new_quadruple(tag, arg1, arg2, result);
                //print_quadruple(quad);
                insert_ll(ic,quad);
            }
            break;
        }

        case NOT: {
            generate_intermediate_code(tree->right, ic);

            if (tree->right != NULL) {
                arg1 = tree->right->info;

                as_info_base(tree->info)->props->name = getTempName();
                result = tree->info;

                Quadruple* quad = new_quadruple(tag, arg1, NULL, result);
                //print_quadruple(quad);
                insert_ll(ic,quad);
            }
            break;
        }

        case ASIG:
        case DEC: {
            generate_intermediate_code(tree->left, ic);
            generate_intermediate_code(tree->right, ic);

            if (tree->left != NULL && tree->right != NULL) {
                arg1 = tree->right->info;
                result = tree->left->info;

                Quadruple* quad = new_quadruple(ASIG, arg1, NULL, result);
                //print_quadruple(quad);
                insert_ll(ic,quad);
            }
            break;
        }

        case RET: {
            generate_intermediate_code(tree->right, ic);

            if (tree->right != NULL) {
                result = tree->right->info;
                Quadruple* quad = new_quadruple(tag, NULL, NULL, result);
                //print_quadruple(quad);
                insert_ll(ic,quad);
            }
            break;
        }

        case WHILE: {

            char* startLabelName = getLabelName();
            char* endLabelName = getLabelName();

            Info* start;
            Info* end;

            as_info_base(tree->info)->props->name = startLabelName;
            start = tree->info;

            Quadruple* startLabel = new_quadruple(LABEL, NULL, NULL, start); // Etiqueta de inicio del while
            print_quadruple(startLabel);

            generate_intermediate_code(tree->left, ic); // Genero código intermedio para la condición

            end = new_info(NO_TYPED, 0, endLabelName, 0, 0);

            if (tree->left != NULL) {
                arg1 = tree->left->info;
                Quadruple* condition = new_quadruple(IFN, arg1, NULL, end); // Si la condición evalua falsa saltamos a endLabel
                //print_quadruple(condition);
                insert_ll(ic,condition);
            }

            generate_intermediate_code(tree->right, ic); // Genero código intermedio para el cuerpo del while

            Quadruple* jumpToStart = new_quadruple(JUMP, NULL, NULL, start); // Salto al inicio del while
            //print_quadruple(jumpToStart);
            insert_ll(ic,jumpToStart);
            Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, end); // Etiqueta de fin del while
            //print_quadruple(endLabel);
            insert_ll(ic,endLabel);
            break;
        }

        case IF: {
            generate_intermediate_code(tree->left, ic); // Genero código intermedio para la condición

            char* nextLabelName = getLabelName();
            Info* next;

            if (tree->left != NULL) {
                arg1 = tree->left->info;
                next = new_info(NO_TYPED, 0, nextLabelName, 0, 0);
                Quadruple* condition = new_quadruple(IFN, arg1, NULL, next);
                //print_quadruple(condition);
                insert_ll(ic,condition);
            }

            if (tree->right->tag != T_E) {
                generate_intermediate_code(tree->right, ic);
                Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, next); // Etiqueta de fin del if
                //print_quadruple(endLabel);
                insert_ll(ic,endLabel);
            } else {
                char* endLabelName = getLabelName();
                Info* end = new_info(NO_TYPED, 0, endLabelName, 0, 0);
                generate_intermediate_code(tree->right->left, ic); // Bloque del then
                Quadruple* jumpToEnd = new_quadruple(JUMP, NULL, NULL, end); // Una vez terminado el then salto al fin del if
                //print_quadruple(jumpToEnd);
                insert_ll(ic,jumpToEnd);
                Quadruple* elseLabel = new_quadruple(LABEL, NULL, NULL, next); // Etiqueta del else
                //print_quadruple(elseLabel);
                insert_ll(ic,elseLabel);
                generate_intermediate_code(tree->right->right, ic); // Bloque del else
                Quadruple* endLabel = new_quadruple(LABEL, NULL, NULL, end); // Etiqueta de fin del if
                //print_quadruple(endLabel);
                insert_ll(ic,endLabel);
            }

            break;
        }

        case FN_DEC: {
          Quadruple* dec = new_quadruple(tree->tag, NULL, NULL, tree->info);
          print_quadruple(dec);
          generate_intermediate_code(tree->right, ic);
          Quadruple* end = new_quadruple(FN_END, NULL, NULL, tree->info);
          //print_quadruple(end);
          insert_ll(ic,end);

          break;
        }

        case FN_CALL: {
            generate_intermediate_code(tree->right, ic); // Parametros
            arg1 = new_info(NO_TYPED, 0, as_info_base(tree->info)->props->name, 0, 0);
            // copy_info(tree->tag, arg1, tree->info);
            as_info_base(tree->info)->props->name = getTempName();
            result = tree->info;
            Quadruple* call = new_quadruple(FN_CALL, arg1, NULL, result);
            //print_quadruple(call);
            insert_ll(ic,call);
            break;
        }

        case PARAM_SEC: {
            generate_intermediate_code(tree->left, ic);
            if (tree->left != NULL) {
                Quadruple* param = new_quadruple(PARAMETER, NULL, NULL, tree->left->info);
                 //print_quadruple(param);
                 insert_ll(ic,param); 
            }
            generate_intermediate_code(tree->right, ic);
            break;
        }



        default:
            generate_intermediate_code(tree->left, ic);
            generate_intermediate_code(tree->right, ic);
            break;
    }
}
