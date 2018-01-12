// util.hpp - General type declarations and hpp file for util.cpp
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

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <sstream>

using namespace std;

// General PLY format notes: http://paulbourke.net/dataformats/ply/

// PLY format type. Can be one of
// "ascii", "binary_big_endian", "binary_little_endian"
const vector<string> formatOptions = { "ascii", "binary_big_endian", "binary_little_endian" };
const vector<string> typeOptions = { "char", "uchar", "short", "ushort", "int", "uint", "float", "double" };
const string PLY = "ply";
const string FORMAT = "format";
const string COMMENT = "comment";
const string ELEMENT = "element";
const string PROPERTY = "property";
const string END_HEADER = "end_header";
const string LIST = "list";

// Structure for returning success/fail & a reason
struct returnResult {
  bool result;
  string reason;
};

// Typedef for 64 bit quantity
#define UINT64 uint64_t

// Utility function
vector<string> split( const string, char );

// Function prototypes for data conversion utilities
int getNumberOfBytes( string type );

UINT64 packAscii( string value, string type );
string unpackAscii( UINT64 value, string type );
UINT64 packBinary( vector<unsigned char> value, string type, string endian );
vector<unsigned char> unpackBinary( UINT64 value, string type, string endian );

UINT64 packCharAscii( string value );
UINT64 packCharBigEndian( vector<unsigned char> value );
UINT64 packCharLittleEndian( vector<unsigned char> value );
string unpackCharAscii( UINT64 value );
vector<unsigned char> unpackCharBigEndian( UINT64 value );
vector<unsigned char> unpackCharLittleEndian( UINT64 value );

UINT64 packShortAscii( string value );
UINT64 packShortBigEndian( vector<unsigned char> value );
UINT64 packShortLittleEndian( vector<unsigned char> value );
string unpackShortAscii( UINT64 value );
vector<unsigned char> unpackShortBigEndian( UINT64 value );
vector<unsigned char> unpackShortLittleEndian( UINT64 value );

UINT64 packIntAscii( string value );
UINT64 packIntBigEndian( vector<unsigned char> value );
UINT64 packIntLittleEndian( vector<unsigned char> value );
string unpackIntAscii( UINT64 value );
vector<unsigned char> unpackIntBigEndian( UINT64 value );
vector<unsigned char> unpackIntLittleEndian( UINT64 value );

UINT64 packFloatAscii( string value );
UINT64 packFloatBigEndian( vector<unsigned char> value );
UINT64 packFloatLittleEndian( vector<unsigned char> value );
string unpackFloatAscii( UINT64 value );
vector<unsigned char> unpackFloatBigEndian( UINT64 value );
vector<unsigned char> unpackFloatLittleEndian( UINT64 value );

UINT64 packDoubleAscii( string value );
UINT64 packDoubleBigEndian( vector<unsigned char> value );
UINT64 packDoubleLittleEndian( vector<unsigned char> value );
string unpackDoubleAscii( UINT64 value );
vector<unsigned char> unpackDoubleBigEndian( UINT64 value );
vector<unsigned char> unpackDoubleLittleEndian( UINT64 value );

#endif
