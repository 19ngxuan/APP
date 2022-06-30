# Makefile for ec_object-oriented, February 24, 2022, Author: Ulrich Eisenecker

CXX = g++
#CXXFLAGS = -std=c++20 -I/Users/ulrich/Catch2-2.x/single_include/catch2
CXXFLAGS = -std=c++20 
CXXFLAGS = -std=c++2a
LDLIBS = -lfmt
SRCDIR = src
OBJDIR = obj

# Declare "doc" and "clean" as phony
.PHONY: doc clean

# Rule 1: Define target "all" with dependencies "eco" and "ect"
all: eco ecg ect

# Rule 2: Define target "eco" with dependency "$(OBJDIR)/eco"
eco: $(OBJDIR)/eco

# Rule 3: Define target "ecg" with dependency "$(OBJDIR)/ecg"
ecg: $(OBJDIR)/ecg

# Rule 4: Define target "ect" with dependency "$(OBJDIR)/ect"
ect: $(OBJDIR)/ect

# Rule 5: Link "eco"
$(OBJDIR)/eco: $(OBJDIR)/ec_cli.o $(OBJDIR)/ec_helper.o \
$(OBJDIR)/ec_tokenizer.o $(OBJDIR)/ec_rules.o \
$(OBJDIR)/ec_knowledgebase.o $(OBJDIR)/ec_parser.o \
$(OBJDIR)/ec_inferenceengine.o \
$(OBJDIR)/ec_console.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $(OBJDIR)/eco \
	$(OBJDIR)/ec_cli.o \
	$(OBJDIR)/ec_helper.o \
	$(OBJDIR)/ec_tokenizer.o \
	$(OBJDIR)/ec_rules.o \
	$(OBJDIR)/ec_knowledgebase.o \
	$(OBJDIR)/ec_parser.o \
	$(OBJDIR)/ec_inferenceengine.o \
	$(OBJDIR)/ec_console.o

# Rule 6: Link "ecg"
$(OBJDIR)/ecg: $(OBJDIR)/ec_gui.o $(OBJDIR)/ec_helper.o \
$(OBJDIR)/ec_tokenizer.o $(OBJDIR)/ec_rules.o \
$(OBJDIR)/ec_knowledgebase.o $(OBJDIR)/ec_parser.o \
$(OBJDIR)/ec_inferenceengine.o \
$(OBJDIR)/ec_fltkgui.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) \
	-L/usr/local/lib -lfltk \
	-lfltk_images -lfltk_jpeg -lfltk_png \
	-lfltk -lz \
	-lpthread -framework Cocoa \
	-o $(OBJDIR)/ecg \
	$(OBJDIR)/ec_helper.o \
	$(OBJDIR)/ec_tokenizer.o \
	$(OBJDIR)/ec_rules.o \
	$(OBJDIR)/ec_knowledgebase.o \
	$(OBJDIR)/ec_parser.o \
	$(OBJDIR)/ec_inferenceengine.o \
	$(OBJDIR)/ec_console.o \
	$(OBJDIR)/ec_fltkgui.o \
	$(OBJDIR)/ec_gui.o

# Rule 7: Compile ec_fltkgui.cpp
$(OBJDIR)/ec_fltkgui.o: $(SRCDIR)/ec_fltkgui.hpp \
$(SRCDIR)/ec_fltkgui.cpp \
$(SRCDIR)/ec_helper.hpp \
$(SRCDIR)/ec_tokenizer.hpp \
$(SRCDIR)/ec_rules.hpp \
$(SRCDIR)/ec_knowledgebase.hpp \
$(SRCDIR)/ec_parser.hpp \
$(SRCDIR)/ec_inferenceengine.hpp \
$(SRCDIR)/ec_interface.hpp \
$(SRCDIR)/ec_console.hpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_fltkgui.o -c $(SRCDIR)/ec_fltkgui.cpp

# Rule 8: Compile ec_cli.cpp
$(OBJDIR)/ec_cli.o: $(SRCDIR)/ec_cli.cpp \
$(SRCDIR)/ec_helper.hpp \
$(SRCDIR)/ec_tokenizer.hpp \
$(SRCDIR)/ec_rules.hpp \
$(SRCDIR)/ec_knowledgebase.hpp \
$(SRCDIR)/ec_parser.hpp \
$(SRCDIR)/ec_inferenceengine.hpp \
$(SRCDIR)/ec_interface.hpp \
$(SRCDIR)/ec_console.hpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_cli.o -c $(SRCDIR)/ec_cli.cpp

# Rule 9: Compile ec_helper.cpp
$(OBJDIR)/ec_helper.o: $(SRCDIR)/ec_helper.hpp \
$(SRCDIR)/ec_helper.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_helper.o \
	-c $(SRCDIR)/ec_helper.cpp

