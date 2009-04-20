// 2009-01-27-22:19
package xtpipes.util;
import org.xml.sax.ext.LexicalHandler;
// import org.xml.sax.ContentHandler;
import java.io.PrintWriter;
public class ScriptsManagerLH implements LexicalHandler {
       ScriptsManager contentHandler;
       PrintWriter log;
   public ScriptsManagerLH( ScriptsManager contentHandler,
                          PrintWriter log, boolean trace ){
     this.contentHandler = contentHandler;
     this.log = (log==null)? new PrintWriter( System.err ) : log;
   }
   public void comment(char[] ch, int start, int length){
     if( contentHandler.inBody ){
        String s = new String(ch, start, length);
        contentHandler.add(  "<!--" + s + "\n-->");
   } }
   public void startEntity(String x){}
   public void endEntity(String x){}
   public void startCDATA(){}
   public void endCDATA(){}
   public void startDTD(String x, String y, String z){}
   public void endDTD(){}
}

