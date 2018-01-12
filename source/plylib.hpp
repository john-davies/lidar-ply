// plylib.hpp - header file for plylib
// Copyright (C) 2018 John Davies
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PLYLIB_H
#define PLYLIB_H

#include "util.hpp"
#include "ply_element.hpp"
#include <string>
#include <vector>

using namespace std;

// Structure for coordinates
struct vertexCoordinates {
  double x;
  double y;
  double z;
};

// Structure for holding bounding box for verticies
struct boundingBox {
  double minX;
  double maxX;
  double minY;
  double maxY;
  double minZ;
  double maxZ;
};

/// Class for managing PLY files. Note that this is designed for
/// importing a PLY file and manipulating it rather than creating
/// a PLY file from scratch.
///

class ply
{

protected:

  /// One of: "ascii", "binary_big_endian", "binary_little_endian"
  ///
  string format;

  /// PLY version number. Should normally always be "1.0"
  ///
  string version;

  /// Comments, array of strings, one per comment
  ///
  vector<string> comments;

  /// Array of elements, e.g. "element vertex 316", "element face 599"
  ///
  vector<plyElement*> elements;

  /// Index for quick access to element containing vertex information
  ///
  int vertexElementIndex = -1;

  /// Index for quick access to element containing face information
  ///
  int faceElementIndex = -1;

public:

  // File IO
  // =======
  /// Populate model from a PLY file.
  /// @param[in] fileName : path to file
  /// @return Success/fail & error message
  ///
  struct returnResult readFromFile( const string fileName );

  /// Write data to a PLY file
  /// @param[in] fileName : path to file
  /// @return Success/fail & error message
  ///
  struct returnResult writeToFile( const string fileName );

  // Data manipulation
  //==================
  /// Get current PLY file format
  /// @return One of "ascii", "binary_big_endian", "binary_little_endian"
  ///
  string getFormat( void );

  /// Set current PLY file format
  /// @param[in] newFormat : One of "ascii", "binary_big_endian", "binary_little_endian"
  /// @return Success/fail & error message
  ///
  struct returnResult setFormat( string newFormat );

  /// Scale the model as specified. Each one of x, y, z values is multiplied
  /// by the appropriate factor.
  /// @param[in] xScale : X axis multiplier
  /// @param[in] yScale : Y axis multiplier
  /// @param[in] zScale : Z axis multiplier
  /// @return Success/fail & error message
  ///
  struct returnResult scaleModel( string xScale, string yScale, string zScale );

  /// Change all the vertex colours to the RGB value specified. If any of the colour
  /// attributes don't exist then they are added to the model
  /// @param[in] red : red value ( 0 - 255 )
  /// @param[in] green : green value ( 0 - 255 )
  /// @param[in] blue : blue value ( 0 - 255 )
  /// @return Success/fail & error message
  ///
  struct returnResult changeAllVertexColours( string red, string green, string blue );

  /// Change the vertex colours to the RGB value.
  /// If any of the colour attributes don't exist then an error is returned
  /// @param[in] index : index of vertex to edit
  /// @param[in] red : red value ( 0 - 255 )
  /// @param[in] green : green value ( 0 - 255 )
  /// @param[in] blue : blue value ( 0 - 255 )
  /// @return Success/fail & error message
  ///
  struct returnResult changeVertexColours( unsigned int vertex, string red, string green, string blue );

  /// Duplicates an existing vertex. This is used as a quick way of copying
  /// all the property elements
  /// @param[in] index : index of data item to modify
  /// @param[out] newIndex : index of new vertex
  /// @return Success/fail & error message
  ///
  struct returnResult dupVertex( unsigned int index, unsigned int& newIndex );

  /// Change the vertex coordinates to the XYZ value.
  /// If any of the coordinate attributes don't exist then an error is returned
  /// @param[in] index : index of vertex to edit
  /// @param[in] x : x value
  /// @param[in] y : y value
  /// @param[in] z : z value
  /// @return Success/fail & error message
  ///
  struct returnResult changeVertexCoords( unsigned int vertex, string x, string y, string z );

  /// Add a new face
  /// @param[in] faceData : vector of strings containing ascii encoded vertex
  /// numbers. Note that no check is made that the verticies exist so an
  /// invalid PLY file may be created in the case of an error
  /// @return Success/fail & error message
  ///
  struct returnResult addFace( vector<string> faceData );

  // Information
  // ===========
  /// Return a printable version of the PLY header file
  /// @return String containing PLY header file
  ///
  string printHeader( void );

  /// @return The number of verticies in the current model
  ///
  unsigned int getVertexCount( void );

  /// @return The number of faces in the current model
  ///
  unsigned int getFaceCount( void );

  /// Return the data for a single face
  /// @param[in] index : index of the face element
  /// @return Vector of strings containing the ascii encoded vertex numbers
  ///
  vector<string> getFaceData( unsigned int index );

  /// Calculate the bounding box for the model
  /// @param[out] boundingBox : structure to return the values
  /// @return Success/fail & error message
  ///
  struct returnResult getBoundingBox( struct boundingBox& box );

  /// Get the XYZ coordinates for a single vertex
  /// @param[in] index : index of the vertex element
  /// @param[out] vertexCoord : structure to return the values
  /// @return Success/fail & error message
  ///
  struct returnResult getCoordinates( unsigned int index, struct vertexCoordinates& vertexCoord );

};

#endif
