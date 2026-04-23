////#include <errno.h>
////#include <stdio.h>
////#include <stdlib.h>
////#include <string.h>
//#include "zip.h"
//
//#include <iostream>
//
//int listArchiveFiles( const char * name )
//{
//    return 0;
//    //// local definitions.
//    //struct zip * archive = 0;
//    //int          error   = 0;
//    //zip_uint64_t nfiles  = 0;
//    //int          i       = 0;
//    //const char * fname   = 0;
//    //// check for name.
//    //if ( name == 0 )
//    //    return -1;
//    //// open archive if exist.
//    //archive = zip_open( name, ZIP_CHECKCONS, & error );
//    //if ( archive == 0 || error != ZIP_ER_OK )
//    //    return error;
//    //// retrieve files number.
//    //nfiles = zip_get_num_entries( archive, 0 );
//    //if ( nfiles == 0 )
//    //    return -1;
//    //// loop on all files & output file names.
//    //for( i = 0 ; i < nfiles ; i++ )
//    //{
//    //     fname = zip_get_name( archive, i, 0 );
//    //     printf( "%s\n", fname );
//    //}
//    //return error;
//}
//
//int main(int argc, char *argv[])
//{
//    if ( argc == 2 )
//        return listArchiveFiles( argv[1] );
//    return 0;
//}
//
///**
// * \fn static int afficherFichierZip(const char* fichierZip)
// * \brief Affiche les fichiers � l'int�rieur de l'archive Zip.
// *
// * \return ZIPZAP_SUCCESS : Retour OK ; ZIPZAP_FAILURE : Retour d'erreur.
// */
////int afficherFichierZip(const char* fichierZip)
////{
////
////  if(fichierZip == NULL)
////      return -1;
////
////  int err = 0;
////  struct zip *f_zip=NULL;
////
////  f_zip = zip_open(fichierZip, ZIP_CHECKCONS, &err); /* on ouvre l'archive zip */
////
////  /* s'il y a des erreurs */
////  if(err != ZIP_ER_OK)
////  {
////      //zip_error_to_str(buf_erreur, sizeof buf_erreur, err, errno);
////      //printf("Error %d : %s\n",err, buf_erreur);
////      return -1;
////  }
////
////  /* si le fichier zip n'est pas ouvert */
////  if(f_zip==NULL)
////  {
////      printf("Erreur � l'ouverture du fichier %s\n", fichierZip);
////      return -1;
////  }
////
////  /* on r�cup�re le nombre de fichiers dans l'archive zip */
////  int count = zip_get_num_files(f_zip);
////  if(count==-1)
////  {
////
////      printf("Erreur � la lecture du fichier %s\n", fichierZip);
////      zip_close(f_zip);
////      f_zip = NULL;
////      return -1;
////  }
////
////  int i = 0;
////  printf("Nombre de fichiers dans l'archive : %d\n",count);
////
////  for(i=0; i<count; i++)
////  {
////      /* on utilise la position "i" pour r�cup�rer le nom des fichiers */
////      printf("%s\n", zip_get_name(f_zip, i, ZIP_FL_UNCHANGED));
////  }
////
////  zip_close(f_zip);
////  f_zip = NULL;
////
////  return -1;
////
////}
//
//
//
//
/////*
////  add_dir.c -- test case for adding file from buffer to archive
////  Copyright (C) 1999-2008 Dieter Baron and Thomas Klausner
////
////  This file is part of libzip, a library to manipulate ZIP archives.
////  The authors can be contacted at <libzip@nih.at>
////
////  Redistribution and use in source and binary forms, with or without
////  modification, are permitted provided that the following conditions
////  are met:
////  1. Redistributions of source code must retain the above copyright
////     notice, this list of conditions and the following disclaimer.
////  2. Redistributions in binary form must reproduce the above copyright
////     notice, this list of conditions and the following disclaimer in
////     the documentation and/or other materials provided with the
////     distribution.
////  3. The names of the authors may not be used to endorse or promote
////     products derived from this software without specific prior
////     written permission.
//// 
////  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
////  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
////  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
////  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
////  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
////  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
////  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
////  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
////  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
////  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
////  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////*/
////
////
////
////#include <errno.h>
////#include <stdio.h>
////#include <stdlib.h>
////#include <string.h>
////
////#include "zip.h"
////
////const char *dir[2] = {
////    "testdir/",
////    "testdir-noslash"
////};
////
////const char *prg;
////
////int
////main(int argc, char *argv[])
////{
////    const char *archive;
////    struct zip *za;
////    char buf[100];
////    size_t i;
////    int err;
////
////    prg = argv[0];
////
////    if (argc != 2) {
////	fprintf(stderr, "usage: %s archive\n", prg);
////	return 1;
////    }
////
////    archive = argv[1];
////    
////    if ((za=zip_open(archive, ZIP_CREATE, &err)) == NULL) {
////	zip_error_to_str(buf, sizeof(buf), err, errno);
////	fprintf(stderr, "%s: can't open zip archive %s: %s\n", prg,
////		archive, buf);
////	return 1;
////    }
////
////    for (i=0; i<sizeof(dir)/sizeof(dir[0]); i++) {
////	if (zip_add_dir(za, dir[i]) == -1) {
////	    fprintf(stderr, "%s: can't add file `%s': %s\n", prg,
////		    dir[i], zip_strerror(za));
////	    return 1;
////	}
////    }
////
////    if (zip_close(za) == -1) {
////	fprintf(stderr, "%s: can't close zip archive %s\n", prg,
////		archive);
////	return 1;
////    }
////
////    return 0;
////}
