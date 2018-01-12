// lidarimage.hpp - header file for lidarimage
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

#ifndef LIDARIMAGE_H
#define LIDARIMAGE_H

#include "util.hpp"
#include <string>
#include <vector>

using namespace std;

/// Class to read and store Imagemagic ".txt" format images.
/// File is of the format:
///   - # ImageMagick pixel enumeration: 20,20,255,srgb
///   - 0,0: (32,31,225)  #201FE1  srgb(32,31,225)
///   - ... further lines as necessary
///

class lidarImage
{

  struct rgb {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
  };

  unsigned int xSize;
  unsigned int ySize;
  unsigned int maxColourDepth;
  struct rgb** values;

public:

  /// Constructor, any errors in memory allocation will cause an exception
  /// @param[in] x : X size of image
  /// @param[in] y : Y size of image
  /// @param[in] colourDepth : colour of image
  ///
  lidarImage( unsigned int x, unsigned int y, unsigned int colourDepth );

  /// Populate the image from a disk file.
  /// @param[in] fileName : path to file
  /// @return Success/fail & error message
  ///
  struct returnResult readFromFile( string fileName );

  /// Add a vertex
  /// @param[in] x : X coordinate of pixel
  /// @param[in] y : y coordinate of pixel
  /// @param[out] red : red value of pixel
  /// @param[out] green : green value of pixel
  /// @param[out] blue : blue value of pixel
  /// @return Success/fail & error message
  ///
  struct returnResult  getPixel( unsigned int x, unsigned int y,
                 unsigned char& red, unsigned char& green, unsigned char& blue );

};

#endif
