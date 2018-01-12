// ply_element_sep.hpp - header file for ply_element_list.cpp
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

#ifndef PLY_ELEMENT_LIST_H
#define PLY_ELEMENT_LIST_H

#include "util.hpp"
#include "ply_element.hpp"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/// Derived class for storing element data of the type
/// "property list uchar int vertex_indices"
///

class plyElementList : public plyElement
{

  // Structure for storing list property definition
  struct listProperty {
    string listType;
    string name;
    string type;
  };

  struct listProperty property;

  // List of data items
  vector< vector<UINT64> > elementData;

public:
  // Constructor
  plyElementList( const string elementName ) : plyElement( elementName ) {};
  // Base class virtual functions
  struct returnResult importDataFromFile( ifstream& inputFile, const string format );
  void writeDataToFile( ofstream& outputFile, const string format );
  string getHeader( void );
  unsigned int size( void );

  // Derived class functions

  /// Set the properties for the list, e.g. "property list uchar int vertex_indices".
  /// Note that this should only be used once before any data is added otherwise
  /// intenal inconsistencies may occur.
  /// @param[in] listType : type definition for the index into the list ( uchar in the example )
  /// @param[in] name : name of list property ( vertex_indices in the example )
  /// @param[in] type : type definition for the list property ( int in the example )
  /// @return result : indicate success / fail
  ///
  struct returnResult setProperty( const string listType, const string name, const string type );

  /// Get the data for a single entry
  /// @param[in] index : index into the list
  /// @return A vector of strings containing ascii encoded data, data is ordered:
  /// [0] listType, [1] name, [2] type
  ///
  vector<string> getData( const unsigned int index );

  /// Append a new data entry to the element
  /// @param[in] data : a vector of ascii encoded data for the new entry, ordered:
  /// [0] listType, [1] name, [2] type
  /// @return The index of the new data entry
  ///
  unsigned int setData( const vector<string> data );

};

#endif
