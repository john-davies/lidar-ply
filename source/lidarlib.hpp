// lidarlib.hpp - header file for lidarlib.hpp
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

#ifndef LIDARLIB_H
#define LIDARLIB_H

#include "util.hpp"
#include <string>
#include <vector>

using namespace std;

/// Class to read LiDAR images from the website -
///  http://lle.gov.wales/catalogue/item/LidarCompositeDataset/?lang=en
///

class lidar
{
  // Header
  unsigned int ncols;
  unsigned int nrows;
  unsigned int xllcorner;
  unsigned int yllcorner;
  float cellsize;
  float NODATA_value;
  // Data
  float** values;

public:

  // File IO
  // =======
  /// Read a LiDAR image from a disk file.
  /// @param[in] fileName : path to file
  /// @return Success/fail & error message
  ///
  struct returnResult readFromFile( const string fileName );

  // Information
  // ===========
  /// Return the non-data elements of the file in a printable format
  /// @return header data
  ///
  string printHeader( void );

  // Get/set data
  // ============
  /// Get the number of columns in the LiDAR data
  /// @return Number of columns
  ///
  unsigned int getNoColumns( void );

  /// Get the number of rows in the LiDAR data
  /// @return Number of rows
  ///
  unsigned int getNoRows( void );

  /// Get the X coordinate of the lower left hand corner
  /// @return X coordinate
  ///
  unsigned int getXllcorner( void );

  /// Get the Y coordinate of the lower left hand corner
  /// @return Y coordinate
  ///
  unsigned int getYllcorner( void );

  /// Get the resolution of the data ( in m )
  /// @return Data resolution
  ///
  float getCellsize( void );

  /// Get the value that indicates that there is no data for a point
  /// @return NODATA value
  ///
  float getNODATA_value( void );

  /// Read a value from the LiDAR data
  /// @param[in] col : column number of data to be read
  /// @param[in] row : row number of data to be read
  /// @param[out] value : value at specified location
  /// @return Success/fail of read
  ///
  bool getValue( unsigned int col, unsigned int row, float &value );

  /// Write a value to the LiDAR data
  /// @param[in] col : column number of data to be written
  /// @param[in] row : row number of data to be written
  /// @param[in] value : value to be written
  /// @return Success/fail of write
  ///
  bool setValue( unsigned int col, unsigned int row, float value );

};

#endif
