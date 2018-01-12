// ply_element.hpp - header file for ply_element.cpp
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
//

#ifndef PLY_ELEMENT_H
#define PLY_ELEMENT_H

#include "util.hpp"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/// Base class for storing the data for each element type
/// e.g. "element vertex 650" or "element face 1236"
/// This class will not be used directly because different elements
/// will need to be processed differently.
///

class plyElement
{

protected:
  /// Name of the element
  ///
  string name;

  /// Number of items to be read if "importDataFromFile" is called.
  /// Note that this value is what is to be read from the file and is only used
  /// for initial reading of the file. It is not updated if the element
  /// is subsequently edited.
  ///
  int count;

public:
  /// Class constructor
  /// @param[in] elementName : name of the element, will be used when writing the
  /// output PLY file.
  ///
  plyElement( const string elementName );

  /// Get Name
  /// @return The name of the element
  ///
  string getName( void );

  /// Set the count value
  /// @param[in] value : value of count ( see note about count usage )
  ///
  void setCount( const int value );

  /// Get the count value
  /// @return The value of "count". However see note about how "count" is
  /// set and the "size()" method should probably be used instead.
  ///
  int getCount( void );

  // Virtual functions for derived classes
  /// Populate the element with data from a file.
  /// @param[in,out] inputFile : File pointer to where the data is to be read from
  /// @param[in] format : format of output file, one of "ascii", "binary_big_endian",
  /// "binary_little_endian"
  ///
  virtual struct returnResult importDataFromFile( ifstream& inputFile, const string format ) = 0;

  /// Write the element data to a file
  ///
  virtual void writeDataToFile( ofstream& outputFile, const string format ) = 0;

  /// Get the element header in a text format that is suitable for printing
  /// in the header of the PLY file, e.g. "element vertex 650"
  ///
  virtual string getHeader( void ) = 0;

  /// Get the actual size of the element. This should probably be used in
  /// preference to "getCount()"
  ///
  virtual unsigned int size( void ) = 0;

};

#endif
