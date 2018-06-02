# PLY & LiDAR Libraries

This repository contains code to assist in the processing of PLY format files and
LiDAR files. There are classes to handle the two data sets as follows:

## PLY

PLY file format definition:

* <https://en.wikipedia.org/wiki/PLY_(file_format)>
* http://paulbourke.net/dataformats/ply/

Classes:

* ply
* plyElement - base class
* plyElementList - derived from plyElement
* plyElementSep - derived from plyElement

## LiDAR

LiDAR files from:

* http://lle.gov.wales/catalogue/item/LidarCompositeDataset/?lang=en

Classes

* lidar
* lidarImage - class to store Imagemagick ".txt" format files
* lidarply - derived class from ply ( above ) which makes it easier to create PLY files programmatically rather than reading from disk

## Example executables

There are two example executables:

### plymenu

This is a menu driven command line program that allows the user to read in and perform certain edits to a PLY file. Its main features are:

* Simple edits, e.g. rescale the model, change all vertex colours, change PLY file type
* Simple hole filling algorthim mainly for use with 3D models created by photgrammetry and their preparation for 3D printing.

V1.1 - new commands to:
* Rescale the model in the Z direction only
* Allow the base to be an absolute size when filling holes

### lidar2ply

This is a command line program to convert the raw LiDAR data files to PLY files. There are options to add an image overlay and to create mesh from the imported points. Single or multiple files can be processed.

Current missing functionality:

* The meshing option ( -m ) will not mesh multiple files correctly.

V1.1:
* Bug fix to handle case insensitivity in .asc files
* "autofill missing data" ( -a ) option removed

## Build instructions

Running "make" will build the libraries and the two executables. The only dependency is that a C++11 compiler is needed.

Some Doxygen based documentation can be created using the following command

    doxygen Doxyfile

## Sample files

In the "samples" directory there are a few sample files for testing / experimentation:

* apple.ply, sphere.ply - PLY files with holes for testing the hole filling algorithm
* test20x20.asc, image.png, image.txt - low resolution ( 20x20 points ) LiDAR test file

## Batch files

The "batch" directory contains an example of how to automate the creation of PLY models containing multiple LiDAR files. Also contains options for overlaying image files or Open Street Map data.

Note: the Open Street Map options uses a 3rd party renderer for creating a PNG image from OSM data. Here I use Ceyx - https://wiki.openstreetmap.org/wiki/Ceyx - and this needs to be downloaded and installed.

The batch file is controlled using a configuration file and a sample file is included.
