	/*508:*/
	#line 10150 "format.w"

#include "basetypes.h"
#include <string.h>
#include <math.h>
#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "error.h"
#include "format.h"
#include "get.h"

	/*244:*/
	#line 4905 "format.w"

Label*labels;
int first_label= -1;
	/*:244*/	/*292:*/
	#line 6068 "format.w"

RangePos*range_pos;
int next_range= 1,max_range;
int*page_on;
	/*:292*/	/*303:*/
	#line 6254 "format.w"

char hbanner[MAX_BANNER+1];
	/*:303*/	/*310:*/
	#line 6392 "format.w"

uint8_t*hpos= NULL,*hstart= NULL,*hend= NULL,*hpos0= NULL;
	/*:310*/	/*316:*/
	#line 6473 "format.w"

char*hin_name= NULL;
uint64_t hin_size= 0;
uint8_t*hin_addr= NULL;
uint64_t hin_time= 0;
	/*:316*/	/*372:*/
	#line 7937 "format.w"

char**hfont_name;
	/*:372*/	/*411:*/
	#line 8637 "format.w"

unsigned int debugflags= DBGNONE;
int option_utf8= false;
int option_hex= false;
int option_force= false;
int option_global= false;
int option_aux= false;
int option_compress= false;
char*stem_name= NULL;
int stem_length= 0;
	/*:411*/	/*414:*/
	#line 8771 "format.w"

FILE*hin= NULL,*hout= NULL,*hlog= NULL;
	/*:414*/
	#line 10163 "format.w"


	/*317:*/
	#line 6480 "format.w"

#ifndef USE_MMAP
void hget_unmap(void)
{if(hin_addr!=NULL)free(hin_addr);
hin_addr= NULL;
hin_size= 0;
}
bool hget_map(void)
{FILE*f;
struct stat st;
size_t s,t;
uint64_t u;
f= fopen(hin_name,"rb");
if(f==NULL)
{MESSAGE("Unable to open file: %s\n",hin_name);return false;}
if(stat(hin_name,&st)<0)
{MESSAGE("Unable to obtain file size: %s\n",hin_name);
fclose(f);
return false;
}
if(st.st_size==0)
{MESSAGE("File %s is empty\n",hin_name);
fclose(f);
return false;
}
u= st.st_size;
if(hin_addr!=NULL)hget_unmap();
hin_addr= malloc(u);
if(hin_addr==NULL)
{MESSAGE("Unable to allocate 0x%"PRIx64" byte for File %s\n",u,hin_name);
fclose(f);
return 0;
}
t= 0;
do{
s= fread(hin_addr+t,1,u,f);
if(s<=0)
{MESSAGE("Unable to read file %s\n",hin_name);
fclose(f);
free(hin_addr);
hin_addr= NULL;
return false;
}
t= t+s;
u= u-s;
}while(u>0);
hin_size= st.st_size;
hin_time= st.st_mtime;
return true;
}

#else

#include <sys/mman.h>

void hget_unmap(void)
{munmap(hin_addr,hin_size);
hin_addr= NULL;
hin_size= 0;
}

bool hget_map(void)
{struct stat st;
int fd;
fd= open(hin_name,O_RDONLY,0);
if(fd<0)
{MESSAGE("Unable to open file %s\n",hin_name);return false;}
if(fstat(fd,&st)<0)
{MESSAGE("Unable to get file size\n");
close(fd);
return false;
}
if(st.st_size==0)
{MESSAGE("File %s is empty\n",hin_name);
close(fd);
return false;
}
if(hin_addr!=NULL)hget_unmap();
hin_size= st.st_size;
hin_time= st.st_mtime;
hin_addr= mmap(NULL,hin_size,PROT_READ,MAP_PRIVATE,fd,0);
if(hin_addr==MAP_FAILED)
{close(fd);
hin_addr= NULL;hin_size= 0;
MESSAGE("Unable to map file into memory\n");
return 0;
}
close(fd);
return hin_size;
}
#endif

	/*:317*/
	#line 10165 "format.w"

	/*304:*/
	#line 6258 "format.w"


