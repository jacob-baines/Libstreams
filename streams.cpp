/*
	Copyright (c) 2013, Jacob Baines
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met: 

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer. 
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdlib>
#include <iostream>

#include "libstreams.hpp"

static void print_usage()
{
	std::wcout << "Usage: " << std::endl;
	std::wcout << "  streams.exe <full file path> ls" << std::endl;
	std::wcout << "  streams.exe <full file path> add <full path of file to add> <name of new stream>" << std::endl;
	std::wcout << "  streams.exe <full file path> cp <name of stream to copy> <full path of the file to copy to>" << std::endl;
	std::wcout << "  streams.exe <full file path> rm <name of stream to delete>" << std::endl;
}

int wmain(int p_argc, wchar_t* p_argv[])
{
	if (p_argc < 3)
	{
		print_usage();
		return EXIT_FAILURE;
	}

	// check that we can manipulate the base file
	FILE* check_file = NULL;
	if (_wfopen_s(&check_file, p_argv[1], L"rb") != 0)
	{
		std::wcerr << "Failed to open " << p_argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	// The file is good. Close it
	fclose(check_file);

	std::size_t option_length = wcslen(p_argv[2]);
	if (option_length == 2 && wcscmp(p_argv[2], L"ls") == 0)
	{
		std::list<std::pair<std::wstring, __int64> > streams;
		if (!streams::list_streams(p_argv[1], streams))
		{
			std::cerr << "Failed to list streams." << std::endl;
			return EXIT_FAILURE;
		}

		// print the list
		for (std::list<std::pair<std::wstring, __int64> >::const_iterator iter =
				streams.begin(); iter != streams.end(); ++iter)
		{
			std::wcout << iter->first << " - " << iter->second << std::endl;
		}
	}
	else if (option_length == 2 && p_argc == 4 && wcscmp(p_argv[2], L"rm") == 0)
	{
		if (!streams::delete_stream(p_argv[1], p_argv[3]))
		{
			std::cerr << "Failed to delete stream." << std::endl;
			return EXIT_FAILURE;
		}
	}
	else if (option_length == 2 && p_argc == 5 && wcscmp(p_argv[2], L"cp") == 0)
	{
		if (!streams::copy_stream(p_argv[1], p_argv[3], p_argv[4]))
		{
			std::cerr << "Failed to copy stream." << std::endl;
			return EXIT_FAILURE;
		}
	}
	else if (option_length == 3 && p_argc == 5 && wcscmp(p_argv[2], L"add") == 0)
	{
		if (!streams::add_stream(p_argv[1], p_argv[3], p_argv[4]))
		{
			std::cerr << "Failed to add a new stream." << std::endl;
			return EXIT_FAILURE;
		}
	}
	else
	{
		print_usage();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
