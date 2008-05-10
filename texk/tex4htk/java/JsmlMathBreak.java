package tex4ht;
/*
JsmlMathBreak.java (2007-09-01-14:10)

*/
import xtpipes.XtpipesUni;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;
import java.io.*;
import java.lang.reflect.*;
import java.util.HashMap;

public class JsmlMathBreak extends DefaultHandler {
        PrintWriter out = null;
        boolean delete = false;
  public JsmlMathBreak(PrintWriter out,
                       HashMap<String,Object> scripts,
                       Method method, PrintWriter log, boolean trace) {
    this.out = out;
  }
  public void characters(char[] ch, int start, int length) {
    String s = XtpipesUni.toUni(ch, start, length, "<>&");
    out.print( s );
    if( !s.trim().equals("") ){
       delete = false;
  } }
  public void startElement(String ns, String sName,
                                      String qName,
                                      Attributes atts) {
    if( !( delete && qName.equals("BREAK") ) ){
      String s = "<" + qName + "\n";
      for (int i = 0; i < atts.getLength(); i++) {
        String name = atts.getQName(i);
        if (name != "xmlns") {
          s += (" " + name + "=\""
              + XtpipesUni.toUni(atts.getValue(i), "<>&\"")
              + "\"");
      } }
      if( qName.equals( "BREAK" ) ){
        s += "/";
        delete = true;
      }
      s += ">";
      out.print(s);
  } }
  public void endElement(String ns, String sName, String qName) {
    if( !qName.equals( "BREAK" ) ){
      String s = "</" + qName + ">";
      out.print(s);
} } }