bool hcheck_banner(char*magic)
{int hbanner_size= 0;
int v;
char*t;
t= hbanner;
if(strncmp(magic,hbanner,4)!=0)
{MESSAGE("This is not a %s file\n",magic);return false;}
else t+= 4;
hbanner_size= (int)strnlen(hbanner,MAX_BANNER);
if(hbanner[hbanner_size-1]!='\n')
{MESSAGE("Banner exceeds maximum size=0x%x\n",MAX_BANNER);return false;}
if(*t!=' ')
{MESSAGE("Space expected in banner after %s\n",magic);return false;}
else t++;
v= strtol(t,&t,10);
if(v!=HINT_VERSION)
{MESSAGE("Wrong HINT version: got %d, expected %d\n",v,HINT_VERSION);return false;}
if(*t!='.')
{MESSAGE("Dot expected in banner after HINT version number\n");return false;}
else t++;
v= strtol(t,&t,10);
if(v!=HINT_SUB_VERSION)
{MESSAGE("Wrong HINT subversion: got %d, expected %d\n",v,HINT_SUB_VERSION);return false;}
if(*t!=' '&&*t!='\n')
{MESSAGE("Space expected in banner after HINT subversion\n");return false;}
LOG("%s file version %d.%d:%s",magic,HINT_VERSION,HINT_SUB_VERSION,t);
DBG(DBGDIR,"banner size=0x%x\n",hbanner_size);
return true;
}
	/*:304*/
	#line 10166 "format.w"

	/*326:*/
	#line 6813 "format.w"

Entry*dir= NULL;
uint16_t section_no,max_section_no;
void new_directory(uint32_t entries)
{DBG(DBGDIR,"Creating directory with %d entries\n",entries);
RNG("Directory entries",entries,3,0x10000);
max_section_no= entries-1;
ALLOCATE(dir,entries,Entry);
dir[0].section_no= 0;dir[1].section_no= 1;dir[2].section_no= 2;
}
	/*:326*/	/*327:*/
	#line 6826 "format.w"

void hset_entry(Entry*e,uint16_t i,uint32_t size,uint32_t xsize,char*file_name)
{e->section_no= i;
e->size= size;e->xsize= xsize;
if(file_name==NULL||*file_name==0)
e->file_name= NULL;
else
e->file_name= strdup(file_name);
DBG(DBGDIR,"Creating entry %d: \"%s\" size=0x%x xsize=0x%x\n",i,file_name,size,xsize);
}
	/*:327*/
	#line 10167 "format.w"


	/*305:*/
	#line 6298 "format.w"

void hget_banner(void)
{int i;
for(i= 0;i<MAX_BANNER&&hpos<hend;i++)
{hbanner[i]= HGET8;
if(hbanner[i]=='\n')break;
}
hbanner[++i]= 0;
}
	/*:305*/	/*318:*/
	#line 6585 "format.w"


static void hdecompress(uint16_t n)
{z_stream z;
uint8_t*buffer;
int i;

DBG(DBGCOMPRESS,"Decompressing section %d from 0x%x to 0x%x byte\n",n,dir[n].size,dir[n].xsize);
z.zalloc= (alloc_func)0;
z.zfree= (free_func)0;
z.opaque= (voidpf)0;
z.next_in= hstart;
z.avail_in= hend-hstart;
if(inflateInit(&z)!=Z_OK)
QUIT("Unable to initialize decompression: %s",z.msg);
ALLOCATE(buffer,dir[n].xsize+MAX_TAG_DISTANCE,uint8_t);
DBG(DBGBUFFER,"Allocating output buffer size=0x%x, margin=0x%x\n",dir[n].xsize,MAX_TAG_DISTANCE);
z.next_out= buffer;
z.avail_out= dir[n].xsize+MAX_TAG_DISTANCE;
i= inflate(&z,Z_FINISH);
DBG(DBGCOMPRESS,"in: avail/total=0x%x/0x%lx ""out: avail/total=0x%x/0x%lx, return %d;\n",
z.avail_in,z.total_in,z.avail_out,z.total_out,i);
if(i!=Z_STREAM_END)
QUIT("Unable to complete decompression: %s",z.msg);
if(z.avail_in!=0)
QUIT("Decompression missed input data");
if(z.total_out!=dir[n].xsize)
QUIT("Decompression output size mismatch 0x%lx != 0x%x",z.total_out,dir[n].xsize);
if(inflateEnd(&z)!=Z_OK)
QUIT("Unable to finalize decompression: %s",z.msg);
dir[n].buffer= buffer;
dir[n].bsize= dir[n].xsize;
hpos0= hpos= hstart= buffer;
hend= hstart+dir[n].xsize;
}
	/*:318*/	/*320:*/
	#line 6673 "format.w"

