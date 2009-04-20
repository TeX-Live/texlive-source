// 2009-01-27-22:19
package xtpipes;
public class XtpipesUni{
   private static int D800 = Integer.parseInt("D800", 16);
private static int DFFF = Integer.parseInt("DFFF", 16);
private static int DC00 = Integer.parseInt("DC00", 16);
private static int X400 = Integer.parseInt("400",16);
private static int X10000 = Integer.parseInt("10000",16);


public static String toUni( char[] ch, int start, int length,
                                           String filter ){
   StringBuffer buf = new StringBuffer(length);
   for (int i = 0; i < length; i++) {
       int chr = ch[ start + i ];
       boolean ascii =  (chr == '\n')
                        || (chr > 31) && (chr < 127) ;
       if( filter.indexOf(chr) > -1 ){ ascii = false; }

       if( (chr >= D800) && (chr<= DFFF) ){
          chr = ((ch[i] - D800) * X400 + (ch[++i] - DC00)) + X10000;
       }


       buf.append(
         ascii ? Character.toString((char) chr)
               : ("&#x"
                 + Integer.toHexString(chr).toUpperCase()
                 + ";" ) );
   }
   return new String(buf);
}

   public static String toUni( String s, String filter ){
   char [] ch = s.toCharArray();
   int length = ch.length;
   return toUni(ch, 0, length, filter);
}

}

