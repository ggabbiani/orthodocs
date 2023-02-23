# Installing `OrthoDocs`

This project uses CMake as build system, the minimum required version is
CMake 3.24.
You can configure with:

```bash
cd /path/to/OrthoDocs/sources
cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=/path/to/install/OrthoDocs
```

and build and install with:

```bash
cmake --build build --target install
```

To regenerate the source `.tar.gz` and `.zip` packages:

```bash
cmake --build build --target package_source
```
