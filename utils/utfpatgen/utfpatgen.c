/*5:*/
#line 109 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

/*4:*/
#line 100 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

#include "utfpatgen.h"
#include <string.h> 

/*:4*/
#line 110 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"


# ifndef TEST
int main(int argc,char*argv[]){
for(int i= 0;i<argc;i++){
if(strcmp(argv[i],"--help")==0){
print_help();
return EXIT_SUCCESS;
}else if(strcmp(argv[i],"--version")==0){
print_version();
return EXIT_SUCCESS;
}
}
print_version();
/*6:*/
#line 151 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct params*params= init_params();
if(params==NULL){
return EXIT_FAILURE;
}
if(!parse_input(argv,argc,params)){
destroy_params(params);
return EXIT_FAILURE;
}
struct translate_table*tt= init_tr_table(256,128);
if(tt==NULL){
destroy_params(params);
return EXIT_FAILURE;
}
if(!read_translate(params,tt)){
destroy_params(params);
destroy_tr_table(tt);
return EXIT_FAILURE;
}
struct pattern_trie*pt= init_pattern_trie(256,128);
if(pt==NULL){
destroy_params(params);
destroy_tr_table(tt);
return EXIT_FAILURE;
}
struct pass_stats ps;
if(!read_patterns(params,pt,tt,&ps)){
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_FAILURE;
}

/*:6*/
#line 124 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"
;
/*7:*/
#line 190 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

printf("hyph_start (lowest -), hyph_finish (highest hyphenation level): ");
size_t hyph_start,hyph_finish;
int result;
while(true){
result= scanf("%zu %zu",&hyph_start,&hyph_finish);
if(result==2&&hyph_start>=1&&hyph_start<=255&&hyph_finish>=1&&hyph_finish<=254){
break;
}else{
printf("Error: Specify 1 <= hyph_start, hyph_finish <= 255! Insert again: ");
while(getchar()!='\n');
}
}
params->hyph_start= (uint8_t)hyph_start;
if(hyph_start> hyph_finish){
params->hyph_finish= ps.max_level;
printf("Warning: hyph_start > hyph_finish, using hyph_finish = %u\n",ps.max_level);
}else{
params->hyph_finish= (uint8_t)hyph_finish;
}

/*:7*/
#line 125 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"
;
/*8:*/
#line 216 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t pat_start,pat_finish,good_wt,bad_wt,thresh;
for(size_t i= params->hyph_start;i<=params->hyph_finish;i++){
params->hyph_level= i;
ps.level_pattern_cnt= 0;
if(params->hyph_level> params->hyph_start){
printf("\n");
}
if(params->hyph_start<=ps.max_level){
printf("Warning: Largest hyphenation value %u in patterns should be less than hyph_start\n",ps.max_level);
}
/*9:*/
#line 253 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

printf("pat_start (shortest -), pat_finish (longest pattern explored): ");
while(true){
result= scanf("%zu %zu",&pat_start,&pat_finish);
if(result==2&&pat_start>=1&&pat_finish>=1&&pat_start<=pat_finish&&pat_start<=255&&pat_finish<=255){
break;
}else{
printf("Error: Specify 1 <= pat_start <= pat_finish <= 255! Insert again: ");
while(getchar()!='\n');
}
}
params->pat_start= (uint8_t)pat_start;
params->pat_finish= (uint8_t)pat_finish;

printf("good_wt (good -), bad_wt (bad pattern weight), threshold: ");
while(true){
result= scanf("%zu %zu %zu",&good_wt,&bad_wt,&thresh);
if(result==3&&good_wt>=1&&bad_wt>=1&&thresh>=1&&good_wt<=255&&bad_wt<=255&&thresh<=255){
break;
}else{
printf("Error: Specify 1 <= good_wt, bad_wt, threshold <= 255! Insert again: ");
while(getchar()!='\n');
}
}
params->good_wt= (uint8_t)good_wt;
params->bad_wt= (uint8_t)bad_wt;
params->thresh= (uint8_t)thresh;

/*:9*/
#line 227 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"
;
/*10:*/
#line 289 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

uint8_t aux_dot;
bool more_this_level[256];
for(size_t i= 0;i<256;i++){
more_this_level[i]= true;
}
for(size_t j= params->pat_start;j<=params->pat_finish;j++){
params->pat_len= j;
params->pat_dot= params->pat_len/2;
aux_dot= params->pat_dot*2;
while(params->pat_dot!=params->pat_len){
params->pat_dot= aux_dot-params->pat_dot;
aux_dot= params->pat_len*2-aux_dot-1;
if(more_this_level[params->pat_dot]){
if(!process_dictionary(params,tt,pt,&ps)){
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_FAILURE;
}
more_this_level[params->pat_dot]= ps.more_to_come;
}
}
for(size_t i= 255;i> 0;i--){
if(!more_this_level[i-1]){
more_this_level[i]= false;
}
}
}


/*:10*/
#line 228 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"
;
if(!delete_bad_patterns(pt)){
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_FAILURE;
}
printf("total of %zu patterns at hyph_level %u\n",ps.level_pattern_cnt,params->hyph_level);
}

/*:8*/
#line 126 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"
;
/*11:*/
#line 324 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

if(!output_patterns(pt,tt,params->output_file)){
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_FAILURE;
}
char c;
bool output= false;
printf("hyphenate word list? (y/n): ");
if(scanf(" %c",&c)<1){
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_FAILURE;
}
if(c=='y'||c=='Y'){
output= true;
}
if(!hyphenate_dictionary(params,tt,pt,output,&ps)){
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_FAILURE;
}

/*:11*/
#line 127 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"
;
destroy_params(params);
destroy_tr_table(tt);
destroy_pattern_trie(pt);
return EXIT_SUCCESS;
}
# endif
#line 134 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

/*:5*//*14:*/
#line 368 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool parse_input(char*argv[],int argc,struct params*params){
if(argc!=5){
fprintf(stderr,"utfpatgen need exactly 4 arguments.\nTry `utfpatgen --help` for more information.\n");
return false;
}
FILE*dictionary_file= fopen(argv[1],"rb");
if(dictionary_file==NULL){
fprintf(stderr,"Could not open dictionary file '%s'.\n",argv[1]);
return false;
}
FILE*pattern_file= fopen(argv[2],"rb");
if(pattern_file==NULL){
fprintf(stderr,"Could not open pattern file '%s'.\n",argv[2]);
fclose(dictionary_file);
return false;
}
FILE*output_file= fopen(argv[3],"wb");
if(output_file==NULL){
fprintf(stderr,"Could not open output file '%s'.\n",argv[3]);
fclose(dictionary_file);
fclose(pattern_file);
return false;
}
FILE*translate_file= fopen(argv[4],"rb");
if(translate_file==NULL){
fprintf(stderr,"Could not open translate file '%s'.\n",argv[4]);
fclose(dictionary_file);
fclose(pattern_file);
fclose(output_file);
return false;
}
params->dictionary_file= dictionary_file;
params->pattern_file= pattern_file;
params->output_file= output_file;
params->translate_file= translate_file;
return true;
}

/*:14*//*15:*/
#line 412 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool read_line(FILE*stream,struct string_buffer*buf){
reset_buffer(buf);
char c;
while((c= fgetc(stream))!=EOF){
if(buf->size>=buf->capacity-1){
if(resize_buffer(buf,2*buf->capacity)==NULL){
return false;
}
}
if(c=='\n'){
if((buf->size> 0)&&(buf->data[buf->size-1]=='\r')){
buf->size-= 1;
}
break;
}
buf->data[buf->size]= c;
buf->size++;
}
buf->data[buf->size]= '\0';
if(c==EOF){
buf->eof= true;
}
return true;
}

/*:15*//*16:*/
#line 441 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void print_help(){
printf("Usage: utfpatgen [OPTION]... DICTIONARY PATTERNS OUTPUT TRANSLATE\n");
printf("\tGenerate the OUTPUT hyphenation file for use with TeX\n");
printf("\tfrom the DICTIONARY, PATTERNS, and TRANSLATE files.\n");
printf("\n--help        print this help and exit\n");
printf("--version     output version information and exit\n");
printf("\nutfpatgen home page: https://ctan.org/pkg/utfpatgen\n");
}

/*:16*//*17:*/
#line 454 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void print_version(){
printf("This is utfpatgen version %s\n",UTFPATGEN_VERSION);
printf("\tCopyright 2026 Ondřej Metelka\n");
printf("\tLicense MIT: https://mit-license.org/\n");
printf("\nThis is free software: you are free to change and redistribute it.\n");
printf("\tThere is NO WARRANTY, to the extent permitted by law.\n");
}

/*:17*//*19:*/
#line 498 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool read_translate(struct params*params,struct translate_table*tt){
rewind(params->translate_file);
struct string_buffer*buf= init_buffer(64);
if(buf==NULL){
return false;
}
struct trie*helper_trie= init_trie(256);
if(helper_trie==NULL){
destroy_buffer(buf);
return false;
}
if(!read_line(params->translate_file,buf)){
destroy_trie(helper_trie);
destroy_buffer(buf);
return false;
}
if(buf->eof){
bool default_mapping= default_ascii_mapping(tt,helper_trie);
destroy_trie(helper_trie);
destroy_buffer(buf);
return default_mapping;
}

size_t out_index,letter_index;
letter_index= tt->letter_count+1;
if(letter_index>=tt->letter_capacity){
size_t new_capacity= tt->letter_capacity*2;
size_t*new_array= realloc(tt->index_to_alphabet,new_capacity*sizeof(size_t));
if(new_array==NULL){
return false;
}
tt->index_to_alphabet= new_array;
tt->letter_capacity= new_capacity;
}
tt->index_to_alphabet[letter_index]= tt->alphabet->size;
if(!insert_pattern(tt->mapping,(const char[]){EDGE_OF_WORD,'\0'},&out_index,helper_trie)||!append_string(tt->alphabet,(const char[]){EDGE_OF_WORD,'\0'})){
return false;
}
tt->mapping->aux[out_index]= letter_index;
tt->letter_count++;

