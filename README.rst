OpenCL Example
==============

This is a small collection of OpenCL example to demostrate the OpenCL APIs.

Contents
--------

- vec_add: A simple vector addition in OpenCL

  - :code:`vec_add_source.c`: Host program to run :code:`vec_add.cl` from
    source.

  - :code:`vec_add_binary.c`: Host program to run :code:`vec_add.cl` from
    the pre-built binary.

  - :code:`vec_add_spir.c`: Host program to run :code:`vec_add.cl` from
    SPIR.

- fma: A simple fma program to demostrate :code:`clLinkProgram()`.

- cl-compile: A simple program to compile OpenCL to prebuilt binaries.

License
-------

The source code and documentation are hereby granted released in public domain.
