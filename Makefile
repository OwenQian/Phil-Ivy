CXX      = g++
CXXFLAGS = -Wall -std=c++11 -g
#CXXFLAGS = -Wall -std=c++11 -O2

VPATH    = Nodes:GameUtilities:handEval

SRCS     = Player.cpp GameObject.cpp potInfo.cpp Node.cpp helper.cpp \
ChoiceNode.cpp OpponentNode.cpp GameUtilities.cpp main.cpp arrays.cpp \
fast_eval.cpp Decision.cpp

INCLUDES = $(wildcard *.h)
OBJDIR   = obj

# Prefixes each of the object files with obj/
OBJECTS  = $(addprefix $(OBJDIR)/, Player.o GameObject.o potInfo.o Node.o \
		   helper.o ChoiceNode.o GameUtilities.o OpponentNode.o arrays.o \
		   fast_eval.o Decision.o main.o )

# Directories to look for headers and src files

# Compilation of Object files
$(OBJDIR)/%.o: %.cpp $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Linking of files
main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f obj/*.o