bool first_line= true;
while(!buf->eof){
if(first_line&&parse_header(buf,params)){

}else if(!parse_letters(buf,tt,helper_trie)){
destroy_trie(helper_trie);
destroy_buffer(buf);
return false;
}
first_line= false;
reset_buffer(buf);
if(!read_line(params->translate_file,buf)){
destroy_trie(helper_trie);
destroy_buffer(buf);
return false;
}
}
destroy_trie(helper_trie);
destroy_buffer(buf);
printf("left_hyphen_min = %u, right_hyphen_min = %u, %zu letters\n",params->left_hyphen_min,params->right_hyphen_min,tt->letter_count);
return true;
}

/*:19*//*20:*/
#line 567 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool is_integer(char c){
return(c>='0'&&c<='9');
}

/*:20*//*21:*/
#line 576 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool is_space(char c){
return(c==' ');
}

/*:21*//*22:*/
#line 586 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool parse_two_digit(struct string_buffer*buf,size_t pos,int8_t*out){
if(pos+1>=buf->size){
return false;
}
char c1= buf->data[pos];
char c2= buf->data[pos+1];
if(is_space(c1)&&is_space(c2)){
return true;
}

if(is_space(c1)){
c1= '0';
}
if(is_space(c2)){
c2= '0';
}
if(!is_integer(c1)||!is_integer(c2)){
return false;
}
*out= (c1-'0')*10+(c2-'0');
return true;
}

/*:22*//*23:*/
#line 615 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool parse_header(struct string_buffer*buf,struct params*params){
int8_t val= -1;
if(!parse_two_digit(buf,0,&val)){
return false;
}else if(val!=-1){
params->left_hyphen_min= val;
}
val= -1;
if(!parse_two_digit(buf,2,&val)){
return false;
}else if(val!=-1){
params->right_hyphen_min= val;
}
if(buf->size>=6&&!is_space(buf->data[5])){
params->bad_hyphen= buf->data[5];
}
if(buf->size>=7&&!is_space(buf->data[6])){
params->missed_hyphen= buf->data[6];
}
if(buf->size>=8&&!is_space(buf->data[7])){
params->good_hyphen= buf->data[7];
}
return true;
}

/*:23*//*24:*/
#line 647 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool parse_letters(struct string_buffer*buf,struct translate_table*tt,struct trie*helper_trie){
if(buf->size==0){
fprintf(stderr,"Empty line in translate file\n");
return false;
}
char separator= buf->data[0];
if(buf->size> 1&&buf->data[1]==separator){
return true;
}
size_t letter_index= tt->letter_count+1;
size_t out_index;
struct string_buffer*letter= init_buffer(4);
if(letter==NULL){
return false;
}
bool lower= true;
if(!append_char(buf,separator)){
destroy_buffer(letter);
return false;
}
for(size_t i= 1;i<buf->size;i++){
char c= buf->data[i];
if(c==separator){
if(letter->size==0){
break;
}
if(!append_char(letter,'\0')||!insert_pattern(tt->mapping,letter->data,&out_index,helper_trie)){
destroy_buffer(letter);
return false;
}
tt->mapping->aux[out_index]= letter_index;
if(lower){
if(letter_index>=tt->letter_capacity){
size_t new_capacity= tt->letter_capacity*2;
size_t*new_array= realloc(tt->index_to_alphabet,new_capacity*sizeof(size_t));
if(new_array==NULL){
destroy_buffer(letter);
return false;
}
tt->index_to_alphabet= new_array;
tt->letter_capacity= new_capacity;
}
tt->index_to_alphabet[letter_index]= tt->alphabet->size;

if(!append_string(tt->alphabet,letter->data)){
destroy_buffer(letter);
return false;
}
tt->letter_count++;
lower= false;
}
reset_buffer(letter);
}else{
if(!append_char(letter,c)){
destroy_buffer(letter);
return false;
}
}
}
destroy_buffer(letter);
return true;
}

/*:24*//*25:*/
#line 715 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool default_ascii_mapping(struct translate_table*tt,struct trie*helper_trie){
size_t out_index;
size_t letter_index;
char upper;
for(char c= 'a';c<='z';c++){
letter_index= tt->letter_count+1;
upper= c-('a'-'A');

if(letter_index>=tt->letter_capacity){
size_t new_capacity= tt->letter_capacity*2;
size_t*new_array= realloc(tt->index_to_alphabet,new_capacity*sizeof(size_t));
if(new_array==NULL){
return false;
}
tt->index_to_alphabet= new_array;
tt->letter_capacity= new_capacity;
}
tt->index_to_alphabet[letter_index]= tt->alphabet->size;
if(!insert_pattern(tt->mapping,(const char[]){c,'\0'},&out_index,helper_trie)||!append_string(tt->alphabet,(const char[]){c,'\0'})){
return false;
}
tt->mapping->aux[out_index]= letter_index;
if(!insert_pattern(tt->mapping,(const char[]){upper,'\0'},&out_index,helper_trie)){
return false;
}
tt->mapping->aux[out_index]= letter_index;
tt->letter_count++;
}
letter_index= tt->letter_count+1;

if(letter_index>=tt->letter_capacity){
size_t new_capacity= tt->letter_capacity*2;
size_t*new_array= realloc(tt->index_to_alphabet,new_capacity*sizeof(size_t));
if(new_array==NULL){
return false;
}
tt->index_to_alphabet= new_array;
tt->letter_capacity= new_capacity;
}
tt->index_to_alphabet[letter_index]= tt->alphabet->size;
if(!insert_pattern(tt->mapping,(const char[]){EDGE_OF_WORD,'\0'},&out_index,helper_trie)||!append_string(tt->alphabet,(const char[]){EDGE_OF_WORD,'\0'})){
return false;
}
tt->mapping->aux[out_index]= letter_index;
tt->letter_count++;
return true;
}

/*:25*//*27:*/
#line 787 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool read_patterns(struct params*params,struct pattern_trie*pt,struct translate_table*tt,struct pass_stats*ps){
ps->level_pattern_cnt= 0;
ps->max_level= 0;
struct string_buffer*buf= init_buffer(16);
if(buf==NULL){
return false;
}
buf->eof= false;
struct pattern*pat= init_pattern(16);
if(pat==NULL){
destroy_buffer(buf);
return false;
}
struct trie*helper_trie= init_trie(256);
if(helper_trie==NULL){
destroy_pattern(pat);
destroy_buffer(buf);
return false;
}
while(!buf->eof){
if(!read_line(params->pattern_file,buf)||!parse_pattern(buf,pat,tt)||!insert_new_pattern(pat,pt,ps,helper_trie)){
destroy_trie(helper_trie);
destroy_pattern(pat);
destroy_buffer(buf);
return false;
}
}
printf("%zu patterns read in\n",ps->level_pattern_cnt);
printf("pattern trie has %zu nodes, trie_max = %zu, %zu outputs\n",pt->t->occupied,pt->t->node_max,pt->ops->count);
destroy_trie(helper_trie);
destroy_pattern(pat);
destroy_buffer(buf);
return true;
}

/*:27*//*28:*/
#line 828 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool parse_pattern(struct string_buffer*buf,struct pattern*out_pattern,struct translate_table*tt){
reset_pattern(out_pattern);
char c;
bool next_hyphen= false;
struct string_buffer*letter= init_buffer(4);
if(letter==NULL){
return false;
}
for(size_t i= 0;i<buf->size;i++){
c= buf->data[i];
if(c==EDGE_OF_WORD&&i!=0&&i!=buf->size-1){
fprintf(stderr,"Edge of word found inside a pattern.\n");
destroy_buffer(letter);
return false;
}
if(next_hyphen){
if(!set_hyphen(out_pattern,out_pattern->size,(uint8_t)c)){
destroy_buffer(letter);
return false;
}
next_hyphen= false;
continue;
}
if(is_utf_start_byte(c)&&letter->size> 0){
if(!append_char(letter,'\0')){
destroy_buffer(letter);
return false;
}
size_t letter_index= get_letter_index(tt,letter->data);
if(letter_index==0){
fprintf(stderr,"Unknown letter %s found in a pattern.\n",letter->data);
destroy_buffer(letter);
return false;
}
if(!convert_index_to_pattern(letter_index,out_pattern)){
destroy_buffer(letter);
return false;
}
reset_buffer(letter);
}
if(c==HYPHEN_FLAG){
next_hyphen= true;
}else if(!append_char(letter,c)){
destroy_buffer(letter);
return false;
}
}
if(next_hyphen){
if(!set_hyphen(out_pattern,out_pattern->size,(uint8_t)c)){
destroy_buffer(letter);
return false;
}
}else if(letter->size> 0){
if(!append_char(letter,'\0')){
destroy_buffer(letter);
return false;
}
size_t letter_index= get_letter_index(tt,letter->data);
if(letter_index==0){
fprintf(stderr,"Unknown letter %s found in a pattern.\n",letter->data);
destroy_buffer(letter);
return false;
}
if(!convert_index_to_pattern(letter_index,out_pattern)){
destroy_buffer(letter);
return false;
}
}
destroy_buffer(letter);
return true;
}

/*:28*//*29:*/
#line 905 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool insert_new_pattern(struct pattern*pat,struct pattern_trie*pt,struct pass_stats*ps,struct trie*helper_trie){
size_t hyphenation_value,node;
size_t current_len= 0;
if(!insert_pattern(pt->t,pat->text,&node,helper_trie)){
return false;
}
for(size_t i= 0;i<pat->size;i++){
hyphenation_value= get_hyphen(pat,i);
if(hyphenation_value> 0){
ps->level_pattern_cnt++;
if(!set_output(pt,node,hyphenation_value,current_len)){
return false;
}
if(hyphenation_value> ps->max_level){
ps->max_level= hyphenation_value;
}
}
if((uint8_t)pat->text[i]!=0xff){
current_len++;
}
}
hyphenation_value= get_hyphen(pat,pat->size);
if(hyphenation_value> 0){
ps->level_pattern_cnt++;
if(!set_output(pt,node,hyphenation_value,current_len)){
return false;
}
}
return true;
}

