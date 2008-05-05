/* recorder.h: filename recorder callback.

   Copyright 2007, 2008 Karl Berry.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef KPATHSEA_RECORDER_H
#define KPATHSEA_RECORDER_H

/* These variables are set by web2c and are used to record filenames for its
   filename -recorder option.  This way they can also be used for files
   opened by kpathsea, e.g., texmf.cnf */
extern void (*kpse_record_input) (const_string);
extern void (*kpse_record_output) (const_string);

#endif /* not KPATHSEA_RECORDER_H */
