
char *string_list_to_str(char **list, const char *sep);
void string_list_print(char **list);
char **string_list_reorder(char **list, char *str);
char **string_list_move_to_start(char **list, size_t idx);
char **string_list_rotate_down(char **list);
char **string_list_rotate_up(char **list);
char **string_list_prepend(char **list, const char *str);
