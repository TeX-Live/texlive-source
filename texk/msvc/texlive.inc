## texlive.inc: Texlive -*- Makefile -*- Variables
## 
## Copyright (C) 2002 Fabrice Popineau <Fabrice.Popineau@supelec.fr>
## 
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
## 
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.


## See file README for build instructions.


## Texlive version number
major_version_number = 8
minor_version_number = 0

## the release motto (usually not shown to users)
release_motto = 

## the build label (user friendly display name)
build_label = 2003

##
build_special_str = 
build_special_name = 

## the beta version number
beta_number = 1

## the alpha version number
alpha_number = 0

## patch number
patch_number = 0

## patch_number_str is the visual representation of patch_number; it
## should be a lowercase letter (or empty if patch_number == 0)
patch_number_str = 

## release state; one of:
##   0  official (package repository)
##   1  pretest (unofficial)
##   2  beta (field test)
##   3  performance tuning (inhouse)
##   4  private (debug symbols)
state = 0

## version number
version_number = $(major_version_number).$(minor_version_number)

## the build number
build_number = 25

## build date (time_t value)
build_date = 1061297504

## build date (human readable)
build_date_str = Tue Aug 19 14:51:44 2003

## name of builder
builder_name_str = $(USERNAME)

## version string (display) and version name (file name)
version_string = $(version_number)$(patch_number_str)
version_name = $(version_number)$(patch_number_str)
!ifdef build_special_str
version_string = $(version_string) $(build_special_str)
version_name = $(version_name)-$(build_special_name)
!endif
!if $(state) == 0
!else if $(state) == 1
version_string = $(version_string) (Snapshot build $(build_number))
!else if $(state) == 2
version_string = $(version_string) beta $(beta_number)
version_name = $(version_name)-beta-$(beta_number)
!else if $(state) == 3
version_string = $(version_string) alpha $(alpha_number)
version_name = $(version_name)-alpha-$(alpha_number)
!else if $(state) == 4
version_string = $(version_string) $(builder_name_str) $(build_number)
version_name = $(version_name)-$(builder_name_str)-$(build_number)
!endif

## define TUNE macro when tuning
!if $(state) == 3
TUNE = 1
!endif

