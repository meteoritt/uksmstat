/*
    uksmstat — small tool to show UKSM statistics
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

#define EUTINT	1
#define EUTFILE	2
#define EUTPAGE	3
#define EUTOPT	4

void show_help()
{
	fprintf(stdout, "uksmstat - small tool to show UKSM statistics\n");
	fprintf(stdout, "© Oleksandr Natalenko aka post-factum, 2012\n");
	fprintf(stdout, "Distributed under terms and conditions of GPLv3+. See COPYING for details.\n");
	fprintf(stdout, "Usage: uksmstat <options>\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "\t-a: show whether UKSM is active\n");
	fprintf(stdout, "\t-u: show unshared memory\n");
	fprintf(stdout, "\t-s: show saved memory\n");
	fprintf(stdout, "\t-k: use kibibytes\n");
	fprintf(stdout, "\t-m: use mebibytes\n");
	fprintf(stdout, "\t-v: be verbose (up to -vv)\n");
	fprintf(stdout, "\t-h: show this help\n");
	exit(0);
}

int main(int argc, char **argv)
{
	// check if there's uksm
	struct stat sb;
	if (stat("/sys/kernel/mm/uksm", &sb) != 0 && S_ISDIR(sb.st_mode))
	{
		fprintf(stderr, "Unable to find uksm interface in /sys/kernel/mm/uksm\n");
		exit(EUTINT);
	}

	// parse cmdline options
	int opts = 0, active = 0, unshared = 0, shared = 0, kilobytes = 0, megabytes = 0, verbose = 0;
	while ((opts = getopt(argc, argv, "auskmvh")) != -1)
	{
		switch (opts)
		{
			case 'a':
				active = 1;
				break;
			case 'u':
				unshared = 1;
				break;
			case 's':
				shared = 1;
				break;
			case 'k':
				kilobytes = 1;
				break;
			case 'm':
				megabytes = 1;
				break;
			case 'v':
				verbose++;
				break;
			case 'h':
				show_help();
				break;
			default:
				fprintf(stderr, "Unknown option: %c\n", opts);
				exit(EUTOPT);
				break;
		}
	}

	if (active == 1)
	{
		FILE *f = fopen("/sys/kernel/mm/uksm/run", "r");
		if (f == NULL)
		{
			fprintf(stderr, "Unable to open run file\n");
			exit(EUTFILE);
		}
		int run;
		fscanf(f, "%d", &run);
		fclose(f);

		if (run == 1)
			fprintf(stdout, "UKSM is active\n");
		else if (run == 0)
			fprintf(stdout, "UKSM is inactive\n");
	}

	// find out page size
	long page_size = sysconf(_SC_PAGESIZE);
	if (page_size == -1)
	{
		fprintf(stderr, "Unable to get page size\n");
		exit(EUTPAGE);
	}

	// show unshared mem
	if (unshared == 1)
	{
		FILE *f = fopen("/sys/kernel/mm/uksm/pages_unshared", "r");
		if (f == NULL)
		{
			fprintf(stderr, "Unable to open pages_unshared file\n");
			exit(EUTFILE);
		}
		long pages_unshared;
		fscanf(f, "%ld", &pages_unshared);
		fclose(f);

		if (verbose == 0)
		{
			if (kilobytes == 1)
				fprintf(stdout, "%ld\n", page_size * pages_unshared / 1024);
			else if (megabytes == 1)
				fprintf(stdout, "%ld\n", page_size * pages_unshared / (1024 * 1024));
		} else if (verbose == 1)
		{
			if (kilobytes == 1)
				fprintf(stdout, "%ld KiB\n", page_size * pages_unshared / 1024);
			else if (megabytes == 1)
				fprintf(stdout, "%ld MiB\n", page_size * pages_unshared / (1024 * 1024));
		} else if (verbose == 2)
		{
			if (kilobytes == 1)
				fprintf(stdout, "Unshared pages: %ld KiB\n", page_size * pages_unshared / 1024);
			else if (megabytes == 1)
				fprintf(stdout, "Unshared pages: %ld MiB\n", page_size * pages_unshared / (1024 * 1024));
		}
	}

	// show shared (saved) mem
	if (shared == 1)
	{
		FILE *f = fopen("/sys/kernel/mm/uksm/pages_sharing", "r");
		if (f == NULL)
		{
			fprintf(stderr, "Unable to open pages_sharing file\n");
			exit(EUTFILE);
		}
		long pages_shared;
		fscanf(f, "%ld", &pages_shared);
		fclose(f);
		if (verbose == 0)
		{
			if (kilobytes == 1)
				fprintf(stdout, "%ld\n", page_size * pages_shared / 1024);
			else if (megabytes == 1)
				fprintf(stdout, "%ld\n", page_size * pages_shared / (1024 * 1024));
		} else if (verbose == 1)
		{
			if (kilobytes == 1)
				fprintf(stdout, "%ld KiB\n", page_size * pages_shared / 1024);
			else if (megabytes == 1)
				fprintf(stdout, "%ld MiB\n", page_size * pages_shared / (1024 * 1024));
		} else if (verbose == 2)
		{
			if (kilobytes == 1)
				fprintf(stdout, "Shared pages: %ld KiB\n", page_size * pages_shared / 1024);
			else if (megabytes == 1)
				fprintf(stdout, "Shared pages: %ld MiB\n", page_size * pages_shared / (1024 * 1024));
		}
	}

	return 0;
}

