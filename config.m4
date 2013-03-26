PHP_ARG_ENABLE(simplegeo, whether to enable Simplegeo
World support,
[ --enable-simplegeo   Enable Hello World support])
if test "$PHP_SIMPLEGEO" = "yes"; then
  AC_DEFINE(HAVE_SIMPLEGEO, 1, [Whether you have Hello World])
  PHP_NEW_EXTENSION(simplegeo, simplegeo.c, $ext_shared)
fi
