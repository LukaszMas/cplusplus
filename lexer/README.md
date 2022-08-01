# Use lexer (`re2c`) to generate c++ code

re2c documentation: [re2c.org][1]  
re2c GitHub: [github repo][2]

1. Make instruction to build `*.os` file:
    - ```
      #SRCS += $(GEN_DIR)/<filename>.cpp
      OBJS += $(OBJ_DIR)/<filename>.o

      $(OBJ_DIR)/<filename>.o: $(SRC_DIR)/filename.re $(INC_DIR)/<dependency>.h
        mkdir -p $(GEN_DIR)
        re2c -o $(GEN_DIR)/<filename>.cpp $(SRC_DIR)/<filename>.re
        $(CXX) $(CXXFLAGS) $(LIBS) -c -o $@ $(GEN_DIR)/<filename>.cpp
      ```

2. CMake instuctions to build with re2c generated dependencies:
    - ```
      find_program(RE2C re2c)

      set(<FILENAME>_H "${CMAKE_CURRENT_BINARY_DIR}/<filename>.h")
      set(<FILENAME>_RE "${CMAKE_CURRENT_SOURCE_DIR}/src/<filename>.re")

      if(RE2C)
        add_custom_command(OUTPUT ${<FILENAME>_H} DEPENDS ${<FILENAME>_RE}
          COMMAND ${RE2C} -i -o ${<FILENAME>_H} ${FILENAME>_RE})
      else()
        message(FATAL_ERROR "Could not find re2c that is required for compilation")
      endif()

      set_property(SOURCE src/<fielname>.cpp APPEND PROPERTY OBJECT_DEPENDS ${<FILENAME>_H})

      set(SOURCES
        ...
        src/<filename>.re
        ...
      )
      ```

[1]: <https://re2c.org/> "re2c.org"
[2]: <https://github.com/skvadrik/re2c> "re2c github"
