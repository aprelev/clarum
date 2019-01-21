#include <clarum/clarum.h>
#include <stdio.h>

/* Clarum allows for easy creation of custom handlers. */
static inline int
versionHandler(
    cla_parser_t *parser,
    cla_option_t *option
) {
    (void) parser;
    (void) option;

    printf("clarum-example, ver. 0.0.0\n");
    return 0;
}

/* Clarum handlers can be chained as illustrated below. */
static inline int
idleHandler(
    cla_parser_t *parser,
    cla_option_t *option
) {
    bool const
        *value = option->valuePtr;
    int
        result = cla_booleanHandler(parser, option);

    return !result
        ? printf("Mode is set to %s\n", *value ? "IDLE" : "LIVE"), 0
        : result;
}

int
main(int argc, char **argv) {
    /* Default values can be set right away. */
    bool
        isRecursive = false,
        isIdle = false;
    char
        *filter = "*"; 
    size_t
        jobs = 1;
    int
        result;

    /* Initialising options. */
    cla_option_t
        options[] = {
        /* -v [--version] terminal option with custom handler, parser will stop after this option. */
        { .tag = 'v', .name = "version", .handler = &versionHandler, .isTerminal = true, },

        /* -r boolean option with default handler; no long form is available. */
        { .tag = 'r', .handler = &cla_booleanHandler, .valuePtr = &isRecursive, },

        /* -i [--idle] boolean option with chained handlers. */
        { .tag = 'i', .name = "idle", .handler = &idleHandler, .valuePtr = &isIdle, },

        /* -f [--filter, --regexp] string option with default handler. */
        { .tag = 'f', .name = "filter", .synonym = "regexp", .handler = &cla_stringHandler, .valuePtr = &filter, },

        /* -j [--jobs, --threads] integer options with default handler which decodes size_t values. */
        { .tag = 'j', .name = "jobs", .synonym = "threads", .handler = &cla_integerHandler, .valuePtr = &jobs, },
    };

    /* Initialising non-lenient parser. */
    cla_parser_t
        parser = {
            .options = options,
            .numberOfOptions = sizeof options / sizeof *options,
        };

    /* Parsing options. */
    result = cla_parseOptions(&parser, argc, argv);

    /* Reporting results. */
    printf("Report:\n");
    printf("  $isRecursive: %s%s\n", isRecursive ? "ON" : "OFF", options[1].isReferenced ? " [referenced]" : "");
    printf("  $isIdle: %s%s\n", isIdle ? "ON" : "OFF", options[2].isReferenced ? " [referenced]" : "");
    printf("  $filter: %s%s\n", filter, options[3].isReferenced ? " [referenced]" : "");
    printf("  $jobs: %zu%s\n", jobs, options[4].isReferenced ? " [referenced]" : "");
    printf("  parser %s terminated\n", parser.isTerminated ? "was" : "was not");
    printf("  parser exited with %d (%s)\n", result, result ? "error" : "normal exit code");

    return 0;
}