/*:29*//*31:*/
#line 962 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool process_dictionary(struct params*params,struct translate_table*tt,struct pattern_trie*pt,struct pass_stats*ps){
ps->good_cnt= 0;
ps->bad_cnt= 0;
ps->miss_cnt= 0;
params->word_weight= 1;
if(params->hyph_level%2==1){
params->good_dot= MISS_HYF;
params->bad_dot= NO_HYF;
}else{
params->good_dot= BAD_HYF;
params->bad_dot= GOOD_HYF;
}
struct count_trie*ct= init_count_trie(256,256);
if(ct==NULL){
return false;
}
printf("processing dictionary with pat_len = %u, pat_dot = %u\n",params->pat_len,params->pat_dot);
if(!process_all_words(params,tt,pt,ps,ct)){
destroy_count_trie(ct);
return false;
}
printf("\n%zu good, %zu bad, %zu missed\n",ps->good_cnt,ps->bad_cnt,ps->miss_cnt);
if(ps->good_cnt+ps->miss_cnt> 0){
printf("%.2f %%, %.2f %%, %.2f %%\n",(100*(float)ps->good_cnt/(float)(ps->good_cnt+ps->miss_cnt)),(100*(float)ps->bad_cnt/(float)(ps->good_cnt+ps->miss_cnt)),(100*(float)ps->miss_cnt/(float)(ps->good_cnt+ps->miss_cnt)));
}
printf("%zu patterns, %zu nodes in count trie, triec_max = %zu\n",ct->t->pattern_count,ct->t->occupied,ct->t->node_max);
if(!collect_count_trie(ct,pt,params,ps)){
destroy_count_trie(ct);
return false;
}
destroy_count_trie(ct);
return true;
}

/*:31*//*32:*/
#line 1002 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool process_all_words(struct params*params,struct translate_table*tt,struct pattern_trie*pt,struct pass_stats*ps,struct count_trie*ct){
rewind(params->dictionary_file);
uint8_t dot_min= params->pat_dot;
uint8_t dot_max= params->pat_len-params->pat_dot;
if(dot_min<params->left_hyphen_min+1){
dot_min= params->left_hyphen_min+1;
}
if(dot_max<params->right_hyphen_min+1){
dot_max= params->right_hyphen_min+1;
}
size_t dot_len= dot_min+dot_max;
struct string_buffer*buf= init_buffer(64);
if(buf==NULL){
return false;
}
buf->eof= false;
struct word*word= init_word(16);
if(word==NULL){
destroy_buffer(buf);
return false;
}
struct trie*helper_trie= init_trie(256);
if(helper_trie==NULL){
destroy_word(word);
destroy_buffer(buf);
return false;
}
while(!buf->eof){
if(!read_line(params->dictionary_file,buf)||!parse_word(buf,tt,params,word)||!hyphenate_word(word,pt,params)){
destroy_trie(helper_trie);
destroy_buffer(buf);
destroy_word(word);
return false;
}
count_dots(word,params,ps);
if(word->length>=dot_len){
if(!process_word(word,ct,params,helper_trie)){
destroy_trie(helper_trie);
destroy_buffer(buf);
destroy_word(word);
return false;
}
}
}
destroy_trie(helper_trie);
destroy_buffer(buf);
destroy_word(word);
return true;
}

/*:32*//*33:*/
#line 1057 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool parse_word(struct string_buffer*buf,struct translate_table*tt,struct params*params,struct word*out_word){
reset_word(out_word);
char edge_of_word[]= {EDGE_OF_WORD,'\0'};
struct string_buffer*letter= init_buffer(4);
if(letter==NULL){
return false;
}
uint8_t weight= params->word_weight;
enum hyphen_type hyf= NO_HYF;
size_t letter_index= get_letter_index(tt,edge_of_word);
if(!convert_index(letter_index,out_word)){
destroy_buffer(letter);
return false;
}
char c;
bool has_weight= false;
for(size_t i= 0;i<buf->size;i++){
c= buf->data[i];
if(has_weight){
weight= (uint8_t)c;
if(i==1){
params->word_weight= weight;
}
has_weight= false;
continue;
}else if(c==HYPHEN_FLAG){
has_weight= true;
continue;
}else if(c==params->good_hyphen){
hyf= GOOD_HYF;
continue;
}else if(c==params->missed_hyphen){
hyf= MISS_HYF;
continue;
}else if(c==params->bad_hyphen){
hyf= BAD_HYF;
continue;
}else if(is_utf_start_byte(c)&&letter->size> 0){
if(!append_char(letter,'\0')){
destroy_buffer(letter);
return false;
}
letter_index= get_letter_index(tt,letter->data);
if(letter_index==0){
fprintf(stderr,"Character '%s' in word '%s' not known\n",letter->data,buf->data);
destroy_buffer(letter);
return false;
}
if(!convert_index(letter_index,out_word)){
destroy_buffer(letter);
return false;
}
if(!set_true_hyphen(out_word,out_word->size-1,4*weight+hyf)){
destroy_buffer(letter);
return false;
}
hyf= NO_HYF;
reset_buffer(letter);
}
weight= params->word_weight;
if(!append_char(letter,c)){
destroy_buffer(letter);
return false;
}
}
if(letter->size> 0){
if(!append_char(letter,'\0')){
destroy_buffer(letter);
return false;
}
letter_index= get_letter_index(tt,letter->data);
if(letter_index==0){
fprintf(stderr,"Character '%s' in word '%s' not known\n",letter->data,buf->data);
destroy_buffer(letter);
return false;
}
if(!convert_index(letter_index,out_word)){
destroy_buffer(letter);
return false;
}
}
letter_index= get_letter_index(tt,edge_of_word);
if(!convert_index(letter_index,out_word)||!set_true_hyphen(out_word,0,0)){
destroy_buffer(letter);
return false;
}
destroy_buffer(letter);
return true;
}

/*:33*//*34:*/
#line 1151 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void count_dots(struct word*word,struct params*params,struct pass_stats*ps){
if(word->length<(uint8_t)(params->right_hyphen_min+1)){
return;
}
size_t current_index= 0;
size_t current_pos= 0;
bool odd_level;
size_t dot_index,hyphenation_value,weight;
enum hyphen_type hyf;
for(size_t dot_pos= params->left_hyphen_min+1;dot_pos<(uint8_t)(word->length-params->right_hyphen_min);dot_pos++){
while(current_pos<dot_pos){
if((uint8_t)word->translated[current_index]!=0xff){
current_pos++;
}
current_index++;
}
dot_index= current_index-1;
odd_level= (get_found_hyphen(word,dot_index)%2==1);
hyphenation_value= get_true_hyphen(word,dot_index);
weight= hyphenation_value/4;
hyf= hyphenation_value%4;
if(hyphenation_value==0){
fprintf(stderr,"Code I hoped unreachable was reached\n");
continue;
}else if(hyf%2==0){
if(odd_level){
ps->bad_cnt+= weight;
}
}else{
if(odd_level){
ps->good_cnt+= weight;
}else{
ps->miss_cnt+= weight;
}
}
}
}

/*:34*//*35:*/
#line 1194 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool process_word(struct word*word,struct count_trie*ct,struct params*params,struct trie*helper_trie){
uint8_t dot_min= params->pat_dot;
uint8_t dot_max= params->pat_len-params->pat_dot;
if(dot_min<params->left_hyphen_min+1){
dot_min= params->left_hyphen_min+1;
}
if(dot_max<params->right_hyphen_min+1){
dot_max= params->right_hyphen_min+1;
}
size_t start_pos,end_pos,start_index,dot_index,end_index,node,weight,cnt_index;
size_t current_pos= 0;
size_t current_index= 0;
bool good_pattern;
enum hyphen_type hyf;
for(size_t dot_pos= dot_min;dot_pos+dot_max<=word->length;dot_pos++){
while(current_pos<dot_pos){
if((uint8_t)word->translated[current_index]!=0xff){
current_pos++;
}
current_index++;
}
dot_index= current_index-1;
if(get_no_more(word,dot_index)){
continue;
}
hyf= get_true_hyphen(word,dot_index)%4;
if(get_found_hyphen(word,dot_index)%2==1){
hyf+= 2;
}
if(hyf==params->good_dot){
good_pattern= true;
}else if(hyf==params->bad_dot){
good_pattern= false;
}else{
continue;
}
start_pos= dot_pos;
start_index= current_index;
while(start_pos+params->pat_dot>=dot_pos){
if(start_index==0||(uint8_t)word->translated[start_index-1]!=0xff){
start_pos--;
}
start_index--;
}
start_pos++;
start_index++;
end_pos= dot_pos;
end_index= current_index;
while(end_index<word->size&&end_pos<start_pos+params->pat_len){
if((uint8_t)word->translated[end_index]!=0xff){
end_pos++;
}
end_index++;
}
if(!insert_substring(ct->t,word->translated,end_index,end_index-start_index,&node,helper_trie)){
return false;
}
if(ct->cnts->size>=ct->cnts->capacity){
size_t new_capacity= 2*ct->cnts->capacity;
if(resize_pattern_counts(ct->cnts,new_capacity)==NULL){
return false;
}
}
cnt_index= ct->t->aux[node];
if(cnt_index==0){
cnt_index= ct->cnts->size;
ct->t->aux[node]= cnt_index;
ct->cnts->size++;
}
weight= get_true_hyphen(word,dot_index)/4;
if(weight==0)weight= params->word_weight;
if(good_pattern){
ct->cnts->good[cnt_index]+= weight;
}else{
ct->cnts->bad[cnt_index]+= weight;
}
}
return true;
}

