MACRO(GET_GIT_REV _dir gitRev)
# Grab git revision
FIND_PROGRAM(GIT_EXECUTABLE git
        DOC "git command line client")
        MARK_AS_ADVANCED(GIT_EXECUTABLE)

IF(GIT_EXECUTABLE)
       SET(svn 0)
       MESSAGE(STATUS "Getting git-svn version from ${_dir}")
       EXECUTE_PROCESS(
               COMMAND ${GIT_EXECUTABLE} rev-parse  --show-toplevel ${_dir}
               WORKING_DIRECTORY ${_dir} 
               RESULT_VARIABLE result 
               OUTPUT_VARIABLE topdir
               )
       EXECUTE_PROCESS(
               COMMAND echo  log --format=oneline -1   ${_dir}
               COMMAND xargs ${GIT_EXECUTABLE} 
               COMMAND head -c 6
               WORKING_DIRECTORY ${_dir} 
               OUTPUT_VARIABLE gitRev
               )
ELSE(GIT_EXECUTABLE)
    SET(gitRev "")
ENDIF(GIT_EXECUTABLE)

ENDMACRO(GET_GIT_REV dir gitRev)
