######################################################
#            PROJECT  : numaprof                     #
#            VERSION  : 1.1.2                        #
#            DATE     : 11/2020                      #
#            AUTHOR   : Valat Sébastien  - CERN      #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(numaprof-portability OBJECT NumaTopo.cpp OS.cpp Clock.cpp)
SET_TARGET_PROPERTIES(numaprof-portability PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
#install procedure
#INSTALL(FILES ${JSON_HEADERS} DESTINATION include/htopml)