/*:35*//*37:*/
#line 1304 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool collect_count_trie(struct count_trie*ct,struct pattern_trie*pt,struct params*params,struct pass_stats*ps){
double bad_eff= (double)params->thresh/(double)params->good_wt;
ps->good_pat_cnt= 0;
ps->bad_pat_cnt= 0;
ps->good_cnt= 0;
ps->bad_cnt= 0;
ps->more_to_come= false;
if(!traverse_count_trie(ct,pt,params,ps)){
return false;
}
printf("%zu good and %zu bad patterns added",ps->good_pat_cnt,ps->bad_pat_cnt);
ps->level_pattern_cnt+= ps->good_pat_cnt;
if(ps->more_to_come){
printf(" (more to come)\n");
}else{
printf("\n");
}
printf("finding %zu good and %zu bad hyphens",ps->good_cnt,ps->bad_cnt);
if(ps->good_pat_cnt> 0){
printf(", efficiency = %.2lf\n",(double)ps->good_cnt/(ps->good_pat_cnt+((double)ps->bad_cnt/bad_eff)));
}else{
printf("\n");
}
printf("pattern trie has %zu nodes, trie_max = %zu, %zu outputs\n",pt->t->occupied,pt->t->node_max,pt->ops->count);
return true;
}

/*:37*//*38:*/
#line 1337 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool traverse_count_trie(struct count_trie*ct,struct pattern_trie*pt,struct params*params,struct pass_stats*ps){
size_t root= 1;
size_t current_len= 0;
uint8_t c;
struct string_buffer*pattern= init_buffer(4*params->pat_len);
if(pattern==NULL){
return false;
}
struct stack*s_base= init_stack(4*params->pat_len);
if(s_base==NULL){
destroy_buffer(pattern);
return false;
}
struct trie*helper_trie= init_trie(256);
if(helper_trie==NULL){
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}
if(!append_char(pattern,'\0')||!put_on_stack(s_base,root)){
destroy_trie(helper_trie);
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}
size_t node,op_index,good,bad,cnt_index;
while(s_base->top> 0){
root= get_top_value(s_base);
pattern->data[pattern->size-1]+= 1;
c= (uint8_t)pattern->data[pattern->size-1];
if(c==0){
pattern->size--;
s_base->top--;
if(pattern->size<1||(uint8_t)pattern->data[pattern->size-1]!=0xff){
current_len--;
}
continue;
}
node= root+c;
if((uint8_t)ct->t->nodes[node]!=c){
continue;
}
if((uint8_t)c!=0xff){
current_len++;
}
if(current_len==params->pat_len){
cnt_index= ct->t->aux[node];
good= ct->cnts->good[cnt_index];
bad= ct->cnts->bad[cnt_index];
if(good> 0||bad> 0){
if(params->good_wt*good<params->thresh){
if(!insert_substring(pt->t,pattern->data,pattern->size,pattern->size,&op_index,helper_trie)||!set_output(pt,op_index,BAD_OP_VALUE,params->pat_dot)){
destroy_trie(helper_trie);
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}
ps->bad_pat_cnt++;
}else if(params->good_wt*good>=params->thresh+params->bad_wt*bad){
if(!insert_substring(pt->t,pattern->data,pattern->size,pattern->size,&op_index,helper_trie)||!set_output(pt,op_index,params->hyph_level,params->pat_dot)){
destroy_trie(helper_trie);
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}
ps->good_pat_cnt++;
ps->good_cnt+= good;
ps->bad_cnt+= bad;
}else{
ps->more_to_come= true;
}
}
if((uint8_t)c!=0xff){
current_len--;
}
continue;
}
root= ct->t->links[node];
if(root==0){
if((uint8_t)c!=0xff){
current_len--;
}
continue;
}
if(!append_char(pattern,'\0')||!put_on_stack(s_base,root)){
destroy_trie(helper_trie);
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}
}
destroy_trie(helper_trie);
destroy_buffer(pattern);
destroy_stack(s_base);
return true;
}

/*:38*//*40:*/
#line 1444 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool delete_bad_patterns(struct pattern_trie*pt){
size_t old_op_cnt= pt->ops->count;
size_t old_trie_cnt= pt->t->occupied;
if(!delete_patterns(pt)){
return false;
}
for(size_t h= 1;h<=pt->ops->capacity;h++){
if(pt->ops->data[h].value==BAD_OP_VALUE){
pt->ops->data[h].value= EMPTY_OP_VALUE;
pt->ops->count--;
pt->ops->data[h].next_op_index= pt->ops->data[0].next_op_index;
pt->ops->data[0].next_op_index= h;
}
}
printf("%zu nodes and %zu outputs deleted\n",old_trie_cnt-pt->t->occupied,old_op_cnt-pt->ops->count);
return true;
}

/*:40*//*41:*/
#line 1468 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool delete_patterns(struct pattern_trie*pt){
size_t root= 1;
struct stack*s_base= init_stack(16);
if(s_base==NULL){
return false;
}
struct stack*s_offset= init_stack(16);
if(s_offset==NULL){
destroy_stack(s_base);
return false;
}
struct stack*s_freed= init_stack(16);
if(s_freed==NULL){
destroy_stack(s_base);
destroy_stack(s_offset);
return false;
}
if(!put_on_stack(s_base,root)||!put_on_stack(s_offset,0)||!put_on_stack(s_freed,(size_t)true)){
destroy_stack(s_base);
destroy_stack(s_offset);
destroy_stack(s_freed);
return false;
}
size_t node;
uint8_t c;
while(s_base->top> 0){
root= get_top_value(s_base);
set_top_value(s_offset,(uint8_t)get_top_value(s_offset)+1);
c= (uint8_t)get_top_value(s_offset);
if(c==0){
bool child_freed= (get_top_value(s_freed)==(size_t)true);
if(child_freed){
if(!set_base_used(pt->t,root,false)){
destroy_stack(s_base);
destroy_stack(s_offset);
destroy_stack(s_freed);
return false;
}
}
s_offset->top--;
s_base->top--;
s_freed->top--;
if(s_base->top> 0){
size_t parent_root= get_top_value(s_base);
uint8_t parent_c= (uint8_t)get_top_value(s_offset);
size_t parent_node= parent_root+parent_c;
if(child_freed){
pt->t->links[parent_node]= 0;
if(pt->t->aux[parent_node]==0&&parent_root!=1){
deallocate_node(pt->t,parent_node);
}else{
set_top_value(s_freed,(size_t)false);
}
}else{
set_top_value(s_freed,(size_t)false);
}
}
continue;
}
node= root+c;
if((uint8_t)pt->t->nodes[node]!=c){
continue;
}
if(!link_around_bad_outputs(pt,node)){
destroy_stack(s_base);
destroy_stack(s_offset);
destroy_stack(s_freed);
return false;
}
if(pt->t->aux[node]> 0||root==1){
set_top_value(s_freed,(size_t)false);
}else{
if(pt->t->links[node]==0){
deallocate_node(pt->t,node);
continue;
}
}
root= pt->t->links[node];
if(root==0){
continue;
}
if(!put_on_stack(s_base,root)||!put_on_stack(s_offset,0)||!put_on_stack(s_freed,(size_t)true)){
destroy_stack(s_base);
destroy_stack(s_offset);
destroy_stack(s_freed);
return false;
}
}
destroy_stack(s_base);
destroy_stack(s_offset);
destroy_stack(s_freed);
return true;
}

/*:41*//*42:*/
#line 1567 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void deallocate_node(struct trie*t,size_t t_index){
size_t old_head= t->links[0];
t->nodes[t_index]= 0;
t->links[0]= t_index;
t->aux[t_index]= 0;
t->links[t_index]= old_head;
t->aux[old_head]= t_index;
t->occupied--;
}

/*:42*//*43:*/
#line 1582 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool link_around_bad_outputs(struct pattern_trie*pt,size_t t_index){
size_t lookup_index= pt->t->aux[t_index];
if(lookup_index==0){
return true;
}
size_t op_index= pt->ops->lookup[lookup_index];
size_t free_list_head= pt->ops->data[0].next_op_index;
size_t h= 0;
pt->ops->data[0].next_op_index= op_index;
size_t n= pt->ops->data[0].next_op_index;
while(n> 0){
if(pt->ops->data[n].value==BAD_OP_VALUE){
pt->ops->data[h].next_op_index= pt->ops->data[n].next_op_index;
}else{
h= n;
}
n= pt->ops->data[h].next_op_index;
}
if(h==0){
if(pt->ops->lookup[lookup_index]> 0){
pt->ops->lookup[lookup_index]= 0;
pt->ops->lookup_cnt--;
}
pt->t->aux[t_index]= 0;
}else{
pt->ops->lookup[lookup_index]= pt->ops->data[0].next_op_index;
}
pt->ops->data[0].next_op_index= free_list_head;
return true;
}

/*:43*//*45:*/
#line 1625 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool output_patterns(struct pattern_trie*pt,struct translate_table*tt,FILE*output_file){
size_t root= 1;
uint8_t c;
struct string_buffer*pattern= init_buffer(16);
if(pattern==NULL){
return false;
}

struct stack*s_base= init_stack(16);
if(s_base==NULL){
destroy_buffer(pattern);
return false;
}
if(!append_char(pattern,'\0')||!put_on_stack(s_base,root)){
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}

size_t node;
while(s_base->top> 0){
root= get_top_value(s_base);
pattern->data[pattern->size-1]+= 1;
c= (uint8_t)pattern->data[pattern->size-1];
if(c==0){
pattern->data[pattern->size-1]= '\0';
pattern->size--;
s_base->top--;
continue;
}
node= root+c;
if((uint8_t)pt->t->nodes[node]!=c){
continue;
}
if(pt->t->aux[node]> 0){
output_pattern(pattern,tt,pt->ops,pt->t->aux[node],output_file);
}
root= pt->t->links[node];
if(root==0){
continue;
}
if(!append_char(pattern,'\0')||!put_on_stack(s_base,root)){
destroy_buffer(pattern);
destroy_stack(s_base);
return false;
}
}
destroy_buffer(pattern);
destroy_stack(s_base);
return true;
}

