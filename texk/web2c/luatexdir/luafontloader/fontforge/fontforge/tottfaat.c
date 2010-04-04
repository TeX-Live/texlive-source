/* Copyright (C) 2000-2008 by George Williams */
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "pfaedit.h"
#include <utype.h>

#include "ttf.h"

/* This file contains routines to create some of the Apple Advanced Typography Tables */
/*  (or GX fonts)  */

/* ************************************************************************** */
/* *************************    The 'morx' table    ************************* */
/* *************************      (and 'feat')      ************************* */
/* ************************************************************************** */

/* Each lookup gets its own subtable, so there may be multiple subtables */
/*  with the same feature/setting. The subtables will be ordered the same */
/*  way the lookups are, which might lead to awkwardness if there are many */
/*  chains and the same feature occurs in several of them */
/* (only the default language will be used) */
struct feature {
    int16 featureType, featureSetting;
    MacFeat *mf, *smf;
    struct macsetting *ms, *sms;
    unsigned int vertOnly: 1;
    unsigned int r2l: 1;	/* I think this is the "descending" flag */
    unsigned int needsOff: 1;
    unsigned int singleMutex: 1;
    unsigned int dummyOff: 1;
    uint8 subtable_type;
    int chain;
    int32 flag, offFlags;
    uint32 feature_start;
    uint32 feature_len;		/* Does not include header yet */
    struct feature *next;	/* features in output order */
    struct feature *nexttype;	/* features in feature/setting order */
    struct feature *nextsame;	/* all features with the same feature/setting */
    int setting_cnt, setting_index, real_index;
};



struct transition { uint16 next_state, dontconsume, ismark, trans_ent; LigList *l; };
struct trans_entries { uint16 next_state, flags, act_index; LigList *l; };


int Macable(SplineFont *sf, OTLookup *otl) {
    int ft, fs;
    FeatureScriptLangList *features;

    switch ( otl->lookup_type ) {
    /* These lookup types are mac only */
      case kern_statemachine: case morx_indic: case morx_context: case morx_insert:
return( true );
    /* These lookup types or OpenType only */
      case gsub_multiple: case gsub_alternate:
      case gpos_single: case gpos_cursive: case gpos_mark2base:
      case gpos_mark2ligature: case gpos_mark2mark:
return( false );
    /* These are OpenType only, but they might be convertable to a state */
    /*  machine */
      case gsub_context:
      case gsub_contextchain: case gsub_reversecchain:
      case gpos_context: case gpos_contextchain:
	if ( sf==NULL || sf->sm!=NULL )
return( false );
	/* Else fall through into the test on the feature tag */;
    /* These two can be expressed in both, and might be either */
      case gsub_single: case gsub_ligature: case gpos_pair:
	for ( features = otl->features; features!=NULL; features = features->next ) {
	    if ( features->ismac || OTTagToMacFeature(features->featuretag,&ft,&fs))
return( true );
	}
      default: /* handle this to silence compiler warnings */
        break;
    }
return( false );
}


/* ************************************************************************** */
/* *************************    utility routines    ************************* */
/* ************************************************************************** */

uint32 MacFeatureToOTTag(int featureType,int featureSetting) {
    int i;
    struct macsettingname *msn = user_macfeat_otftag ? user_macfeat_otftag : macfeat_otftag;

    for ( i=0; msn[i].otf_tag!=0; ++i )
	if ( msn[i].mac_feature_type == featureType &&
		msn[i].mac_feature_setting == featureSetting )
return( msn[i].otf_tag );

return( 0 );
}

int OTTagToMacFeature(uint32 tag, int *featureType,int *featureSetting) {
    int i;
    struct macsettingname *msn = user_macfeat_otftag ? user_macfeat_otftag : macfeat_otftag;

    for ( i=0; msn[i].otf_tag!=0; ++i )
	if ( msn[i].otf_tag == tag ) {
	    *featureType = msn[i].mac_feature_type;
	    *featureSetting = msn[i].mac_feature_setting;
return( true );
	}

    *featureType = 0;
    *featureSetting = 0;
return( false );
}

