DEBUG    = -O3
CXXFLAGS = $(DEBUG) $(shell pkg-config --cflags OpenEXR)
LIBS     = $(shell pkg-config --libs  OpenEXR) 
CXX      =  g++

SOURCES =  src/envmaker.cpp src/exr.cpp

OBJS = $(SOURCES:.cpp=.o)
DEPS = $(SOURCES:.cpp=.d)

TARGET = solatlong

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
all:	$(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(DEPS)

-include $(DEPS)