/*:45*//*46:*/
#line 1681 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void output_pattern(struct string_buffer*pattern,struct translate_table*tt,struct outputs*ops,size_t op_index,FILE*output_file){
if(op_index==0){
return;
}
size_t pattern_position= 0;
size_t level,letter_index;
char*word_index= pattern->data;
size_t edge_of_word_idx= get_letter_index(tt,(char[2]){EDGE_OF_WORD,'\0'});
while(word_index<pattern->data+pattern->size){
level= get_highest_level(ops,op_index,pattern_position);
if(level> 0){
fputc('\xfe',output_file);
fputc((uint8_t)level,output_file);
}
letter_index= convert_byte_sequence(&word_index);
if(letter_index==edge_of_word_idx){
fputc('.',output_file);
}else{
fprintf(output_file,"%s",tt->alphabet->data+tt->index_to_alphabet[letter_index]);
}
pattern_position++;
}
level= get_highest_level(ops,op_index,pattern_position);
if(level> 0){
fputc('\xfe',output_file);
fputc((uint8_t)level,output_file);
}
fputc('\n',output_file);
}

/*:46*//*47:*/
#line 1716 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t get_highest_level(struct outputs*ops,size_t start_index,size_t position){
size_t highest= 0;
size_t op_index= ops->lookup[start_index];
struct output op;
while(op_index> 0){
op= ops->data[op_index];
if(op.position==position&&op.value!=BAD_OP_VALUE&&op.value> highest){
highest= op.value;
}
op_index= op.next_op_index;
}
return highest;
}

/*:47*//*49:*/
#line 1744 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool hyphenate_word(struct word*word,struct pattern_trie*pt,struct params*params){
size_t current_index= 0;
size_t current_pos= 0;
size_t node,base,start_index,dot_index,end_index,op_index,dot_pos,end_pos;
struct output op;
if(word->length<(uint8_t)(params->right_hyphen_min+1)){
return true;
}
size_t start_pos= 0;
for(size_t i= 0;i<word->length-params->right_hyphen_min;i++){
while(current_pos<start_pos){
if((uint8_t)word->translated[current_index]!=0xff){
current_pos++;
}
current_index++;
}
start_index= current_index;
end_index= current_index;
end_pos= current_pos;
node= 1+(uint8_t)word->translated[start_index];
while(pt->t->nodes[node]==word->translated[end_index]){
if((uint8_t)word->translated[end_index]!=0xff){
end_pos++;
}
end_index++;
op_index= pt->ops->lookup[pt->t->aux[node]];
while(op_index> 0){
op= pt->ops->data[op_index];
dot_pos= start_pos;
dot_index= start_index;
while(dot_pos<start_pos+op.position){
if((uint8_t)word->translated[dot_index]!=0xff){
dot_pos++;
}
dot_index++;
}
if(dot_index> 0){
dot_index--;
if(op.value<BAD_OP_VALUE&&get_found_hyphen(word,dot_index)<op.value){
if(!set_found_hyphen(word,dot_index,op.value)){
return false;
}
}
if(op.value>=params->hyph_level){
if((end_pos+params->pat_dot<=dot_pos+params->pat_len)&&(dot_pos<=start_pos+params->pat_dot)){
if(!set_no_more(word,dot_index,true)){
return false;
}
}
}
}
op_index= op.next_op_index;
}
base= pt->t->links[node];
if(base==0){
break;
}
node= base+(uint8_t)word->translated[end_index];
}
start_pos++;
}
return true;
}

/*:49*//*50:*/
#line 1814 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool hyphenate_dictionary(struct params*params,struct translate_table*tt,struct pattern_trie*pt,bool output,struct pass_stats*ps){
ps->good_cnt= 0;
ps->bad_cnt= 0;
ps->miss_cnt= 0;
params->word_weight= 1;
FILE*pattmp= NULL;
if(output){
char*filename= malloc(11*sizeof(char));
if(filename==NULL){
return false;
}
sprintf(filename,"pattmp.%u",params->hyph_level);
pattmp= fopen(filename,"w");
if(pattmp==NULL){
free(filename);
return false;
}
printf("writing %s\n",filename);
free(filename);
}
if(!hyphenate_all_words(params,tt,pt,pattmp,ps)){
if(pattmp!=NULL){
fclose(pattmp);
}
return false;
}
printf("\n%zu good, %zu bad, %zu missed\n",ps->good_cnt,ps->bad_cnt,ps->miss_cnt);
if(ps->good_cnt+ps->miss_cnt> 0){
printf("%.2f %%, %.2f %%, %.2f %%\n",(100*(float)ps->good_cnt/(float)(ps->good_cnt+ps->miss_cnt)),(100*(float)ps->bad_cnt/(float)(ps->good_cnt+ps->miss_cnt)),(100*(float)ps->miss_cnt/(float)(ps->good_cnt+ps->miss_cnt)));
}
if(pattmp!=NULL){
fclose(pattmp);
}
return true;
}

/*:50*//*51:*/
#line 1855 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool hyphenate_all_words(struct params*params,struct translate_table*tt,struct pattern_trie*pt,FILE*pattmp,struct pass_stats*ps){
rewind(params->dictionary_file);
struct string_buffer*buf= init_buffer(64);
if(buf==NULL){
return false;
}
struct word*word= init_word(16);
if(word==NULL){
destroy_buffer(buf);
return false;
}
buf->eof= false;
while(!buf->eof){
if(!read_line(params->dictionary_file,buf)||!parse_word(buf,tt,params,word)||!hyphenate_word(word,pt,params)){
destroy_buffer(buf);
destroy_word(word);
return false;
}
count_dots(word,params,ps);
if(pattmp!=NULL&&word->length> 2){
output_hyphenated_word(pattmp,word,tt,params);
}
}
destroy_buffer(buf);
destroy_word(word);
return true;
}

/*:51*//*52:*/
#line 1887 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void output_hyphenated_word(FILE*pattmp,struct word*word,struct translate_table*tt,struct params*params){
if(params->word_weight> 1){
fprintf(pattmp,"%d",params->word_weight);
}
size_t weight,letter_index,dot_pos= 0;
size_t edge_of_word_idx= get_letter_index(tt,(char[2]){EDGE_OF_WORD,'\0'});
char*word_index= word->translated;
bool has_hyphen,found_hyphen;
while(word_index<word->translated+word->size){
found_hyphen= (get_found_hyphen(word,word_index-word->translated)%2==1);
weight= get_true_hyphen(word,word_index-word->translated);
has_hyphen= (weight%2==1);
letter_index= convert_byte_sequence(&word_index);
if(letter_index==edge_of_word_idx){
continue;
}
fprintf(pattmp,"%s",tt->alphabet->data+tt->index_to_alphabet[letter_index]);
dot_pos++;
if(weight==0||dot_pos<params->left_hyphen_min||dot_pos>=word->length-params->right_hyphen_min-1){
continue;
}
weight/= 4;
if(weight!=params->word_weight){
fprintf(pattmp,"%zu",weight);
}
if(found_hyphen&&has_hyphen){
fputc((char)params->good_hyphen,pattmp);
}else if(found_hyphen&&!has_hyphen){
fputc((char)params->bad_hyphen,pattmp);
}else if(!found_hyphen&&has_hyphen){
fputc((char)params->missed_hyphen,pattmp);
}
}
fputc('\n',pattmp);
}

/*:52*//*54:*/
#line 1942 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

inline bool is_utf_start_byte(uint8_t byte){
return byte<0x80||byte> 0xbf;
}

/*:54*//*56:*/
#line 2001 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct trie*init_trie(size_t capacity){
struct trie*t= malloc(sizeof(struct trie));
if(t==NULL){
fputs("Allocation error\n",stderr);
return NULL;
}

t->capacity= capacity;

t->nodes= calloc(capacity,sizeof(char));
t->links= calloc(capacity,sizeof(size_t));
t->aux= calloc(capacity,sizeof(size_t));
t->taken= calloc((capacity/8)+1,sizeof(char));

if(t->nodes==NULL||t->links==NULL||t->aux==NULL||t->taken==NULL||!set_base_used(t,1,true)){
fputs("Allocation error\n",stderr);
free(t->nodes);
free(t->links);
free(t->aux);
free(t->taken);
free(t);
return NULL;
}

t->node_max= 0;
t->base_max= 0;
t->occupied= 0;
t->pattern_count= 0;
relink_trie(t);

return t;
}

/*:56*//*57:*/
#line 2040 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool put_first_level(struct trie*t){
size_t root= 1;
size_t n_bytes= 255;
if(t->capacity<n_bytes+2){
size_t new_capacity= (((n_bytes+2)/t->capacity)+1)*t->capacity;
if(resize_trie(t,new_capacity)==NULL){
return false;
}
}
for(size_t i= 1;i<=n_bytes;i++){
t->nodes[root+i]= (uint8_t)i;
t->links[root+i]= 0;
t->aux[root+i]= 0;
}

t->node_max= root+n_bytes;
t->base_max= root;
t->occupied= n_bytes;

if(!set_base_used(t,root,true)){
return false;
}
set_links(t,0,t->node_max+1);
return true;
}

struct trie*resize_trie(struct trie*t,size_t new_capacity){
void*new_nodes= realloc(t->nodes,new_capacity*sizeof(char));
if(new_nodes==NULL){
fputs("Allocation error\n",stderr);return NULL;
}
t->nodes= new_nodes;

size_t*new_links= realloc(t->links,new_capacity*sizeof(size_t));
if(new_links==NULL){
fputs("Allocation error\n",stderr);return NULL;
}
t->links= new_links;

size_t*new_aux= realloc(t->aux,new_capacity*sizeof(size_t));
if(new_aux==NULL){
fputs("Allocation error\n",stderr);return NULL;
}
t->aux= new_aux;

size_t old_taken_bytes= (t->capacity/8)+1;
size_t new_taken_bytes= (new_capacity/8)+1;
char*new_taken= realloc(t->taken,new_taken_bytes*sizeof(char));
if(new_taken==NULL){
fputs("Allocation error\n",stderr);return NULL;
}
t->taken= new_taken;

if(new_taken_bytes> old_taken_bytes){
memset(t->taken+old_taken_bytes,0,(new_taken_bytes-old_taken_bytes));
}
memset(t->nodes+t->capacity,0,(new_capacity-t->capacity)*sizeof(char));
t->capacity= new_capacity;
relink_trie(t);
return t;
}