# Rule 10: Compile ec_tokenizer.cpp
$(OBJDIR)/ec_tokenizer.o: $(SRCDIR)/ec_tokenizer.hpp \
$(SRCDIR)/ec_tokenizer.cpp 
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_tokenizer.o \
	-c $(SRCDIR)/ec_tokenizer.cpp 

# Rule 11: Compile ec_rules.cpp
$(OBJDIR)/ec_rules.o: $(SRCDIR)/ec_rules.hpp \
$(SRCDIR)/ec_rules.cpp \
$(SRCDIR)/ec_tokenizer.hpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_rules.o \
	-c $(SRCDIR)/ec_rules.cpp

# Rule 12: Compile ec_knowledgebase.cpp
$(OBJDIR)/ec_knowledgebase.o: $(SRCDIR)/ec_knowledgebase.hpp \
$(SRCDIR)/ec_knowledgebase.cpp \
$(SRCDIR)/ec_rules.hpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_knowledgebase.o \
	-c $(SRCDIR)/ec_knowledgebase.cpp

# Rule 13: Compile ec_parser.cpp
$(OBJDIR)/ec_parser.o: $(SRCDIR)/ec_parser.cpp \
$(SRCDIR)/ec_parser.hpp  $(SRCDIR)/ec_tokenizer.hpp\
$(SRCDIR)/ec_knowledgebase.hpp \
$(SRCDIR)/ec_helper.hpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_parser.o \
	-c $(SRCDIR)/ec_parser.cpp 

# Rule 14: Compile ec_inferenceengine.cpp
$(OBJDIR)/ec_inferenceengine.o: $(SRCDIR)/ec_inferenceengine.cpp \
$(SRCDIR)/ec_inferenceengine.hpp  \
$(SRCDIR)/ec_knowledgebase.hpp \
$(SRCDIR)/ec_interface.hpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_inferenceengine.o \
	-c $(SRCDIR)/ec_inferenceengine.cpp 

# Rule 15: Compile ec_console.cpp
$(OBJDIR)/ec_console.o: $(SRCDIR)/ec_interface.hpp \
$(SRCDIR)/ec_console.hpp  \
$(SRCDIR)/ec_console.cpp 
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_console.o \
	-c $(SRCDIR)/ec_console.cpp 

# Rule 16: Compile ec_gui.cpp
$(OBJDIR)/ec_gui.o: $(SRCDIR)/ec_gui.cpp \
$(SRCDIR)/ec_interface.hpp \
$(SRCDIR)/ec_fltkgui.hpp  \
$(SRCDIR)/ec_fltkgui.cpp 
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_gui.o \
	-c $(SRCDIR)/ec_gui.cpp 

# Rule 17: Link "ect" and execute it
$(OBJDIR)/ect: $(OBJDIR)/ec_full_test.o \
$(OBJDIR)/ec_helper.o \
$(OBJDIR)/ec_tokenizer.o \
$(OBJDIR)/ec_rules.o \
$(OBJDIR)/ec_knowledgebase.o \
$(OBJDIR)/ec_parser.o \
$(OBJDIR)/ec_inferenceengine.o \
$(OBJDIR)/ec_console.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $(OBJDIR)/ect \
	$(OBJDIR)/ec_full_test.o \
	$(OBJDIR)/ec_helper.o \
	$(OBJDIR)/ec_tokenizer.o \
	$(OBJDIR)/ec_rules.o \
	$(OBJDIR)/ec_knowledgebase.o \
	$(OBJDIR)/ec_parser.o \
	$(OBJDIR)/ec_inferenceengine.o \
	$(OBJDIR)/ec_console.o
	$(OBJDIR)/ect

# Rule 18: Compile ec_full_test.cpp
$(OBJDIR)/ec_full_test.o: $(SRCDIR)/ec_full_test.cpp \
$(SRCDIR)/ec_helper_test.cpp \
$(SRCDIR)/ec_tokenizer_test.cpp \
$(SRCDIR)/ec_rules_test.cpp \
$(SRCDIR)/ec_knowledgebase_test.cpp \
$(SRCDIR)/ec_parser_test.cpp \
$(SRCDIR)/ec_inferenceengine_test.cpp \
$(SRCDIR)/ec_console_test.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJDIR)/ec_full_test.o \
	-c $(SRCDIR)/ec_full_test.cpp 

# Rule 19: Delete all binaries and executable
clean:
	rm -f $(OBJDIR)/*
	rm -rf doc/*

# Rule 20: Generate documentation
doc:
	doxygen
