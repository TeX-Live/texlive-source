// 2009-01-27-22:19
package xtpipes.util;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.*;
import java.io.*;
import java.lang.reflect.*;
import java.util.HashMap;
import java.util.Stack;
import java.util.ArrayList;
import java.util.HashSet;
import xtpipes.XtpipesUni;

public class ScriptsManager extends DefaultHandler {
     boolean inBody = false;
ArrayList<String> nsName = new ArrayList<String>(),
                         nsValue = new ArrayList<String>();
Stack<Integer> nsStack = new Stack<Integer>();

     PrintWriter out = null, log = null;
     HashMap<String,Object> scripts = null;
     Method method = null;
     boolean savemode=false;
     String code="", match = null;
     Stack<Object[]> stack = new Stack<Object[]>();
   public ScriptsManager( PrintWriter out,
                          HashMap<String,Object> scripts,
                          Method method,
                          PrintWriter log, boolean trace ){
     this.out = out;
     this.log = (log==null)? new PrintWriter( System.err ) : log;
     this.scripts = scripts;
     this.method = method;
   }
   public void characters(char[] ch, int start, int length){
     add( XtpipesUni.toUni(ch, start, length, "<>&") );
   }
   public void startElement(String ns, String sName,
                        String qName, Attributes atts) {
   int top = nsName.size();
nsStack.push( new Integer(top) );

   String key = (atts==null)?
               null
             : (qName + "::" + atts.getValue("class"));
boolean flag = (key != null) && scripts.containsKey(key);

if( !flag ){
   key = qName;
   flag = scripts.containsKey(key);
}

   inBody = true;
   String s =  "<" + qName + "\n";
   for(int i=0; i<atts.getLength(); i++ ){
      String name = atts.getQName(i),
             value = atts.getValue(i);
      if( name.startsWith("xmlns") ){
  if( (name.length() == 5) || (name.charAt(5) == ':') ){
     boolean bool = false;
for(int k=nsName.size(); k>0; ){
  k--;
  if( ((String) nsName.get(k)) . equals(name) ){
     bool = ((String) nsValue.get(k)) . equals(value);
     break;
} }

     if( !bool ){
        nsName.add(name); nsValue.add(value);
} }  }

      s += " " + name + "=\"" +
         XtpipesUni.toUni(value, "<>&\"") + "\"";
   }
   if( flag ){ HashSet<String> registry = new HashSet<String>();
for(int i=nsName.size(); i>top; ){
  i--;
  registry.add( (String) nsName.get(i) );
}
for(int i=top; i>0; ){
  i--;
  String nm = (String) nsName.get(i);
  if( ! registry.contains(nm) ){
     registry.add( nm );
     s += " " + nm + "=\"" +
         XtpipesUni.toUni( (String) nsValue.get(i), "<>&\"") + "\"";
} }
 }
   s += ">" ;
   if( flag ){
   Object [] state = { new Boolean(savemode), code, match };
   stack.push( state );
   savemode=true; code=""; match= key;
} else {
   Object [] state = { new Boolean(savemode), null, null };
   stack.push( state );
}
add( s );

}

   public void endElement(String ns, String sName, String qName){
   String s = "</" + qName + ">";
   add( s );
   Object [] state = (Object []) stack.pop();
   if( (String) state[1] != null ){
     Object parmValues[] = new Object[2];
parmValues[0] = scripts.get( match );
parmValues[1] = code;
try {
  s = (String) method.invoke( null, parmValues );
} catch(java.lang.reflect.InvocationTargetException e){
   log.println("--- ScriptsManager Error 1 --- " + e.getCause() );
   log.flush();
} catch (Exception e){
   log.println("--- ScriptsManager Error 2 --- " + e );
   log.flush();
}

     savemode = ((Boolean) state[0]).booleanValue();
code = (String) state[1];
match = (String) state[2];

     int top = ((Integer) nsStack.pop()) . intValue();
for(int i=nsName.size(); i>top; ){
  i--;
  nsName.remove(i);
  nsValue.remove(i);
}

     if( !s.equals("") ){
       int m = s.indexOf('>');
char [] attrs = s.substring(0,m).toCharArray();
int result = qName.length()+1,
    mark = result,
    from=-1,
    control = 12
;
char delimiter = ' ';
String name="";
for(int i=result; i<m; i++ ){
  attrs[result++] = attrs[i];
  switch( control ){
    case  12
: { if( attrs[i] == '=' ){
   name = (new String(attrs,mark,result-mark-1)).trim();
   control = 13
;
}
  break; }
    case 13
: { if( (attrs[i] == '"') || (attrs[i] == '\'') ){
   delimiter = attrs[i];
   control = 14
;
   from = result;
}
 break; }
    case 14
: { if( attrs[i] == delimiter ){
   if( name.startsWith("xmlns")
       && ((name.length() == 5) || (name.charAt(5) == ':')) ){
      String value = (new String(attrs,from,result-from-1)).trim();
      boolean bool = false;
for(int k=nsName.size(); k>0; ){
  k--;
  if( ((String) nsName.get(k)) . equals(name) ){
     bool = ((String) nsValue.get(k)) . equals(value);
     break;
} }

      if( bool ){ result = mark; }
   }
   mark = result;
   control = 12
;
}
 break; }
} }
s =  (new String(attrs,0, Math.min(result,attrs.length)))
          + s.substring(m);

       add( s );
     }
   } else { int top = ((Integer) nsStack.pop()) . intValue();
for(int i=nsName.size(); i>top; ){
  i--;
  nsName.remove(i);
  nsValue.remove(i);
}
 }
}

   protected void add(String s){
      if( savemode ){ code+=s; }
      else { out.print(s); }
}  }

