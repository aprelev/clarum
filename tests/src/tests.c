#include <clarum/clarum.h>
#include <snow/snow.h>
#include <stdio.h>

describe(clarum) {
    subdesc(interface) {
        it("checks number of arguments") {
            char
                *argv[] = {"binary"};
            int
                argc = sizeof argv / sizeof *argv;
            cla_parser_t
                parser;

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors, "tried to parse single argument");
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

        it("checks for invalid value for boolean option") {
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
    }

    subdesc(configurability) {
        it("skips unknown options") {
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
                    .doesStopOnUnknownOptions = false,
                };

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors, "error was returned");
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, 100500, "value was not parsed");
        }

        it("respects $doesStopParser setting") {
            char
                *argv[] = {"binary", "--halts"};
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .tag = 'h',
                        .name = "halts",
                        .doesStopParser = true,
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
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(parser.isStopped, true, "parser was not stopped");
        }
    }

    subdesc(parser) {
        it("parses long string option") {
            char
                *argv[] = {"binary", "--string=foo"},
                *value = NULL;
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .tag = 's',
                        .name = "string",
                        .valuePtr = &value,
                        .handler = &cla_stringHandler,
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
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq_str(value, "foo", "argument was not parsed");
        }
    }
}

snow_main();
