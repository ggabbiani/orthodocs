# Installing OrthoDocs

This project uses CMake as build system, the minimum required version is CMake 3.24.

## Per-user installation

For installing OrthoDocs in user-domain proceed as below:

```bash
# change directory where this repo is located
cd /path/to/OrthoDocs/sources
# configure the build
cmake -S . -B build
# build and install
cmake --build build --target install
```

To generate source `.tar.gz` and `.zip` packages:

```bash
cmake --build build --target package_source
```

With this kind of installation remember that:

- no admin privilege is required for [un]install
- the `orthodocs` command will be available to the installer user only
- system-domain is untouched

Even if thought as a temporary installation for developers, there is nothing wrong using this type of installation as 'final'. Only remember that  - depending on the hosting system type - it may be necessary to manually add the BINDIR (the directory containing the executables) to the user command path as described in the table below:

| Os        | installation prefix           | BINDIR                                                |
| --------- | ----------------------------- | ----------------------------------------------------- |
| Linux     | $HOME/.local [^1]             | -                                                     |
| macOS     | $HOME/.local [^2]             | add in command path modifying the user shell profile  |
| Windows   | %LOCALAPPDATA%\OrthoDocs [^4] | add to the user PATH environment variable             |

## System-wide installation

For a system-wide installation use the provided packages on site or generate the OS dependent package by yourself typing the following commands:

```bash
# change directory where this repo is located
cd /path/to/OrthoDocs/sources
# configure the build
cmake -S . -B build
# build and generate the package
cmake --build build --target package
```

The OS dependent packages will use as installation prefix one of the following:

| Os        | installation prefix           |
| --------- | ----------------------------- |
| Linux     | /usr [^1]                     |
| macOS     | /opt/OrthoDocs [^3]           |
| Windows   | %PROGRAMFILES%\Orthodocs [^5] |

With this kind of installation remember that:

- admin privileges are required
- the `orthodocs` command will be available to all users
- system-domain is modified in both the filesystem and - depending on the target OS - the system path

[^1]: on Linux systems `$HOME/.local/bin` and `/usr/bin` are assumed to be in system path **by default**

[^2]: on macOS systems `$HOME/.local/bin` is not in user path, to use `orthodocs` either call its full path or modify the **user default-shell profile**

[^3]: on macOS `/opt/OrthoDocs` is not in system path, the installer will modify system PATH populating the `/etc/paths.d/` drop-in directory and refreshing the system by calling the `path_helper`

[^4]: On Windows %LOCALAPPDATA% corresponds to `C:\Users\{user name}\AppData\Local`. The bin directory `C:\Users\{user name}\AppData\Local\OrthoDocs\bin` will be set in **user path**

[^5]: On Windows %PROGRAMFILES% corresponds to `C:\Program Files`. The bin directory `C:\Program Files\OrthoDocs\bin` will be set in **system path**
