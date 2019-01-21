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
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, true, "argument was not set");
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

        it("parses short string option") {
            char
                *argv[] = {"binary", "-s=foo"},
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

        it("parses long integer option") {
            char
                *argv[] = {"binary", "--integer=100500"};
            size_t
                value = 0;
            int
                argc = sizeof argv / sizeof *argv;
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
                };

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, 100500, "argument was not parsed");
        }

        it("parses short integer option") {
            char
                *argv[] = {"binary", "-i=100500"};
            size_t
                value = 0;
            int
                argc = sizeof argv / sizeof *argv;
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
                };

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, 100500, "argument was not parsed");
        }

        it("parses long boolean option") {
            char
                *argv[] = {"binary", "--bool=on"};
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
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, true, "argument was not parsed");
        }

        it("parses short boolean option") {
            char
                *argv[] = {"binary", "-b=on"};
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
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, true, "argument was not parsed");
        }

        it("parses multiple long options") {
            char
                *argv[] = {"binary", "--integer=100500", "--bool=on", "--string=foo"};
            size_t
                integerValue = 0;
            bool
                booleanValue = false;
            char
                *stringValue = NULL;
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .tag = 'i',
                        .name = "integer",
                        .valuePtr = &integerValue,
                        .handler = &cla_integerHandler,
                    }, {
                        .tag = 'b',
                        .name = "bool",
                        .valuePtr = &booleanValue,
                        .handler = &cla_booleanHandler,
                    }, {
                        .tag = 's',
                        .name = "string",
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isSet, true, "value was not reported as set");
            asserteq(options[2].isSet, true, "value was not reported as set");
            asserteq(integerValue, 100500, "integer option was not parsed");
            asserteq(booleanValue, true, "boolean option was not parsed");
            asserteq(stringValue, "foo", "string option was not parsed");
        }

        it("parses multiple short options") {
            char
                *argv[] = {"binary", "-i=100500", "-b=on", "-s=foo"};
            size_t
                integerValue = 0;
            bool
                booleanValue = false;
            char
                *stringValue = NULL;
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .tag = 'i',
                        .name = "integer",
                        .valuePtr = &integerValue,
                        .handler = &cla_integerHandler,
                    }, {
                        .tag = 'b',
                        .name = "bool",
                        .valuePtr = &booleanValue,
                        .handler = &cla_booleanHandler,
                    }, {
                        .tag = 's',
                        .name = "string",
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isSet, true, "value was not reported as set");
            asserteq(options[2].isSet, true, "value was not reported as set");
            asserteq(integerValue, 100500, "integer option was not parsed");
            asserteq(booleanValue, true, "boolean option was not parsed");
            asserteq(stringValue, "foo", "string option was not parsed");
        }

        it("parses multiple mixed options") {
            char
                *argv[] = {"binary", "-i=100500", "--bool=on", "-s=foo"};
            size_t
                integerValue = 0;
            bool
                booleanValue = false;
            char
                *stringValue = NULL;
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .tag = 'i',
                        .name = "integer",
                        .valuePtr = &integerValue,
                        .handler = &cla_integerHandler,
                    }, {
                        .tag = 'b',
                        .name = "bool",
                        .valuePtr = &booleanValue,
                        .handler = &cla_booleanHandler,
                    }, {
                        .tag = 's',
                        .name = "string",
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isSet, true, "value was not reported as set");
            asserteq(options[2].isSet, true, "value was not reported as set");
            asserteq(integerValue, 100500, "integer option was not parsed");
            asserteq(booleanValue, true, "boolean option was not parsed");
            asserteq(stringValue, "foo", "string option was not parsed");
        }

        it("parses sequential short options") {
            char
                *argv[] = {"binary", "-abc", "--string=foo"};
            bool
                booleanValueA = false,
                booleanValueB = false,
                booleanValueC = false;
            char
                *stringValue = NULL;
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .tag = 'a',
                        .name = "a",
                        .valuePtr = &booleanValueA,
                        .handler = &cla_booleanHandler,
                    }, {
                        .tag = 'b',
                        .name = "b",
                        .valuePtr = &booleanValueB,
                        .handler = &cla_booleanHandler,
                    }, {
                        .tag = 'c',
                        .name = "c",
                        .valuePtr = &booleanValueC,
                        .handler = &cla_booleanHandler,
                    }, {
                        .tag = 's',
                        .name = "string",
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isSet, true, "value was not reported as set");
            asserteq(options[2].isSet, true, "value was not reported as set");
            asserteq(options[3].isSet, true, "value was not reported as set");
            asserteq(booleanValueA, true, "boolean option A was not parsed");
            asserteq(booleanValueB, true, "boolean option B was not parsed");
            asserteq(booleanValueC, true, "boolean option C was not parsed");
            asserteq(stringValue, "foo", "string option was not parsed");
        }
    }
}

snow_main();
