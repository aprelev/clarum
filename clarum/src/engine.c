#include <clarum/clarum.h>
#include <string.h>

static inline bool
isEscapeCharacter(
    char chr
) {
    return chr == '-' || chr == '/';
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
    return chr
        ? !isEscapeCharacter(chr) && !isDelimiterCharacter(chr)
        : false;
}

static inline bool
optionNameEquals(
    cla_option_t const *option,
    char const *str
) {
    return option->name
        ? !memcmp(str, option->name, strlen(option->name))
        : false;
}

static inline bool
optionSynonymEquals(
    cla_option_t const *option,
    char const *str
) {
    return option->synonym
        ? !memcmp(str, option->synonym, strlen(option->synonym))
        : false;
}

static inline bool
isReferencedOption(
    cla_option_t const *option,
    char const *str,
    bool byTag
) {
    return byTag
        ? *str == option->tag
        : optionNameEquals(option, str) || optionSynonymEquals(option, str);
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
            /* Increments to skip the delimiter. */
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
    if (!str[0])
        /* @str = '--\0', this stops parsing by convention. */
        return cla_noErrors; 
    
    if (isOptionCharacter(str[0])) {
        cla_option_t
            *option = getOption(parser, str, byTag);

        if (!option) {
            return !parser->isLenient
                ? parser->isTerminated = true, cla_unknowOptionError
                : cla_noErrors;
        }

        option->isReferenced = true;
        option->argument = getArgument(str);
        parser->isTerminated = option->isTerminal;

        return option->handler
            ? option->handler(parser, option)
            : cla_noErrors;
    }

    /* @str has invalid syntax. */
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

    for (; numberOfArguments-- && !parser->isTerminated; ++arguments) {
        char
            *argument = *arguments;

        if (!isEscapeCharacter(argument[0])) {
            /* Not a valid option. */
            parser->next = argument;
            break;
        }

        if (!isEscapeCharacter(argument[1])) {
            /* Short '-x' or '-abc' form. */
            for (size_t i = 1; isOptionCharacter(argument[i]); ++i) {
                status = parseOption(parser, &argument[i], true);
                if (status)
                    return status;
            }
        } else {
            /* Long '--name' form. */
            status = parseOption(parser, &argument[2], false);
            if (status)
                return status;
        }
    }

    return status;
}

static inline bool
isRequiredOptionMissing(
    cla_option_t const *options,
    size_t numberOfOptions
) {
    for (size_t i = 0; i < numberOfOptions; ++i) {
        cla_option_t const
            option = options[i];

        if (option.isRequired && !option.isReferenced)
            return true;
    }

    return false;
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

    if (argc > 1) {
        int
            status;

        /* Skips first argument (binary name). */
        status = parseOptions(parser, --argc, ++argv);
        if (status)
            return status;

        /* Checks whether all required options were referenced. */
        if (isRequiredOptionMissing(parser->options, parser->numberOfOptions))
            return cla_missingOptionError;
    }

    return cla_noErrors;
}
