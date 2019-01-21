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
    }

    subdesc(configurability) {
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

    subdesc(parser) {
        it("parses long string option") {
            char
                *argv[] = {"binary", "--string=foo"},
                *value = NULL;
            int
                argc = sizeof argv / sizeof *argv;
            cla_option_t
                options[] = {{
                        .name = "string",
                        .handler = &cla_stringHandler,
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

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq_str(value, "foo", "option value was not set");
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
                        .handler = &cla_stringHandler,
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

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq_str(value, "foo", "option value was not set");
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
                        .name = "integer",
                        .handler = &cla_integerHandler,
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

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq(value, 100500, "option value was not decoded");
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
                        .handler = &cla_integerHandler,
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

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq(value, 100500, "option value was not decoded");
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
                        .name = "bool",
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

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq(value, true, "option value was not decoded");
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

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors);
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq(value, true, "option value was not decoded");
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
                        .name = "integer",
                        .valuePtr = &integerValue,
                        .handler = &cla_integerHandler,
                    }, {
                        .name = "bool",
                        .valuePtr = &booleanValue,
                        .handler = &cla_booleanHandler,
                    }, {
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
            asserteq(options[0].isReferenced, true, "option was not reported as referenced");
            asserteq(options[1].isReferenced, true, "option was not reported as referenced");
            asserteq(options[2].isReferenced, true, "option was not reported as referenced");
            asserteq(integerValue, 100500, "integer option value was not decoded");
            asserteq(booleanValue, true, "boolean option value was not decoded");
            asserteq(stringValue, "foo", "string option value was not set");
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
                        .handler = &cla_integerHandler,
                        .valuePtr = &integerValue,
                    }, {
                        .tag = 'b',
                        .handler = &cla_booleanHandler,
                        .valuePtr = &booleanValue,
                    }, {
                        .tag = 's',
                        .handler = &cla_stringHandler,
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isReferenced, true, "option was not reported as referenced");
            asserteq(options[2].isReferenced, true, "option was not reported as referenced");
            asserteq(integerValue, 100500, "integer option value was not decoded");
            asserteq(booleanValue, true, "boolean option value was not decoded");
            asserteq(stringValue, "foo", "string option value was not set");
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
                        .handler = &cla_integerHandler,
                        .valuePtr = &integerValue,
                    }, {
                        .name = "bool",
                        .handler = &cla_booleanHandler,
                        .valuePtr = &booleanValue,
                    }, {
                        .tag = 's',
                        .handler = &cla_stringHandler,
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isReferenced, true, "option was not reported as referenced");
            asserteq(options[2].isReferenced, true, "option was not reported as referenced");
            asserteq(integerValue, 100500, "integer option value was not decoded");
            asserteq(booleanValue, true, "boolean option value was not decoded");
            asserteq(stringValue, "foo", "string option value was not set");
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
                        .handler = &cla_booleanHandler,
                        .valuePtr = &booleanValueA,
                    }, {
                        .tag = 'b',
                        .handler = &cla_booleanHandler,
                        .valuePtr = &booleanValueB,
                    }, {
                        .tag = 'c',
                        .handler = &cla_booleanHandler,
                        .valuePtr = &booleanValueC,
                    }, {
                        .name = "string",
                        .handler = &cla_stringHandler,
                        .valuePtr = &stringValue,
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
            asserteq(options[1].isReferenced, true, "option was not reported as referenced");
            asserteq(options[2].isReferenced, true, "option was not reported as referenced");
            asserteq(options[3].isReferenced, true, "option was not reported as referenced");
            asserteq(booleanValueA, true, "boolean option A value was not decoded");
            asserteq(booleanValueB, true, "boolean option B value was not decoded");
            asserteq(booleanValueC, true, "boolean option C value was not decoded");
            asserteq(stringValue, "foo", "string option value was not set");
        }
    }
}

snow_main();
