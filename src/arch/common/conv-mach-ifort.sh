# for Intel Fortran compiler 8.0 and higher which is renamed to ifort from ifc
CMK_CF77="ifort -auto "
CMK_CF90="ifort -auto "
CMK_CF90_FIXED="$CMK_CF90 -132 -FI "
F90DIR=`which ifort 2> /dev/null`
if test -x "$F90DIR" 
then
  F90LIBDIR="`dirname $F90DIR`/../lib"
  F90MAIN="$F90LIBDIR/for_main.o"
fi
# for_main.o is important for main() in f90 code
CMK_F90MAINLIBS="$F90MAIN "
CMK_F90LIBS="-L$F90LIBDIR -lifcore -lifport "
CMK_F77LIBS="$CMK_F90LIBS"

CMK_MOD_NAME_ALLCAPS=
CMK_MOD_EXT="mod"
CMK_F90_USE_MODDIR=""
