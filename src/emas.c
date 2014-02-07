//  Copyright (c) 2014 Jakub Filipowicz <jakubf@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "keywords.h"
#include "parser.h"
#include "prog.h"
#include "lexer_utils.h"
#include "writers.h"

enum output_types {
	O_DEBUG	= 1,
	O_RAW	= 2,
	O_EMELF	= 3,
};

int yyparse();
int yylex_destroy();
extern FILE *yyin;

char *input_file;
char *output_file;
int otype = O_RAW;

// -----------------------------------------------------------------------
void usage()
{
	printf("Usage: emas [options] [input [output]]\n");
	printf("Where options are one or more of:\n");
	printf("   -v         : print version and exit\n");
	printf("   -h         : print help and exit\n");
	printf("   -c <cpu>   : set CPU type: mera400, mx16\n");
	printf("   -O <otype> : set output type: raw, debug, emelf (defaults to raw)\n");
}

// -----------------------------------------------------------------------
int parse_args(int argc, char **argv)
{
	int option;
	while ((option = getopt(argc, argv,"c:O:vh")) != -1) {
		switch (option) {
			case 'c':
				if (prog_cpu(optarg, CPU_FORCED)) {
					printf("Unknown cpu: '%s'.\n", optarg);
					return -1;
				}
				break;
			case 'O':
				if (!strcmp(optarg, "raw")) {
					otype = O_RAW;
				} else if (!strcmp(optarg, "debug")) {
					otype = O_DEBUG;
				} else if (!strcmp(optarg, "emelf")) {
					otype = O_EMELF;
				} else {
					printf("Unknown output type: '%s'.\n", optarg);
					return -1;
				}
				break;
			case 'h':
				usage();
				exit(0);
				break;
			case 'v':
				printf("EMAS v%s - modern MERA 400 assembler\n", EMAS_VERSION);
				exit(0);
				break;
			default:
				return -1;
		}
	}

	if (optind == argc) {
		input_file = NULL;
		output_file = NULL;
	} else if (optind == argc-1) {
		input_file = argv[optind];
		output_file = NULL;
	} else if (optind == argc-2) {
		input_file = argv[optind];
		output_file = argv[optind+1];
	} else {
		printf("Wrong usage.\n");
		return -1;
	}

	return 0;
}

// -----------------------------------------------------------------------
int main(int argc, char **argv)
{
	int ret = 1;
	int res;

	res = parse_args(argc, argv);

	if (res) {
		printf("\n");
		usage();
		goto cleanup;
	}

	if (kw_init() < 0) {
		printf("Internal dictionary initialization failed.\n");
		goto cleanup;
	}

	sym = dh_create(16000, 1);
	if (!sym) {
		printf("Failed to create symbol table.\n");
		goto cleanup;
	}

	if (input_file) {
		yyin = fopen(input_file, "r");
		loc_push(input_file);
	} else {
		yyin = stdin;
		loc_push("(stdin)");
	}

	if (!yyin) {
		printf("Cannot open source file: '%s'\n", input_file);
		goto cleanup;
	}

	if (yyparse()) {
		fclose(yyin);
		goto cleanup;
	}

	fclose(yyin);

	if (!program) { // shouldn't happen - parser should always produce program (even empty one)
		printf("Parse produced empty tree.\n");
		goto cleanup;
	}

	res = assemble(program, 0);

	if (res < 0) {
		printf("%s\n", aerr);
		goto cleanup;
	}

	if (res > 0) {
		res = assemble(program, 1);
	}

	switch (otype) {
		case O_RAW:
			if (res) {
				printf("%s\n", aerr);
				goto cleanup;
			}
			res = writer_raw(program, input_file, output_file);
			break;
		case O_DEBUG:
			if (res) {
				printf("%s\n", aerr);
				goto cleanup;
			}
			res = writer_debug(program, input_file, output_file);
			break;
		case O_EMELF:
			res = writer_emelf(program, sym, input_file, output_file);
			break;
		default:
			printf("Unknown output type.\n");
			goto cleanup;
	}

	ret = 0;

cleanup:
	yylex_destroy();
	st_drop(filenames);
	st_drop(program);
	dh_destroy(sym);
	kw_destroy();

	return ret;
}

// vim: tabstop=4 autoindent