void hget_section(uint16_t n)
{DBG(DBGDIR,"Reading section %d\n",n);
RNG("Section number",n,0,max_section_no);
if(dir[n].buffer!=NULL&&dir[n].xsize>0)
{hpos0= hpos= hstart= dir[n].buffer;
hend= hstart+dir[n].xsize;
}
else
{hpos0= hpos= hstart= hin_addr+dir[n].pos;
hend= hstart+dir[n].size;
if(dir[n].xsize>0)hdecompress(n);
}
}
	/*:320*/	/*337:*/
	#line 7051 "format.w"

void hget_entry(Entry*e)
{	/*15:*/
	#line 712 "format.w"

uint8_t a,z;
uint32_t node_pos= hpos-hstart;
if(hpos>=hend)QUIT("Attempt to read a start byte at the end of the section");
HGETTAG(a);
	/*:15*/
	#line 7053 "format.w"

DBG(DBGDIR,"Reading directory entry\n");
switch(a)
{case TAG(0,b000+0):HGET_ENTRY(b000+0,*e);break;
case TAG(0,b000+1):HGET_ENTRY(b000+1,*e);break;
case TAG(0,b000+2):HGET_ENTRY(b000+2,*e);break;
case TAG(0,b000+3):HGET_ENTRY(b000+3,*e);break;
case TAG(0,b100+0):HGET_ENTRY(b100+0,*e);break;
case TAG(0,b100+1):HGET_ENTRY(b100+1,*e);break;
case TAG(0,b100+2):HGET_ENTRY(b100+2,*e);break;
case TAG(0,b100+3):HGET_ENTRY(b100+3,*e);break;
default:TAGERR(a);break;
}
	/*16:*/
	#line 719 "format.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
	/*:16*/
	#line 7066 "format.w"

}
	/*:337*/	/*338:*/
	#line 7083 "format.w"

static void hget_root(Entry*root)
{DBG(DBGDIR,"Root entry at "SIZE_F"\n",hpos-hstart);
hget_entry(root);
root->pos= hpos-hstart;
max_section_no= root->section_no;
root->section_no= 0;
if(max_section_no<2)QUIT("Sections 0, 1, and 2 are mandatory");
}

void hget_directory(void)
{int i;
Entry root= {0};
hget_root(&root);
DBG(DBGDIR,"Directory\n");
new_directory(max_section_no+1);
dir[0]= root;
DBG(DBGDIR,"Directory entry 1 at 0x%"PRIx64"\n",dir[0].pos);
hget_section(0);
for(i= 1;i<=max_section_no;i++)
{hget_entry(&(dir[i]));
dir[i].pos= dir[i-1].pos+dir[i-1].size;
DBG(DBGDIR,"Section %d at 0x%"PRIx64"\n",i,dir[i].pos);
}
}

void hclear_dir(void)
{int i;
if(dir==NULL)return;
for(i= 0;i<3;i++)
if(dir[i].xsize>0&&dir[i].buffer!=NULL)free(dir[i].buffer);
free(dir);dir= NULL;
}

	/*:338*/	/*356:*/
	#line 7499 "format.w"

