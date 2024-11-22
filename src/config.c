#include "../headers/asm.h"
#include "../headers/config.h"

Config* cfg;

Config* new_config() {
    Config* config = (Config*)malloc(sizeof(Config));
    if (config == NULL) {
        fprintf(stderr, "Failed to allocate memory for Config\n");
        exit(EXIT_FAILURE);
    }
    config->help = false;
    config->verbose = false;
    config->debug = false;
    config->external = false;
    config->output = false;
    config->output_path = NULL;
    config->input_path = NULL;
    config->input_base_name = NULL;
    config->external_path = NULL;
    config->asm_path = NULL;
    config->target = BIN;
    config->asm_target = X86_64_LINUX_GNU;
    return config;
}

void free_config(Config* config) {
    if(config == NULL) return;
    if(config->output_path != NULL) free(config->output_path);
    if(config->input_path != NULL) free(config->input_path);
    if(config->input_base_name != NULL) free(config->input_base_name);
    if(config->external_path != NULL) free(config->external_path);
    if(config->asm_path != NULL) free(config->asm_path);
    free(config);
}

void print_config(Config* config) {
    if(config == NULL) return;
    printf("Config:\n");
    printf("  help: %s\n", config->help ? "true" : "false");
    printf("  verbose: %s\n", config->verbose ? "true" : "false");
    printf("  debug: %s\n", config->debug ? "true" : "false");
    printf("  external: %s\n", config->external ? "true" : "false");
    printf("  output: %s\n", config->output ? "true" : "false");
    printf("  output_path: %s\n", config->output_path ? config->output_path : "NULL");
    printf("  input_path: %s\n", config->input_path ? config->input_path : "NULL");
    printf("  input_base_name: %s\n", config->input_base_name ? config->input_base_name : "NULL");
    printf("  external_path: %s\n", config->external_path ? config->external_path : "NULL");
    printf("  asm_path: %s\n", config->asm_path ? config->asm_path : "NULL");
    printf("  target: %d\n", config->target);
    printf("  asm_target: %d\n", config->asm_target);
}

void write_stdlib(){
    FILE *f = fopen("stdlib.c", "w");
    assert(f != NULL, "Failed to open file 'stdlib.c'\n");
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "#include <stdbool.h>\n\n");
    fprintf(f, "void print_int(int x) {\n");
    fprintf(f, "    printf(\"%%d\\n\", x);\n");
    fprintf(f, "}\n\n");
    fprintf(f, "void print_bool(bool x) {\n");
    fprintf(f, "    printf(\"%%s\\n\", x ? \"true\" : \"false\");\n");
    fprintf(f, "}\n");
    fflush(f);
    fclose(f);
}
