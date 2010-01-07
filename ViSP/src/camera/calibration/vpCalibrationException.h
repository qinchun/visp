/****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 1998-2010 Inria. All rights reserved.
 *
 * This software was developed at:
 * IRISA/INRIA Rennes
 * Projet Lagadic
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * http://www.irisa.fr/lagadic
 *
 * This file is part of the ViSP toolkit
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE included in the packaging of this file.
 *
 * Licensees holding valid ViSP Professional Edition licenses may
 * use this file in accordance with the ViSP Commercial License
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact visp@irisa.fr if any conditions of this licensing are
 * not clear to you.
 *
 * Description:
 * Exceptions that can be emited by the vpCalibration class and its derivates.
 *
 * Authors:
 * Anthony Saunier
 *
 *****************************************************************************/

#ifndef __vpCalibrationException_H
#define __vpCalibrationException_H


/* ------------------------------------------------------------------------- */
/* --- INCLUDE ------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */


/* Classes standards. */
//
#include <iostream>                /* Classe std::ostream.    */
#include <string>                  /* Classe string.     */
#include <visp/vpConfig.h>
#include <visp/vpException.h>


/* ------------------------------------------------------------------------- */
/* --- CLASS --------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

/*!

  \class vpCalibrationException

  \ingroup Exception

  \brief Error that can be emited by the vpCalibration class.
 */
class VISP_EXPORT vpCalibrationException : public vpException
{
public:
  /*!
    \brief Lists the possible error than can be emmited while calling
    vpCalibration member
   */
  enum errorCodeEnum
    {
      //! error returns by a constructor
      constructionError,
      //! something is not initialized
      notInitializedError,
      //! function not implemented
      notImplementedError,
      //! index out of range
      outOfRangeError,
      //! iterative algorithm doesn't converge
      convergencyError,
      forbiddenOperatorError,
    } ;

public:
  vpCalibrationException (const int code, const char * msg)
    : vpException(code, msg){ ; }
  vpCalibrationException (const int code, const std::string & msg)
    : vpException(code, msg){ ; }
  vpCalibrationException (const int code)
    : vpException(code){ ; }
 // vpMatrixException() : vpException() { ;}
};





#endif /* #ifndef __vpCalibrationException_ERROR_H */


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
