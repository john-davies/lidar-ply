// ply_element_sep.hpp - header file for ply_element_sep.cpp
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

#ifndef PLY_ELEMENT_SEP_H
#define PLY_ELEMENT_SEP_H

#include "util.hpp"
#include "ply_element.hpp"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/// Derived class for storing element data of the type
/// "element vertex 316" and properties of the type "property float x"
///

class plyElementSep : public plyElement
{

  // Structure for storing property definition
  struct elementProperty {
    string name;
    string type;
  };

  // Vector to store properties
  vector<elementProperty> properties;

  // Vector to store data
  vector< vector<UINT64> > elementData;

public:
  // Constructor
  plyElementSep( const string elementName ) : plyElement( elementName ) {};
  // Base class virtual functions
  struct returnResult importDataFromFile( ifstream& inputFile, const string format );
  void writeDataToFile( ofstream& outputFile, const string format );
  string getHeader( void );
  unsigned int size( void );

  // Dervied class functions

  /// Add a property to the list, e.g. "property float x"
  /// Note that this should only be used before any data is added otherwise
  /// intenal inconsistencies may occur.
  /// @param[in] name : name of property ( x in the example )
  /// @param[in] type : type definition for the property ( float in the example )
  /// @return result : indicate success / fail
  ///
  struct returnResult addProperty( const string name, const string type );

  /// Get the data for a single entry
  /// @param[in] index : index into the list
  /// @param[in] name : name of the property ( e.g. x in the example above )
  /// @return A string containing ascii encoded data of the value
  ///
  string getData( unsigned int index, const string name );

  /// Set the value for a single data property
  /// @param[in] index : index of data item to modify
  /// @param[in] name : name of property
  /// @param[in] value : ascii encoded value for property
  ///
  void setData( unsigned int index, const string name, const string value );

  /// Duplicates an existing data item. This is used as a quick way of copying
  /// all the property elements
  /// @param[in] index : index of data item to modify
  /// @return index of new data item
  ///
  unsigned int dupVertex( unsigned int index );

  /// Appends a new data item. Note that no type checking is done and an
  /// exception is thrown if the number of values supplied don't match the
  /// number required for the definition.
  /// @param[in] values : vector of UINT64 encoded values
  /// @return index of new data item
  ///
  unsigned int addVertex( vector<UINT64> values );

};

#endif
