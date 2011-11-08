# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_plot.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt

FIND_PATH ( ZZIP_INCLUDE_DIR zzip.h
    /usr/include/zzip
    /usr/include/
    )
	  
FIND_LIBRARY ( ZZIP_LIBRARIES NAMES zzip )

# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE ( FindPackageHandleStandardArgs )
find_package_handle_standard_args( ZZIP DEFAULT_MSG ZZIP_LIBRARIES ZZIP_INCLUDE_DIR )
