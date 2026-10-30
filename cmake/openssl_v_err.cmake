# CMake function to find the headerfile x509_vfy.h and search for all occurrences
# of X509_V_ERR_ and replace them by #ifdef x V_ERR(x) #endif

if (OPENSSL_INCLUDE_DIR)
    # OPENSSL_INCLUDE_DIR may be a ;-separated list (e.g. a separate source and
    # build include dir when OpenSSL is built via CPM). When OpenSSL is built
    # from source the generated x509_vfy.h is not present until the OpenSSL
    # build has run, so fall back to the source template x509_vfy.h.in, which
    # carries the same X509_V_ERR_ definitions.
    foreach(_openssl_inc IN LISTS OPENSSL_INCLUDE_DIR)
        if (EXISTS "${_openssl_inc}/openssl/x509_vfy.h")
            set(OPENSSL_V_ERR_H "${_openssl_inc}/openssl/x509_vfy.h")
            break()
        elseif (EXISTS "${_openssl_inc}/openssl/x509_vfy.h.in")
            set(OPENSSL_V_ERR_H "${_openssl_inc}/openssl/x509_vfy.h.in")
            break()
        endif()
    endforeach()
    if (OPENSSL_V_ERR_H)
        message(STATUS "Parsing ${OPENSSL_V_ERR_H} for X509_V_ERR_")
        file(READ "${OPENSSL_V_ERR_H}" X509_VFY_H)
        string(REGEX MATCHALL "X509_V_ERR_[A-Z_0-9]+" X509_VFY_ERRORS ${X509_VFY_H})
        foreach(ERR ${X509_VFY_ERRORS})
            string(APPEND X509_VFY_ERRORS_C "#ifdef ${ERR}\n V_ERR(${ERR})\n#endif\n")
        endforeach()
        file(WRITE ${CMAKE_BINARY_DIR}/openssl_v_err.c ${X509_VFY_ERRORS_C})
    endif()
endif()
