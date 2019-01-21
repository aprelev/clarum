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
    cla_missingOptionError,
    cla_unknowOptionError,
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
    /// Single character tag (short form), e.g. 'f'.
    char const tag;

    /// String name (long form), e.g. 'foo'.
    char const *name;

    /// String synonym (long form), e.g. 'Foo'.  
    char const *synonym;

    /// Callback function to be invoked on parsing.
    ///
    /// @details
    /// Default handlers require cla_option_t::valuePtr.
    cla_handler_t * const handler;

    /// Points to value decoded by handler.
    void *valuePtr;

    /// Points to option value in argv, set by parser.
    char *argument;

    /// Specifies whether this option stops further parsing.
    bool const isTerminal;

    /// Specifies whether this option shall be present.
    bool const isRequired;

    /// Is set by parser iff option was encountered.
    bool isReferenced;
};

/// Represents a context of CLI options parser.
struct cla_parser_t {

    /// Array of CLI options.
    cla_option_t *options;

    /// Number of options.
    size_t numberOfOptions;

    /// Specifies whether parser should terminate on unknown options.
    bool const isLenient;

    /// Is set to first unprocessed option.
    ///
    /// @details
    /// Parser stops on first option which does not start with '-' character.
    char const *next;

    /// Is set iff parser was terminated during parsing.
    ///
    /// @detail
    /// Parser encountered
    ///   + either terminal option (ct_option_t::isTerminal),
    ///   + or unknown option while not being lenient (cla_parser_t::isLenient).
    ///
    /// @see
    /// ct_option_t::isTerminal
    /// cla_parser_t::isLenient
    bool isTerminated;
};

/// Parses @p argc and @p argv against collection of options.
///
/// @param parser
/// [in, out] Parser instance.
///
/// @param argc
/// [in] Number of CLI arguments.
///
/// @param argv
/// [in] Array of CLI arguments.
///
/// @returns
/// Null reference error on null @p options, or @p argv.
/// Illegal input error on syntax errors.
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
