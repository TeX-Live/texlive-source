/* recorder.h: filename recorder callback.

   Copyright 2007 Karl Berry.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef KPATHSEA_RECORDER_H
#define KPATHSEA_RECORDER_H

/* Thess variables get set by web2c and are used to record filenames for its
 * filename recorder. This way they can also be used in kpathsea, e.g. for
 * texmf.cnf */
void (*kpse_record_input)(char*);
void (*kpse_record_output)(char*);

#endif /* not KPATHSEA_RECORDER_H */
