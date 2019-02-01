# Makefile
#
# date: 20160615 version: 1.0.0 update: initial 

CC = clang++
#CC = gcc

TARGET = app_cloud2
SOURCES = app_cloud2.cpp CloudInContainer.cpp WalkerInContainer.cpp Cloud.cpp Walker.cpp Vector3D.cpp
OBJECTS = $(SOURCES:.cpp=.o)
OPT = 

# gsl lib in mac
GSL_LIB = -L/usr/local/Cellar/gsl/1.16/lib -lgsl -lgslcblas
GSL_INCLUDE = -I/usr/local/Cellar/gsl/1.16/include
# gsl lib in linux
#GSL_LIB=-L/usr/lib -lgsl -lgslcblas 
#GSL_INCLUDE=-I/usr/include


all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(GSL_LIB) -o $@

.cpp.o:
	$(CC) $(GSL_INCLUDE) $(OPT) -c $< -o $@

clean:
	rm *o $(TARGET)

