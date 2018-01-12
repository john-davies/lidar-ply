Description {#mainpage}
===========

Classes to handle two data sets:

# PLY files

PLY file format definition:

* <https://en.wikipedia.org/wiki/PLY_(file_format)>
* http://paulbourke.net/dataformats/ply/

Classes:

* ply
* plyElement - base class
* plyElementList - derived from plyElement
* plyElementSep - derived from plyElement

# LiDAR files

LiDAR files from:

*                http://lle.gov.wales/catalogue/item/LidarCompositeDataset/?lang=en

Classes

* lidar
* lidarImage - class to store Imagemagick ".txt" format files
* lidarply - derived class from ply ( above ) which makes it easier to create PLY files programmatically rather than reading from disk