void destroy_trie(struct trie*t){
free(t->nodes);
free(t->links);
free(t->aux);
free(t->taken);
free(t);
}

/*:57*//*58:*/
#line 2114 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void relink_trie(struct trie*t){
size_t last_free= 0;
for(size_t node= 2;node<t->capacity;node++){
if(!is_node_occupied(t,node)){
set_links(t,last_free,node);
last_free= node;
}
}
set_links(t,last_free,0);
}

/*:58*//*59:*/
#line 2130 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool copy_node(struct trie*from,size_t from_index,struct trie*to,size_t to_index){
if(to_index>=to->capacity){
size_t new_capacity= ((to_index/to->capacity)+1)*to->capacity;
if(resize_trie(to,new_capacity)==NULL){
return false;
}
}
bool from_free= (from->nodes[from_index]==0);
bool to_free= (to->nodes[to_index]==0);
to->nodes[to_index]= from->nodes[from_index];
to->links[to_index]= from->links[from_index];
to->aux[to_index]= from->aux[from_index];
if(!from_free&&to_free){
to->occupied++;
}else if(from_free&&!to_free){
to->occupied--;
}
return true;
}

bool get_base_used(struct trie*t,size_t index){
if(index>=t->capacity){
return false;
}
size_t byte_index= index/8;
size_t bit_index= index%8;
return(t->taken[byte_index]&(1<<bit_index))!=0;
}

bool set_base_used(struct trie*t,size_t index,bool used){
if(index>=t->capacity){
if(resize_trie(t,index+1)==NULL){
return false;
}
}
size_t byte_index= index/8;
size_t bit_index= index%8;
if(used){
t->taken[byte_index]|= (1<<bit_index);
}else{
t->taken[byte_index]&= ~(1<<bit_index);
}

return true;
}

/*:59*//*60:*/
#line 2181 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void set_links(struct trie*t,size_t from,size_t to){
t->links[from]= to;
t->aux[to]= from;
}

/*:60*//*61:*/
#line 2190 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool is_node_occupied(struct trie*t,size_t index){
return t->nodes[index]!=0;
}

/*:61*//*62:*/
#line 2200 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool insert_pattern(struct trie*t,const char*pattern,size_t*out_op_index,struct trie*helper_trie){
size_t length= strlen(pattern);
return insert_substring(t,pattern,length,length,out_op_index,helper_trie);
}

/*:62*//*63:*/
#line 2211 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool insert_substring(struct trie*t,const char*pattern,size_t end,size_t length,size_t*out_op_index,struct trie*helper_trie){
size_t index= end-length;
size_t base= 1;
size_t node= base+(uint8_t)pattern[index];
size_t fit;
size_t node_prev= 0;
bool new_pattern= false;
while(index<end&&base> 0){
node= base+(uint8_t)pattern[index];
if(node>=t->capacity){
size_t new_capacity= ((node/t->capacity)+1)*t->capacity;
if(resize_trie(t,new_capacity)==NULL){
return false;
}
}
if(t->nodes[node]!=pattern[index]){
new_pattern= true;
if(t->nodes[node]==0){
t->nodes[node]= pattern[index];
set_links(t,t->aux[node],t->links[node]);
t->aux[node]= 0;
t->links[node]= 0;
t->occupied++;
if(node> t->node_max){
t->node_max= node;
}
}else{
if(!repack(t,helper_trie,&node_prev,&node,pattern[index])){
return false;
}
}
}
index++;
node_prev= node;
base= t->links[node];
}
helper_trie->links[1]= 0;
helper_trie->aux[1]= 0;
helper_trie->node_max= 1;
while(index<end){
helper_trie->nodes[1]= pattern[index];
if(!first_fit(t,helper_trie,&fit)){
return false;
}
t->links[node]= fit;
base= fit;
node= base+(uint8_t)pattern[index];
index++;
new_pattern= true;
}
*out_op_index= node;
if(new_pattern){
t->pattern_count++;
}
return true;
}

/*:63*//*64:*/
#line 2274 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool repack(struct trie*t,struct trie*q,size_t*node,size_t*base,char value){
if(!unpack(t,*base-(uint8_t)value,q)){
return false;
}
if(q->node_max>=q->capacity){
size_t new_capacity= ((q->node_max/q->capacity)+1)*q->capacity;
if(resize_trie(q,new_capacity)==NULL){
return false;
}
}
q->nodes[q->node_max]= value;
q->links[q->node_max]= 0;
q->aux[q->node_max]= 0;
size_t fit;
if(!first_fit(t,q,&fit)){
return false;
}
*base= fit;
t->links[*node]= *base;
*base+= (uint8_t)value;
return true;
}

/*:64*//*65:*/
#line 2301 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool unpack(struct trie*from,size_t base,struct trie*to){
to->node_max= 1;
for(size_t i= 1;i<256;i++){
size_t from_index= base+i;
if((uint8_t)from->nodes[from_index]==i){
if(!copy_node(from,from_index,to,to->node_max)){
return false;
}
deallocate_node(from,from_index);
to->node_max++;
}
}
if(!set_base_used(from,base,false)){
return false;
}
return true;
}

/*:65*//*66:*/
#line 2325 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool first_fit(struct trie*t,struct trie*q,size_t*out_base){
size_t base;
if(!find_base_for_first_fit(t,q,&base)){
return false;
}
for(size_t q_index= 1;q_index<=q->node_max;q_index++){
size_t t_index= base+(uint8_t)q->nodes[q_index];
set_links(t,t->aux[t_index],t->links[t_index]);
if(!copy_node(q,q_index,t,t_index)){
return false;
}
if(t_index> t->node_max){
t->node_max= t_index;
}
}
if(!set_base_used(t,base,true)){
return false;
}
*out_base= base;
return true;
}

/*:66*//*67:*/
#line 2353 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool find_base_for_first_fit(struct trie*t,struct trie*q,size_t*out_base){
size_t t_index;
uint8_t offset;
if(q->node_max> 5&&t->capacity> t->node_max+1){
t_index= t->node_max+1;
}else{
t_index= t->links[0];
}
for(size_t i= 0;i<=t->capacity;i++){
if(t_index==0){
t_index= t->capacity+1;
size_t new_capacity= 2*t->capacity;
if(!resize_trie(t,new_capacity)){
return false;
}
}
offset= (uint8_t)q->nodes[1];
if(t_index<=offset){
t_index= t->links[t_index];
continue;
}
*out_base= t_index-offset;
size_t max_target_index= *(out_base)+255;
if(max_target_index>=t->capacity){
size_t new_capacity= ((max_target_index/t->capacity)+1)*t->capacity;
if(resize_trie(t,new_capacity)==NULL){
return false;
}
}
if(get_base_used(t,*out_base)){
t_index= t->links[t_index];
continue;
}
bool conflict= false;
for(size_t q_index= q->node_max;q_index>=2;q_index--){
if(is_node_occupied(t,*out_base+(uint8_t)q->nodes[q_index])){
conflict= true;
break;
}
}
if(!conflict){
return true;
}
t_index= t->links[t_index];
}
printf("Loop detected!\n");
return false;
}

/*:67*//*68:*/
#line 2407 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t traverse_trie(struct trie*t,const char*pattern){
size_t index= 1;
size_t node= (uint8_t)pattern[0]+1;
size_t base= t->links[node];
while(index<strlen(pattern)&&base> 0){
base+= (uint8_t)pattern[index];
if(t->nodes[base]!=pattern[index]){
return 0;
}
node= base;
base= t->links[node];
index++;
}
if(index<strlen(pattern)){
return 0;
}
return node;
}

/*:68*//*69:*/
#line 2459 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct outputs*init_outputs(size_t capacity){
struct outputs*ops= malloc(sizeof(struct outputs));
if(ops==NULL){
fputs("Allocation error\n",stderr);
return NULL;
}
ops->capacity= capacity;
ops->count= 0;
ops->data= calloc(capacity+1,sizeof(struct output));
if(ops->data==NULL){
fputs("Allocation error\n",stderr);
free(ops);
return NULL;
}
ops->data[0].next_op_index= 1;
ops->lookup_cap= 2*capacity;
ops->lookup_cnt= 0;
ops->lookup= calloc(2*capacity+1,sizeof(size_t));
if(ops->lookup==NULL){
fputs("Allocation error\n",stderr);
free(ops->data);
free(ops);
return NULL;
}
return ops;
}

struct outputs*resize_outputs(struct outputs*ops,size_t capacity){
struct output*new_data= realloc(ops->data,(capacity+1)*sizeof(struct output));
if(new_data==NULL){
fputs("Allocation error\n",stderr);
return NULL;
}
ops->data= new_data;
size_t diff= capacity-ops->capacity;
memset(ops->data+ops->capacity+1,0,diff*sizeof(struct output));
ops->capacity= capacity;
return ops;
}

void destroy_outputs(struct outputs*ops){
free(ops->data);
free(ops->lookup);
free(ops);
}

