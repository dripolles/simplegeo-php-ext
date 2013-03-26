#ifndef PHP_SIMPLEGEO_H
#define PHP_SIMPLEGEO_H 1
#define PHP_SIMPLEGEO_WORLD_VERSION "1.0"
#define PHP_SIMPLEGEO_WORLD_EXTNAME "simplegeo"

PHP_FUNCTION(simplegeo_point_in_polygon);


extern zend_module_entry simplegeo_module_entry;
#define phpext_simplegeo_ptr &simplegeo_module_entry

#endif
