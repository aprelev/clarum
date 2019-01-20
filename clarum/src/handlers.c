#include <clarum/clarum.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>

static inline int
decodeBooleanValue(
    bool *value,
    char const *str
) {
    if (!strcmp(str, "true") || !strcmp(str, "yes") ||
        !strcmp(str, "on") || !strcmp(str, "1")) {
        *value = true;
        return cla_noErrors;
    }

    if (!strcmp(str, "false") || !strcmp(str, "no") ||
        !strcmp(str, "off") || !strcmp(str, "0")) {
        *value = false;
        return cla_noErrors;
    }

    return cla_illegalInputError;
}

int
cla_booleanHandler(
    cla_parser_t *parser,
    cla_option_t *option
) {
    (void) parser;

    if (!option->value)
        /* No value holder provided. */
        return cla_nullReferenceError;

    if (!option->argument) {
        /* No explicit value is passed, this counts as true. */
        *((bool *) option->value) = true;
        return cla_noErrors;
    }

    return decodeBooleanValue(option->value, option->argument);
}

static inline size_t
parseDecimalCharacter(
    char decimalCharacter
) {
    const size_t
        invalidValue = 10;

    if (isdigit(decimalCharacter))
        return (size_t) (decimalCharacter - '0');

    return invalidValue;
}

static inline int
parseIntegerFromDecimalString(
    size_t *value,
    char const *str
) {
    const size_t
        maximumStringLength = ceil(log10(SIZE_MAX)) + 1;

    if (!value || !str)
        return cla_nullReferenceError;

    if (strlen(str) >= maximumStringLength)
        /* @str is too long to be decimal string. */
        return cla_illegalInputError;

    for (; *str; ++str) {
        size_t const
            temporary = *value,
            digit = parseDecimalCharacter(*str);

        if (digit > 9)
            /* @str contains non-decimal character. */
            return cla_illegalInputError;

        *value *= 10;
        *value += digit;

        if (temporary >= *value)
            /* @str represents integer which cannot be stored (incurs overflow). */
            return cla_illegalInputError;
    }

    return cla_noErrors;
}

int
cla_integerHandler(
    cla_parser_t *parser,
    cla_option_t *option
) {
    (void) parser;

    return option->argument
        ? parseIntegerFromDecimalString(option->value, option->argument)
        : cla_nullReferenceError;
}

int
cla_stringCopyHandler(
    cla_parser_t *parser,
    cla_option_t *option
) {
    (void) parser;

    return option->argument
        ? strcpy(option->value, option->argument), cla_noErrors
        : cla_nullReferenceError;
}

int
cla_stringReferenceHandler(
    cla_parser_t *parser,
    cla_option_t *option
) {
    (void) parser;

    return option->argument
        /* @option->argument was set from mutable argv, so it's OK. */
        ? option->value = (char *) option->argument, cla_noErrors
        : cla_nullReferenceError;
}
