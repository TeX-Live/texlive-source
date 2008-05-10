/**********************************************************/
/* xv4ht.java                            2006-07-03-21:41 */
/* Copyright (C) 2005--2006    Eitan M. Gurari            */
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
/* If you modify this program your changing its signature */
/* with a directive of the following form will be         */
/* appreciated.                                           */
/*      #define PLATFORM "signature"                      */
/*                                                        */
/*                             gurari@cse.ohio-state.edu  */
/*                 http://www.cse.ohio-state.edu/~gurari  */
/**********************************************************/

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Hashtable;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.EntityResolver;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXNotRecognizedException;
import org.xml.sax.SAXNotSupportedException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;
import org.xml.sax.ext.LexicalHandler;
import org.xml.sax.helpers.DefaultHandler;

class Xv4htContentHandler extends DefaultHandler {

   String catalog;
   boolean withinDTD = false;
   Xv4htErrorHandler errHandler;
   Xv4htLexicalHandler lexicalHandler;

   public Xv4htContentHandler(XMLReader xmlReader, String catalog){
     super();
     this.catalog = catalog;
     lexicalHandler = new Xv4htLexicalHandler();
try {
  xmlReader.setProperty(
     "http://xml.org/sax/properties/lexical-handler",
     lexicalHandler
  );
}
catch (SAXNotRecognizedException e) {
  System.err.println(
   "err 1");
  return;
}
catch (SAXNotSupportedException e) {
  System.err.println(
   "err 2");
  return;
}

     errHandler = new Xv4htErrorHandler();
xmlReader.setErrorHandler( errHandler );

     xmlReader.setEntityResolver( new Xv4htEntityResolver() );

   }

   public void startPrefixMapping(String prefix, String uri){
   }

   public void endPrefixMapping(String prefix)  {
   }

   public void ignorableWhitespace(
                           char[] ch, int start, int length) {
   }

   public void notationDecl(
             String name, String publicId, String systemId) {
   }
   public class Xv4htEntityResolver implements EntityResolver {

  private Hashtable entities = new Hashtable();

  public Xv4htEntityResolver() {
     try{
      FileReader myIn  = new FileReader( catalog );
      while( true ){
        int c;
        String publicID = "";
        String systemID = "";

        while( (c = myIn.read()) != '\"' ){
           if( c == -1 ){ break; }
        }
        if( c == -1 ){ break; }
        while( (c = myIn.read()) != '\"' ){
           publicID += (char) c;
           if( c == -1 ){ break; }
        }
        if( c == -1 ){ break; }

        while( (c = myIn.read()) != '\"' ){
           if( c == -1 ){ break; }
        }
        if( c == -1 ){ break; }
        while( (c = myIn.read()) != '\"' ){
           systemID += (char) c;
           if( c == -1 ){ break; }
        }
        if( c == -1 ){ break; }

        entities.put(publicID, systemID);
      }
} catch(java.io.IOException e){   }

  }

  public InputSource resolveEntity(String publicID,
   String systemID) throws SAXException {
     if( publicID== null ){ return null; }
     if( entities.containsKey(publicID) ){
       String url = (String) entities.get(publicID);
       InputSource local = new InputSource(url);
       return local;
     }
     return null;
  }

}

   class Xv4htLexicalHandler implements LexicalHandler {
  public void startDTD(String name, String publicId, String systemId)
   throws SAXException {
     withinDTD = true;
     errHandler.errorOff();
  }

  public void endDTD() throws SAXException {
     withinDTD = false;
     errHandler.errorOn();
  }
public void comment (char[] text, int start, int length)
   throws SAXException {
}
  public void startEntity(String name) throws SAXException {}
  public void endEntity(String name) throws SAXException {}
  public void startCDATA() throws SAXException {}
  public void endCDATA() throws SAXException {}
}

   
}

class Xv4htErrorHandler extends DefaultHandler {
  private boolean errOn = true;

  public void warning(SAXParseException e) {
    showError( e, "Warning" );
  }

  public void error(SAXParseException e) {
    showError( e, "Warning" );
  }

  public void fatalError(SAXParseException e) {
    showError( e, "Warning" );
  }

  public void errorOff() {
     errOn = false;
  }

  public void errorOn() {
     errOn = true;
  }

  private void showError(SAXParseException e, String m){
    if( errOn ){
      int c = e.getColumnNumber();
      System.out.println(
        m + ": " + e.getSystemId() + " " + e.getLineNumber()
        + ((c==-1)? "" : ("," + c))
        + ": " + e.getMessage()
      );
  } }
}

public class xv4ht{
  static public void main (String[] args)
     throws SAXException,
            ParserConfigurationException,
            IOException
  {
     SAXParserFactory factory =
         SAXParserFactory.newInstance();
     factory.setValidating( true );
     factory.setNamespaceAware( true );
     SAXParser saxParser = factory.newSAXParser();
     XMLReader xmlReader = saxParser.getXMLReader();
     if( args.length != 2 ){
  System.out.print(
       "--- A command line of the following form is required ---"
     + "\n      java [-cp <path>] xv4ht <filename> <catalog>"
     + "\nreceived:\n      java"
  );
  for(int i=0; i<args.length; i++ ){
     System.out.print( " " + args[i] );
  }
  System.out.println();
  System.exit( 1 );
}

     xmlReader.setContentHandler( new Xv4htContentHandler(xmlReader, args[1])
 );
     xmlReader.parse ( new File(args[0]).toURL().toString() );
  }
}

