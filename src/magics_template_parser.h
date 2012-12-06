#ifndef MAGICS_TEMPLATE_PARSER_HH
#define MAGICS_TEMPLATE_PARSER_HH

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<locale.h>

#if  defined  (HAVE_LIBXML)
#include<libxml/parser.h>
#include<libxml/tree.h>
#endif


#if  defined  (HAVE_LIBXML)
int magics_template_parser( xmlNode * a_node );
#endif

int SetMagicsParameterValue( char *param_name, char *param_type, char *param_value );


#endif
