package tex4ht;
/**********************************************************/ 
/* OoFilterImpl.java                     2009-03-11-03:09 */
/* Copyright (C) 2006--2009    Eitan M. Gurari            */
/*                                                        */
/* This work may be distributed and/or modified under the */
/* conditions of the LaTeX Project Public License, either */
/* version 1.3 of this license or (at your option) any    */
/* later version. The latest version of this license is   */
/* in                                                     */
/*   http://www.latex-project.org/lppl.txt                */
/* and version 1.3 or later is part of all distributions  */
/* of LaTeX version 2003/12/01 or later.                  */
/*                                                        */
/* This work has the LPPL maintenance status "maintained".*/
/*                                                        */
/* This Current Maintainer of this work                   */
/* is Eitan M. Gurari.                                    */
/*                                                        */
/*                             gurari@cse.ohio-state.edu  */
/*                 http://www.cse.ohio-state.edu/~gurari  */
/**********************************************************/

import java.util.*;
import org.xml.sax.helpers.*;
import org.xml.sax.*;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.FileReader;

public class OoFilter extends XMLFilterImpl {
     PrintWriter out = null;
   public OoFilter( PrintWriter out,
                    PrintWriter log, boolean trace ){
     this.out = out;
   }
   public void startElement(String ns, String sName,
                           String qName, Attributes attr) {
      if( qName.equals( "draw:frame" ) ){
        String name = attr.getValue("draw:name");
if( (name != null)
    && (attr.getValue("svg:width") == null)
    && (attr.getValue("svg:hieght") == null)
){
  java.awt.Image image = new javax.swing.ImageIcon(name).getImage();
  int width = image.getWidth(null);
  int height = image.getHeight(null);
  if( (width>0) && (height>0) ){
     org.xml.sax.helpers.AttributesImpl attrs =
                   new  org.xml.sax.helpers.AttributesImpl( attr );
     attrs.addAttribute(null, "svg:width", "svg:width",
                                 "String", (width * 72 / 110) + "pt");
     attrs.addAttribute(null, "svg:width", "svg:height",
                                 "String", (height * 72 / 110) + "pt");
     attr = attrs;
} }

      }
      try{
           super.startElement(ns, sName, qName, attr);
      } catch( Exception e ){
        System.err.println( "--- OoFilter Error 1 --- " + e);
   }  }
   public void processingInstruction(String target, String fileName) {
      if( target.equals("tex4ht-lg") ){
         fileName = fileName.trim();
         if( fileName.replaceAll(" ","").startsWith("file=") ){
            HashMap <String,HashMap <String,String>> map
       = new HashMap<String,HashMap <String,String>>();
String [] font_css_base = {"" , "-", ""};
String [] font_css_mag = {"x-x-", ""};
int base_font_size = 10;
int second;

            int length = fileName.length();
int loc = fileName.indexOf(fileName.charAt(length-1));
fileName = fileName.substring(loc+1,length-1);
try{
   String s = "";
   FileReader fr  = new FileReader( fileName );
   BufferedReader in = new BufferedReader( fr );
                      try{
   String key = "", body = "";
   while( (s=in.readLine()) != null ){
     if( s.startsWith("htfcss: ") ){
        s = s.substring(8);
        int idx = s.indexOf(' ');
        if( idx == 0 ){
           body += s;
        } else {
           body = body.trim();
if( body.startsWith( "@media " ) ){
   body = body.substring(7).trim();
   if( body.startsWith("print ") ){
     body = body.substring(6).trim();
   } else { key = ""; }
}
if( !key.equals("") ){
   String [] property = body.split(";");
   for( int i=0; i<property.length; i++ ){
      if( !property[i].trim().equals("") ){
         int indx = property[i].indexOf(":");
         if( indx != -1 ){
            String name = property[i].substring(0,indx).trim();
            String value = (property[i]+' ').substring(indx+1).trim();
            if( !name.equals("") && !value.equals("") ){
               if( map.containsKey(key) ){
   HashMap <String,String> entry = map.get(key);
   entry.put(name,value);
}
else
{
   HashMap <String,String> entry = new HashMap <String,String>();
   entry.put(name,value);
   map.put(key,entry);
}

}  }  }  }  }

           if( idx == -1 ){
              key = s; body = "";
           } else {
              key = s.substring(0,idx);
              body = s.substring(idx);
   } }  }  }
   body = body.trim();
if( body.startsWith( "@media " ) ){
   body = body.substring(7).trim();
   if( body.startsWith("print ") ){
     body = body.substring(6).trim();
   } else { key = ""; }
}
if( !key.equals("") ){
   String [] property = body.split(";");
   for( int i=0; i<property.length; i++ ){
      if( !property[i].trim().equals("") ){
         int indx = property[i].indexOf(":");
         if( indx != -1 ){
            String name = property[i].substring(0,indx).trim();
            String value = (property[i]+' ').substring(indx+1).trim();
            if( !name.equals("") && !value.equals("") ){
               if( map.containsKey(key) ){
   HashMap <String,String> entry = map.get(key);
   entry.put(name,value);
}
else
{
   HashMap <String,String> entry = new HashMap <String,String>();
   entry.put(name,value);
   map.put(key,entry);
}

}  }  }  }  }

} catch(java.lang.NumberFormatException e){
   System.err.println( "--- OoFilter Error 3 --- Improper record: " + s);
}

   in.close();
fr  = new FileReader( fileName );
in = new BufferedReader( fr );
 
   in.close();
fr  = new FileReader( fileName );
in = new BufferedReader( fr );
 try{
   while( (s=in.readLine()) != null ){
     if( s.startsWith("Font_css_base: ") ){
       int idx = s.indexOf("Font_css_mag: ");
       if( idx != -1 ){
          String [] pattern = s.substring(15,idx).trim().split("%s");
          if( pattern.length == 3 ){ font_css_base = pattern; }
          pattern = (s+' ').substring(idx).trim().split("%s");
          if( pattern.length == 2 ){ font_css_mag = pattern; }
   } } }
} catch(Exception e){
   System.err.println( "--- OoFilter Error 4 --- Improper record: " + s);
}

   in.close();
fr  = new FileReader( fileName );
in = new BufferedReader( fr );
 try{
   while( (s=in.readLine()) != null ){
     if( s.startsWith("Font_Size:") ){
        base_font_size = Integer.parseInt( s.substring(10).trim() );
   } }
} catch(java.lang.NumberFormatException e){
   System.err.println( "--- OoFilter Error 5 --- Improper record: " + s);
}

   in.close();
fr  = new FileReader( fileName );
in = new BufferedReader( fr );
 try{
   while( (s=in.readLine()) != null ){
     if( s.startsWith("Font(") ){
        String [] match = s.split("\"");
        match[1] = match[1].trim();
        match[2] = match[3].trim();
        match[3] = match[5].trim();
        match[4] = match[7].trim();
        {
   if( match[3].replaceAll("[0-9]","").equals("") ){
      second =   (int)
              (  Integer.parseInt( match[3] )
               * Long.parseLong( match[4] )
               / base_font_size
              );
      while( second > 700 ){  second /= 10; }
   } else { second = 100; }
   if(  (int) (  Double.parseDouble(match[2])
            / Long.parseLong(match[4])
            + 0.5
           )
      == base_font_size
){
   second = 100;
};

}

        HashMap <String,String> entry = map.get(match[1]);
if( (entry != null) || (second < 98) || (second > 102) ){
   String styleName = font_css_base[0] + match[1] +
                      font_css_base[1] + match[2] +
                      font_css_base[2];
   if( !match[4].equals("100") ){
      styleName += font_css_mag[0] + match[4] + font_css_mag[1];
   }
   org.xml.sax.helpers.AttributesImpl attr =
                      new org.xml.sax.helpers.AttributesImpl();
attr.addAttribute("", "style:name", "style:name", "String", styleName);
attr.addAttribute("", "style:family", "style:family", "String", "text");

   super.startElement(null, "style:style", "style:style", attr);
   attr = new org.xml.sax.helpers.AttributesImpl();
if( entry != null ){
   Object [] name = entry.keySet().toArray();
   for(int i=0; i < name.length; i++){
      String value = entry.get(name[i]);
      attr.addAttribute("", "fo:" + (String) name[i],
                            "fo:" + (String) name[i],
                                                "String", value);
}  }
if( (second < 98) || (second > 102) ){
   attr.addAttribute("", "fo:font-size", "fo:font-size",
                                  "String", (second / 10.0) + "pt");
}

   super.startElement(null, "style:text-properties",
                                 "style:text-properties", attr);
   super.endElement(null, "style:text-properties",
                                       "style:text-properties");
   super.endElement(null, "style:style", "style:style");
}

   } }
} catch(Exception e){
   System.err.println( "--- OoFilter Error 6 --- Improper record: " + s);
}

} catch(Exception e){
   System.err.println( "--- OoFilter Error 2 --- " + e);
}

   }  }  }
}

