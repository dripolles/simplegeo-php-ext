#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_simplegeo.h"

typedef struct _Point {
    double x;
    double y;
} Point;

char make_point(zval *zpoint, Point *point);
char extract_double(zval *zpoint, uint position, double *x);
char point_in_polygon(Point * ppoint, Point * vertices, uint v_size);
char point_equals(Point p1, Point p2);

static zend_function_entry simplegeo_functions[] = {
    PHP_FE(simplegeo_point_in_polygon, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry simplegeo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_SIMPLEGEO_WORLD_EXTNAME,
    simplegeo_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_SIMPLEGEO_WORLD_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SIMPLEGEO
ZEND_GET_MODULE(simplegeo)
#endif

PHP_FUNCTION(simplegeo_point_in_polygon)
{
    zval *zpoint, *zpolygon, **zpolypoint;
    Point * point;
    Point * polygon;
    uint i, n;
    char in_polygon;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "aa", &zpoint, &zpolygon) == FAILURE) {
        RETURN_NULL();
    }

    point = emalloc(sizeof(Point));
    if (!make_point(zpoint, point)) {
        RETURN_NULL();
    }

    n = Z_ARRVAL_P(zpolygon)->nNumOfElements;
    polygon = emalloc(sizeof(Point) * n);

    zend_printf("N: %d\n", n);
    for (i = 0; i < n; i++) {
        if (zend_hash_index_find(Z_ARRVAL_P(zpolygon), i, (void**)&zpolypoint) == FAILURE) {
            RETURN_NULL();
        }

        if (!make_point(*zpolypoint, &polygon[i])) {
            RETURN_NULL();
        }
        zend_printf("Vertex %d -- x: %f y: %f\n", i, polygon[i].x, polygon[i].y, n);


    }
    zend_printf("Point -- x: %f y: %f\n", point->x, point->y);

    in_polygon = point_in_polygon(point, polygon, n);

    efree(point);
    efree(polygon);

    RETURN_BOOL(in_polygon);
}

char make_point(zval *zpoint, Point *point)
{
    if (!extract_double(zpoint, 0, &(point->x))) return 0;
    if (!extract_double(zpoint, 1, &(point->y))) return 0;

    return 1;
}

char extract_double(zval *zpoint, uint position, double *d)
{
    zval **zvalue;

    if (zend_hash_index_find(Z_ARRVAL_P(zpoint), position, (void**)&zvalue) == FAILURE) {
        return 0;
    }

    convert_to_double(*zvalue);
    *d = Z_DVAL_PP(zvalue);
}

char point_in_polygon(Point * ppoint, Point * vertices, uint v_size)
{
    uint i, intersections;
    Point point, vertex1, vertex2;
    double min1, min2, max1, max2, xinters;

    point = *ppoint;
    for (i = 0; i < v_size; i++) {
        if (point_equals(point, vertices[i])) return 1;
    }
    // Check if the point is inside the polygon or on the boundary
    intersections = 0;

    for (i = 1; i < v_size; i++) {
        vertex1 = vertices[i-1];
        vertex2 = vertices[i];
        // Check if point is on an horizontal polygon boundary
        if ( vertex1.x < vertex2.x ) {
            min1 = vertex1.x; max1 = vertex2.x;
        } else {
            min1 = vertex2.x; max1 = vertex1.x;
        }

        if (
                vertex1.y == vertex2.y &&
                vertex1.y == point.y &&
                point.x > min1 &&
                point.x < max1
           ) {
            return 1;
        }

        if ( vertex1.y < vertex2.y ) {
            min2 = vertex1.y; max2 = vertex2.y;
        } else {
            min2 = vertex2.y; max2 = vertex1.y;
        }

        if ( point.y > min2 && point.y <= max2 && point.x <= max1 && vertex1.y != vertex2.y) {
            xinters =
                (point.y - vertex1.y) * (vertex2.x - vertex1.x) /
                (vertex2.y - vertex1.y) + vertex1.x;

            if (xinters == point.x) { // Check if point is on the polygon boundary (other than horizontal)
                return 1;
            }

            if (vertex1.x == vertex2.x || point.x <= xinters) {
                intersections++;
            }
        }
    }

    return ((intersections % 2) != 0);
}

char point_equals(Point p1, Point p2)
{
    return (p1.x == p2.x) && (p1.y == p2.y);
}

