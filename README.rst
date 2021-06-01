MeshLink Library
========================

(Copyright) 2021 Cadence Design Systems, Inc.  All rights reserved worldwide.

Introduction
~~~~~~~~~~~~

The MeshLink Library provides mesh-geometry associativity in an open, geometry
kernel neutral, framework. The library is written using the C++ object-oriented
programming model, but bindings for C, FORTRAN, and Python 3 languages are also
available.

The public API was designed with a mesh-centric focus as that is the *common
natural language* of third-party simulation software systems.  The MeshLink
API provides a common interface for creating, modifying, and querying
geometry-mesh associativity using mesh nomenclature. 

**The MeshLink Schema was developed as part of NASA Contract 80NSSC18C0109.**

**The MeshLink Library was developed as part of U.S. Air Force Contract FA9101-18-P-0042.**

Geometry Association
--------------------

The term geometry association is used to denote the relationship between a mesh
element and the supporting computational geometry used to constrain the
location of the mesh element in Cartesian space.  

Mesh-geometry associativity is defined by the mesh generation software in the
MeshLink XML file.  The MeshLink XML file utilizes the MeshLink Schema (XSD) to
link mesh elements to geometry elements in an application defined manner.  An
example implementation of MeshLink XML file export has been added to Pointwise
version 18.3R1.

Mesh associativity to the geometry is defined in terms of mesh elements within
mesh topology containers.

Mesh Topology
-------------

Mesh topology refers to 1D, 2D, and 3D constructs within the mesh heirarchy.

**Mesh String**: A 1D collection of mesh edges.  A Mesh String may reference
one or more geometry curves.

**Mesh Sheet**: A 2D collection of mesh faces.  A Mesh Sheet may reference one
or more geometry surfaces.

**Mesh Model**: A 3D mesh volume.  A Mesh Model is a container for Mesh String
and Mesh Sheet elements used in the definition of the volume mesh.

.. image:: https://raw.github.com/pointwise/MeshLink/master/doc/meshlink_topology.png

Geometry Kernel
---------------

The MeshLink Library is meant to be compatible with a number of 3rd party
geometry kernels.  Only a few basic of functions must be supported by a
geometry kernel in order to be useful for computational work with MeshLink.  A
geometry kernel must be able to read the geometry data file and to support
computational evaluations on the geometric entities contained in the file.
Typical computational operations are closest point projection and parametric
evaluation.

Read/Write
----------

New in MeshLink v1.1 is the ability to write a MeshLink XML file. This provides
an application the ability to read and process mesh and geometry files and
update the associativity as needed. Mesh that has been adapted can
then be re-used later by the same or other MeshLink-aware applications
without the need to re-do transformitive mesh operations.

Getting Started
~~~~~~~~~~~~~~~~

Dependencies
------------

The following are required to build and run the MeshLink library:

* C++ compiler
* XML parser
    * Apache Xerces example provided
* 3rd party geometry kernel
    * Reference implementation built and tested with `Project Geode`_.


+----------+------------------------+---------------------+
|Platform  |Tested C/C++ Compilers  |Tested XML Parsers   |
+==========+========================+=====================+
|Windows   |Visual Studio 2017      |Apache Xerces 3.2.2  |
+----------+------------------------+---------------------+
|Linux     |gcc 4.7.2               |Apache Xerces 3.2.2  |
+----------+------------------------+---------------------+
|Mac OS    |clang 8.0.0             |Apache Xerces 3.2.2  |
+----------+------------------------+---------------------+


Building
--------

The MeshLink Library consists of the main API in the src/meshlink directory and
the reference MeshLink XML file parser implementation in src/mlparser_xerces
and the reference geometry kernel implementation in src/mlkernel_geode.

Makefiles for Linux and Mac OSX and Microsoft Visual Studio project files are
provided for the x86_64 architecture. 

An example build procedure:

.. code:: bash

    cd <path_to_src_dir>/meshlink
    make -f Makefile.<platform> BUILD=Release 

    cd <path_to_src_dir>/mlparser_xerces
    make -f Makefile.<platform> BUILD=Release 

    cd <path_to_src_dir>/mlkernel_geode
    make -f Makefile.<platform> BUILD=Release 

To build the MeshLink library and Xerces parser implementation on Windows with Visual Studio 2017:

.. code:: bash

    cd <path_to_src_dir>/meshlink
    devenv meshlink.sln /build "LibraryRelease|x64"


Testing
-------

Test harness applications are provided for C++ (app/harness_cpp), C
(app/harness_c), and FORTRAN (app/harness_f) bindings.

Makefiles for Linux and Mac OSX and Microsoft Visual Studio project files are
provided for the x86_64 architecture. 

The src directory contains a top-level Makefile with targets for building the
test applications and executing the test on Linux and Mac OSX platforms. Note
that all test harnesses require access to the Geode geometry kernel (not provided).

To build a harness and execute the test:

.. code:: bash

    cd <path_to_src_dir>
    make BUILD=Release test_harness_cpp

To build all the Geode/Xerces test harnesses on Windows with Visual Studio 2017:

.. code:: bash

    cd <path_to_src_dir>/meshlink
    devenv meshlink.sln /build "GeodeTestRelease|x64"


A successful test will parse the example MeshLink XML file and print details of
the loaded mesh-geometry associativity data.  If building with the Pointwise
Project Geode geometry kernel, additional tests of the computational geometry
access routines will be performed.


Reference
~~~~~~~~~

* `Project Geode`_
* `Apache Xerces`_
* `Python 3`_
* `XML Schema`_

Disclaimer
~~~~~~~~~~

This file is licensed under the Cadence Public License Version 1.0 (the
"License"), a copy of which is found in the included file named
LICENSE, and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY
APPLICABLE LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE
LIABLE TO ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF
THIS FILE.  Please see the License for the full text of applicable terms.


.. _Project Geode: https://www.pointwise.com/geode
.. _Apache Xerces: https://xerces.apache.org
.. _Python 3: https://www.python.org
.. _XML Schema: https://www.w3.org/TR/xmlschema-0
