%-----------------------------------------------------------------------------%
% nissya.bst: 『社会学評論』用 BibTeX style file.
%
% 飯田修さんによって作成された jpolisci.bst ファイルが、武田史郎さんによって経
% 済学用に書き換えられ、さらにそれを樋口耕一が『社会学評論』用に書き換えたのが
% このファイルです。
%
% 使い方などは、同梱の説明書(PDF)をまずご覧下さい。
%
% なお、どうしても文献リストに「編」ではなく「編著」と出力したい場合は、文献DB
% を作成する際、「editor」フィールドと「hentyo」フィールドに同じ名前を入力して
% 下さい。
%
% 以下は元の jecon.bst のヘッダー部分です．
%-----------------------------------------------------------------------------%
%
% jecon.bst: 経済学用 BibTeX style file.
%
% これは，飯田修さんによって作成された jpolisci.bst ファイルを経済学用に
% 武田史郎 <zbc08106@park.zero.ad.jp> が改変したものです．
%
% First-written:       <2002/11/03>
% Time-stamp:	       <2003-01-03 21:52:21 Shiro Takeda>
% Version 1.4
% $Id: nissya.bst,v 1.31 2005/02/18 19:23:10 ko-ichi Exp $
%
% 元の jpolisci.bst の著作権は飯田さんにあります．
% ぼくが改変した部分の著作権はどうでもいいです．好きなように扱ってくださ
% い．
%
% オリジナルの jpolisci.bst は，
% <http://www.bol.ucla.edu/~oiida/jpolisci/> で入手することができます．
%
% ぼくが改変した部分は主にプログラム中のコメント行の部分です．コメント行
% の部分 (とその周り) を見れば，どう変えたかわかると思います．
%
% コメント，要望は Shiro Takeda <zbc08106@park.zero.ad.jp> まで．
%
% 以下は元の jpolisci.bst のヘッダー部分です．
%
% JPOLISCI: a bibliography style for papers in political sicence
%           and other fields that cite Japanese literature.
%           Vertical writing (tate-gaki) compatible.

	% Version 20000910 for JBibTeX versions 0.30 or later,
        % and pLaTeX2e or later.
	% Copyright (C) 2000, all rights reserved.
	%
	% This file was derived from jbtxbst.doc.
	%
	% Copying of this file is authorized only if either
	% (1) you make absolutely no changes to your copy, including name, or
	% (2) if you do make changes, you name it something other than
	% jbtxbst.doc, jplain.bst, junsrt.bst, jalpha.bst, jabbrv.bst,
	% tipsj.bst, jipsj.bst, tieice.bst, jname.bst, jorsj.bst, jglsj.bst,
        % seg.bst, jpolisci.bst.
	%
	% This restriction helps ensure that all standard styles are identical.
	% The file jbtxbst.doc has the documentation for this style.
	%

        % Author:
        %
        % 飯田修 Osamu Iida
        % email: oiida@newmail.net
        % URL  : http://www.bol.ucla.edu/~oiida/jpolisci/ (in Japanese)
        %
        % Bug reports, technical suggestions and any other comments
        % will be appreciated.
        % 

        % Acknowledgement:
        %
        % The following files were referred to while creating this file:
        %
        %   jglsj.bst  (by 千葉智章 Tomoaki Chiba)
        %     Function "format.lab.name" in jpolisci.bst derived from
        %     it with small modifications only. The author appreciates it.
        %
        %   seg.bst (by Martin Karrenbach)
        %     A method for making two sorts of labels is based on this
        %     file's feats.
        %
        %   jalpha.bst (by 松井正一 Shoichi Matsui)
        %     jpolisci.bst principally owes its functionality to it.
        %
        %   藤田眞作『続 LaTeX2e 階梯・縦組編』（アジソン・ウェスレイ、1998 年）。
        %
        % Friends in BBSes at IR-web <http://pws.prserv.net/oi/ -- now
        % obsolete> and KH's Web Site <http://home.att.ne.jp/wind/polisci/>
        % gave me titles of Japanese political science journals which are
        % included in MACROs.

        % This file is distributed with absolutely no warranty.
        % The author doesn't assume any kind of responsibility for
        % any inconvenience caused by using this file.

        % Last modified: 2000/12/05

%------------------%
%   下準備・・・   %
%------------------%

ENTRY { address author booktitle chapter edition editor howpublished
  institution journal key month note number organization pages publisher
  school series title type volume year yomi jauthor jkanyaku jtitle
  jpublisher jyear hentyo}

  { is.kanji.entry field.used is.honyaku }

  { label label2 label3 label4 label.pure extra.label sort.label extra.label.bysame }


INTEGERS { output.state before.all mid.sentence after.sentence after.block }

