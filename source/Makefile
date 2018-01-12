CC = g++
CFLAGS  = -Wall -std=gnu++11 -g

all: plymenu lidar2ply

# ----------------------------------------------------------------------------
# Sample applications

plymenu: plymenu.o
	$(CC) $(CFLAGS) -o plymenu plymenu.o plylib.o ply_element.o ply_element_sep.o ply_element_list.o util.o

plymenu.o: plymenu.cpp plylib.o
	$(CC) $(CFLAGS) -c plymenu.cpp

lidar2ply: lidar2ply.o lidarimage.o
	$(CC) $(CFLAGS) -o lidar2ply lidar2ply.o lidarlib.o plylib.o ply_element.o ply_element_sep.o ply_element_list.o util.o lidarply.o lidarimage.o

lidar2ply.o: lidar2ply.cpp lidarlib.o plylib.o lidarply.o
		$(CC) $(CFLAGS) -c lidar2ply.cpp

# ----------------------------------------------------------------------------
# Utilities

util.o: util.cpp util.hpp
	$(CC) $(CFLAGS) -c util.cpp

# ----------------------------------------------------------------------------
# PLY Library

plylib.o: plylib.cpp plylib.hpp ply_element.o ply_element_sep.o ply_element_list.o util.o
	$(CC) $(CFLAGS) -c plylib.cpp

ply_element.o: ply_element.cpp ply_element.hpp util.o
		$(CC) $(CFLAGS) -c ply_element.cpp

ply_element_sep.o: ply_element_sep.cpp ply_element_sep.hpp util.o
		$(CC) $(CFLAGS) -c ply_element_sep.cpp

ply_element_list.o: ply_element_list.cpp ply_element_list.hpp util.o
		$(CC) $(CFLAGS) -c ply_element_list.cpp


# ----------------------------------------------------------------------------
# Basic PLY

lidarply.o: lidarply.cpp lidarply.hpp
	$(CC) $(CFLAGS) -c lidarply.cpp

# ----------------------------------------------------------------------------
# LiDAR Library

lidarlib.o: lidarlib.cpp lidarlib.hpp plylib.o
	$(CC) $(CFLAGS) -c lidarlib.cpp

lidarimage.o: lidarimage.cpp lidarimage.hpp util.o
	$(CC) $(CFLAGS) -c lidarimage.cpp


# ----------------------------------------------------------------------------
# Clean up

clean:
	rm *.o

# ----------------------------------------------------------------------------
