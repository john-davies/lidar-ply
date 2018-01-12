// basicply.hpp - header file for basicply
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

#ifndef LIDARPLY_H
#define LIDARPLY_H

#include "plylib.hpp"
#include "util.hpp"
#include "ply_element.hpp"
#include <string>
#include <vector>

using namespace std;

/// Derived class to make creating PLY files easier. The vertex and face
/// elements have the following properties:
///   - Vertex
///     - XYZ coordinates as float types
///     - RBG values as int types
///     - XYZ normals as float types
///   - Face
///     - List type as int
///     - List name - "vertex_index"
///     - Data element type as int
///

class lidarply : public ply
{

public:

  /// Class constructor, creates blank vertex and face elements
  ///
  lidarply( void );

  /// Add a vertex
  /// @param[in] x : X coordinate of new vertex
  /// @param[in] y : y coordinate of new vertex
  /// @param[in] z : z coordinate of new vertex
  /// @param[in] red : red component of vertex colour
  /// @param[in] green : green component of vertex colour
  /// @param[in] blue : blue component of vertex colour
  /// @return The index of the new vertex
  ///
  unsigned int addVertex( float x, float y, float z, unsigned int red, unsigned int green, unsigned int blue );

};

#endif
