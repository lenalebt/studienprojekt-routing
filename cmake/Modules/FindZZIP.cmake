# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_plot.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt

find_path ( ZZIP_INCLUDE_DIR zzip.h /usr/include/zzip /usr/include/ )
	  
find_library ( ZZIP_LIBRARIES NAMES zzip )

# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if 
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args( ZZIP DEFAULT_MSG ZZIP_LIBRARIES ZZIP_INCLUDE_DIR )
