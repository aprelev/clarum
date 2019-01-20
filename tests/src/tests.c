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
    }

    subdesc(configurability) {
        it("skips unknown options") {
            char
                *argv[] = {"binary", "--sample=off", "--integer=100500"};
            int
                argc = sizeof argv / sizeof *argv,
                value = 0;
            cla_option_t
                options[] = {
                    {'i', "integer", &value, &cla_integerHandler},
                };
            size_t const
                numberOfOptions = sizeof options / sizeof *options;
            cla_parser_t
                parser = {
                    options,
                    numberOfOptions,
                    .doesStopOnUnknownOptions = false,
                };

            asserteq(cla_parseOptions(&parser, argc, argv), cla_noErrors, "error was returned");
            asserteq(options[0].isSet, true, "value was not reported as set");
            asserteq(value, 100500, "value was not parsed");
        }
    }
}

snow_main();
