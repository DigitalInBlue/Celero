if(CELERO_ENABLE_EXPERIMENTS)
	# Run the SUBDIRLIST macro located in the top level CMakeLists.txt file.
	FILE(GLOB _ALL_FILES ./ ./*)
	FOREACH(_FILE ${_ALL_FILES})
	  IF(IS_DIRECTORY ${_FILE})
	    ADD_SUBDIRECTORY(${_FILE})
	  ENDIF()
	ENDFOREACH()
endif()
