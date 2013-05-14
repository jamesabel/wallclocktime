// wallclocktime.cpp
//

#include <stddef.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <string.h>
#include <time.h>

void wallclocktime_help(void)
{
	printf("usage : wallclocktime -start | -stop\n");
	printf("\n");
	printf("To use:\n");
	printf("wallclocktime -start\n");
	printf("< run your program >\n");
	printf("wallclocktime -stop\n");
}

double get_current_time(void)
{
	__int64 ctr = 0, freq = 0;
	QueryPerformanceCounter((LARGE_INTEGER *) &ctr);
	QueryPerformanceFrequency((LARGE_INTEGER *) &freq);
	return ((double) ctr) / ((double) freq);
}

void ErrorExit(DWORD LastError) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        LastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

	wprintf(L"%s", lpMsgBuf);
}

int main(int argc, char* argv[])
{
	char *special_filename = "__wct__.txt";
	double start_time = -1.0; // negative means error (didn't update)
	FILE *f = NULL;
	int open_status = -1;
	double current_time = 0.0;
	int sfa_status = -1;

	if (argc < 2)
	{
		wallclocktime_help();
	}
	else if (_stricmp(argv[1], "-start") == 0)
	{
		SetFileAttributes(special_filename, FILE_ATTRIBUTE_NORMAL); // if it already exists, make it non-hidden so we can write it
		f = NULL;
		open_status = fopen_s(&f, special_filename, "w");
		if ((f == NULL) || (open_status != 0))
		{
			printf("Error : could not open %s for writing - exiting\n", special_filename);
			exit(EXIT_FAILURE);
		}
		else
		{
			current_time = get_current_time();
			fprintf_s(f, "%f\n", current_time);
			fclose(f);
		}

		sfa_status = SetFileAttributes(special_filename, FILE_ATTRIBUTE_HIDDEN);
		if (sfa_status == 0)
		{
			printf("Warning : could not set %s to hidden\n", special_filename);
			ErrorExit(GetLastError());
		}
	}
	else if (_stricmp(argv[1], "-stop") == 0)
	{
		current_time = get_current_time();
		f = NULL;
		open_status = fopen_s(&f, special_filename, "r");
		if ((f == NULL) || (open_status != 0))
		{
			printf("Error : %s not found\n", special_filename);
			printf("You have to do a -start first\n");
			printf("\n");
			wallclocktime_help();
		}
		else
		{
			int n_scanned;
			float start_time_float = -2.0;
			n_scanned = fscanf_s(f, "%f", &start_time_float);
			// printf("%f\n", start_time_float);
			if (n_scanned > 0)
			{
				printf("%f\n", current_time - start_time_float);
			}
			fclose(f);
		}
	}
	else {
		wallclocktime_help();
	}
	return 0;
}