void hget_max_definitions(void)
{Kind k;
	/*15:*/
	#line 712 "format.w"

uint8_t a,z;
uint32_t node_pos= hpos-hstart;
if(hpos>=hend)QUIT("Attempt to read a start byte at the end of the section");
HGETTAG(a);
	/*:15*/
	#line 7502 "format.w"

if(a!=TAG(list_kind,0))QUIT("Start of maximum list expected");
for(k= 0;k<32;k++)max_ref[k]= max_default[k];max_outline= -1;
while(true)
{int n;
if(hpos>=hend)QUIT("Unexpected end of maximum list");
node_pos= hpos-hstart;
HGETTAG(a);
k= KIND(a);
if(k==list_kind)break;
if(INFO(a)&b001)HGET16(n);else n= HGET8;
switch(a)
{	/*238:*/
	#line 4835 "format.w"


case TAG(outline_kind,b100):
case TAG(outline_kind,b101):max_outline= n;
DBG(DBGDEF|DBGLABEL,"max(outline) = %d\n",max_outline);break;
	/*:238*/
	#line 7514 "format.w"

default:
if(max_fixed[k]>max_default[k])
QUIT("Maximum value for kind %s not supported",definition_name[k]);
RNG("Maximum number",n,max_default[k],MAX_REF(k));
max_ref[k]= n;
DBG(DBGDEF,"max(%s) = %d\n",definition_name[k],max_ref[k]);
break;
}
	/*16:*/
	#line 719 "format.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
	/*:16*/
	#line 7523 "format.w"

}
if(INFO(a)!=0)QUIT("End of maximum list with info %d",INFO(a));
}
	/*:356*/
	#line 10169 "format.w"

	/*52:*/
	#line 1228 "format.w"

#define HGET_UTF8C(X)  (X)= HGET8; if ((X&0xC0)!=0x80) \
  QUIT("UTF8 continuation byte expected at " SIZE_F " got 0x%02X\n",hpos-hstart-1,X)

uint32_t hget_utf8(void)
{uint8_t a;
a= HGET8;
if(a<0x80)return a;
else
{if((a&0xE0)==0xC0)
{uint8_t b;HGET_UTF8C(b);
return((a&~0xE0)<<6)+(b&~0xC0);
}
else if((a&0xF0)==0xE0)
{uint8_t b,c;HGET_UTF8C(b);HGET_UTF8C(c);
return((a&~0xF0)<<12)+((b&~0xC0)<<6)+(c&~0xC0);
}
else if((a&0xF8)==0xF0)
{uint8_t b,c,d;HGET_UTF8C(b);HGET_UTF8C(c);HGET_UTF8C(d);
return((a&~0xF8)<<18)+((b&~0xC0)<<12)+((c&~0xC0)<<6)+(d&~0xC0);
}
else QUIT("UTF8 byte sequence expected");
}
}
	/*:52*/	/*75:*/
	#line 1603 "format.w"

float32_t hget_float32(void)
{union{float32_t d;uint32_t bits;}u;
HGET32(u.bits);
return u.d;
}
	/*:75*/	/*145:*/
	#line 2822 "format.w"

void hget_size_boundary(Info info)
{uint32_t n;
if(info<2)return;
n= HGET8;
if(n-1!=0x100-info)QUIT("Size boundary byte 0x%x with info value %d at "SIZE_F,
n,info,hpos-hstart-1);
}

uint32_t hget_list_size(Info info)
{uint32_t n= 0;
if(info==1)return 0;
else if(info==2)n= HGET8;
else if(info==3)HGET16(n);
else if(info==4)HGET24(n);
else if(info==5)HGET32(n);
else QUIT("List info %d must be 1, 2, 3, 4, or 5",info);
return n;
}

void hget_list(List*l)
{if(KIND(*hpos)!=list_kind&&
KIND(*hpos)!=text_kind&&KIND(*hpos)!=param_kind)
QUIT("List expected at 0x%x",(uint32_t)(hpos-hstart));
else
{
	/*15:*/
	#line 712 "format.w"

uint8_t a,z;
uint32_t node_pos= hpos-hstart;
if(hpos>=hend)QUIT("Attempt to read a start byte at the end of the section");
HGETTAG(a);
	/*:15*/
	#line 2848 "format.w"

l->k= KIND(a);
HGET_LIST(INFO(a),*l);
	/*16:*/
	#line 719 "format.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
	/*:16*/
	#line 2851 "format.w"

DBG(DBGNODE,"Get list at 0x%x size=%u\n",l->p,l->s);
}
}
	/*:145*/
	#line 10170 "format.w"

	/*426:*/
	#line 8943 "format.w"

