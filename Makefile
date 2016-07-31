CXX      = g++
CXXFLAGS = -Wall -std=c++11

SRCS     = Player.cpp GameObject.cpp potInfo.cpp main.cpp Node.cpp helper.cpp
OBJDIR   = obj

# Prefixes each of the object files with obj/
OBJECTS  = $(addprefix $(OBJDIR)/, Player.o GameObject.o potInfo.o main.o Node.o helper.o)
vpath %.cpp Helper
vpath %.h Helper
vpath %.cpp Nodes
vpath %.h Nodes

# Compilation of Object files
$(OBJDIR)/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Linking of files
main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@
