/*
    uksmctl — small tool to control UKSM
    Copyright © 2012 Oleksandr Natalenko aka post-factum <pfactum@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>

#define UKSMDIR		"/sys/kernel/mm/uksm"
#define UKSMRUN		UKSMDIR"/run"

void show_help()
{
	fprintf(stdout, "uksmctl - small tool to control UKSM statistics\n");
	fprintf(stdout, "© Oleksandr Natalenko aka post-factum, 2012\n");
	fprintf(stdout, "Distributed under terms and conditions of GPLv3+. See COPYING for details.\n");
	fprintf(stdout, "Usage: uksmctl <options>\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "\t-a: activate UKSM\n");
	fprintf(stdout, "\t-d: deactivate UKSM\n");
	fprintf(stdout, "\t-s: toggle UKSM state\n");
	fprintf(stdout, "\t-v: be verbose (up to -vv)\n");
	fprintf(stdout, "\t-h: show this help\n");
	exit(EX_OK);
}

int main(int argc, char **argv)
{
	// define vars
	int opts = 0, activate = 0, deactivate = 0, toggle = 0, verbose = 0;
	struct stat sb;
	FILE *f;

	// check root privileges
	if (0 != getuid())
	{
		fprintf(stderr, "You have to be root in order to use uksmctl\n");
		exit(EX_NOPERM);
	}

	// check if there's uksm
	if (0 != stat(UKSMDIR, &sb) && S_ISDIR(sb.st_mode))
	{
		fprintf(stderr, "Unable to find uksm interface in %s\n", UKSMDIR);
		exit(EX_OSFILE);
	}

	// parse cmdline options
	while (-1 != (opts = getopt(argc, argv, "adsvh")))
	{
		switch (opts)
		{
			case 'a':
				activate = 1;
				break;
			case 'd':
				deactivate = 1;
				break;
			case 's':
				toggle = 1;
				break;
			case 'v':
				verbose++;
				if (verbose > 2)
				{
					fprintf(stderr, "Invalid -v switches count\n");
					exit(EX_USAGE);
				}
				break;
			case 'h':
				show_help();
				break;
			default:
				fprintf(stderr, "Unknown option: %c\n", opts);
				exit(EX_USAGE);
				break;
		}
	}

	// activate UKSM
	if (1 == activate)
	{
		f = fopen(UKSMRUN, "w");
		if (NULL == f)
		{
			fprintf(stderr, "Unable to open run file\n");
			exit(EX_OSFILE);
		}
		fprintf(f, "%d", 1);
		fclose(f);

		switch (verbose)
		{
			case 1:
				fprintf(stdout, "1\n");
				break;
			case 2:
				fprintf(stdout, "UKSM activated\n");
		}

		exit(EX_OK);
	}

	// deactivate UKSM
	if (1 == deactivate)
	{
		f = fopen(UKSMRUN, "w");
		if (NULL == f)
		{
			fprintf(stderr, "Unable to open run file\n");
			exit(EX_OSFILE);
		}
		fprintf(f, "%d", 0);
		fclose(f);

		switch (verbose)
		{
			case 1:
				fprintf(stdout, "0\n");
			case 2:
				fprintf(stdout, "UKSM deactivated\n");
		}

		exit(EX_OK);
	}

	// toggle UKSM state
	if (1 == toggle)
	{
		f = fopen(UKSMRUN, "r");
		if (NULL == f)
		{
			fprintf(stderr, "Unable to open run file\n");
			exit(EX_OSFILE);
		}
		unsigned int run = 0;
		fscanf(f, "%d", &run);
		fclose(f);
		
		switch (run)
		{
			case 0:
				run = 1;
				switch (verbose)
				{
					case 2:
						fprintf(stdout, "UKSM was inactive, activating\n");
						break;
				}
				break;
			case 1:
				run = 0;
				switch (verbose)
				{
					case 2:
						fprintf(stdout, "UKSM was active, deactivating\n");
						break;
				}
				break;
		}

		f = fopen(UKSMRUN, "w");
		if (NULL == f)
		{
			fprintf(stderr, "Unable to open run file\n");
			exit(EX_OSFILE);
		}
		fprintf(f, "%d", run);
		fclose(f);

		switch (run)
		{
			case 0:
				switch (verbose)
				{
					case 1:
						fprintf(stdout, "0\n");
						break;
					case 2:
						fprintf(stdout, "UKSM deactivated\n");
						break;
				}
				break;
			case 1:
				switch (verbose)
				{
					case 1:
						fprintf(stdout, "1\n");
						break;
					case 2:
						fprintf(stdout, "UKSM activated\n");
						break;
				}
				break;
		}

		exit(EX_OK);
	}

	return EX_OK;
}

