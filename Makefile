CXX      = g++
CXXFLAGS = -Wall -std=c++11 -g

VPATH    = Nodes:GameUtilities

SRCS     = Player.cpp GameObject.cpp potInfo.cpp Node.cpp helper.cpp \
ChoiceNode.cpp OpponentNode.cpp GameUtilities.cpp main.cpp
INCLUDES = $(wildcard *.h)
OBJDIR   = obj

# Prefixes each of the object files with obj/
OBJECTS  = $(addprefix $(OBJDIR)/, Player.o GameObject.o \
potInfo.o Node.o helper.o ChoiceNode.o GameUtilities.o OpponentNode.o main.o )

# Directories to look for headers and src files

# Compilation of Object files
$(OBJDIR)/%.o: %.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Linking of files
main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f obj/*.o
