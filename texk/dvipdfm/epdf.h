#ifndef _EPDF_H_
#define _EPDF_H_

#include <stdio.h>
#include "pdfspecial.h"
#include "pdfobj.h"

extern pdf_obj *pdf_include_page (FILE *file, struct xform_info *p,
				  char *res_name);

#endif /* _EPDF_H_ */
