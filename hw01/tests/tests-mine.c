/*
 * You can write your own tests in this file
 */

#define CUT
#include "libs/cut.h"

#include "libs/mainwrap.h"

#include <stdlib.h>

// Test template just for you ;)
TEST(encode_word_and_unfinished)
{
    const unsigned char input[] = {0xff, 0xff, 0xff, 0xff, 0x01, 0x02};
    const unsigned char output[] = {0x17, 0xff, 0xff, 0xff, 0x7e, 0x00, 0x88, 0x88, 0x00, 0x00};
    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_word)
{
    const unsigned char input[] = {
        // TODO
    };
    const unsigned char output[] = {
        // TODO
    };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_word_and_unfinished)
{
    const unsigned char input[] = {0x17, 0xff, 0xff, 0xff, 0x7e, 0x01, 0x02};
    const unsigned char output[] = {0xff, 0xff, 0xff, 0xff};
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_FAILURE);
    CHECK_FILE(stderr, "Wrong code word\n");
    CHECK_BINARY_FILE(stdout, output);
}