FUNCTION {not}
{   { #0 }
    { #1 }
  if$
}

FUNCTION {and}
{   'skip$
    { pop$ #0 }
  if$
}

FUNCTION {or}
{   { pop$ #1 }
    'skip$
  if$
}

FUNCTION {init.state.consts}
{ #0 'before.all :=
  #1 'mid.sentence :=
  #2 'after.sentence :=
  #3 'after.block :=
}

STRINGS { s t u str.p.start str.p.end str.a str.b str.c str.last.author}

FUNCTION {add.kanji.period}
{
  add.period$ duplicate$
  #-1 #1 substring$ "." =
    {#-2 global.max$ substring$ "．" *}
    'skip$
  if$
}

%----------------%
%   出力系関数   %
%----------------%

FUNCTION {output.bibitem}
{ newline$
  "\nissyaitem[" write$
  label3 write$
  "]{" write$
  label.pure write$
  "}{" write$
  label2 write$
  "}{" write$
  label4 write$
  "}{" write$
  label write$
  "}{" write$
  cite$ write$
  "}" write$
  newline$
  ""
  before.all 'output.state :=
}

FUNCTION {output.nonnull}
{ 's :=
	output.state mid.sentence =
		{
			is.kanji.entry
				{ write$ }
%				{ " " * write$ }
				{ "" * write$ }
			if$
    	}
    	{
    		output.state after.block =
				{
					is.kanji.entry
						{add.kanji.period write$}
						{add.period$ write$}
					if$
					"" write$
				}
				{
					output.state before.all =
						'write$
						{
							is.kanji.entry
 								{ add.kanji.period write$ }
								{ add.period$ " " * write$ }
							if$
						}
					if$
				}
			if$
			mid.sentence 'output.state :=
		}
	if$
	s
}

FUNCTION {output}
{ duplicate$ empty$
    'pop$
    'output.nonnull
  if$
}

FUNCTION {output.check}
{ 't :=
  duplicate$ empty$
    { pop$ "empty " t * " in " * cite$ * warning$ }
    'output.nonnull
  if$
}

FUNCTION {fin.entry}
{
  is.kanji.entry
    {is.honyaku
        'skip$
        {add.kanji.period}
     if$}
    {is.honyaku
        'skip$
        {add.period$}
     if$}
  if$
  write$
  newline$
}

FUNCTION {new.block}
{ output.state before.all =
    'skip$
    { after.block 'output.state := }
  if$
}

FUNCTION {new.sentence}
{ output.state after.block =
    'skip$
    { output.state before.all =
	'skip$
	{ after.sentence 'output.state := }
      if$
    }
  if$
}

FUNCTION {new.block.checka}
{ empty$
    'skip$
    'new.block
  if$
}

FUNCTION {new.block.checkb}
{ empty$
  swap$ empty$
  and
    'skip$
    'new.block
  if$
}

FUNCTION {new.sentence.checka}
{ empty$
    'skip$
    'new.sentence
  if$
}

FUNCTION {new.sentence.checkb}
{ empty$
  swap$ empty$
  and
    'skip$
    'new.sentence
  if$
}

FUNCTION {field.or.null}
{ duplicate$ empty$
    { pop$ "" }
    'skip$
  if$
}

FUNCTION {emphasize}
{ duplicate$ empty$
	{ pop$ "" }
%	{ duplicate$ is.kanji.str$
	{ duplicate$ is.kanji.str$ is.kanji.entry or
	{ "『"   swap$ * "』" * }
	{ 
%		add.period$
		"{\em " swap$ * ",} " *
        }
      if$
    }
  if$
}

%--------------------------%
%   著者名を整形する関数   %
%--------------------------%

INTEGERS { nameptr namesleft numnames }
FUNCTION {format.names}
{ 's :=
  #1 'nameptr :=
  s num.names$ 'numnames :=
  numnames 'namesleft :=
	
	{ namesleft #0 > }
	{
		s nameptr "{ff}{ll}" format.name$ is.kanji.str$
			{s nameptr "{ff}{ll}" format.name$ 't :=}
			{ nameptr #1 >
%%%				{ s nameptr "{ff~}{vv~}{ll}{, jj}" format.name$ 't :=}
%%%				{ s nameptr "{ll,}{~ff}{~vv}{, jj}" format.name$ 't :=}
				{ s nameptr "{f.~}{v~}{ll}{, jj}" format.name$ 't :=}
				{ s nameptr "{ll}{,~f.}{v}{, jj}" format.name$ 't :=}
			  if$
			}
		if$
	
		nameptr #1 >
		{ namesleft #1 >
			{ s is.kanji.str$
				{ "・" * t * }
				{ ", " * t * }
			  if$
			}
			{
				t "others" =
					{
						s is.kanji.str$
							{ "ほか"    * }
							{ " et~al." *}
						if$
					}
					{
						s is.kanji.str$
							{"・" *  t * }
							{ 
								numnames #2 =
									{ " \& " * }
									{ ", \& " *}
								  if$
								t *
							}
			 			if$
			 		}
				if$
			}
		  if$
		}
		't
	  if$
		nameptr #1 + 'nameptr :=
		namesleft #1 - 'namesleft :=
		}
	while$
}
FUNCTION {format.names.ed}
{ 's :=
  #1 'nameptr :=
  s num.names$ 'numnames :=
  numnames 'namesleft :=
	
	{ namesleft #0 > }
	{
		s nameptr "{ff}{ll}" format.name$ is.kanji.str$
			{s nameptr "{ff}{ll}" format.name$ 't :=}
%%%			{ s nameptr "{ff~}{vv~}{ll}{, jj}" format.name$ 't :=}
			{ s nameptr "{f.~}{v~}{ll}{, jj}" format.name$ 't :=}
		if$
		nameptr #1 >
		{ namesleft #1 >
			{ s is.kanji.str$
				{ "・" * t * }
				{ ", " * t * }
			  if$
			}
			{
				t "others" =
					{
						s is.kanji.str$
							{ "ほか"    * }
							{ " et~al." *}
						if$
					}
					{
						s is.kanji.str$
							{"・" *  t * }
							{ 
								numnames #2 =
									{ " \& " * }
									{ ", \& " *}
								  if$
								t *
							}
			 			if$
			 		}
				if$
			}
		  if$
		}
		't
	  if$
		nameptr #1 + 'nameptr :=
		namesleft #1 - 'namesleft :=
		}
	while$
}

FUNCTION {format.authors}
{ author empty$
    { "" }
    {
		extra.label.bysame "bysame" =
			{"\bysame "}
			{ author format.names }
		if$
    }
  if$
}

INTEGERS {bysame.check}
FUNCTION {format.editors}
{ editor empty$
    { "" }
    {
	#0 'bysame.check :=
	type$ "incollection" =
	type$ "inbook" =
	or
		{ editor format.names.ed }
		{
			extra.label.bysame "bysame" =
				{
					#1 'bysame.check :=
					"\bysame "
				}
				{ editor format.names }
			if$
		}
	if$

	hentyo empty$
		{"編" 't :=}
		{
			editor hentyo =
				{"編著" 't :=}
				{"編" 't :=}
			if$
		}
	if$
	
	#1 bysame.check =
		{ "" *}
		{
			editor num.names$ #1 >
				{
					editor is.kanji.str$
						{t * }
						{ " eds." * }
					if$
				}
				{
					editor is.kanji.str$
						{t *}
						{ " ed." * }
					if$
				}
			if$
		}
	if$
    }
  if$
}

%----------------------%
%   その他の整形関数   %
%----------------------%


FUNCTION {format.title}
{ title empty$
    { "" }
    { title is.kanji.str$
      { "「" title * "」" * }
      { " ``" title * ",'' " * }
%        title add.period$
%        " ``" swap$ * "'' " *
     if$
    }
  if$
}

FUNCTION {format.misc.title}
{ title empty$
    { "" }
    { title is.kanji.str$
      { "「" title * "」" * }
      { " ``" title * ",'' " * 
      }
     if$
    }
  if$
}

INTEGERS { start.or.end }
FUNCTION {n.dashify}
{
	't :=
	  ""
	% 初ページ, 終ページの初期化
	#0 'start.or.end :=
	"" 'str.p.start :=
	"" 'str.p.end :=
    % 初ページ, 終ページの取得
    { t empty$ not }
	{ t #1 #1 substring$ "-" =
		{
			#1 'start.or.end :=
			t #2 global.max$ substring$ 't := 
		}
		{ start.or.end
			{
				str.p.end t #1 #1 substring$ * 'str.p.end :=
				t #2 global.max$ substring$ 't := 
			}
			{
				str.p.start t #1 #1 substring$ * 'str.p.start :=
				t #2 global.max$ substring$ 't := 
			}
		  if$
		}
	  if$
	}
  while$
%	str.p.start * "--" * str.p.end * % そのまま書き出す場合
	% ページ数の記載を省略（後ろのページ数は前のページ数と異なる桁だけ表記）
	str.p.start text.length$ str.p.end text.length$ =
		{
			str.p.start 'str.a           :=
			str.p.end   'str.b           :=
			{ str.a empty$ not }
				{ str.a #1 #1 substring$  str.b #1 #1 substring$ =
					{
						str.a #2 global.max$ substring$ 'str.a :=
						str.b #2 global.max$ substring$ 'str.b :=
					}
					{
						str.b 'str.p.end :=
						"" 'str.a :=
					}
				  if$
				}
			while$
		}
		{ skip$ }
	if$
	start.or.end #1 =
		{str.p.start * "--" * str.p.end *}
		{ str.p.start * }
	if$
}

FUNCTION {format.date}
{ year  empty$
    { month empty$
	{ "" }
	{ "there's a month but no year or yyear in " cite$ * warning$
	}
      if$
    }
    { is.kanji.entry
        { extra.label empty$
            { ", ~"  year * extra.label * ", " * }
            { ", ~"  year * "" * extra.label * ",  " * }
%             { " (\kanji "  year * extra.label * ") " * }
%             { " (\kanji "  year * "\rensuji{" * extra.label * "}) " * }
          if$
        }
        { ",\hspace{.3em}"  year * extra.label * ", " * }
      if$
    }
  if$
}

FUNCTION {format.btitle}
{ title emphasize
}

FUNCTION {tie.or.space.connect}
{ duplicate$ text.length$ #3 <
    { "~" }
    { " " }
  if$
  swap$ * *
}

FUNCTION {either.or.check}
{ empty$ 'pop$
	{"can't use both " swap$ * " fields in " * cite$ * warning$ }
  if$
}

FUNCTION {format.bvolume}
{ volume empty$
    { "" }
    { volume is.kanji.str$
	{ "第" volume * "巻, " *}
	{ is.kanji.entry
	    {"第 " volume * "巻, " *}
% 	    {"第\kanji " volume * "巻、" *}
	    {"Vol." volume tie.or.space.connect}
	  if$
	}
      if$
      series empty$
	'skip$
	{ series is.kanji.str$
	    { series ", " * swap$ * }
	    { " of " * series emphasize * }
	  if$
	}
      if$
      "volume and number" number either.or.check
    }
  if$
}

FUNCTION {format.number.series}
{ volume empty$
    { number empty$
	{ series field.or.null }
	{ number is.kanji.str$
	    { " 第" number * "号, " * }
	    {"No." number tie.or.space.connect}
	  if$
	  series empty$
	    { "there's a number but no series in " cite$ * warning$ }
	    { series is.kanji.str$
	        { series ", " * swap$ * }
	        { " in " * series * }
	      if$
	    }
	  if$
	}
      if$
    }
    { "" }
  if$
}

FUNCTION {format.edition}
{ edition empty$
    { "" }
    { edition is.kanji.str$
	{ ", 第" edition * "版" * }
% 	{ "、第" edition * "版、" * }
        { is.kanji.entry
	    {", 第" edition * "版" *}
% 	    {"、第" edition * "版、" *}
	    { output.state mid.sentence =
	      { edition "l" change.case$ ". edition" * }
	      { edition "t" change.case$ ". edition" * }
              if$
	    }
	  if$
        }
      if$
    }
  if$
}

INTEGERS { multiresult }

FUNCTION {multi.page.check}
{ 't :=
  #0 'multiresult :=
    { multiresult not
      t empty$ not
      and
    }
    { t #1 #1 substring$
      duplicate$ "-" =
      swap$ duplicate$ "," =
      swap$ "+" =
      or or
	{ #1 'multiresult := }
	{ t #2 global.max$ substring$ 't := }
      if$
    }
  while$
  multiresult
}

FUNCTION {format.pages}
{ pages empty$
    { "" }
    { pages is.kanji.str$
%      { pages "頁" * }
      { pages "" * }
      { is.kanji.entry
        { pages multi.page.check
%          { "" pages n.dashify * "頁" * }
          { "" pages n.dashify * "" * }
%          { "" pages n.dashify * "頁" * }
          { "" pages n.dashify * "" * }
         if$
        }
        { pages multi.page.check
%         { "pp." pages n.dashify tie.or.space.connect }
          { "" pages n.dashify tie.or.space.connect }
%         { "p." pages tie.or.space.connect }
          { "" pages tie.or.space.connect }
         if$
        }
       if$
      }
     if$
    }
  if$
}

FUNCTION {format.vol.num.pages}
{ volume empty$
     {"" *}
     { "" volume * }
  if$
  number empty$
    {"" *}
    { "(" * number * ")" * }
  if$
	pages empty$
		'skip$
		{ duplicate$ empty$
			{ pop$ format.pages }
			{
%				is.kanji.entry
%					{"：" * format.pages *}
%					{
						": " * format.pages *
%					}
%				if$
			}
		  if$
		}
	if$
}

FUNCTION {format.chapter.pages}
{ chapter empty$
    'format.pages
    { type empty$
	{ chapter is.kanji.str$
	    { "" }
	    { is.kanji.entry
		{"章"}
		{"ch."}
	      if$
	    }
	  if$
	}
	{ type "l" change.case$ }
      if$
      chapter is.kanji.str$ not is.kanji.entry and
          {"第 " chapter * swap$ *}
%           {"第\kanji " chapter * swap$ *}
          { chapter is.kanji.str$ is.kanji.entry and
		{"第" chapter * "章" * * }
		{chapter tie.or.space.connect}
	    if$
	  }
      if$
      pages empty$
	'skip$
	{ is.kanji.entry
	    { ", " * format.pages * }
	    { ", " * format.pages * }
	  if$
	}
      if$
    }
  if$
}

FUNCTION {format.in.ed.booktitle}
{ booktitle empty$
    { "" }
    { editor empty$
	{ booktitle is.kanji.str$
	    { " " booktitle emphasize * }
	    { " " booktitle emphasize * }
	  if$
	}
	{ booktitle is.kanji.str$
	    { "" format.editors * booktitle emphasize * }
	    { " " format.editors * ", " * booktitle emphasize * }
	  if$
	}
      if$
    }
  if$
}

FUNCTION {format.address}
{ address empty$
    { "" }
    { is.kanji.entry
        { address "：" * }
        { address ": " * }
      if$
    }
   if$
}

FUNCTION {empty.misc.check}
{ author empty$ title empty$ howpublished empty$
  month empty$ year empty$ note empty$
  and and and and and
  key empty$ not and
    { "all relevant fields are empty in " cite$ * warning$ }
    'skip$
  if$
}

FUNCTION {format.thesis.type}
{ type empty$
    'skip$
    { pop$
      type "t" change.case$
    }
  if$
}

FUNCTION {format.tr.number}
{ type empty$
    { "Technical Report" }
    'type
  if$
  number empty$
    { "t" change.case$ }
    { number tie.or.space.connect }
  if$
}

FUNCTION {format.article.crossref}
{ key empty$
    { journal empty$
	{ "need key or journal for " cite$ * " to crossref " * crossref *
	  warning$
	  ""
	}
 	{ title is.kanji.str$
 	    { "『" journal * "』" *  }
  	    { " {\em " journal * "\/} " * }
          if$
	}
      if$
    }
    { title is.kanji.str$
 	{ " " key * }
  	{ " " key * }
      if$
    }
  if$
  " \cite{" * crossref * "}" *
}

FUNCTION {format.crossref.editor}
{ editor format.names.ed }

FUNCTION {format.book.crossref}
{ volume empty$
    { "empty volume in " cite$ * "'s crossref of " * crossref * warning$
       title is.kanji.str$
 	{"  "} {" "} if$
    }
    { volume is.kanji.str$
	{ volume }
	{ is.kanji.entry
	    {"第 " volume * "巻" *}
% 	    {"第\kanji " volume * "巻" *}
	    {"Vol." volume tie.or.space.connect " of " *  }
	  if$
	}
      if$
    }
  if$
  editor empty$
  editor field.or.null author field.or.null =
  or
    { key empty$
	{ series empty$
	    { "need editor, key, or series for " cite$ * " to crossref " *
	      crossref * warning$
	      "" *
	    }
	    { series is.kanji.str$
		{ "『" series * "』, " * swap$ * }
	        { "{\em " * series * "\/} " * }
	      if$
	    }
	  if$
	}
	{ key * }
      if$
    }
    { format.crossref.editor * }
  if$
  " \cite{" * crossref * "}" *
}

FUNCTION {format.incoll.inproc.crossref}
{ editor empty$
  editor field.or.null author field.or.null =
  or
	{ key empty$
		{ booktitle empty$
			{ "need editor, key, or booktitle for " cite$ * " to crossref " *
				crossref * warning$
				""
			}
			{ booktitle is.kanji.str$
				{ booktitle emphasize }
				{" {\em " booktitle * "\/} " * }
			  if$
			}
		  if$
		}
		{ "" key * }
	  if$
	}
	{
		"" format.crossref.editor *
		is.kanji.entry
			{
				hentyo empty$
					{ "編 " * }
					{ "編著 " * }
				if$
			}
			{
				editor num.names$ #1 >
					{" eds., " *}
					{" ed., " *}
				if$
			}
		if$
		booktitle is.kanji.str$
			{ booktitle emphasize * ", " *}
			{
				" {\em " * 
				booktitle *
				"},\/ " *
			}
%			{" {\em " booktitle * "\/} " * }
		if$
		address empty$
			{"" *}
			{address * ": " *}
		if$
		publisher empty$
			{"" * }
			{publisher *}
		if$
		is.kanji.entry
			{", ~" *}
			{", " * }
		if$
	}
  if$
%  " \cite{" * crossref * "}" *
}

FUNCTION {format.in.period}
{ is.kanji.entry
    { ", ~" * }
    { ", " * }
   if$
}

FUNCTION {format.hoyakusho}
{ 
  jauthor empty$                        % is.honyaku変数のセット
  jkanyaku empty$                       % fin.entry時にこの変数を参照し、
  jtitle empty$                         % ピリオドを付与を制御
  jpublisher empty$
  jyear empty$
  and and and and
    'skip$
    { #1 'is.honyaku := }
  if$

  jauthor empty$
  jkanyaku empty$
  jtitle empty$
  jpublisher empty$
  jyear empty$
  and and and and
  { "" }
  { add.period$
    jyear empty$
      { "（" * }
      { "（＝" * jyear * ", " * }
    if$
    jauthor empty$
      { jkanyaku empty$
          { "" * }
          { jkanyaku format.names
            "監訳" *
          }
        if$
      }
      { jauthor format.names
        "訳" *
      }
    if$
    jtitle empty$
      { "" * }
      { jtitle emphasize * }
    if$
    jpublisher empty$
      { "" * }
      { jpublisher * }
    if$
%    jyear empty$
%     { "" * }
%      { jyear is.kanji.str$
%        { jyear * "年" * }
%        { "" * jyear * "年" * }
%       if$
%      }
%    if$
%    add.kanji.period
%    "（＝" swap$ * "）" *
%    add.kanji.period
    ".~）" *
  }
 if$
}

FUNCTION {format.book.pages}
{ pages empty$
    { "" }
    { pages is.kanji.str$
      { pages "" * }
      { is.kanji.entry
        { " " pages * }
        { "" pages *}
       if$
      }
     if$
    }
  if$
}

FUNCTION {article}
{ output.bibitem
  format.authors "author" output.check
  format.date "year"  output.check
  format.title "title" output.check
  crossref missing$
    { journal emphasize "journal" output.check
      format.vol.num.pages output
    }
    { format.article.crossref output.nonnull
      format.pages output
    }
  if$
  new.block
  note output
  fin.entry
}

%--------------------------------%
%   文献タイプ名と同名の関数群   %
%--------------------------------%

FUNCTION {book}
{ output.bibitem
  author empty$
     { format.editors "author and editor" output.check }
     { format.authors output.nonnull
       crossref missing$
 	{ "author and editor" editor either.or.check }
 	'skip$
       if$
    }
  if$

  format.date "year"  output.check
  format.btitle "title" output.check
  crossref missing$
    { format.bvolume output
      format.number.series output
      format.address output
      publisher "publisher" output.check
    }
    { format.book.crossref output.nonnull
    }
  if$
  format.edition output
  format.book.pages output
  format.hoyakusho output
  new.block
  note output
  fin.entry
}

FUNCTION {booklet}
{ output.bibitem
  format.authors output
  format.date output
  format.misc.title "title" output.check
  howpublished output
  address output
  new.block
  note output
  fin.entry
}

FUNCTION {inbook}
{ output.bibitem
  author empty$
    { format.editors "author and editor" output.check }
    { format.authors output.nonnull
      crossref missing$
	{ "author and editor" editor either.or.check }
	'skip$
      if$
    }
  if$
  format.date "year"  output.check
  format.btitle "title" output.check
  crossref missing$
    { format.bvolume output
      format.chapter.pages "chapter and pages" output.check
      format.number.series output
      format.address output
      publisher "publisher" output.check
    }
    { format.chapter.pages "chapter and pages" output.check
      format.book.crossref output.nonnull
    }
  if$
  format.edition output
  new.block
  note output
  fin.entry
}

FUNCTION {incollection}
{ output.bibitem
  format.authors "author" output.check
  format.date "year"  output.check
  format.title "title" output.check
  crossref missing$
    { format.in.ed.booktitle "booktitle" output.check
      format.bvolume output
      format.number.series output
      format.address output
      publisher empty$
          { ", ~" }
          { publisher format.in.period }
      if$
      output.nonnull
      format.edition output
      format.chapter.pages output
    }
    { format.incoll.inproc.crossref output.nonnull
      format.chapter.pages output
    }
  if$
  new.block
  note output
  fin.entry
}

FUNCTION {inproceedings}
{ output.bibitem
  format.authors "author" output.check
  format.date "year"  output.check
  format.title "title" output.check
  crossref missing$
    { format.in.ed.booktitle "booktitle" output.check
      format.bvolume output
      format.number.series output
      address empty$
	{ organization publisher new.sentence.checkb
	  organization output
	  publisher output
	}
	{ address output.nonnull
	  organization output
	  publisher output
	}
      if$
    }
    { format.incoll.inproc.crossref output.nonnull
      format.pages output
    }
  if$
  new.block
  note output
  fin.entry
}

FUNCTION {conference} { inproceedings }

FUNCTION {manual}
{ output.bibitem
  author empty$
    { organization empty$
	'skip$
	{ organization output.nonnull
	  address output
	}
      if$
    }
    { format.authors output.nonnull }
  if$
  format.date output
  format.btitle "title" output.check
  author empty$
    { organization empty$
	{ address new.block.checka
	  address output
	}
	'skip$
      if$
    }
    { organization address new.block.checkb
      organization output
      address output
    }
  if$
  format.edition output
  new.block
  note output
  fin.entry
}

FUNCTION {mastersthesis}
{ output.bibitem
  format.authors "author" output.check
  format.date "year"  output.check
  format.title "title" output.check
%  "Master's thesis" format.thesis.type output.nonnull
  school "school" output.check
  school is.kanji.str$
      {"修士論文" format.thesis.type output.nonnull}
      {"Master's thesis" format.thesis.type output.nonnull}
  if$
  address output
  new.block
  note output
  fin.entry
}

FUNCTION {phdthesis}
{ output.bibitem
  format.authors "author" output.check
  format.date "year"  output.check
  format.title "title" output.check
%  format.phd
%  format.thesis.type output.nonnull
  school "school" output.check
  school is.kanji.str$
      {"博士論文" format.thesis.type output.nonnull}
      {"Ph.D. dissertation" format.thesis.type output.nonnull}
  if$
  address output
  new.block
  note output
  fin.entry
}

FUNCTION {misc}
{ output.bibitem
  format.authors output
  format.date output
  format.misc.title output
  howpublished output
%  new.block
  note output
  fin.entry
  empty.misc.check
}

FUNCTION {proceedings}
{ output.bibitem
  editor empty$
    { organization output }
    { format.editors output.nonnull }
  if$
  format.date "year"  output.check
  format.btitle "title" output.check
  format.bvolume output
  format.number.series output
  address empty$
    { editor empty$
	{ publisher new.sentence.checka }
	{ organization publisher new.sentence.checkb
	  organization output
	}
      if$
      publisher output
    }
    { format.address output.nonnull
      editor empty$
	'skip$
	{ organization output }
      if$
      publisher output
    }
  if$
  new.block
  note output
  fin.entry
}

FUNCTION {techreport}
{ output.bibitem
  format.authors "author" output.check
  format.date "year"  output.check
  format.title "title" output.check
  format.tr.number output.nonnull
  institution "institution" output.check
  address output
  new.block
  note output
  fin.entry
}

FUNCTION {unpublished}
{ output.bibitem
  format.authors "author" output.check
  format.date output
  format.title "title" output.check
  note "note" output.check
  fin.entry
}

FUNCTION {default.type} { misc }

MACRO {jan} {"January"}

MACRO {feb} {"February"}

MACRO {mar} {"March"}

MACRO {apr} {"April"}

MACRO {may} {"May"}

MACRO {jun} {"June"}

MACRO {jul} {"July"}

MACRO {aug} {"August"}

MACRO {sep} {"September"}

MACRO {oct} {"October"}

MACRO {nov} {"November"}

MACRO {dec} {"December"}

READ

FUNCTION {set.is.kanji.entry}
{
  author    field.or.null is.kanji.str$
  title     field.or.null is.kanji.str$ or
  editor    field.or.null is.kanji.str$ or
  journal   field.or.null is.kanji.str$ or
  booktitle field.or.null is.kanji.str$ or
  series    field.or.null is.kanji.str$ or
  'is.kanji.entry :=
}

ITERATE {set.is.kanji.entry}

INTEGERS { author.field editor.field organization.field title.field key.field }

FUNCTION {init.field.constants}
{ #0 'author.field :=
  #1 'editor.field :=
  #2 'organization.field :=
  #3 'title.field :=
  #4 'key.field :=
}

EXECUTE {init.field.constants}

FUNCTION {sortify}
{ purify$
  "l" change.case$
}

INTEGERS { len }

FUNCTION {chop.word}
{ 's :=
  'len :=
  s #1 len substring$ =
    { s len #1 + global.max$ substring$ }
    's
  if$
}

INTEGERS { et.al.char.used }

FUNCTION {initialize.et.al.char.used}
{ #0 'et.al.char.used :=
}

EXECUTE {initialize.et.al.char.used}

%----------------------------%
%   著者名ラベルの整形関数   %
%----------------------------%

FUNCTION {format.lab.names}
{ 's :=
   s num.names$ 'numnames :=
   numnames #2 >                        % 3人以上の場合
	{ s is.kanji.str$
	  { s #1 "{ff}" format.name$ 't :=
		t empty$
			{ s #1 "{ll}" format.name$}
			{ s #1 "{ff}" format.name$}
		if$
 		"ほか" * }
	  { s #1 "{ll}" format.name$
		" et~al." * }
	  if$
	}
	{ numnames #2 =                     % 2人の場合
	  { s is.kanji.str$                           % 漢字著者の場合
		{ s #1 "{ff}{vv}{ll}{jj}" format.name$ 't :=        % 1人め
		  t is.kanji.str$
			{
				s #1 "{ff}" format.name$ 't :=
				t empty$
					{ s #1 "{ll}" format.name$}
					{ s #1 "{ff}" format.name$}
				if$
			}
			{ s #1 "{ll}" format.name$}
		  if$
		  s #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =% 2人目
 			{"ほか" *}
			{ "・" *
			  s #2 "{ff}{vv}{ll}{jj}" format.name$ 't :=
			  t is.kanji.str$
				{
					s #2 "{ff}" format.name$ 't :=
					t empty$
						{ s #2 "{ll}" format.name$ *}
						{ s #2 "{ff}" format.name$ *}
					if$
				}
				{s #2 "{ll}" format.name$ *}
			  if$
			}
			if$
		}
		{ s #1 "{ll}" format.name$
		  s #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =
			{ " et~al." * }
			{ " \& " *
			  s #2 "{ll}" format.name$ *}
			if$
		}
	    if$
	  }
	  { s is.kanji.str$
		{ s #1 "{ff}" format.name$ 't :=
		  t empty$
			{ s #1 "{ll}" format.name$}
			{ s #1 "{ff}" format.name$}
		  if$
		}
		{ s #1 "{ll}" format.name$ }
	    if$
	  }
	if$
	}
  if$
	author empty$
		{
			is.kanji.entry
				{"編"  * }
				{ numnames #1 >
					{" eds." *}
					{" ed." *}
				  if$
				}
			if$
		}
		'skip$
	if$
}

FUNCTION {format.lab.names.full}
{   's :=
	"" 't :=
	s num.names$ 'numnames :=
	numnames #2 >
		{
			s is.kanji.str$
				{
					s #1 "{ff}{ll}" format.name$
					"ほか" * 
				}
				{
					s #1 "{ll,}{~f.}{v}{, jj}" format.name$
					" et~al." * 
				}
			if$
		}
		{
			numnames #2 =
				{
					s is.kanji.str$
						{
							s #1 "{ff}{ll}" format.name$
							s #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =
								{ "ほか" * }
								{ 
									"・" *
									s #2 "{ff}{ll}" format.name$ *
								}
							if$
						}
						{
							s #1 "{ll,}{~f.}{v}{, jj}" format.name$
							s #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =
								{
									" et~al." * 
								}
								{
									" \& " *
									s #2 "{ll,}{~f.}{v}{, jj}" format.name$ *
								}
							if$
						}
					if$
				}
				{
					s is.kanji.str$
						{ s #1 "{ff}{ll}" format.name$ }
						{ s #1 "{ll,}{~f.}{v}{, jj}" format.name$ }
					if$
				}
			if$
		}
	if$
	author empty$
		{
			is.kanji.entry
				{"編"  * }
				{ numnames #1 >
					{" eds." *}
					{" ed." *}
				  if$
				}
			if$
		}
		'skip$
	if$
}

INTEGERS {is.making.pure}
STRINGS  {hoka.kh}
FUNCTION {format.lab.names.full.ff}
{'s :=
	"" 't :=
	is.making.pure #2 =
		{"" 'hoka.kh :=}
		{
			s is.kanji.str$
				{"ほか"    'hoka.kh :=}
				{" et~al." 'hoka.kh :=}
			if$
		}
	if$
	s num.names$ 'numnames :=
	numnames #2 >
		{
			s is.kanji.str$
				{
					s #1 "{ff}{ll}" format.name$
					hoka.kh *
				}
				{
%%%					s #1  "{ff~}{vv~}{ll}{, jj}" format.name$
					s #1  "{f.~}{v~}{ll}{, jj}" format.name$
					hoka.kh * 
				}
			if$
		}
		{
			numnames #2 =
				{
					s is.kanji.str$
						{
							s #1 "{ff}{ll}" format.name$
							s #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =
								{ t hoka.kh * 't :=}
								{ 
									t "・" * 't :=
									t s #2 "{ff}{ll}" format.name$ * 't :=
								}
							if$
							t *
						}
						{
%%%							s #1 "{ff~}{vv~}{ll}{, jj}" format.name$
							s #1 "{f.~}{v~}{ll}{, jj}" format.name$
							s #2 "{ff }{vv }{ll}{ jj}" format.name$ "others" =
								{
									hoka.kh *
								}
								{
									" \& " *
%%%									s #2 "{ff~}{vv~}{ll}{, jj}" format.name$ *
									s #2 "{f.~}{v~}{ll}{, jj}" format.name$ *
								}
							if$
						}
					if$
				}
				{
					s is.kanji.str$
						{ s #1 "{ff}{ll}" format.name$ }
%%%						{ s #1  "{ff~}{vv~}{ll}{, jj}" format.name$ }
						{ s #1  "{f.~}{v~}{ll}{, jj}" format.name$ }
					if$
				}
			if$
		}
	if$
	
	is.making.pure #2 =
		'skip$
		{
			author empty$
				{
					is.kanji.entry
						{"編"  * }
						{ numnames #1 >
							{" eds." *}
							{" ed." *}
						  if$
						}
					if$
				}
				'skip$
			if$
		}
	if$

}

FUNCTION {author.key.label}
{ author empty$
    { key empty$
	{ cite$ #1 #3 substring$ }
	{ key #3 text.prefix$ }
      if$
    }
    { author format.lab.names }
  if$
}

FUNCTION {author.key.label.full}
{
	author empty$
		{
			key empty$
				{ cite$ #1 #3 substring$ }
				{ key #3 text.prefix$ }
			if$
		}
		{ author format.lab.names.full }
	if$
}

FUNCTION {author.key.label.full.ff}
{
	author empty$
		{
			key empty$
				{ cite$ #1 #3 substring$ }
				{ key #3 text.prefix$ }
			if$
		}
		{ author format.lab.names.full.ff }
	if$
}

FUNCTION {author.editor.key.label}
{ author empty$
    { editor empty$
	{ key empty$
	    { cite$ #1 #3 substring$ }
	    { key #3 text.prefix$ }
	  if$
	}
	{ editor format.lab.names }
      if$
    }
    { author format.lab.names }
  if$
}

FUNCTION {author.editor.key.label.full}
{ author empty$
    { editor empty$
	{ key empty$
	    { cite$ #1 #3 substring$ }
	    { key #3 text.prefix$ }
	  if$
	}
	{ editor format.lab.names.full }
      if$
    }
    { author format.lab.names.full }
  if$
}

FUNCTION {author.editor.key.label.full.ff}
{ author empty$
    { editor empty$
	{ key empty$
	    { cite$ #1 #3 substring$ }
	    { key #3 text.prefix$ }
	  if$
	}
	{ editor format.lab.names.full.ff }
      if$
    }
    { author format.lab.names.full.ff }
  if$
}


FUNCTION {author.key.organization.label}
{ author empty$
    { key empty$
	{ organization empty$
	    { cite$ #1 #3 substring$ }
	    { "The " #4 organization chop.word #3 text.prefix$ }
	  if$
	}
	{ key #3 text.prefix$ }
      if$
    }
    { author format.lab.names }
  if$
}

FUNCTION {editor.key.organization.label}
{ editor empty$
    { key empty$
	{ organization empty$
	    { cite$ #1 #3 substring$ }
	    { "The " #4 organization chop.word #3 text.prefix$ }
	  if$
	}
	{ key #3 text.prefix$ }
      if$
    }
    { editor format.lab.names }
  if$
}

FUNCTION {calc.label}
{ type$ "book" =
  type$ "inbook" =
  or
    'author.editor.key.label
    { type$ "proceedings" =
		'editor.key.organization.label
		{ type$ "manual" =
		    'author.key.organization.label
		    'author.key.label
		  if$
		}
	  if$
    }
  if$
  duplicate$
  "" swap$ *
  "" year *
  'label :=
	% 翻訳書対策
	jyear empty$
		'skip$
		{
			label "=" jyear * * 'label :=
		}
	if$
	pop$
}

FUNCTION {calc.label2}
{
	type$ "book" =
	type$ "inbook" =
	or
		'author.editor.key.label.full
		{
			type$ "proceedings" =
				'editor.key.organization.label
				{ type$ "manual" =
				    'author.key.organization.label
				    'author.key.label.full
				  if$
				}
			if$
		}
	if$
  duplicate$
  "" swap$ *
  'label2 :=
  sortify label  sortify label2  *  'sort.label :=
   pop$
}

FUNCTION {calc.label3}
{ type$ "book" =
  type$ "inbook" =
  or
    'author.editor.key.label
    { type$ "proceedings" =
	'editor.key.organization.label
	{ type$ "manual" =
	    'author.key.organization.label
	    'author.key.label
	  if$
	}
      if$
    }
  if$
  duplicate$
  "" swap$ *
  'label3 :=
  pop$
}

FUNCTION {calc.label4}
{
	type$ "book" =
	type$ "inbook" =
	or
		'author.editor.key.label.full.ff
		{
			type$ "proceedings" =
				'editor.key.organization.label
				{ type$ "manual" =
				    'author.key.organization.label
				    'author.key.label.full.ff
				  if$
				}
			if$
		}
	if$
  duplicate$
  "" swap$ *
  'label4 :=
  label4 purify$ 'label.pure :=
  pop$
}

FUNCTION {calc.label.pure}
{
	#2 'is.making.pure :=
	type$ "book" =
	type$ "inbook" =
	or
		'author.editor.key.label.full.ff
		{
			type$ "proceedings" =
				'editor.key.organization.label
				{ type$ "manual" =
				    'author.key.organization.label
				    'author.key.label.full.ff
				  if$
				}
			if$
		}
	if$
  duplicate$
  "" swap$ *
  't :=
  t purify$ 'label.pure :=
  pop$
  #1 'is.making.pure :=
}

FUNCTION {sort.format.names}
{
	's :=
	yomi empty$
		'skip$
		{ yomi 's := }
	if$
	""
	#1 'nameptr :=
	s num.names$ 'numnames :=
	numnames 'namesleft :=
	{ namesleft #0 > }
	{
		nameptr #1 >
			{ ", " * }
			'skip$
		if$
		s nameptr "{vv{ } }{ll{ }}{  ff{ }}{  jj{ }}" format.name$ 't :=
		nameptr numnames = t "others" = and
			{ s is.kanji.str$
				{"ほか" * }
				{" et~al." *}
			 if$
			}
			{ t sortify * }
		if$
		nameptr #1 + 'nameptr :=
		namesleft #1 - 'namesleft :=
	}
	while$
}

FUNCTION {sort.format.title}
{ 't :=
  "A " #2
    "An " #3
      "The " #4 t chop.word
    chop.word
  chop.word
  sortify
  #1 global.max$ substring$
}

FUNCTION {author.sort}
{ author empty$
    { key empty$
		{	"to sort, need author or key in " cite$ * warning$ "" }
		{ key sortify }
      if$
    }
    { author sort.format.names }
  if$
}

FUNCTION {author.editor.sort}
{ author empty$
    { editor empty$
	{ key empty$
	    { "to sort, need author, editor, or key in " cite$ * warning$
	      ""
	    }
	    { key sortify }
	  if$
	}
	{ editor sort.format.names }
      if$
    }
    { author sort.format.names }
  if$
}

FUNCTION {author.organization.sort}
{ author empty$
    { organization empty$
	{ key empty$
	    { "to sort, need author, organization, or key in " cite$ * warning$
	      ""
	    }
	    { key sortify }
	  if$
	}
	{ "The " #4 organization chop.word sortify }
      if$
    }
    { author sort.format.names }
  if$
}

FUNCTION {editor.organization.sort}
{ editor empty$
    { organization empty$
	{ key empty$
	    { "to sort, need editor, organization, or key in " cite$ * warning$
	      ""
	    }
	    { key sortify }
	  if$
	}
	{ "The " #4 organization chop.word sortify }
      if$
    }
    { editor sort.format.names }
  if$
}

FUNCTION {presort}
{ calc.label
	type$ "book" =
	type$ "inbook" =
	or
		'author.editor.sort
		{
			type$ "proceedings" =
				'editor.organization.sort
				{ 
					type$ "manual" =
						'author.organization.sort
						'author.sort
					if$
				}
			if$
		}
	if$
	't :=
	pop$
	author empty$
		{t " 2 " * 't :=}
		{t " 1 " * 't :=}
	if$
	t "    " * 't :=
	t year field.or.null sortify * 't :=
	t "    " * 't :=
	t title field.or.null sort.format.title * 't :=
	t 'sort.key$ :=
}

FUNCTION {presort2}
{ calc.label2 }

FUNCTION {presort3}
{ calc.label3 }

FUNCTION {presort4}
{ calc.label4 }

FUNCTION {presort5}
{ calc.label.pure }

ITERATE {presort}
ITERATE {presort2}
ITERATE {presort3}
ITERATE {presort4}
ITERATE {presort5}

SORT

STRINGS { longest.label last.sort.label next.extra prev.author this.author}

INTEGERS { longest.label.width last.extra.num }

FUNCTION {initialize.longest.label}
{ "" 'longest.label :=
  #0 int.to.chr$ 'last.sort.label :=
  "" 'next.extra :=
  #0 'longest.label.width :=
  #0 'last.extra.num :=
  "" 'this.author :=
}

% sort.labelが前回のものと同一であれば、extra.labelをセット
% authorが前回のものと同一であれば、extra.label.bysameをセット

FUNCTION {forward.pass}
{ last.sort.label sort.label =
    { last.extra.num #1 + 'last.extra.num :=
      last.extra.num int.to.chr$ 'extra.label :=
    }
    { "a" chr.to.int$ 'last.extra.num :=
      "" 'extra.label :=
      sort.label 'last.sort.label :=
    }
  if$

  author empty$
    { editor empty$
      { "" }
      'format.editors
      if$
    }
    'format.authors
  if$
  'this.author :=

%  format.names 'this.author :=
  this.author prev.author =
    { "bysame" 'extra.label.bysame := }
    { this.author "" =
        { "abcxyz" }
        'this.author
      if$
      'prev.author :=
    }
  if$
}

FUNCTION {reverse.pass}
{ next.extra "b" =
    { "a" 'extra.label := }
    'skip$
  if$
  extra.label empty$ not
    { label "" * extra.label * "" * }
    { label extra.label * "" * }
  if$
  "" field.or.null *
  'label :=
  label width$ longest.label.width >
    { label 'longest.label :=
      label width$ 'longest.label.width :=
    }
    'skip$
  if$
  extra.label 'next.extra :=
}

EXECUTE {initialize.longest.label}

ITERATE {forward.pass}

REVERSE {reverse.pass}

FUNCTION{punctuation}
{
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"  write$ newline$
  "%"                                                                    write$ newline$
  "%  This bbl file is created through nissya.bst"                        write$ newline$
  "%  nissya.bst is available at"                                         write$ newline$
  "%  <http://hey.to/KO-ichi>"                        write$ newline$
  "%"                                                                    write$ newline$
  "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"  write$ newline$
  newline$
}

EXECUTE {punctuation}

FUNCTION {begin.bib}
{ et.al.char.used
    { "\newcommand{\etalchar}[1]{$^{#1}$}" write$ newline$ }
    'skip$
  if$
  preamble$ empty$
    'skip$
    { preamble$ write$ newline$ }
  if$
  "\ifx\undefined\bysame" write$ newline$
  "\newcommand{\bysame}{\rule[.5ex]{4zw}{.15pt}}"
       write$ newline$
  "\fi" write$ newline$
  newline$
  "\begin{thebibliography}{xx}" write$ newline$
}

EXECUTE {begin.bib}

EXECUTE {init.state.consts}

ITERATE {call.type$}

FUNCTION {end.bib}
{ newline$
  "\end{thebibliography}" write$ newline$
}

EXECUTE {end.bib}
