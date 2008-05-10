package tex4ht;
import org.xml.sax.helpers.*;
import org.xml.sax.*;
import java.io.PrintWriter;

public class JsmlFilter extends XMLFilterImpl {
     PrintWriter out = null;
   public JsmlFilter( PrintWriter out, PrintWriter log, boolean trace ){
     this.out = out;
   }
   public void startElement(String ns, String sName,
                           String qName, Attributes attr) {
      try{
        if(    qName.equals( "p" )
|| qName.equals( "h2" )
|| qName.equals( "h3" )
|| qName.equals( "h4" )
|| qName.equals( "ul" )
|| qName.equals( "ol" )
|| qName.equals( "li" )
|| qName.equals( "dd" )
|| qName.equals( "dl" )
 ){
          Attributes att = new AttributesImpl();
          super.startElement(ns, "PARA", "PARA", att);
        }
        super.startElement(ns, sName, qName, attr);
      } catch( Exception e ){
        System.out.println( "--- JsmlFilter Error 1 --- " + e);
   }  }
   public void endElement(String ns, String sName, String qName){
      try{
        super.endElement(ns, sName, qName);
        if(    qName.equals( "p" )
|| qName.equals( "h2" )
|| qName.equals( "h3" )
|| qName.equals( "h4" )
|| qName.equals( "ul" )
|| qName.equals( "ol" )
|| qName.equals( "li" )
|| qName.equals( "dd" )
|| qName.equals( "dl" )
 ){
             super.endElement(ns, "PARA", "PARA");
        }
      } catch( Exception e ){
        System.out.println( "--- JsmlFilter Error 2 --- " + e);
}  }  }

