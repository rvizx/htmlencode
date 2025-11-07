
/**************************************************************************
 * htmlencode - encoder and decoder for HTML entities                    *
 * Copyright (C) 2025                                                     *
 *                                                                        *
 * This program is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. *
 **************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void usage(FILE *fd)
{
    fputs(
        "Usage: htmlencode [OPTION]\n"
        "Translate to and from HTML entity encoding.\n"
        "Reads from standard input, writes to standard output.\n"
        "\n"
        "  -a encode all characters\n"
        "  -b do not automatically encode non printable (i.e. binary) characters\n"
        "  -c specify a different set of special characters when encoding\n"
        "  -d decode data\n"
        "  -l encode input line by line\n"
        "  -n do not output the trailing newline when encoding\n"
        "  -x use hexadecimal entities (&#xHH;) instead of decimal (&#NNN;)\n"
        "\n"
        "  -h display this help and exit\n",
        fd);
}

static bool encode_all = false;
static bool encode_binary = true;
static bool action_decode = false;
static bool help = false;
static bool suppress_newline = false;
static bool line_mode = false;
static bool use_hex = false;

static const char *special = "<>&\"'";

int parse_options(int argc, char *argv[])
{
    int c;
    while (((c = getopt(argc, argv, ":abc:dhlnx"))) != -1) {
        switch (c) {
            case 'a':
                encode_all = true;
                break;
            case 'b':
                encode_binary = false;
                break;
            case 'c':
                special = optarg;
                break;
            case 'd':
                action_decode = true;
                break;
            case 'h':
                help = true;
                break;
            case 'l':
                line_mode = true;
                break;
            case 'n':
                suppress_newline = true;
                break;
            case 'x':
                use_hex = true;
                break;
            case '?':
                fprintf(stderr, "unrecognized option `-%c'\n\n", optopt);
                usage(stderr);
                return 1;
            case ':':
                fprintf(stderr, "missing argument for option -%c\n\n", optopt);
                return 1;
            default:
                fputs("getopt malfunction\n", stderr);
                return 1;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "unrecognized option `%s'\n\n", argv[optind]);
        usage(stderr);
        return 1;
    }

    return 0;
}

const char* get_named_entity(char c)
{
    switch (c) {
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '&': return "&amp;";
        case '"': return "&quot;";
        case '\'': return "&#39;";
        default: return NULL;
    }
}

void encode()
{
    char c;
    while (((c = getchar())) != EOF) {
        if (line_mode && c == '\n') {
            putchar('\n');
        } else if (encode_all
                   || (encode_binary && !isprint(c))
                   || strchr(special, c)) {
            const char *named = get_named_entity(c);
            if (named && !encode_all) {
                fputs(named, stdout);
            } else {
                if (use_hex) {
                    printf("&#x%02hhX;", (unsigned char)c);
                } else {
                    printf("&#%hhu;", (unsigned char)c);
                }
            }
        } else {
            putchar(c);
        }
    }
    if (!suppress_newline) {
        putchar('\n');
    }
}

void decode()
{
    char c;
    while (((c = getchar())) != EOF) {
        if (c != '&') {
            putchar(c);
        } else {
            char entity[32];
            int i = 0;
            char next;
            
            while (((next = getchar())) != EOF && next != ';' && i < 31) {
                entity[i++] = next;
            }
            entity[i] = '\0';
            
            if (next != ';') {
                fprintf(stderr, "Bad entity: `&%s'\n", entity);
                exit(1);
            }
            
            // Named entities
            if (strcmp(entity, "lt") == 0) {
                putchar('<');
            } else if (strcmp(entity, "gt") == 0) {
                putchar('>');
            } else if (strcmp(entity, "amp") == 0) {
                putchar('&');
            } else if (strcmp(entity, "quot") == 0) {
                putchar('"');
            } else if (strcmp(entity, "apos") == 0 || strcmp(entity, "#39") == 0) {
                putchar('\'');
            }
            // Numeric entities
            else if (entity[0] == '#') {
                unsigned n;
                if (entity[1] == 'x' || entity[1] == 'X') {
                    // Hex entity
                    if (sscanf(entity + 2, "%x", &n) != 1) {
                        fprintf(stderr, "Bad hex entity: `&%s;'\n", entity);
                        exit(1);
                    }
                } else {
                    // Decimal entity
                    if (sscanf(entity + 1, "%u", &n) != 1) {
                        fprintf(stderr, "Bad decimal entity: `&%s;'\n", entity);
                        exit(1);
                    }
                }
                putchar(n);
            } else {
                fprintf(stderr, "Unknown entity: `&%s;'\n", entity);
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (parse_options(argc, argv)) {
        return 1;
    }

    if (help) {
        usage(stdout);
    } else if (action_decode) {
        decode();
    } else {
        encode();
    }

    return 0;
}

