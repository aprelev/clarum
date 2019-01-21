#pragma once

#include <stddef.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef
    struct cla_parser_t
    cla_parser_t;

typedef
    struct cla_option_t
    cla_option_t;

enum {
    cla_noErrors = 0,
    cla_nullReferenceError,
    cla_illegalInputError,
};

/// Callback type for handling non-built-in types of CLI arguments.
///
/// @details
/// Current option is passed via @p option pointer,
/// where `argument` field is set one of the values:
///   + NULL in case original argument had no assignment character, e.g. '--opt';
///   + empty string in case original argument had empty value, e.g. '--opt=';
///   + value of fully-qualified argument, e.g. '--opt=value';
/// and `value` field unset.
/// Use `value` field to store processed result.
///
/// @param options
/// [in, out] CLI arguments parsing context.
///
/// @param option
/// [in, out] CLI arguments parsing context.
///
/// @returns
/// Null reference error on null @p options, or @p option.
///
/// @returns
/// Invalid option argument error on, well, invalid argument.
typedef int
cla_handler_t(
    cla_parser_t *parser,
    cla_option_t *option
);

/// Represents single CLI option.
struct cla_option_t {

    /// Single character tag for short form specification, e.g. '-f', optional.
    char const tag;

    /// Full name for complete form specification, e.g. '--foo', optional.
    char const *name;

    /// Points to decoded value, may be of any type.
    /// Pointed-to value is typically stored within user context,
    /// and is set by handler when option is processed, optional.
    void *valuePtr;

    /// Callback function to be invoked when option is parsed, optional.
    cla_handler_t * const handler;

    /// Argument value fetched from CLI arguments string.
    /// Points to first argument character in argv, when arguments is specified.
    char *argument;

    /// If set to `true`, encountering this option stops further parsing.
    bool const doesStopParser;

    bool isSet;
};

/// Represents a context of CLI options parser.
struct cla_parser_t {

    /// Array of CLI options.
    cla_option_t *options;

    /// Number of options.
    size_t numberOfOptions;

    bool const doesStopOnUnknownOptions;

    /// Is set iff parser encounters halting option.
    ///
    /// @see
    /// ct_option_t
    bool isStopped;

    /// Is set to first unprocessed option.
    ///
    /// @details
    /// Parser stops on first option which does not start with '-' character.
    char const *next;
};

/// Parses @p argc and @p argv against @p options collection.
///
/// @details
/// For each known encountered option `argument` field is set,
/// and `handler` is invoked when present.
///
/// If encountered option is unknown, it is skipped.
/// This allows for chaining parser invocations
/// against different collections of options.
///
/// @note
/// When invoked with @p argc less than 2, does nothing.
///
/// @param options
/// [in, out] Collection of options.
///
/// @param argc
/// [in] Number of CLI arguments.
///
/// @param argv
/// [in] Array of CLI arguments.
///
/// @returns
/// Null reference error on null @p options, or @p argv.
int
cla_parseOptions(
    cla_parser_t *parser,
    int argc,
    char **argv
);

/// Default callback handler for boolean values.
///
/// @details
/// Accepts:
///   + 'on', 'true', 'yes', and '1' as 'true' values;
///   + 'off', 'false', 'no', and '0' as 'false' values.
///
/// When no value is set, underlying option value is set to 'true'.
///
/// @returns
/// Invalid option argument when option argument
/// does not match any supported value.
cla_handler_t
cla_booleanHandler;

/// Default callback handler for integer values.
///
/// @details
/// Employs ct_parseIntegerFromDecimalString() behind the scenes.
///
/// @returns
/// Invalid option argument error when option argument is not set.
cla_handler_t
cla_integerHandler;

/// Default callback handler for string values.
///
/// @details
/// Shallow-copies argument string into option value.
///
/// @warning
/// No allocation is performed, allocated value is expected.
///
/// @returns
/// Invalid option argument error when option argument is not set.
///
/// @returns
/// Null reference error when option value is null.
cla_handler_t
cla_stringHandler;

#if defined(__cplusplus)
}
#endif