bool resize_lookup(struct outputs*ops,size_t new_cap,struct trie*t){
size_t*new_lookup= calloc(new_cap+1,sizeof(size_t));
size_t*old_lookup= ops->lookup;
if(new_lookup==NULL){
return false;
}
ops->lookup= new_lookup;
ops->lookup_cap= new_cap;
size_t old_hash,new_hash,op_index;
struct output op;
for(size_t node= 0;node<t->capacity;node++){
if(!is_node_occupied(t,node)||t->aux[node]==0){
continue;
}
old_hash= t->aux[node];
op_index= old_lookup[old_hash];
op= ops->data[op_index];
new_hash= hash_trie_output(ops,op.value,op.position,op.next_op_index);
ops->lookup[new_hash]= op_index;
t->aux[node]= new_hash;
}
free(old_lookup);
return true;
}

/*:69*//*70:*/
#line 2535 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t hash_trie_output(struct outputs*ops,size_t value,size_t position,size_t next_op_index){
size_t hash= ((next_op_index+313*position+361*value)%ops->lookup_cap)+1;
size_t op_index;
while(true){
op_index= ops->lookup[hash];
if(op_index==0){
return hash;
}else if(ops->data[op_index].value==value&&ops->data[op_index].position==position&&ops->data[op_index].next_op_index==next_op_index){
return hash;
}else if(hash> 1){
hash-= 1;
}else{
hash= ops->lookup_cap;
}
}
return 0;
}

/*:70*//*71:*/
#line 2564 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct pattern_trie*init_pattern_trie(size_t trie_capacity,size_t outputs_capacity){
struct pattern_trie*pt= malloc(sizeof(struct pattern_trie));
if(pt==NULL){
return NULL;
}
pt->t= init_trie(trie_capacity);
if(pt->t==NULL){
free(pt);
return NULL;
}
if(!put_first_level(pt->t)){
free(pt->t);
free(pt);
return NULL;
}
pt->ops= init_outputs(outputs_capacity);
if(pt->ops==NULL){
free(pt->t);
free(pt);
return NULL;
}
return pt;
}

void destroy_pattern_trie(struct pattern_trie*pt){
destroy_trie(pt->t);
destroy_outputs(pt->ops);
free(pt);
}

/*:71*//*72:*/
#line 2599 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool new_trie_output(struct pattern_trie*pt,size_t value,size_t position,size_t next_op_index,size_t*op_index){
if(pt->ops->count>=pt->ops->capacity-1){
if(resize_outputs(pt->ops,pt->ops->capacity*2)==NULL){
return false;
}
}
if(pt->ops->lookup_cnt*4> pt->ops->lookup_cap*3){
if(!resize_lookup(pt->ops,pt->ops->lookup_cap*2,pt->t)){
return false;
}
}
size_t hash= hash_trie_output(pt->ops,value,position,next_op_index);
if(pt->ops->lookup[hash]==0){
pt->ops->count++;
struct output new_op= {.value= value,.position= position,.next_op_index= next_op_index};
size_t free_list_head= pt->ops->data[0].next_op_index;
if(pt->ops->data[free_list_head].next_op_index==0){
pt->ops->data[0].next_op_index= pt->ops->count+1;
}else{
pt->ops->data[0].next_op_index= pt->ops->data[free_list_head].next_op_index;
}
pt->ops->data[free_list_head]= new_op;
pt->ops->lookup[hash]= free_list_head;
pt->ops->lookup_cnt++;
}
*op_index= hash;
return true;
}

/*:72*//*73:*/
#line 2633 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool set_output(struct pattern_trie*pt,size_t node,size_t value,size_t position){
size_t op_index;
if(!new_trie_output(pt,value,position,pt->ops->lookup[pt->t->aux[node]],&op_index)){
return false;
}
pt->t->aux[node]= op_index;
return true;
}

/*:73*//*74:*/
#line 2655 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct pattern_counts*init_pattern_counts(size_t capacity){
struct pattern_counts*pc= malloc(sizeof(struct pattern_counts));
if(pc==NULL){
fprintf(stderr,"Allocation error\n");
return NULL;
}
pc->good= calloc(capacity,sizeof(size_t));
pc->bad= calloc(capacity,sizeof(size_t));
if(pc->good==NULL||pc->bad==NULL){
fprintf(stderr,"Allocation error\n");
free(pc->good);
free(pc->bad);
free(pc);
return NULL;
}
pc->capacity= capacity;
pc->size= 1;
return pc;
}

struct pattern_counts*resize_pattern_counts(struct pattern_counts*pc,size_t new_capacity){
size_t*new_good= realloc(pc->good,new_capacity*sizeof(size_t));
if(new_good==NULL){
fprintf(stderr,"Allocation error\n");
return NULL;
}
pc->good= new_good;
size_t*new_bad= realloc(pc->bad,new_capacity*sizeof(size_t));
if(new_bad==NULL){
fprintf(stderr,"Allocation error\n");
return NULL;
}
pc->bad= new_bad;
size_t diff= new_capacity-pc->capacity;
memset(pc->good+pc->capacity,0,diff*sizeof(size_t));
memset(pc->bad+pc->capacity,0,diff*sizeof(size_t));
pc->capacity= new_capacity;
return pc;
}

void destroy_pattern_counts(struct pattern_counts*pc){
free(pc->good);
free(pc->bad);
free(pc);
}

/*:74*//*75:*/
#line 2716 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct count_trie*init_count_trie(size_t trie_capacity,size_t counts_capacity){
struct count_trie*ct= malloc(sizeof(struct count_trie));
if(ct==NULL){
return NULL;
}
ct->t= init_trie(trie_capacity);
if(ct->t==NULL){
free(ct);
return NULL;
}
if(!put_first_level(ct->t)){
free(ct->t);
free(ct);
return NULL;
}
ct->cnts= init_pattern_counts(counts_capacity);
if(ct->cnts==NULL){
free(ct->t);
free(ct);
return NULL;
}
return ct;
}

void destroy_count_trie(struct count_trie*ct){
destroy_trie(ct->t);
destroy_pattern_counts(ct->cnts);
free(ct);
}

/*:75*//*76:*/
#line 2758 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct string_buffer*init_buffer(size_t capacity){
struct string_buffer*buf= malloc(sizeof(struct string_buffer));
if(buf==NULL){
fputs("Allocation error\n",stderr);
return NULL;
}
buf->capacity= capacity;
buf->size= 0;
buf->data= malloc(capacity*sizeof(char));
buf->eof= false;
if(buf->data==NULL){
fputs("Allocation error\n",stderr);
free(buf);
return NULL;
}
buf->data[0]= '\0';
return buf;
}

struct string_buffer*resize_buffer(struct string_buffer*buf,size_t new_capacity){
char*new_ptr= realloc(buf->data,new_capacity);
if(new_ptr==NULL){
fputs("Allocation error\n",stderr);
return NULL;
}
buf->data= new_ptr;
buf->capacity= new_capacity;
return buf;
}

void reset_buffer(struct string_buffer*buf){
buf->eof= false;
buf->size= 0;
buf->data[0]= '\0';
}

void destroy_buffer(struct string_buffer*buf){
free(buf->data);
free(buf);
}

/*:76*//*77:*/
#line 2805 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool append_char(struct string_buffer*buf,char c){
if(buf->size+1>=buf->capacity){
if(resize_buffer(buf,2*buf->capacity)==NULL){
return false;
}
}
buf->data[buf->size]= c;
buf->size++;
return true;
}

/*:77*//*78:*/
#line 2822 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool append_string(struct string_buffer*buf,const char*str){
size_t len= strlen(str)+1;
if(buf->size+len>=buf->capacity){
if(resize_buffer(buf,2*(buf->size+len))==NULL){
return false;
}
}
strcpy(&buf->data[buf->size],str);
buf->size+= len;
return true;
}

/*:78*//*79:*/
#line 2849 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct translate_table*init_tr_table(size_t mapping_capacity,size_t alphabet_capacity){
struct translate_table*tt= malloc(sizeof(struct translate_table));
if(tt==NULL){
fprintf(stderr,"Allocation error\n");
return NULL;
}
struct trie*mapping= init_trie(mapping_capacity);
if(mapping==NULL){
free(tt);
return NULL;
}
if(!put_first_level(mapping)){
destroy_trie(mapping);
free(tt);
return NULL;
}
struct string_buffer*alphabet= init_buffer(alphabet_capacity);
if(alphabet==NULL){
destroy_trie(mapping);
free(tt);
return NULL;
}
tt->mapping= mapping;
tt->alphabet= alphabet;
tt->index_to_alphabet= malloc(alphabet_capacity*sizeof(size_t));
if(tt->index_to_alphabet==NULL){
destroy_trie(tt->mapping);
destroy_buffer(tt->alphabet);
free(tt);
return NULL;
}
tt->letter_count= 0;
tt->letter_capacity= alphabet_capacity;
if(!append_char(tt->alphabet,'\0')){
free(tt->index_to_alphabet);
destroy_trie(tt->mapping);
destroy_buffer(tt->alphabet);
free(tt);
return NULL;
}
return tt;
}

void destroy_tr_table(struct translate_table*tt){
destroy_trie(tt->mapping);
destroy_buffer(tt->alphabet);
free(tt->index_to_alphabet);
free(tt);
}

/*:79*//*80:*/
#line 2904 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

char*get_lower(struct translate_table*tt,const char*letter){
size_t index= traverse_trie(tt->mapping,letter);
if(index==0||tt->mapping->aux[index]> tt->letter_count||tt->mapping->aux[index]==0){
return NULL;
}
size_t alphabet_offset= tt->index_to_alphabet[tt->mapping->aux[index]];
if(alphabet_offset>=tt->alphabet->size){
return NULL;
}
return tt->alphabet->data+alphabet_offset;
}

/*:80*//*81:*/
#line 2921 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t get_letter_index(struct translate_table*tt,char*letter){
size_t index= traverse_trie(tt->mapping,letter);
if(index==0||tt->mapping->aux[index]> tt->letter_count||tt->mapping->aux[index]==0){
return 0;
}
return tt->mapping->aux[index];
}

