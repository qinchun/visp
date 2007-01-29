/****************************************************************************
 *
 * $Id: testIoPGM.cpp,v 1.6 2007-01-29 14:38:03 asaunier Exp $
 *
 * Copyright (C) 1998-2006 Inria. All rights reserved.
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
 * Read and write PGM images on the disk.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/


#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpParseArgv.h>
#include <visp/vpIoTools.h>

/*!
  \example testIoPGM.cpp

  \brief Read and write PGM images on the disk. Also test exceptions.

*/

// List of allowed command line options
#define GETOPTARGS	"i:o:h"


/*

  Print the program options.

  \param ipath: Input image path.
  \param opath : Output image path.
  \param user : Username.

 */
void usage(char *name, char *badparam, string ipath, string opath, string user)
{
  fprintf(stdout, "\n\
Read and write PGM images on the disk. Also test exceptions.\n\
\n\
SYNOPSIS\n\
  %s [-i <input image path>] [-o <output image path>]\n\
     [-h]\n						      \
", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -i <input image path>                                %s\n\
     Set image input path.\n\
     From this path read \"ViSP-images/Klimt/Klimt.pgm\"\n\
     image.\n\
     Setting the VISP_INPUT_IMAGE_PATH environment\n\
     variable produces the same behaviour than using\n\
     this option.\n\
\n\
  -o <output image path>                               %s\n\
     Set image output path.\n\
     From this directory, creates the \"%s\"\n\
     subdirectory depending on the username, where \n\
     Klimt_grey.pgm output image is written.\n\
\n\
  -h\n\
     Print the help.\n\n",
	  ipath.c_str(), opath.c_str(), user.c_str());

}

/*!

  Set the program options.

  \param ipath: Input image path.
  \param opath : Output image path.
  \param user : Username.
  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, char **argv,
		string &ipath, string &opath, string user)
{
  char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'i': ipath = optarg; break;
    case 'o': opath = optarg; break;
    case 'h': usage(argv[0], NULL, ipath, opath, user); return false; break;

    default:
      usage(argv[0], optarg, ipath, opath, user); return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL, ipath, opath, user);
    cerr << "ERROR: " << endl;
    cerr << "  Bad argument " << optarg << endl << endl;
    return false;
  }

  return true;
}

int
main(int argc, char ** argv)
{

  string env_ipath;
  string opt_ipath;
  string opt_opath;
  string ipath;
  string opath;
  string filename;
  string username;

  // Get the VISP_IMAGE_PATH environment variable value
  char *ptenv = getenv("VISP_INPUT_IMAGE_PATH");
  if (ptenv != NULL)
    env_ipath = ptenv;

  // Set the default input path
  if (! env_ipath.empty())
    ipath = env_ipath;

  // Set the default output path
#ifdef UNIX
  opt_opath = "/tmp";
#elif WIN32
  opt_opath = "C:\\temp";
#endif

  // Get the user login name
  vpIoTools::getUserName(username);

  // Read the command line options
  if (getOptions(argc, argv, opt_ipath, opt_opath, username) == false) {
    exit (-1);
  }

  // Get the option values
  if (!opt_ipath.empty())
    ipath = opt_ipath;
  if (!opt_opath.empty())
    opath = opt_opath;

  // Append to the output path string, the login name of the user
  string dirname = opath + vpIoTools::path("/") + username;

  // Test if the output path exist. If no try to create it
  if (vpIoTools::checkDirectory(dirname) == false) {
    try {
      // Create the dirname
      vpIoTools::makeDirectory(dirname);
    }
    catch (...) {
      usage(argv[0], NULL, ipath, opath, username);
      cerr << endl
	   << "ERROR:" << endl;
      cerr << "  Cannot create " << dirname << endl;
      cerr << "  Check your -o " << opath << " option " << endl;
      exit(-1);
    }
  }

  // Compare ipath and env_ipath. If they differ, we take into account
  // the input path comming from the command line option
  if (opt_ipath.empty()) {
    if (ipath != env_ipath) {
      cout << endl
	   << "WARNING: " << endl;
      cout << "  Since -i <visp image path=" << ipath << "> "
	   << "  is different from VISP_IMAGE_PATH=" << env_ipath << endl
	   << "  we skip the environment variable." << endl;
    }
  }

  // Test if an input path is set
  if (opt_ipath.empty() && env_ipath.empty()){
    usage(argv[0], NULL, ipath, opath, username);
    cerr << endl
	 << "ERROR:" << endl;
    cerr << "  Use -i <visp image path> option or set VISP_INPUT_IMAGE_PATH "
	 << endl
	 << "  environment variable to specify the location of the " << endl
	 << "  image path where test images are located." << endl << endl;
    exit(-1);
  }

  // Create a grey level image
  vpImage<unsigned char> I ;

  // Load a grey image from the disk
  filename = ipath +  vpIoTools::path("/ViSP-images/Klimt/Klimt.pgm");
  vpImageIo::readPGM(I, filename);
  // Write the content of the image on the disk
  filename = opath +  vpIoTools::path("/Klimt_grey.pgm");
  vpImageIo::writePGM(I, filename) ;

  // Try to load a non existing image (test for exceptions)
  try
  {
    // Load a non existing grey image
    filename = ipath +  vpIoTools::path("/ViSP-images/image-that-does-not-exist.pgm");
    vpImageIo::readPGM(I, filename) ;
  }
  catch(vpImageException e)
  {
    vpERROR_TRACE("at main level");
    cout << e << endl ;
  }

  // Try to write an image to a non existing directory
  try
  {
    filename = opath +  vpIoTools::path("/directory-that-does-not-exist/Klimt.pgm");
    vpImageIo::writePGM(I, filename) ;
  }
  catch(vpImageException e)
  {
    vpERROR_TRACE("at main level");
    cout << e << endl ;
  }
}

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
