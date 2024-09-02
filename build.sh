#!/bin/bash

# Script para compilar el lexer

function show_help() {
    echo -e "\n\e[33mUso\e[0m: ./make-lexer.sh [opciones]"
    echo -e "\n\e[33mOpciones\e[0m:"
    echo -e "  --all, -a            Corre todos los comandos"
    echo -e "  --bison [args]       Argumentos para bison y corre bison"
    echo -e "  --flex [args]        Argumentos para flex y corre flex"
    echo -e "  --gcc [args]         Argumentos para gcc y corre gcc"
    echo -e "  --make [args]        Argumentos para make y corre make"
    echo -e "  --help               Muestra este mensaje"
    echo -e "\n\e[33mEjemplos\e[0m:"
    echo -e "  ./make-lexer.sh --all                    Corre todos los comandos"
    echo -e "  ./make-lexer.sh --all --gcc -Wall        Corre todos los comandos y le pasa argumentos a gcc"
    echo -e "  ./make-lexer.sh --bison                  Corre solo bison"
    echo -e "  ./make-lexer.sh --flex -w --bison -t     Corre solo flex y bison con argumentos"
}

# Argumentos por defecto
GCC_ARGS=()
BISON_ARGS=(-d)
FLEX_ARGS=()
MAKE_ARGS=()

RUN=()

# Indica el comando al que se le pasan los argumentos
args_of=''

for arg in "$@"; do
    [[ "$arg" =~ ("--help"|"-h") ]] && show_help                          && exit
    [[ "$arg" =~ ("--all"|"-a") ]]                     && RUN+=('all')    && continue
    [[ "$arg" == "--bison" ]]       && args_of='bison' && RUN+=('bison')  && continue
    [[ "$arg" == "--flex" ]]        && args_of='flex'  && RUN+=('flex')   && continue
    [[ "$arg" == "--gcc" ]]         && args_of='gcc'   && RUN+=('gcc')    && continue
    [[ "$arg" == "--make" ]]        && args_of='make'  && RUN+=('make')   && continue

    [[ "$args_of" == 'bison' ]]     && BISON_ARGS+=($arg)                 && continue
    [[ "$args_of" == 'flex' ]]      && FLEX_ARGS+=($arg)                  && continue
    [[ "$args_of" == 'gcc' ]]       && GCC_ARGS+=($arg)                   && continue
    [[ "$args_of" == 'make' ]]      && MAKE_ARGS+=($arg)                  && continue
done

[[ "${RUN[*]}" == *"flex"* || "${RUN[*]}" == *"all"* ]] \
    && echo -e "\n\e[33m⠶ flex \e[0m (flex ${FLEX_ARGS[*]} preproyecto-lexico.l)" \
    && flex ${FLEX_ARGS[*]} preproyecto-lexico.l

[[ "${RUN[*]}" == *"bison"* || "${RUN[*]}" == *"all"* ]] \
    && echo -e "\n\e[33m⠶ bison \e[0m (bison ${BISON_ARGS[*]} preproyecto-sintaxis.y)" \
    && bison ${BISON_ARGS[*]} preproyecto-sintaxis.y

[[ "${RUN[*]}" == *"make"* || "${RUN[*]}" == *"all"* ]] \
    && echo -e "\n\e[33m⠶ make \e[0m (make dbg ${MAKE_ARGS[*]})" \
    && make dbg ${MAKE_ARGS[*]}

[[ "${RUN[*]}" == *"gcc"* || "${RUN[*]}" == *"all"* ]] \
    && echo -e "\n\e[33m⠶ gcc \e[0m (gcc ${GCC_ARGS[*]} lex.yy.c preproyecto-sintaxis.tab.c build/ast.o build/utils.o -o lexer)" \
    && gcc ${GCC_ARGS[*]} lex.yy.c preproyecto-sintaxis.tab.c build/ast.o build/utils.o -o lexer



