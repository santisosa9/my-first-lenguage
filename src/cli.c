#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../headers/cli.h"
#include "../headers/config.h"
#include "../headers/utils.h"

extern Config* cfg;

void print_help() {
    puts("Usage: ./compiler [options] <input_file>");
    puts("Input file must have a `.ctds` extension.");
    puts("");
    puts("Options:");
    puts("  -h, --help                      Show this help message.");
    puts("  -v, --verbose                   Show verbose output.");
    puts("  -d, --debug                     Show debug output.");
    puts("  -t, --target <target>           Specify the compilation target.");
    puts("      <target>:");
    puts("          parse                   Parse the input file.");
    puts("          codinter                Generate intermediate code.");
    puts("          asm                     Generate assembly code.");
    puts("          bin           (default) Generate binary code.");
    puts("  -l, --lib <lib_file>            Specify the external library C file.");
    puts("  -o, --output <output_file>      Specify the output file.");
}


bool parse_args(int argc, char const *argv[]) {
    argc--; argv++; // Skip the first argument, which is the program name.

    if (argc == 0) {
        fprintf(stderr, "Error: No arguments provided.\n");
        return false;
    }

    bool help_occurred = false;
    bool verbose_occurred = false;
    bool debug_occurred = false;
    bool external_occurred = false;
    bool output_occurred = false;
    bool input_occurred = false;
    bool target_occurred = false;

    char* output_path = NULL;
    char* input_path = NULL;
    char* input_base_name = NULL;
    char* external_path = NULL;
    char* asm_path = NULL;

    Target target = BIN;
    char* target_extensions[] = {".ast", ".ci", ".s", ""};

    AsmTarget asm_target = X86_64_LINUX_GNU;

    regex_t *help_flag_regex = new_regex("^-h|(-|--)help$");
    regex_t *verbose_flag_regex = new_regex("^-v|(-|--)verbose$");
    regex_t *debug_flag_regex = new_regex("^-d|(-|--)debug$");
    regex_t *target_flag_regex = new_regex("^-t|(-|--)target$");
    regex_t *target_regex = new_regex("^(parse|codinter|asm|bin)$");
    regex_t *output_flag_regex = new_regex("^-o|(-|--)output$");
    regex_t *lib_flag_regex = new_regex("^-l|(-|--)lib$");
    regex_t *lib_path_regex = new_regex("^[^\\-].*\\.c");
    regex_t *output_path_regex = new_regex("^[^\\-].*"); // Match any string that does not start with a hyphen (flag symbol).
    regex_t *input_path_regex = new_regex("^([^\\-].*)\\.ctds$"); // Capture the base name of the input file.

    for (int i = 0; i < argc; i++) {

        if (match(help_flag_regex, argv[i])) {
            help_occurred = true;
        }
        else if (match(verbose_flag_regex, argv[i])) {
            verbose_occurred = true;
        }
        else if (match(debug_flag_regex, argv[i])) {
            debug_occurred = true;
        }
        else if (match(lib_flag_regex, argv[i])) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: No external library provided.\n");
                return false;
            }

            if (!match(lib_path_regex, argv[i + 1])) {
                fprintf(stderr, "Error: Invalid external library provided.\n");
                fprintf(stderr, "provided library: %s\n", argv[i + 1]);
                fprintf(stderr, "External library must have a `.c` extension.\n");
                return false;
            }

            external_occurred = true;
            external_path = strdup(argv[i + 1]);
            i++;
        }
        else if (match(target_flag_regex, argv[i])) {

            if (i + 1 >= argc) {
                fprintf(stderr, "Error: No target provided.\n");
                return false;
            }

            regmatch_t pmatch[2];
            if (!match_sub(target_regex, argv[i + 1], 2, pmatch)) {
                fprintf(stderr, "Error: Invalid target provided.\n");
                fprintf(stderr, "provided target: %s\n", argv[i + 1]);
                fprintf(stderr, "Valid targets: parse, codinter, asm, bin.\n");
                return false;
            }

            if (strcmp("parse", get_substring(argv[i + 1], pmatch[1])) == 0) {
                target = PARSE;
            }
            else if (strcmp("codinter", get_substring(argv[i + 1], pmatch[1])) == 0) {
                target = CODINT;
            }
            else if (strcmp("asm", get_substring(argv[i + 1], pmatch[1])) == 0) {
                target = ASM;
            }
            else if (strcmp("bin", get_substring(argv[i + 1], pmatch[1])) == 0) {
                target = BIN;
            }

            target_occurred = true;
            i++;

        }
        else if (match(output_flag_regex, argv[i])) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: No output file provided.\n");
                return false;
            }

            output_occurred = true;
            if (!match(output_path_regex, argv[i + 1])) {
                fprintf(stderr, "Error: Invalid output file provided.\n");
                fprintf(stderr, "provided file: %s\n", argv[i + 1]);
                fprintf(stderr, "Output file must not start with a hyphen.\n");
                return false;
            }

            output_path = strdup(argv[i + 1]);
            i++;
        }
        else {
            regmatch_t pmatch[2];
            if (!match_sub(input_path_regex, argv[i], 2, pmatch)) {
                fprintf(stderr, "Error: Invalid source code file provided.\n");
                fprintf(stderr, "Source code file must have a `.ctds` extension.\n");
                return false;
            }

            input_path = strdup(argv[i]);
            input_base_name = get_substring(argv[i], pmatch[1]);
            input_occurred = true;
        }
    }

    if (!input_occurred && !help_occurred) {
        fprintf(stderr, "Error: No source code file provided.\n");
        return false;
    }

    if (!output_occurred && !help_occurred) {
        char* ext = target == BIN ? "" : target_extensions[target];
        output_path = (char*) malloc(strlen(input_base_name) + strlen(ext) + 1);
        sprintf(output_path, "%s%s", input_base_name, ext);
    }

    if (debug_occurred) {
        asm_path = (char*) malloc(strlen(input_base_name) + strlen(".s") + 1);
        sprintf(asm_path, "%s%s", input_base_name, ".s");
    }

    if (target_occurred && target == ASM && !output_occurred) {
        asm_path = (char*) malloc(strlen(input_base_name) + strlen(".s") + 1);
        sprintf(asm_path, "%s%s", input_base_name, ".s");
    }

    if (target_occurred && target == ASM && output_occurred) {
        asm_path = output_path;
    }

    if (input_occurred && asm_path == NULL) {
        asm_path = (char*) malloc(strlen(input_base_name) + strlen(".s") + 1);
        sprintf(asm_path, "%s%s", input_base_name, ".s");
    }

    regfree(help_flag_regex);
    regfree(verbose_flag_regex);
    regfree(debug_flag_regex);
    regfree(output_flag_regex);
    regfree(output_path_regex);
    regfree(input_path_regex);
    regfree(target_flag_regex);
    regfree(target_regex);
    regfree(lib_flag_regex);
    regfree(lib_path_regex);


    cfg = new_config();
    cfg->help = help_occurred;
    cfg->verbose = verbose_occurred;
    cfg->debug = debug_occurred;
    cfg->external = external_occurred;
    cfg->output = output_occurred;
    cfg->output_path = output_path;
    cfg->input_path = input_path;
    cfg->input_base_name = input_base_name;
    cfg->external_path = external_path;
    cfg->asm_path = asm_path;
    cfg->target = target;
    cfg->asm_target = asm_target;

    return true;
}
