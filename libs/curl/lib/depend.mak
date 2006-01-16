$(objdir)/transfer.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	strequal.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	netrc.h \
	transfer.h \
	sendf.h \
	speedcheck.h \
	getpass.h \
	progress.h \
	getdate.h \
	http.h \
	url.h \
	getinfo.h \
	ssluse.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/getenv.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/ssluse.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h

$(objdir)/ftp.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	if2ip.h \
	progress.h \
	transfer.h \
	escape.h \
	http.h \
	ftp.h \
	strequal.h \
	ssluse.h \
	connect.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/getpass.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/strequal.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/http.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	transfer.h \
	sendf.h \
	progress.h \
	base64.h \
	strequal.h \
	ssluse.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/progress.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	progress.h

$(objdir)/speedcheck.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	speedcheck.h

$(objdir)/getinfo.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h

$(objdir)/llist.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	llist.h

$(objdir)/easy.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	strequal.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	transfer.h \
	ssluse.h \
	url.h \
	getinfo.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/escape.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h

$(objdir)/security.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/timeval.obj: \
	timeval.h \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/base64.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/mprintf.h \
	base64.h

$(objdir)/telnet.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	transfer.h \
	sendf.h \
	$(curldir)/include/curl/mprintf.h \
	arpa_telnet.h

$(objdir)/sendf.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/dict.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	transfer.h \
	sendf.h \
	progress.h \
	strequal.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/if2ip.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/strtok.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/cookie.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	getdate.h \
	strequal.h \
	strtok.h

$(objdir)/version.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h

$(objdir)/file.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	progress.h \
	sendf.h \
	escape.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/hostip.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/ldap.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	escape.h \
	transfer.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/netrc.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	strequal.h \
	strtok.h

$(objdir)/http_chunks.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/getdate.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	getdate.h

$(objdir)/connect.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	sendf.h \
	if2ip.h

$(objdir)/mprintf.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/url.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	netrc.h \
	base64.h \
	ssluse.h \
	if2ip.h \
	transfer.h \
	sendf.h \
	getpass.h \
	progress.h \
	strequal.h \
	escape.h \
	strtok.h \
	ftp.h \
	dict.h \
	telnet.h \
	http.h \
	file.h \
	ldap.h \
	url.h \
	connect.h \
	$(curldir)/include/curl/mprintf.h

$(objdir)/hash.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	hash.h \
	llist.h

$(objdir)/formdata.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	formdata.h \
	strequal.h

$(objdir)/dllinit.obj: \
	

$(objdir)/krb4.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h

$(objdir)/multi.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	multi.h \
	urldata.h \
	hostip.h \
	hash.h \
	llist.h \
	cookie.h \
	formdata.h \
	timeval.h \
	http_chunks.h \
	transfer.h \
	url.h

