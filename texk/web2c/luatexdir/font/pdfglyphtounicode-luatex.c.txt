static void set_glyph_unicode(char *s, glyph_unicode_entry * gp)
{
    char buf[SMALL_BUF_SIZE], buf2[SMALL_BUF_SIZE], *p;
    long code;
    boolean last_component;
    glyph_unicode_entry tmp, *ptmp;
    /*tex Skip dummy entries. */
    if (s == NULL || s == notdef)
        return;
    /*tex Strip everything after the first dot. */
    p = strchr(s, '.');
    if (p != NULL) {
        *buf = 0;
        strncat(buf, s, (size_t) (p - s));
        s = buf;
    }
    if (strlen(s) == 0)
        return;
    /*tex Check for case of multiple components separated by |_|. */
    p = strchr(s, '_');
    if (p != NULL) {
        assert(strlen(s) < sizeof(buf));
        if (s != buf) {
            strcpy(buf, s);
            p = strchr(buf, '_');
            s = buf;
        }
        *buf2 = 0;
        last_component = false;
        for (;;) {
            *p = 0;
            tmp.code = UNI_UNDEF;
            set_glyph_unicode(s, &tmp);
            switch (tmp.code) {
            case UNI_UNDEF:
                /*tex Not found, do nothing. */
                break;
            case UNI_STRING:
                /*tex |s| matched an entry with string value in the database. */
                assert(tmp.unicode_seq != NULL);
                assert(strlen(buf2) + strlen(tmp.unicode_seq) < sizeof(buf2));
                strcat(buf2, tmp.unicode_seq);
                break;
            case UNI_EXTRA_STRING:
                /*tex |s| is a multiple value of form "uniXXXX" */
                assert(strlen(buf2) + strlen(tmp.unicode_seq) < sizeof(buf2));
                strcat(buf2, tmp.unicode_seq);
                xfree(tmp.unicode_seq);
                break;
            default:
                /*tex
                    |s| matched an entry with numeric value in the database, or a
                    value derived from |uXXXX|.
                */
                assert(tmp.code >= 0);
                strcat(buf2, utf16be_str(tmp.code));
            }
            if (last_component)
                break;
            s = p + 1;
            p = strchr(s, '_');
            if (p == NULL) {
                p = strend(s);
                last_component = true;
            }
        }
        gp->code = UNI_EXTRA_STRING;
        gp->unicode_seq = xstrdup(buf2);
        return;
    }
    /*tex Lookup glyph name in the database. */
    tmp.name = s;
    tmp.code = UNI_UNDEF;
    ptmp = (glyph_unicode_entry *) avl_find(glyph_unicode_tree, &tmp);
    if (ptmp != NULL) {
        gp->code = ptmp->code;
        gp->unicode_seq = ptmp->unicode_seq;
        return;
    }
    /*tex Check for case of |uniXXXX|, multiple 4-hex-digit values allowed. */
    if (str_prefix(s, "uni")) {
        p = s + strlen("uni");
        code = check_unicode_value(p, true);
        if (code != UNI_UNDEF) {
            if (strlen(p) == 4) {
                /*tex Single value: */
                gp->code = code;
            } else {
                /*tex Multiple value: */
                gp->code = UNI_EXTRA_STRING;
                gp->unicode_seq = xstrdup(p);
            }
        }
        /*tex Since the last case cannot happen: */
        return;
    }
    /*tex Check for case of |uXXXX|, a single value up to 6 hex digits. */
    if (str_prefix(s, "u")) {
        p = s + strlen("u");
        code = check_unicode_value(p, false);
        if (code != UNI_UNDEF) {
            assert(code >= 0);
            gp->code = code;
        }
    }
}