uint32_t hff_list_pos= 0,hff_list_size= 0;
uint8_t hff_tag;
void hff_hpos(void)
{signed char i,k;
hff_tag= *hpos;
DBGTAG(hff_tag,hpos);
i= hnode_size[hff_tag];
if(i>0){hpos= hpos+i;return;}
else if(i<0)
{k= 1+(i&0x3);i= i>>2;
hpos= hpos-i;
while(k>0)
{hff_hpos();k--;}
hpos++;
return;
}
else if(hff_tag<=TAG(param_kind,5))
	/*428:*/
	#line 8988 "format.w"

switch(INFO(hff_tag)){
case 1:hff_list_pos= hpos-hstart+1;hff_list_size= 0;hpos= hpos+2;return;
case 2:hpos++;hff_list_size= HGET8;hff_list_pos= hpos-hstart+1;hpos= hpos+1+hff_list_size+1+1+1;return;
case 3:hpos++;HGET16(hff_list_size);hff_list_pos= hpos-hstart+1;hpos= hpos+1+hff_list_size+1+2+1;return;
case 4:hpos++;HGET24(hff_list_size);hff_list_pos= hpos-hstart+1;hpos= hpos+1+hff_list_size+1+3+1;return;
case 5:hpos++;HGET32(hff_list_size);hff_list_pos= hpos-hstart+1;hpos= hpos+1+hff_list_size+1+4+1;return;
}
	/*:428*/
	#line 8961 "format.w"

TAGERR(hff_tag);
}
	/*:426*/	/*457:*/
	#line 9396 "format.w"

float32_t hteg_float32(void)
{union{float32_t d;uint32_t bits;}u;
HTEG32(u.bits);
return u.d;
}
	/*:457*/	/*495:*/
	#line 9824 "format.w"

void hteg_size_boundary(Info info)
{uint32_t n;
if(info<2)return;
n= HTEG8;
if(n-1!=0x100-info)QUIT("List size boundary byte 0x%x does not match info value %d at "SIZE_F,
n,info,hpos-hstart);
}

uint32_t hteg_list_size(Info info)
{uint32_t n;
if(info==1)return 0;
else if(info==2)n= HTEG8;
else if(info==3)HTEG16(n);
else if(info==4)HTEG24(n);
else if(info==5)HTEG32(n);
else QUIT("List info %d must be 1, 2, 3, 4, or 5",info);
return n;
}

void hteg_list(List*l)
{	/*454:*/
	#line 9368 "format.w"

uint8_t a,z;
uint32_t node_pos= hpos-hstart;
if(hpos<=hstart)return;
HTEGTAG(z);
	/*:454*/
	#line 9845 "format.w"

if(KIND(z)!=list_kind&&KIND(z)!=text_kind&&KIND(z)!=param_kind)
QUIT("List expected at 0x%x",(uint32_t)(hpos-hstart));
else
{uint32_t s;
l->k= KIND(z);
l->s= hteg_list_size(INFO(z));
hteg_size_boundary(INFO(z));
hpos= hpos-l->s;
l->p= hpos-hstart;
hteg_size_boundary(INFO(z));
s= hteg_list_size(INFO(z));
if(s!=l->s)QUIT("List sizes at "SIZE_F" and 0x%x do not match 0x%x != 0x%x",
hpos-hstart,node_pos-1,s,l->s);
	/*455:*/
	#line 9375 "format.w"

HTEGTAG(a);
if(a!=z)QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",NAME(a),INFO(a),NAME(z),INFO(z),
hpos-hstart,node_pos-1);
	/*:455*/
	#line 9859 "format.w"

}
}

void hteg_param_list(List*l)
{if(KIND(*(hpos-1))!=param_kind)return;
hteg_list(l);
}


	/*:495*/
	#line 10171 "format.w"

	/*:508*/
