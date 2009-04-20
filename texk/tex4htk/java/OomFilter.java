package tex4ht;
/**********************************************************/ 
/* OomFilterImpl.java                     2009-03-11-03:09 */
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


import org.xml.sax.helpers.*;
import org.xml.sax.*;
import java.io.PrintWriter;

public class OomFilter extends XMLFilterImpl {
     PrintWriter out = null;
     boolean mtext = false;
   public OomFilter( PrintWriter out,
                     PrintWriter log, boolean trace ){
     this.out = out;
   }
   public void startElement(String ns, String sName,
                           String qName, Attributes attr) {
      if( qName.equals( "math:mtext" ) ){ mtext = true; }
      try{
           super.startElement(ns, sName, qName, attr);
      } catch( Exception e ){
        System.err.println( "--- OomFilter Error 1 --- " + e);
      }
   }
   public void endElement(String ns, String sName, String qName){
      if( qName.equals( "math:mtext" ) ){ mtext = false; }
      try{
        super.endElement(ns, sName, qName);
      } catch( Exception e ){
        System.err.println( "--- OomFilter Error 2 --- " + e);
   }  }
   public void characters(char[] ch, int start, int length){
      try{
        if ( mtext ) {
           for(int i = start; i<start+length; i++){
              if( ch[i] == 160 ){ ch[i] = ' '; }
        }  }
        super.characters(ch, start, length);
      } catch( Exception e ){
        System.out.println( "--- OomFilter Error 3 --- " + e);
}  }  }

