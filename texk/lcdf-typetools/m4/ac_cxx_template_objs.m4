dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_template_objs.html
dnl
dnl Adapted for Autoconf 2.63  Peter Breitenlohner  <peb@mppmu.mpg.de>
dnl
AC_DEFUN([AC_CXX_TEMPLATE_OBJS],
[AC_CACHE_CHECK(where template objects are stored, ac_cv_cxx_templobjs,
 [ ac_cv_cxx_templobjs='unknown'
   if test "$GXX" = yes; then
     ac_cv_cxx_templobjs='nowhere'
   else
     case $CXX in
       CC|*/CC)
        cat > conftest.cc <<EOF
template<class T> class A { public : A () {} };
template<class T> void f (const A<T>&) {}
main()
{ A<double> d;
  A<int> i;
  f (d);
  f (i);
  return 0;
}
EOF
        if test "$ac_cv_cxx_templobjs" = 'unknown' ; then
          if test -d Templates.DB ; then
            rm -fr Templates.DB
          fi
          if $CXX $CXXFLAGS -ptr. -c conftest.cc 1> /dev/null 2>&1; then
            if test -d Templates.DB ; then
#             this should be Sun CC <= 4.2
              CXXFLAGS="$CXXFLAGS -ptr."
              if test x"$LIBTOOL" = x ; then
                ac_cv_cxx_templobjs='Templates.DB/*.o'
              else
                ac_cv_cxx_templobjs='Templates.DB/*.lo'
              fi
              rm -fr Templates.DB
            fi
          fi
        fi
        if test "$ac_cv_cxx_templobjs" = 'unknown' ; then
          if test -d SunWS_cache ; then
            rm -fr SunWS_cache
          fi
          if $CXX $CXXFLAGS -c conftest.cc 1> /dev/null 2>&1; then
            if test -d SunWS_cache ; then
#             this should be Sun WorkShop C++ compiler 5.x
#             or Sun Forte C++ compiler >= 6.x
              if test x"$LIBTOOL" = x ; then
                ac_cv_cxx_templobjs='SunWS_cache/*/*.o'
              else
                ac_cv_cxx_templobjs='SunWS_cache/*/*.lo'
              fi
              rm -fr SunWS_cache
            fi
          fi
        fi
        rm -f conftest* ;;
     esac
   fi])
case $ac_cv_cxx_templobjs in
  unknown|nowhere)
  TEMPLATE_OBJS="" ;;
  *)
  TEMPLATE_OBJS="$ac_cv_cxx_templobjs" ;;
esac
AC_SUBST(TEMPLATE_OBJS)])

