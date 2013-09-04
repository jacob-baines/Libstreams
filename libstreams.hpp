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
#ifndef LIBSTREAMS_HPP
#define LIBSTREAMS_HPP

#include <windows.h>
#include <string>
#include <list>

namespace streams {

/*!
	list_streams finds all streams associatied with a given file and
	stores them in the passed in list. Every file has a default stream
	"::$DATA" that is also included in the list.

	\param[in] p_file_path the path to the file to analyze
	\param[in,out] p_found a list of discovered streams (name, size) 
	\return true if we encountered no file errors. false otherwise
*/
bool list_streams(wchar_t* p_file_path, std::list<std::pair<std::wstring, __int64> >& p_found)
{
	WIN32_FIND_STREAM_DATA stream_struct;
	HANDLE stream_handle = FindFirstStreamW(p_file_path,
		FindStreamInfoStandard, &stream_struct, 0);
	if (stream_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do
	{
		p_found.push_back(
			std::make_pair(stream_struct.cStreamName, stream_struct.StreamSize.QuadPart));
	}
	while (FindNextStreamW (stream_handle, &stream_struct));

	FindClose(stream_handle);
	return true;
}

/*!
	Add an alternate stream to the provided file. The alternate stream will contain the
	contents of the second file passed in.

	\param[in] p_orig_file_path the file we want to add a stream to
	\param[in] p_file_path the file that contains the data we'll add to the stream
	\param[in] p_stream_name the name we will assign the stream (excuded the ':' and '$DATA')
	\return true if the copy was successful
*/
bool add_stream(wchar_t* p_orig_file_path, wchar_t* p_file_path, wchar_t* p_stream_name)
{
	std::wstring new_stream_path(p_orig_file_path);
	new_stream_path.append(L":");
	new_stream_path.append(p_stream_name);
	return CopyFileW(p_file_path, new_stream_path.c_str(), false) != 0;
}

/*!
	Copy a stream from the given file into a new file. If new file already
	exists we WILL overwrite it.
	
	Note that this implementation seems a little too complicated. CopyFileW
	didn't seem to work (for reasons unknown) so I did the work manually.

	\param[in] p_orig_file_path the file to read the stream from
	\param[in] p_stream_name the name of the stream to copy (full form: ":test:$DATA")
	\param[in] p_copy_to the full file path of the file we want to write the data to
	\return true if the copy was successful
*/
bool copy_stream(wchar_t* p_orig_file_path, wchar_t* p_stream_name, wchar_t* p_copy_to)
{
	// find the correct file stream
	WIN32_FIND_STREAM_DATA stream_struct;
	HANDLE stream_handle = FindFirstStreamW(p_orig_file_path,
		FindStreamInfoStandard, &stream_struct, 0);
	if (stream_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	bool found = false;
	std::wstring looking_for(p_stream_name);
	do
	{
		std::wstring compare_file(stream_struct.cStreamName);
		if (looking_for.compare(compare_file) == 0)
		{
			found = true;
		}
	}
	while (!found && FindNextStreamW (stream_handle, &stream_struct));

	// don't need this anymore
	FindClose(stream_handle);

	if (!found)
	{
		return false;
	}

	// we found the stream we wanted - open it with fopen
	std::wstring stream_path(p_orig_file_path);
	stream_path.append(p_stream_name);

	FILE* read_stream = NULL;
	if (_wfopen_s(&read_stream, stream_path.c_str(), L"rb") != 0)
	{
		return false;
	}

	// open the file we want to write to
	FILE* write_stream = NULL;
	if (_wfopen_s(&write_stream, p_copy_to, L"w+") != 0)
	{
		return false;
	}

	char data[1024] = {0};
	std::size_t data_read = 0;
	while ((data_read = fread(data, 1, sizeof(data), read_stream)) > 0)
	{
		fwrite(data, 1, data_read, write_stream);
	}

	fclose(read_stream);
	fclose(write_stream);
	return true;
}

/*!
	Delete a stream from the provided file.

	\param[in] p_file_path the file whose stream we want to delete
	\param[in] p_stream_name the name of the stream to delete (full form: ':test:$DATA')
	\return true if the deletion was successful
*/
bool delete_stream(wchar_t* p_file_path, wchar_t* p_stream_name)
{
	std::wstring stream_path(p_file_path);
	stream_path.append(p_stream_name);
	return DeleteFileW(stream_path.c_str()) != 0;
}
} //namespace
#endif
