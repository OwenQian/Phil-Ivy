CXX      = g++
CXXFLAGS = -Wall -std=c++11

SRCS     = Player.cpp GameObject.cpp potInfo.cpp main.cpp Node.cpp helper.cpp
INCLUDES = $(wildcard *.h)
OBJDIR   = obj

# Prefixes each of the object files with obj/
OBJECTS  = $(addprefix $(OBJDIR)/, Player.o GameObject.o potInfo.o main.o Node.o helper.o)

# Directories to look for headers and src files
VPATH    = Nodes:Helper

# Compilation of Object files
$(OBJDIR)/%.o: %.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c -o $@ $<


# Linking of files
main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@
