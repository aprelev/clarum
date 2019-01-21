#include <clarum/clarum.h>
#include <snow/snow.h>

describe(interface) {
    it("checks number of arguments") {
        char
            *argv[] = {"binary"};
        int
            argc = sizeof argv / sizeof *argv;
        cla_parser_t
            parser;

        asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors, "tried to parse binary name");
    }

    it("checks for null pointers") {
        char
            *argv[] = {"binary"};
        int
            argc = sizeof argv / sizeof *argv;
        cla_parser_t
            parser;

        asserteq(cla_parseOptions(NULL, argc, argv), cla_nullReferenceError, "@parser was not checked for NULL");
        asserteq(cla_parseOptions(&parser, argc, NULL), cla_nullReferenceError, "@argv was not checked for NULL");
    }

    it("checks for invalid value of boolean options") {
        char
            *argv[] = {"binary", "--bool=sample"};
        int
            argc = sizeof argv / sizeof *argv;
        bool
            value;
        cla_option_t
            options[] = {{
                    .tag = 'b',
                    .name = "bool",
                    .valuePtr = &value,
                    .handler = &cla_booleanHandler,
                },
            };
        size_t const
            numberOfOptions = sizeof options / sizeof *options;
        cla_parser_t
            parser = {
                .options = options,
                .numberOfOptions = numberOfOptions,
            };

        asserteq(cla_parseOptions(&parser, argc, argv), cla_illegalInputError);
    }

    it("does not require explicit value for boolean options") {
        char
            *argv[] = {"binary", "-b"};
        bool
            value = false;
        int
            argc = sizeof argv / sizeof *argv;
        cla_option_t
            options[] = {{
                    .tag = 'b',
                    .name = "bool",
                    .valuePtr = &value,
                    .handler = &cla_booleanHandler,
                },
            };
        size_t const
            numberOfOptions = sizeof options / sizeof *options;
        cla_parser_t
            parser = {
                .options = options,
                .numberOfOptions = numberOfOptions,
            };

        asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
        asserteq(options[0].isReferenced, true, "option was not reported as referenced");
        asserteq(value, true, "option value was not decoded");
    }

    it("can skip unknown options") {
        char
            *argv[] = {"binary", "--sample=off", "--integer=100500"};
        int
            argc = sizeof argv / sizeof *argv,
            value = 0;
        cla_option_t
            options[] = {{
                    .tag = 'i',
                    .name = "integer",
                    .valuePtr = &value,
                    .handler = &cla_integerHandler,
                },
            };
        size_t const
            numberOfOptions = sizeof options / sizeof *options;
        cla_parser_t
            parser = {
                .options = options,
                .numberOfOptions = numberOfOptions,
                .isLenient = true,
            };

        asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors, "error was returned");
        asserteq(parser.isTerminated, false, "parser was terminated");
        asserteq(options[0].isReferenced, true, "option was not reported as referenced");
        asserteq(value, 100500, "option value was not decoded");
    }

    it("can terminate on unknown options") {
        char
            *argv[] = {"binary", "--sample=off", "--integer=100500"};
        int
            argc = sizeof argv / sizeof *argv,
            value = 0;
        cla_option_t
            options[] = {{
                    .tag = 'i',
                    .name = "integer",
                    .valuePtr = &value,
                    .handler = &cla_integerHandler,
                },
            };
        size_t const
            numberOfOptions = sizeof options / sizeof *options;
        cla_parser_t
            parser = {
                .options = options,
                .numberOfOptions = numberOfOptions,
                .isLenient = false,
            };

        asserteq(cla_parseOptions(&parser, argc, argv), cla_unknowOptionError, "error was not returned");
        asserteq(parser.isTerminated, true, "parser was not terminated");
        asserteq(options[0].isReferenced, false, "option was reported as referenced");
        asserteq(value, 0, "option value was decoded");
    }

    it("respects terminal options") {
        char
            *argv[] = {"binary", "--halts", "--continue=yes"};
        int
            argc = sizeof argv / sizeof *argv;
        bool
            value = false;
        cla_option_t
            options[] = {{
                    .name = "halts",
                    .isTerminal = true,
                },{
                    .name = "continue",
                    .handler = &cla_booleanHandler,
                    .valuePtr = &value,
                },
            };
        size_t const
            numberOfOptions = sizeof options / sizeof *options;
        cla_parser_t
            parser = {
                .options = options,
                .numberOfOptions = numberOfOptions,
            };

        asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors, "error was returned");
        asserteq(parser.isTerminated, true, "parser was not terminated");
        asserteq(options[1].isReferenced, false, "second option was reported as referenced");
        asserteq(value, false, "second option value was decoded");
    }
}
