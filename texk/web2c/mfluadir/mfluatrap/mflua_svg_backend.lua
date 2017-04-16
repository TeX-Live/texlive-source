--
--
-- Experimental svg back end
--
--

--print ("mflua_svg_backend")
local MFbuiltin = mflua.MFbuiltin
local MF        = mflua.MF
print_scaled    = mflua.MF.print_scaled

local svg = svg or {} 
svg = { 
   ['enabled']=true,
   ['enabled_raw']=false,
   ['output_dir']='svg',
   ['svg_preamble'] = [=[<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="15cm" height="15cm" viewBox="%s %s %s %s"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>cubic Bézier commands in path data</title>
  <desc>B char</desc>
  <style type="text/css"><![CDATA[
    .Border { fill:none; stroke:blue; stroke-width:1 }
    .Connect { fill:none; stroke:#888888; stroke-width:2 }
    .SamplePath { fill:none; stroke:black; stroke-width:1 }
    .EndPoint { fill:none; stroke:#888888; stroke-width:2 }
    .CtlPoint { fill:#888888; stroke:none }
    .AutoCtlPoint { fill:none; stroke:blue; stroke-width:4 }
    .Label { font-size:22; font-family:Verdana }
  ]]></style>
    <g transform="matrix(1 0 0 -1 0 0)">
     <g transform="translate(0 %s)">
  %s 
     </g>
   </g>
  </svg>]=],
   ['emsize']= 1000,
   ['filename']= 'SourceCode',
   ['font'] = [=[
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd" >
<svg>
<metadata>
Created by FontForge 20110222 at Sun Sep 18 10:37:34 2011
 By root
Copyleft 2002, 2003, 2005 Free Software Foundation.
 </metadata>
<defs>
<font id="SourceCode" >
  <font-face 
    font-size='%s' 
    units-per-em="%s"
    ascent='800' descent='200'	  
    x-height='%s'	 
    font-family="SourceCode"
    font-weight="400"
    font-stretch="normal"
    panose-1="2 2 6 3 5 4 5 2 3 4"
    alphabetic='0' 	 
      />
   <missing-glyph horiz-adv-x="280" />

   %s
  </font>
</defs></svg>
]=],
['glyph'] = 
 [==[<glyph glyph-name="%s" unicode="%s" orientation="h" 
      horiz-adv-x="%s" vert-adv-y="%s" 
      d="%s" >%s
    </glyph>
]==],
['hkern'] = [==[<hkern g1="%s" u1="%s" g2="%s" u2="%s" k="%s" />]==],
['vkern'] = [==[<vkern g1="%s" u1="%s" g2="%s" u2="%s" k="%s" />]==],
['hkerns']='',
['vkerns']='',
['char']={
 ['0']={glyph_name='uni0393',unicode='&#x0393;'}, -- U+0393 GREEK CAPITAL LETTER GAMMA 
 ['1']={glyph_name='uni0394',unicode='&#x0394;'}, --U+0394 GREEK CAPITAL LETTER DELTA
 ['2']={glyph_name='uni0398',unicode='&#x0398;'}, -- U+0398 GREEK CAPITAL LETTER THETA
 ['3']={glyph_name='uni039b',unicode='&#x039b;'}, -- U+039B GREEK CAPITAL LETTER LAMBDA
 ['4']={glyph_name='uni039e',unicode='&#x039e;'}, --U+039E GREEK CAPITAL LETTER XI
 ['5']={glyph_name='uni03a0',unicode='&#x03a0;'}, --U+03A0 GREEK CAPITAL LETTER PI
 ['6']={glyph_name='uni03a3',unicode='&#x03a3;'}, -- U+03A3 GREEK CAPITAL LETTER SIGMA
 ['7']={glyph_name='uni03a5',unicode='&#x03a5;'}, --U+03A5 GREEK CAPITAL LETTER UPSILON
 ['8']={glyph_name='uni03a6',unicode='&#x03a6;'}, -- U+03A6 GREEK CAPITAL LETTER PHI
 ['9']={glyph_name='uni03a8',unicode='&#x03a8;'}, -- U+03A8 GREEK CAPITAL LETTER PSI
 ['10']={glyph_name='uni03a9',unicode='&#x03a9;'}, -- U+03A9 GREEK CAPITAL LETTER OMEGA
 ['11']={glyph_name='unifb00',unicode='ff'}, -- U+FB00 LATIN SMALL LIGATURE FF
 ['12']={glyph_name='unifb01',unicode='fi'}, --U+FB01 LATIN SMALL LIGATURE FI
 ['13']={glyph_name='unifb02',unicode='fl'}, -- U+FB02 LATIN SMALL LIGATURE FL
 ['14']={glyph_name='unifb03',unicode='ffi'}, --U+FB03 LATIN SMALL LIGATURE FFI
 ['15']={glyph_name='unifb04',unicode='ffl'}, -- U+FB04 LATIN SMALL LIGATURE FFL
 ['16']={glyph_name='uni0131',unicode='&#x0131;'}, -- U+0131 LATIN SMALL LETTER DOTLESS I
 ['17']={glyph_name='uni0237',unicode='&#x0237;'}, -- U+0237 LATIN SMALL LETTER DOTLESS J
 ['18']={glyph_name='uni0060',unicode='&#x0060;'}, -- U+0060 GRAVE ACCENT
 ['19']={glyph_name='uni00b4',unicode='&#x00b4;'}, -- U+00B4 ACUTE ACCENT
 ['20']={glyph_name='uni02c7',unicode='&#x02c7;'}, -- U+02C7 CARON
 ['21']={glyph_name='uni02d8',unicode='&#x02d8;'}, -- U+02D8 BREVE
 ['22']={glyph_name='uni00af',unicode='&#x00af;'}, -- U+00AF MACRON
 ['23']={glyph_name='uni02da',unicode='&#x02da;'}, -- U+02DA RING ABOVE
 ['24']={glyph_name='uni00b8',unicode='&#x00b8;'}, -- U+00B8 CEDILLA
 ['25']={glyph_name='uni00df',unicode='&#x00df;'}, -- U+00DF LATIN SMALL LETTER SHARP S
 ['26']={glyph_name='uni00e6',unicode='&#x00e6;'}, -- U+00E6 LATIN SMALL LETTER AE
 ['27']={glyph_name='uni0153',unicode='&#x0153;'}, -- U+0153 LATIN SMALL LIGATURE OE
 ['28']={glyph_name='uni00f8',unicode='&#x00f8;'}, -- U+00F8 LATIN SMALL LETTER O WITH STROKE
 ['29']={glyph_name='uni00c6',unicode='&#x00c6;'}, --U+00C6 LATIN CAPITAL LETTER AE
 ['30']={glyph_name='uni0152',unicode='&#x0152;'}, --U+0152 LATIN CAPITAL LIGATURE OE
 ['31']={glyph_name='uni00D8',unicode='&#x00D8;'}, --U+00D8 LATIN CAPITAL LETTER O WITH STROKE 
 ['32']={glyph_name='uni0337',unicode='&#x0337;'}, --U+0337 COMBINING SHORT SOLIDUS OVERLAY
 ['33']={glyph_name='uni0021',unicode='&#x0021;'}, 
 ['34']={glyph_name='uni201d',unicode='&#x201d;'}, -- U+201D RIGHT DOUBLE QUOTATION MARK
 ['35']={glyph_name='uni0023',unicode='&#x0023;'}, 
 ['36']={glyph_name='uni0024',unicode='&#x0024;'}, 
 ['37']={glyph_name='uni0025',unicode='&#x0025;'}, 
 ['38']={glyph_name='uni0026',unicode='&#x0026;'}, 
 ['39']={glyph_name='uni0027',unicode='&#x0027;'}, 
 ['40']={glyph_name='uni0028',unicode='&#x0028;'}, 
 ['41']={glyph_name='uni0029',unicode='&#x0029;'}, 
 ['42']={glyph_name='uni002a',unicode='&#x002a;'}, 
 ['43']={glyph_name='uni002b',unicode='&#x002b;'}, 
 ['44']={glyph_name='uni002c',unicode='&#x002c;'}, 
 ['45']={glyph_name='uni002d',unicode='&#x002d;'}, -- U+002D HYPHEN-MINUS
 ['46']={glyph_name='uni002e',unicode='&#x002e;'}, 
 ['47']={glyph_name='uni002f',unicode='&#x002f;'}, 
 ['48']={glyph_name='uni0030',unicode='&#x0030;'}, 
 ['49']={glyph_name='uni0031',unicode='&#x0031;'}, 
 ['50']={glyph_name='uni0032',unicode='&#x0032;'}, 
 ['51']={glyph_name='uni0033',unicode='&#x0033;'}, 
 ['52']={glyph_name='uni0034',unicode='&#x0034;'}, 
 ['53']={glyph_name='uni0035',unicode='&#x0035;'}, 
 ['54']={glyph_name='uni0036',unicode='&#x0036;'}, 
 ['55']={glyph_name='uni0037',unicode='&#x0037;'}, 
 ['56']={glyph_name='uni0038',unicode='&#x0038;'}, 
 ['57']={glyph_name='uni0039',unicode='&#x0039;'}, 
 ['58']={glyph_name='uni003a',unicode='&#x003a;'}, 
 ['59']={glyph_name='uni003b',unicode='&#x003b;'}, 
 ['60']={glyph_name='uni00a1',unicode='&#x00a1;'}, -- U+00A1 INVERTED EXCLAMATION MARK, but only if ligs>1
 ['61']={glyph_name='uni003d',unicode='&#x003d;'}, 
 ['62']={glyph_name='uni00bf',unicode='&#x00bf;'}, --U+00BF INVERTED QUESTION MARK
 ['63']={glyph_name='uni003f',unicode='&#x003f;'}, 
 ['64']={glyph_name='uni0040',unicode='&#x0040;'}, 
 ['65']={glyph_name='uni0041',unicode='&#x0041;'}, 
 ['66']={glyph_name='uni0042',unicode='&#x0042;'}, 
 ['67']={glyph_name='uni0043',unicode='&#x0043;'}, 
 ['68']={glyph_name='uni0044',unicode='&#x0044;'}, 
 ['69']={glyph_name='uni0045',unicode='&#x0045;'}, 
 ['70']={glyph_name='uni0046',unicode='&#x0046;'}, 
 ['71']={glyph_name='uni0047',unicode='&#x0047;'}, 
 ['72']={glyph_name='uni0048',unicode='&#x0048;'}, 
 ['73']={glyph_name='uni0049',unicode='&#x0049;'}, 
 ['74']={glyph_name='uni004a',unicode='&#x004a;'}, 
 ['75']={glyph_name='uni004b',unicode='&#x004b;'}, 
 ['76']={glyph_name='uni004c',unicode='&#x004c;'}, 
 ['77']={glyph_name='uni004d',unicode='&#x004d;'}, 
 ['78']={glyph_name='uni004e',unicode='&#x004e;'}, 
 ['79']={glyph_name='uni004f',unicode='&#x004f;'}, 
 ['80']={glyph_name='uni0050',unicode='&#x0050;'}, 
 ['81']={glyph_name='uni0051',unicode='&#x0051;'}, 
 ['82']={glyph_name='uni0052',unicode='&#x0052;'}, 
 ['83']={glyph_name='uni0053',unicode='&#x0053;'}, 
 ['84']={glyph_name='uni0054',unicode='&#x0054;'}, 
 ['85']={glyph_name='uni0055',unicode='&#x0055;'}, 
 ['86']={glyph_name='uni0056',unicode='&#x0056;'}, 
 ['87']={glyph_name='uni0057',unicode='&#x0057;'}, 
 ['88']={glyph_name='uni0058',unicode='&#x0058;'}, 
 ['89']={glyph_name='uni0059',unicode='&#x0059;'}, 
 ['90']={glyph_name='uni005a',unicode='&#x005a;'}, 
 ['91']={glyph_name='uni005b',unicode='&#x005b;'}, 
 ['92']={glyph_name='uni201c',unicode='&#x201c;'}, -- U+201C LEFT DOUBLE QUOTATION MARK
 ['93']={glyph_name='uni005d',unicode='&#x005d;'}, 
 ['94']={glyph_name='uni0302',unicode='&#x0302;'}, --U+0302 COMBINING CIRCUMFLEX ACCENT
 ['95']={glyph_name='uni0307',unicode='&#x0307;'}, --U+0307 COMBINING DOT ABOVE
 ['96']={glyph_name='uni2018',unicode='&#x2018;'}, --U+2018 LEFT SINGLE QUOTATION MARK
 ['97']={glyph_name='uni0061',unicode='&#x0061;'}, 
 ['98']={glyph_name='uni0062',unicode='&#x0062;'}, 
 ['99']={glyph_name='uni0063',unicode='&#x0063;'}, 
 ['100']={glyph_name='uni0064',unicode='&#x0064;'}, 
 ['101']={glyph_name='uni0065',unicode='&#x0065;'}, 
 ['102']={glyph_name='uni0066',unicode='&#x0066;'}, 
 ['103']={glyph_name='uni0067',unicode='&#x0067;'}, 
 ['104']={glyph_name='uni0068',unicode='&#x0068;'}, 
 ['105']={glyph_name='uni0069',unicode='&#x0069;'}, 
 ['106']={glyph_name='uni006a',unicode='&#x006a;'}, 
 ['107']={glyph_name='uni006b',unicode='&#x006b;'}, 
 ['108']={glyph_name='uni006c',unicode='&#x006c;'}, 
 ['109']={glyph_name='uni006d',unicode='&#x006d;'}, 
 ['110']={glyph_name='uni006e',unicode='&#x006e;'}, 
 ['111']={glyph_name='uni006f',unicode='&#x006f;'}, 
 ['112']={glyph_name='uni0070',unicode='&#x0070;'}, 
 ['113']={glyph_name='uni0071',unicode='&#x0071;'}, 
 ['114']={glyph_name='uni0072',unicode='&#x0072;'}, 
 ['115']={glyph_name='uni0073',unicode='&#x0073;'}, 
 ['116']={glyph_name='uni0074',unicode='&#x0074;'}, 
 ['117']={glyph_name='uni0075',unicode='&#x0075;'}, 
 ['118']={glyph_name='uni0076',unicode='&#x0076;'}, 
 ['119']={glyph_name='uni0077',unicode='&#x0077;'}, 
 ['120']={glyph_name='uni0078',unicode='&#x0078;'}, 
 ['121']={glyph_name='uni0079',unicode='&#x0079;'}, 
 ['122']={glyph_name='uni007a',unicode='&#x007a;'}, 
 ['123']={glyph_name='uni2013',unicode='&#x2013;'}, -- U+2013 EN DASH
 ['124']={glyph_name='uni2014',unicode='&#x2014;'}, -- U+2014 EM DASH
 ['125']={glyph_name='uni030b',unicode='&#x030b;'}, --U+030B COMBINING DOUBLE ACUTE ACCENT
 ['126']={glyph_name='uni0303',unicode='&#x0303;'}, --U+0303 COMBINING TILDE
 ['127']={glyph_name='uni0308',unicode='&#x0308;'}, --U+0308 COMBINING DIAERESIS
 ['128']={glyph_name='uni0080',unicode='&#x0080;'}, 
 ['129']={glyph_name='uni0081',unicode='&#x0081;'}, 
 ['130']={glyph_name='uni0082',unicode='&#x0082;'}, 
 ['131']={glyph_name='uni0083',unicode='&#x0083;'}, 
 ['132']={glyph_name='uni0084',unicode='&#x0084;'}, 
 ['133']={glyph_name='uni0085',unicode='&#x0085;'}, 
 ['134']={glyph_name='uni0086',unicode='&#x0086;'}, 
 ['135']={glyph_name='uni0087',unicode='&#x0087;'}, 
 ['136']={glyph_name='uni0088',unicode='&#x0088;'}, 
 ['137']={glyph_name='uni0089',unicode='&#x0089;'}, 
 ['138']={glyph_name='uni008a',unicode='&#x008a;'}, 
 ['139']={glyph_name='uni008b',unicode='&#x008b;'}, 
 ['140']={glyph_name='uni008c',unicode='&#x008c;'}, 
 ['141']={glyph_name='uni008d',unicode='&#x008d;'}, 
 ['142']={glyph_name='uni008e',unicode='&#x008e;'}, 
 ['143']={glyph_name='uni008f',unicode='&#x008f;'}, 
 ['144']={glyph_name='uni0090',unicode='&#x0090;'}, 
 ['145']={glyph_name='uni0091',unicode='&#x0091;'}, 
 ['146']={glyph_name='uni0092',unicode='&#x0092;'}, 
 ['147']={glyph_name='uni0093',unicode='&#x0093;'}, 
 ['148']={glyph_name='uni0094',unicode='&#x0094;'}, 
 ['149']={glyph_name='uni0095',unicode='&#x0095;'}, 
 ['150']={glyph_name='uni0096',unicode='&#x0096;'}, 
 ['151']={glyph_name='uni0097',unicode='&#x0097;'}, 
 ['152']={glyph_name='uni0098',unicode='&#x0098;'}, 
 ['153']={glyph_name='uni0099',unicode='&#x0099;'}, 
 ['154']={glyph_name='uni009a',unicode='&#x009a;'}, 
 ['155']={glyph_name='uni009b',unicode='&#x009b;'}, 
 ['156']={glyph_name='uni009c',unicode='&#x009c;'}, 
 ['157']={glyph_name='uni009d',unicode='&#x009d;'}, 
 ['158']={glyph_name='uni009e',unicode='&#x009e;'}, 
 ['159']={glyph_name='uni009f',unicode='&#x009f;'}, 
 ['160']={glyph_name='uni00a0',unicode='&#x00a0;'}, 
 ['161']={glyph_name='uni00a1',unicode='&#x00a1;'}, 
 ['162']={glyph_name='uni00a2',unicode='&#x00a2;'}, 
 ['163']={glyph_name='uni00a3',unicode='&#x00a3;'}, 
 ['164']={glyph_name='uni00a4',unicode='&#x00a4;'}, 
 ['165']={glyph_name='uni00a5',unicode='&#x00a5;'}, 
 ['166']={glyph_name='uni00a6',unicode='&#x00a6;'}, 
 ['167']={glyph_name='uni00a7',unicode='&#x00a7;'}, 
 ['168']={glyph_name='uni00a8',unicode='&#x00a8;'}, 
 ['169']={glyph_name='uni00a9',unicode='&#x00a9;'}, 
 ['170']={glyph_name='uni00aa',unicode='&#x00aa;'}, 
 ['171']={glyph_name='uni00ab',unicode='&#x00ab;'}, 
 ['172']={glyph_name='uni00ac',unicode='&#x00ac;'}, 
 ['173']={glyph_name='uni00ad',unicode='&#x00ad;'}, 
 ['174']={glyph_name='uni00ae',unicode='&#x00ae;'}, 
 ['175']={glyph_name='uni00af',unicode='&#x00af;'}, 
 ['176']={glyph_name='uni00b0',unicode='&#x00b0;'}, 
 ['177']={glyph_name='uni00b1',unicode='&#x00b1;'}, 
 ['178']={glyph_name='uni00b2',unicode='&#x00b2;'}, 
 ['179']={glyph_name='uni00b3',unicode='&#x00b3;'}, 
 ['180']={glyph_name='uni00b4',unicode='&#x00b4;'}, 
 ['181']={glyph_name='uni00b5',unicode='&#x00b5;'}, 
 ['182']={glyph_name='uni00b6',unicode='&#x00b6;'}, 
 ['183']={glyph_name='uni00b7',unicode='&#x00b7;'}, 
 ['184']={glyph_name='uni00b8',unicode='&#x00b8;'}, 
 ['185']={glyph_name='uni00b9',unicode='&#x00b9;'}, 
 ['186']={glyph_name='uni00ba',unicode='&#x00ba;'}, 
 ['187']={glyph_name='uni00bb',unicode='&#x00bb;'}, 
 ['188']={glyph_name='uni00bc',unicode='&#x00bc;'}, 
 ['189']={glyph_name='uni00bd',unicode='&#x00bd;'}, 
 ['190']={glyph_name='uni00be',unicode='&#x00be;'}, 
 ['191']={glyph_name='uni00bf',unicode='&#x00bf;'}, 
 ['192']={glyph_name='uni00c0',unicode='&#x00c0;'}, 
-- ['193']={glyph_name='uni00c1',unicode='&#x00c1;'}, 
 ['193']={glyph_name='uni0060',unicode='&#x0060;'}, 
-- ['194']={glyph_name='uni00c2',unicode='&#x00c2;'}, 
 ['194']={glyph_name='uni00b4',unicode='&#x00b4;'}, 
-- ['195']={glyph_name='uni00c3',unicode='&#x00c3;'}, 
 ['195']={glyph_name='uni005e',unicode='&#x005e;'}, 
 ['196']={glyph_name='uni00c4',unicode='&#x00c4;'}, 
 ['197']={glyph_name='uni00c5',unicode='&#x00c5;'}, 
 ['198']={glyph_name='uni00c6',unicode='&#x00c6;'}, 
 ['199']={glyph_name='uni00c7',unicode='&#x00c7;'}, 
 ['200']={glyph_name='uni00c8',unicode='&#x00c8;'}, 
 ['201']={glyph_name='uni00c9',unicode='&#x00c9;'}, 
 ['202']={glyph_name='uni00ca',unicode='&#x00ca;'}, 
 ['203']={glyph_name='uni00cb',unicode='&#x00cb;'}, 
 ['204']={glyph_name='uni00cc',unicode='&#x00cc;'}, 
 ['205']={glyph_name='uni00cd',unicode='&#x00cd;'}, 
 ['206']={glyph_name='uni00ce',unicode='&#x00ce;'}, 
 ['207']={glyph_name='uni00cf',unicode='&#x00cf;'}, 
 ['208']={glyph_name='uni00d0',unicode='&#x00d0;'}, 
 ['209']={glyph_name='uni00d1',unicode='&#x00d1;'}, 
 ['210']={glyph_name='uni00d2',unicode='&#x00d2;'}, 
 ['211']={glyph_name='uni00d3',unicode='&#x00d3;'}, 
 ['212']={glyph_name='uni00d4',unicode='&#x00d4;'}, 
 ['213']={glyph_name='uni00d5',unicode='&#x00d5;'}, 
 ['214']={glyph_name='uni00d6',unicode='&#x00d6;'}, 
 ['215']={glyph_name='uni00d7',unicode='&#x00d7;'}, 
 ['216']={glyph_name='uni00d8',unicode='&#x00d8;'}, 
 ['217']={glyph_name='uni00d9',unicode='&#x00d9;'}, 
 ['218']={glyph_name='uni00da',unicode='&#x00da;'}, 
 ['219']={glyph_name='uni00db',unicode='&#x00db;'}, 
 ['220']={glyph_name='uni00dc',unicode='&#x00dc;'}, 
 ['221']={glyph_name='uni00dd',unicode='&#x00dd;'}, 
 ['222']={glyph_name='uni00de',unicode='&#x00de;'}, 
 ['223']={glyph_name='uni00df',unicode='&#x00df;'}, 
 ['224']={glyph_name='uni00e0',unicode='&#x00e0;'}, 
 ['225']={glyph_name='uni00e1',unicode='&#x00e1;'}, 
 ['226']={glyph_name='uni00e2',unicode='&#x00e2;'}, 
 ['227']={glyph_name='uni00e3',unicode='&#x00e3;'}, 
 ['228']={glyph_name='uni00e4',unicode='&#x00e4;'}, 
 ['229']={glyph_name='uni00e5',unicode='&#x00e5;'}, 
 ['230']={glyph_name='uni00e6',unicode='&#x00e6;'}, 
 ['231']={glyph_name='uni00e7',unicode='&#x00e7;'}, 
 ['232']={glyph_name='uni00e8',unicode='&#x00e8;'}, 
 ['233']={glyph_name='uni00e9',unicode='&#x00e9;'}, 
 ['234']={glyph_name='uni00ea',unicode='&#x00ea;'}, 
 ['235']={glyph_name='uni00eb',unicode='&#x00eb;'}, 
 ['236']={glyph_name='uni00ec',unicode='&#x00ec;'}, 
 ['237']={glyph_name='uni00ed',unicode='&#x00ed;'}, 
 ['238']={glyph_name='uni00ee',unicode='&#x00ee;'}, 
 ['239']={glyph_name='uni00ef',unicode='&#x00ef;'}, 
 ['240']={glyph_name='uni00f0',unicode='&#x00f0;'}, 
 ['241']={glyph_name='uni00f1',unicode='&#x00f1;'}, 
 ['242']={glyph_name='uni00f2',unicode='&#x00f2;'}, 
 ['243']={glyph_name='uni00f3',unicode='&#x00f3;'}, 
 ['244']={glyph_name='uni00f4',unicode='&#x00f4;'}, 
 ['245']={glyph_name='uni00f5',unicode='&#x00f5;'}, 
 ['246']={glyph_name='uni00f6',unicode='&#x00f6;'}, 
 ['247']={glyph_name='uni00f7',unicode='&#x00f7;'}, 
 ['248']={glyph_name='uni00f8',unicode='&#x00f8;'}, 
 ['249']={glyph_name='uni00f9',unicode='&#x00f9;'}, 
 ['250']={glyph_name='uni00fa',unicode='&#x00fa;'}, 
 ['251']={glyph_name='uni00fb',unicode='&#x00fb;'}, 
 ['252']={glyph_name='uni00fc',unicode='&#x00fc;'}, 
 ['253']={glyph_name='uni00fd',unicode='&#x00fd;'}, 
 ['254']={glyph_name='uni00fe',unicode='&#x00fe;'}, 
 ['255']={glyph_name='uni00ff',unicode='&#x00ff;'}, 
 ['256']={glyph_name='uni0100',unicode='&#x0100;'}, 
         },

}

local function _eval_tonumber(q,offset)
   local qx,qy,xo,yo
   local w 
   local _offset = offset 
   if _offset == nil then _offset = '(0,0)' end
   w=string.gmatch(q,"[-0-9.]+"); qx,qy=w(),w()
   w=string.gmatch(_offset,"[-0-9.]+"); xo,yo=w(),w()
   return {tonumber(qx+xo),tonumber(qy+yo)}
end


local function get_svg_glyph(valid_curves,char,cycles,tfm)
   -- Write the svg
   --
   --  print('BEZ  MFbuiltin.hppp='..print_scaled(MFbuiltin.hppp()))
   --  print('BEZ  MFbuiltin.vppp='..print_scaled(MFbuiltin.vppp()))
   --  print('BEZ  MFbuiltin.designsize='..print_scaled(MFbuiltin.designsize()))
   local tfm = tfm
   local index = tostring( char['index'] ) -- better a string or a number
   local design_size=tonumber ( print_scaled(MFbuiltin.designsize()) ) --pt 
   local char_wd=tonumber( char['char_wd'] ) -- pt
   local char_ht=tonumber( char['char_ht'] ) -- pt
   local char_dp=tonumber( char['char_dp'] ) -- pt

   --local xheight =  0.458333 *  design_size -- must be read from tfm !!

   local x_resolution = math.floor(0.5+tonumber( print_scaled(MFbuiltin.hppp()) )* 72.27)
   local y_resolution = math.floor(0.5+tonumber( print_scaled(MFbuiltin.vppp()) )* 72.27)
   assert(x_resolution==y_resolution, string.format('Error on _get_svg_glyph x_res=%d and y_res=%d differ',x_resolution,y_resolution))

   local resolution = x_resolution 
   local emsize = mflua.svg.emsize -- 1000, type 1, also known as em_unit: 1000 emsize = 1em
   local em_unit  = emsize  

   local em_unit_for_pixel = (72.27/design_size) * (emsize / resolution)
   local bp_for_pt = 72/72.27
   local char_wd_emunit = (char_wd/design_size) *em_unit
   local char_ht_emunit = (char_ht/design_size) *em_unit
   local char_dp_emunit = (char_dp/design_size) *em_unit
    
   local outdir = mflua.svg.output_dir or '.'
   local fname  = tostring(char['charname'])
   if fname and fname ~= '' then 
      mflua.svg.char[index] ={glyph_name='', unicode = ''};
      mflua.svg.char[index].glyph_name = fname
      local w = string.gmatch(fname,'uni(.+)')
      local unicode_hex = w()
      mflua.svg.char[index].unicode = [[&#x]] .. unicode_hex ..';'
   end
      
   fname = mflua.svg.char[index].glyph_name
   assert(fname~=nil, string.format("Error on svg file name for char index %s: it's nil",index))
   --local f = io.open(outdir..'/'.. fname..'.svg','w')
   local w = string.gmatch(fname,'uni(.+)')
   local unicode_hex = w()
   local unicode =  mflua.svg.char[index].unicode
   local unicode_range = 'U+'..unicode_hex
   local glyph_name = fname 
   local bezier = ''
   local maxx,maxy=-1e9,-1e9
   local minx,miny=1e9,1e9
   for i,cycle in pairs(cycles) do 
      local path=''
      local _i=1
      for _,j in ipairs(cycle) do 
	 --local curve = valid_curves[j]
	 local curve = j 
	 local p,c1,c2,q,offset=curve[1],curve[2],curve[3],curve[4],curve[5]
	 p=_eval_tonumber(p,offset)
	 c1=_eval_tonumber(c1,offset)
	 c2=_eval_tonumber(c2,offset)
	 q=_eval_tonumber(q,offset)
	 -- em_unit_for_pixel
	 p[1],p[2] = p[1]*em_unit_for_pixel,p[2]*em_unit_for_pixel
	 q[1],q[2] = q[1]*em_unit_for_pixel,q[2]*em_unit_for_pixel
	 c1[1],c1[2] = c1[1]*em_unit_for_pixel,c1[2]*em_unit_for_pixel
	 c2[1],c2[2] = c2[1]*em_unit_for_pixel,c2[2]*em_unit_for_pixel
	 if _i==1 then bezier = bezier..string.format("M%s %s ",p[1],p[2]) end
	 _i=_i+1	 
	 bezier = bezier .. string.format("C%s %s %s %s %s %s\n",c1[1],c1[2],c2[1],c2[2],q[1],q[2])
	  --print("BEZ bez="..bezier)
	 if p[1]>maxx then maxx=p[1] end
	 if c1[1]>maxx then maxx=c1[1] end
	 if c2[1]>maxx then maxx=c2[1] end
	 if q[1]>maxx then maxx=q[1] end
	 if p[2]>maxy then maxy=p[2] end
	 if c1[2]>maxy then maxy=c1[2] end
	 if c2[2]>maxy then maxy=c2[2] end
	 if q[2]>maxy then maxy=q[2] end
	 --
	 if p[1]<minx then minx=p[1] end
	 if c1[1]<minx then minx=c1[1] end
	 if c2[1]<minx then minx=c2[1] end
	 if q[1]<minx then minx=q[1] end
	 if p[2]<miny then miny=p[2] end
	 if c1[2]<miny then miny=c1[2] end
	 if c2[2]<miny then miny=c2[2] end
	 if q[2]<miny then miny=q[2] end
      end
      bezier = bezier .. 'Z\n'
   end
   --print('BEZ (x,y) (X,Y)=',minx,miny,maxx,maxy)
   --print('BEZ', svg_glyph,glyph_name,unicode,char_wd_emunit,char_ht_emunit,bezier)
   local trunk = ''
   local svg_glyph = mflua.svg.glyph
   local svg_font = mflua.svg.font
   trunk = string.format(svg_glyph,
			 glyph_name,unicode,
			 char_wd_emunit,
			 char_ht_emunit,
			 bezier,'')
   local svg_preamble = mflua.svg.svg_preamble
   local paths = string.format('<path style="fill:#000000;stroke=none;fill-rule:nonzero" d="%s" />', bezier)
   local raw=string.format(svg_preamble,minx,miny,maxx,maxy,-maxy,paths)
   return trunk,raw
end -- _get_svg_glyph


local function svg_kern_and_lig(chartable,t,tfm)
   --
   -- Store kerns and ligs
   -- 
   local current_chars={}
   local index
   local kern, next_char,additional_space
   local bp_for_pt = 72/72.27
   local x_resolution = math.floor(0.5+tonumber( print_scaled(MFbuiltin.hppp()) )* bp_for_pt)
   local y_resolution = math.floor(0.5+tonumber( print_scaled(MFbuiltin.vppp()) )* bp_for_pt)
   local design_size=tfm.font.designsize -- pt
   assert(x_resolution==y_resolution, string.format('Error on _get_svg_glyph x_res=%d and y_res=%d differ',x_resolution,y_resolution))
   local resolution = x_resolution 
   local emsize = mflua.svg.emsize -- 1000, type 1, also known as em_unit: 1000 emsize = 1em
   local em_unit  = emsize  
   local em_unit_for_pixel = (bp_for_pt/design_size) * (emsize / resolution)

   local hkern,vkern = '' ,''
   for i,_ in ipairs(t) do 
      index = t[i]
      current_chars[index]=true
   end
   for i,_ in ipairs(t) do 
      index = t[i]
      if tfm.chars[index] and  tfm.chars[index].kern then 
	 kerntable = tfm.chars[index].kern
	 for _,k in ipairs(kerntable) do 
	    next_char,additional_space = tostring(k.next_char),tonumber(k.additional_space)
	    --
	    if additional_space~=0 then 
	       local a_name,a_unicode = mflua.svg.char[tostring(index)].glyph_name,mflua.svg.char[tostring(index)].unicode
	       local b_name,b_unicode = mflua.svg.char[next_char].glyph_name,mflua.svg.char[next_char].unicode
	       hkern = hkern .. "\n".. string.format(mflua.svg.hkern,
						     a_name,a_unicode,
						     b_name,b_unicode,
						     additional_space*design_size*em_unit_for_pixel)
	    end
	 end
      end
   end
   return hkern,vkern
end

local function store_svg_font(tfm)
   -- local outdir = mflua.svg.output_dir or '.'
   -- local fname = mflua.svg.filename
   -- assert(fname~=nil, string.format("Error on svg file name for %s: it's nil",fname))
   -- local f = io.open(outdir..'/'.. fname..'.svg','w')
   local svg_font = mflua.svg.font
   local design_size=tonumber ( print_scaled(MFbuiltin.designsize()) ) --pt 
   local emsize = mflua.svg.emsize -- 1000, type 1, also known as em_unit: 1000 emsize = 1em
   local bp_for_pt = 72/72.27
   local xheight =  tfm.font.x_height * design_size --0.458333 *  design_size -- must be read from tfm !!
   local trunk =''
   local char = mflua.svg.char
   for index=0,2^16-1 do
      if not(char[tostring(index)]==nil) then 
	 --print("BEZ char[tostring(index)].data=",index,char[tostring(index)].data)
	 trunk= trunk.. (char[tostring(index)].data or '')
      end
   end
   trunk = trunk.."\n"..mflua.svg.hkerns.."\n"..mflua.svg.vkerns
   trunk = string.format(svg_font,
			 string.format("%2.2f",design_size*bp_for_pt),
			 emsize,
			 string.format("%2.2f",xheight*bp_for_pt),
			 trunk)
   return trunk


end

svg.get_svg_glyph    = get_svg_glyph
svg.svg_kern_and_lig = svg_kern_and_lig
svg.store_svg_font   = store_svg_font

return svg
