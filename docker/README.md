# Directory Contents

This directory contains the Dockerfile(s) needed for the CI/CD of Linux distros
different from Ubuntu. The Dockerfile are used by the CI/CD workflow to
generate the required runner image onto which execute the CMake configure,
build and tests and the CPack package generation for the target Distro.

A possible future development of the concept - in order to speed up the process -
could be to store on Docker Hub or GitHub Packages the images themselves.