/*:81*//*82:*/
#line 2936 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool convert_index(size_t index,struct word*word){
if(index==0){
return true;
}
size_t ff_count= (index-1)/254;
size_t remainder= ((index-1)%254)+1;
for(size_t i= 0;i<ff_count;i++){
if(!append_char_to_word(word,(char)0xff)){
return false;
}
}
if(!append_char_to_word(word,(char)remainder)){
return false;
}
return true;
}

/*:82*//*83:*/
#line 2958 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t convert_byte_sequence(char**sequence){
if(sequence==NULL||*sequence==NULL){
return 0;
}
size_t ff_count= 0;
while(**sequence==(char)0xff){
ff_count++;
(*sequence)++;
}
size_t remainder= (uint8_t)**sequence;
(*sequence)++;
return ff_count*254+remainder;
}

/*:83*//*84:*/
#line 3019 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct params*init_params(){
struct params*p= malloc(sizeof(struct params));
if(p==NULL){
fputs("Allocation error\n",stderr);
return NULL;
}
p->left_hyphen_min= 2;
p->right_hyphen_min= 3;
p->bad_hyphen= '.';
p->missed_hyphen= '-';
p->good_hyphen= '*';

p->word_weight= 1;

p->dictionary_file= NULL;
p->pattern_file= NULL;
p->output_file= NULL;
p->translate_file= NULL;
return p;
}

void reset_params(struct params*p){
p->left_hyphen_min= 2;
p->right_hyphen_min= 3;
p->bad_hyphen= '.';
p->missed_hyphen= '-';
p->good_hyphen= '*';
}

void destroy_params(struct params*p){
if(p->dictionary_file!=NULL){
fclose(p->dictionary_file);
}
if(p->pattern_file!=NULL){
fclose(p->pattern_file);
}
if(p->output_file!=NULL){
fclose(p->output_file);
}
if(p->translate_file!=NULL){
fclose(p->translate_file);
}
free(p);
}

/*:84*//*86:*/
#line 3097 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct stack*init_stack(size_t capacity){
struct stack*s= malloc(sizeof(struct stack));
if(s==NULL){
fprintf(stderr,"Allocation error\n");
return NULL;
}
s->data= malloc(capacity*sizeof(size_t));
if(s->data==NULL){
fprintf(stderr,"Allocation error\n");
free(s->data);
free(s);
return NULL;
}
s->capacity= capacity;
s->top= 0;
return s;
}

struct stack*resize_stack(struct stack*s,size_t new_capacity){
size_t*new_stack= realloc(s->data,new_capacity*sizeof(size_t));
if(new_stack==NULL){
fprintf(stderr,"Allocation error\n");
destroy_stack(s);
return NULL;
}
s->data= new_stack;
s->capacity= new_capacity;
return s;
}

void destroy_stack(struct stack*s){
free(s->data);
free(s);
}

/*:86*//*87:*/
#line 3137 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool put_on_stack(struct stack*s,size_t value){
if(s->top>=s->capacity){
size_t new_capacity= 2*(s->top);
if(resize_stack(s,new_capacity)==NULL){
return false;
}
}
s->data[s->top]= value;
s->top++;
return true;
}

/*:87*//*88:*/
#line 3154 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

size_t get_top_value(struct stack*s){
if(s->top==0){
return 0;
}
return s->data[s->top-1];
}

/*:88*//*89:*/
#line 3165 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

void set_top_value(struct stack*s,size_t value){
if(s->top==0){
return;
}
s->data[s->top-1]= value;
}

/*:89*//*90:*/
#line 3197 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct word*init_word(size_t capacity){
struct word*word= malloc(sizeof(struct word));
if(word==NULL){
return NULL;
}
word->translated= calloc(capacity,sizeof(char));
if(word->translated==NULL){
free(word);
return NULL;
}
word->true_hyphens= calloc(capacity,sizeof(size_t));
if(word->true_hyphens==NULL){
free(word->translated);
free(word);
return NULL;
}
word->found_hyphens= calloc(capacity,sizeof(uint8_t));
if(word->found_hyphens==NULL){
free(word->translated);
free(word->true_hyphens);
free(word);
return NULL;
}
word->no_more= calloc(capacity,sizeof(bool));
if(word->no_more==NULL){
free(word->translated);
free(word->true_hyphens);
free(word->found_hyphens);
free(word);
return NULL;
}
word->size= 0;
word->length= 0;
word->capacity= capacity;
return word;
}

struct word*resize_word(struct word*word,size_t new_capacity){
char*new_translated= realloc(word->translated,new_capacity*sizeof(char));
if(new_translated==NULL){fprintf(stderr,"Allocation error\n");return NULL;}
word->translated= new_translated;

size_t*new_true_hyphens= realloc(word->true_hyphens,new_capacity*sizeof(size_t));
if(new_true_hyphens==NULL){fprintf(stderr,"Allocation error\n");return NULL;}
word->true_hyphens= new_true_hyphens;

uint8_t*new_found_hyphens= realloc(word->found_hyphens,new_capacity*sizeof(uint8_t));
if(new_found_hyphens==NULL){fprintf(stderr,"Allocation error\n");return NULL;}
word->found_hyphens= new_found_hyphens;

bool*new_no_more= realloc(word->no_more,new_capacity*sizeof(bool));
if(new_no_more==NULL){fprintf(stderr,"Allocation error\n");return NULL;}
word->no_more= new_no_more;

size_t diff= new_capacity-word->capacity;
memset(word->translated+word->capacity,'\0',diff*sizeof(char));
memset(word->true_hyphens+word->capacity,0,diff*sizeof(size_t));
memset(word->found_hyphens+word->capacity,0,diff*sizeof(uint8_t));
memset(word->no_more+word->capacity,false,diff*sizeof(bool));

word->capacity= new_capacity;
return word;
}

void reset_word(struct word*word){
word->length= 0;
word->size= 0;
memset(word->translated,0,word->capacity*sizeof(char));
memset(word->true_hyphens,0,word->capacity*sizeof(size_t));
memset(word->found_hyphens,0,word->capacity*sizeof(uint8_t));
memset(word->no_more,false,word->capacity*sizeof(bool));
}

void destroy_word(struct word*word){
free(word->translated);
free(word->true_hyphens);
free(word->found_hyphens);
free(word->no_more);
free(word);
}

size_t get_true_hyphen(struct word*word,size_t index){
if(index>=word->size){
return 0;
}
return word->true_hyphens[index];
}

bool set_true_hyphen(struct word*word,size_t index,size_t value){
if(index>=word->size){
return false;
}
word->true_hyphens[index]= value;
return true;
}

uint8_t get_found_hyphen(struct word*word,size_t index){
if(index>=word->size){
return 0;
}
return word->found_hyphens[index];
}

bool set_found_hyphen(struct word*word,size_t index,uint8_t value){
if(index>=word->size){
return false;
}
word->found_hyphens[index]= value;
return true;
}

bool get_no_more(struct word*word,size_t index){
if(index>=word->size){
return false;
}
return word->no_more[index];
}

bool set_no_more(struct word*word,size_t index,bool value){
if(index>=word->size){
return false;
}
word->no_more[index]= value;
return true;
}
/*:90*//*91:*/
#line 3327 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool append_char_to_word(struct word*word,char c){
if(word->size>=word->capacity-1){
if(!resize_word(word,2*word->capacity)){
return false;
}
}
word->translated[word->size]= c;
word->size++;
if((uint8_t)c!=0xff){
word->length++;
}
return true;
}

/*:91*//*92:*/
#line 3354 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

struct pattern*init_pattern(size_t capacity){
struct pattern*pat= malloc(sizeof(struct pattern));
if(pat==NULL){
return NULL;
}
pat->text= calloc(capacity,sizeof(char));
if(pat->text==NULL){
free(pat);
return NULL;
}
pat->hyphens= calloc(capacity,sizeof(uint8_t));
if(pat->hyphens==NULL){
free(pat->text);
free(pat);
return NULL;
}

pat->length= 0;
pat->size= 0;
pat->capacity= capacity;
return pat;
}

struct pattern*resize_pattern(struct pattern*pat,size_t new_capacity){
char*new_text= realloc(pat->text,new_capacity*sizeof(char));
uint8_t*new_hyphens= realloc(pat->hyphens,new_capacity*sizeof(uint8_t));

if(new_text==NULL||new_hyphens==NULL){
fprintf(stderr,"Allocation error\n");
return NULL;
}

pat->text= new_text;
pat->hyphens= new_hyphens;

memset(pat->text+pat->capacity,'\0',(new_capacity-pat->capacity)*sizeof(char));
memset(pat->hyphens+pat->capacity,0,(new_capacity-pat->capacity)*sizeof(uint8_t));

pat->capacity= new_capacity;
return pat;
}

void reset_pattern(struct pattern*pat){
pat->length= 0;
pat->size= 0;
memset(pat->text,'\0',pat->capacity*sizeof(char));
memset(pat->hyphens,0,pat->capacity*sizeof(uint8_t));
}

void destroy_pattern(struct pattern*pat){
free(pat->text);
free(pat->hyphens);
free(pat);
}

uint8_t get_hyphen(struct pattern*pat,size_t index){
if(index>=pat->capacity){
return 0;
}
return pat->hyphens[index];
}

bool set_hyphen(struct pattern*pat,size_t index,uint8_t value){
if(index>=pat->capacity){
return false;
}
pat->hyphens[index]= value;
return true;
}

/*:92*//*93:*/
#line 3431 "../../../utils/utfpatgen/utfpatgen-src/utfpatgen.w"

bool convert_index_to_pattern(size_t index,struct pattern*pat){
if(index==0)return true;

size_t ff_count= (index-1)/254;
size_t remainder= ((index-1)%254)+1;
size_t total_bytes= ff_count+1;

if(pat->size>=pat->capacity-total_bytes){
if(!resize_pattern(pat,2*(pat->capacity+total_bytes))){
return false;
}
}

for(size_t i= 0;i<ff_count;i++){
pat->text[pat->size]= (char)0xff;
pat->size++;
}

pat->text[pat->size]= (char)remainder;
pat->size++;

return true;
}

/*:93*/
