#pragma once
#ifndef VTKIOFITS_EXPORT_H
#define VTKIOFITS_EXPORT_H

#ifdef VTKIOFITS_STATIC_DEFINE
#  define VTKIOFITS_EXPORT
#  define VTKIOFITS_NO_EXPORT
#else
#  ifndef VTKIOFITS_EXPORT
#    ifdef vtkIOFits_EXPORTS
/* We are building this library */
#      define VTKIOFITS_EXPORT __declspec(dllexport)
#    else
/* We are using this library */
#      define VTKIOFITS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef VTKIOFITS_NO_EXPORT
#    define VTKIOFITS_NO_EXPORT 
#  endif
#endif

#ifndef VTKIOFITS_DEPRECATED
#  define VTKIOFITS_DEPRECATED __declspec(deprecated)
#  define VTKIOFITS_DEPRECATED_EXPORT VTKIOFITS_EXPORT __declspec(deprecated)
#  define VTKIOFITS_DEPRECATED_NO_EXPORT VTKIOFITS_NO_EXPORT __declspec(deprecated)
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define VTKIOFITS_NO_DEPRECATED
#endif

#endif
