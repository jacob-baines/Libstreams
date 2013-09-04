#Streams
A simple tool to list, add, remove, and copy NTFS alternate data streams. All the functionality of the tool is found in the header file for easy integration into any project.

## Command Descriptions
Streams supports four commands: ls, add, cp, and rm.
**ls**: lists all the streams associated with the specified file. Streams are listed in the format <name> - <size>
**add**: adds a new stream to the specified file using a file and name given.
**cp**: copies the specified stream to a file
**rm**: deletes a stream in the specified file

## Example
```
C:\>streams C:\octocat.jpg ls
::$DATA - 5763
:Zone.Identifier:$DATA - 26

C:\>streams C:\octocat.jpg add C:\hello.txt new_stream

C:\>streams C:\octocat.jpg ls
::$DATA - 5763
:new_stream:$DATA - 7
:Zone.Identifier:$DATA - 26

C:\>streams C:\octocat.jpg cp :new_stream:$DATA C:\stream_file.txt

C:\>streams C:\octocat.jpg rm :new_stream:$DATA

C:\>streams C:\octocat.jpg ls
::$DATA - 5763
:Zone.Identifier:$DATA - 26
```

## License
FreeBSD. See license.
