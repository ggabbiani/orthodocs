# Installing `OrthoDocs`

This project uses CMake as build system, the minimum required version is CMake 3.24.

## User-domain installation

CMake installation of OrthoDocs is configured by default in user-domain:

| Os        | installation prefix           |
| --------- | ----------------------------- |
| Linux     | $HOME/.local [^1]             |
| macOS     | $HOME/.local [^2]             |
| Windows   | %LOCALAPPDATA%\OrthoDocs [^4] |

With this kind of installation remember:

- no admin privilege is required for [un]install
- the `orthodocs` command is available to the installer user only
- system-domain is left untouched

Even if thought as a temporary installation for developers, there is nothing wrong using this type of installation as 'final'. Only remember that  - depending on the hosting system type - it may be necessary to modify the user profile for having the `orthodocs` command in path.

If you think that this kind of installation is suitable for your needs, open a shell in your target system (Linux, macOS or Windows) and proceed as below:

```bash
# change directory where this repo is located
cd /path/to/OrthoDocs/sources
# configure the build
cmake -H . -B build
# build and install
cmake --build build --target install
```

to generate source `.tar.gz` and `.zip` packages:

```bash
cmake --build build --target package_source
```

## System-domain installation

For a system-wide installation, besides modifying the installation prefix of the previous cmake invocation, you can:

- use the provided packages on site
- generate the OS dependent package by yourself.

The OS dependent packages will use as installation prefix one of the following:

| Os        | installation prefix           |
| --------- | ----------------------------- |
| Linux     | /usr [^1]                     |
| macOS     | /opt/OrthoDocs [^3]           |
| Windows   | %PROGRAMFILES%\Orthodocs [^5] |

With this kind of installation remember that:

- admin privileges are required
- the `orthodocs` command is available to all users
- system-domain is modified in both the filesystem and - depending on the target OS - the system path

To generate the OS dependant package usable for system-wide installation open a shell in your target system (Linux, macOS or Windows) and type the following:

```bash
# change directory where this repo is located
cd /path/to/OrthoDocs/sources
# configure the build
cmake -H . -B build
# build and generate the package
cmake --build build --target package
```

[^1]: on Linux systems `$HOME/.local/bin` and `/usr/bin` are assumed to be in system path **by default**

[^2]: on macOS systems `$HOME/.local/bin` is not in user path, the installer will modify the **user default-shell profile**

[^3]: on macOS `/opt/OrthoDocs` is not in system path, the installer will modify system PATH populating the `/etc/paths.d/` drop-in directory

[^4]: On Windows %LOCALAPPDATA% corresponds to `C:\Users\{user name}\AppData\Local`. The bin directory `C:\Users\{user name}\AppData\Local\OrthoDocs\bin` will be set in **user path**

[^5]: On Windows %PROGRAMFILES% corresponds to `C:\Program Files`. The bin directory `C:\Program Files\OrthoDocs\bin` will be set in **system path**
