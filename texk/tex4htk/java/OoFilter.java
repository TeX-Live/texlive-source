package tex4ht;
/**********************************************************/ 
/* OoFilterImpl.java                     2008-02-20-10:22 */
/* Copyright (C) 2006--2008    Eitan M. Gurari            */
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
        } } }
      try{
           super.startElement(ns, sName, qName, attr);
      } catch( Exception e ){
        System.err.println( "--- OoFilter Error 1 --- " + e);
}  }  }

