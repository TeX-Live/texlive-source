package tex4ht;
import org.xml.sax.helpers.*;
import org.xml.sax.*;
import java.io.PrintWriter;

public class GroupMn extends XMLFilterImpl {
     PrintWriter out = null;
     boolean inMn = false;
     String ns;
     int level = -1;
   public GroupMn( PrintWriter out, PrintWriter log, boolean trace){
     this.out = out;
   }
   public void startElement(String ns, String sName,
                           String qName, Attributes attr) {
      level++;
      try{
        if( inMn ){
           if( level == 0 ){ if( !qName.equals( "mn" ) ){
  if( qName.equals( "mo" ) ){
     String cls = attr.getValue( "class" );
     if( (cls == null) || !cls.equals("MathClass-punc") ){
        inMn = false;
        super.endElement(ns, "mn-group", "mn-group");
     }
  } else {
     inMn = false;
     super.endElement(ns, "mn-group", "mn-group");
} }
 }
        } else { if( qName.equals( "mn" ) ){
   inMn = true; level = 0;
   Attributes att = new AttributesImpl();
   super.startElement(ns, "mn-group", "mn-group", att);
   this.ns = ns;
} else if( qName.equals( "mo" ) ){
   String cls = attr.getValue( "class" );
   if( (cls != null) && cls.equals("MathClass-punc") ){
      inMn = true; level = 0;
      Attributes att = new AttributesImpl();
      super.startElement(ns, "mn-group", "mn-group", att);
      this.ns = ns;
}  }
 }
        super.startElement(ns, sName, qName, attr);
      } catch( Exception e ){
        System.out.println( "--- GroupMn Error 1 --- " + e);
      }
   }
   public void endElement(String ns, String sName, String qName){
      try{
        if( level < 0) {
          if( inMn ){
   inMn = false;
   super.endElement(ns, "mn-group", "mn-group");
}

        }
        super.endElement(ns, sName, qName);
      } catch( Exception e ){
        System.out.println( "--- GroupMn Error 2 --- " + e);
      }
      level--;
   }
   public void characters(char[] ch, int start, int length){
      try{
        if ( inMn  && (level < 0) ) {
           String s = new String(ch, start, length);
           if (!s.trim().equals("")) {
             inMn = false;
             super.endElement(ns, "mn-group", "mn-group");
        }  }
        super.characters(ch, start, length);
      } catch( Exception e ){
        System.out.println( "--- GroupMn Error 3 --- " + e);
}  }  }

