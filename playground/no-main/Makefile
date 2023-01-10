# Makefile for compiling example program without main() function

# -----------------------------------------------------------------------------
# Variables to control Makefile operation

CXXFLAGS = -Wall -g -nostartfiles
PROGRAM = no-main
SRCS = $(wildcard *.cpp)

# --- build -------------------------------------------------------------------
#  Build PROGRAM utilizing make's implicit rules

$(PROGRAM): $(SRCS)
	$(CXX) -o $@ $(SRCS) $(CXXFLAGS)
	@echo "Program \"$(PROGRAM)\" built successfully!"

# --- clean -------------------------------------------------------------------
# Clean up build artifact
.PHONY: clean
clean:
	@rm -rf $(PROGRAM)
	@echo "Build artefacts removed!"

