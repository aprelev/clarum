#include <clarum/clarum.h>
#include <string.h>

static inline bool
isEscapeCharacter(
    char chr
) {
    switch (chr) {
        case '-': /* linux-style escape */
        case '/': /* windows-style escape */
            return true;

        default:
            return false;
    }
}

static inline bool
isDelimiterCharacter(
    char chr
) {
    return chr == '=';
}

static inline bool
isOptionCharacter(
    char chr
) {
    return chr && !isEscapeCharacter(chr) && !isDelimiterCharacter(chr);
}

static inline bool
isReferencedOption(
    cla_option_t const *option,
    char const *str,
    bool byTag
) {
    if (byTag)
        return *str == option->tag;
    else
        return !memcmp(str, option->name, strlen(option->name));
}

static inline cla_option_t *
getOption(
    cla_parser_t *parser,
    char const *str,
    bool byTag
) {
    for (size_t i = 0; i < parser->numberOfOptions; ++i) {
        cla_option_t
            *current = &parser->options[i];

        if (isReferencedOption(current, str, byTag))
            return current;
    }

    return NULL;
}

static inline char *
getArgument(
    char *str
) {
    for (size_t i = 0; i < strlen(str); ++i) {
        if (isDelimiterCharacter(str[i]))
            /* Skips found delimiter. */
            return &str[i + 1];
    }

    return NULL;
}

static inline int
parseOption(
    cla_parser_t *parser,
    char *str,
    bool byTag
) {
    if (!str[0]) {
        /* Argument is '--', this stops parsing by convention. */
        return cla_noErrors;
    }
    
    if (isOptionCharacter(str[0])) {
        cla_option_t
            *option = getOption(parser, str, byTag);

        if (!option) {
            parser->isStopped = parser->doesStopOnUnknownOptions;
            return cla_noErrors;
        }

        option->argument = getArgument(str);
        option->isSet = true;
        parser->isStopped = option->doesStopParser;

        return option->handler
            ? option->handler(parser, option)
            : cla_noErrors;
    }

    /* Argument contains illegal characters sequence, e.g. '--='. */
    return cla_illegalInputError;
}

static inline int
parseOptions(
    cla_parser_t *parser,
    int numberOfArguments,
    char **arguments
) {
    int
        status = cla_noErrors;

    for (; numberOfArguments-- && !status && !parser->isStopped; ++arguments) {
        char
            *argument = *arguments;

        if (!isEscapeCharacter(argument[0])) {
            /* Not a valid option. */
            parser->next = argument;
            break;
        }

        if (!isEscapeCharacter(argument[1])) {
            /* Short '-x' or '-abc' (tag) form. */
            for (size_t i = 1; isOptionCharacter(argument[i]) && !status; ++i)
                status = parseOption(parser, &argument[i], true);
        } else {
            /* Complete '--name' (name) form. */
            status = parseOption(parser, &argument[2], false);
        }
    }

    return status;
}

int
cla_parseOptions(
    cla_parser_t *parser,
    int argc,
    char **argv
) {
    if (!parser || !argv)
        /* Null @parser or @argv. */
        return cla_nullReferenceError;

    if (argc < 2)
        /* No options passed. */
        return cla_noErrors;

    /* Skips first argument (binary name). */
    return parseOptions(parser, --argc, ++argv);
}
