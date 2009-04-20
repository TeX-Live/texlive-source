package xtpipes;
/*
Xtpipes.java (2009-01-27-22:19)
*/
// import xtpipes.util.InputObject;
// import xtpipes.util.FileInfo;
import java.net.URLConnection;
import java.io.*;
import java.lang.reflect.*;
import java.util.HashMap;
import java.util.Stack;
import javax.xml.parsers.*;
import javax.xml.transform.*;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.*;
import org.w3c.dom.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;
import java.net.URL;
import java.lang.reflect.Constructor;
import java.util.ArrayList;


public class Xtpipes {
   private static HashMap <String,Object> map;
private static boolean needScript;
private static boolean returnDom;
private static String result;
static PrintWriter logWriter = new PrintWriter( System.err );
private static String     inFile,
                          inData;
private static boolean exceptionErrs, messages;
public static InputObject inputObject;
private static String outFileName;
private static PrintWriter outPrintWriter;
private static boolean returnToFile = false;
public static String scriptFile;
private static String scriptMap;
static String i_scriptDir;
private static TransformerFactory fc;
private static Transformer identityTransformer;
private static SAXParserFactory saxFactory;
private static DocumentBuilder domBuilder;
private static Stack <XMLReader> saxReaderStack;
private static Method method;
private static String  rootName;
static boolean trace;
private static String [] ml2xml = null;
static Class<?> ml2xmlClassObj = null;
public static String errMssg;

   public static void main(String args[]) throws Exception {
  map = new HashMap  <String,Object> ();
needScript = true;
returnDom = false;
result = null;
inFile = null;
inData = null;
exceptionErrs = false;
messages = false;
outFileName = null;
outPrintWriter = null;
scriptFile = null;
i_scriptDir = null;
scriptMap = null;
saxReaderStack = new Stack <XMLReader> ();
rootName = null;
trace = false;

  mainMethod(args);
}

   public static void xtpipes(String [] args,
                           OutputStream out,
                           PrintWriter log)
                                                throws Exception {
  map = new HashMap  <String,Object> ();
needScript = true;
returnDom = false;
result = null;
inFile = null;
inData = null;
exceptionErrs = false;
messages = false;
outFileName = null;
outPrintWriter = null;
scriptFile = null;
i_scriptDir = null;
scriptMap = null;
saxReaderStack = new Stack <XMLReader> ();
rootName = null;
trace = false;

  outPrintWriter = new XtpipesPrintWriter( out, true );
  logWriter = (log==null)? (new PrintWriter( System.err )) : log;
  mainMethod(args);
  outPrintWriter.flush();
}

   public static void xtpipes(String [] args,
                           OutputStreamWriter out,
                           PrintWriter log)
                                                throws Exception {
  map = new HashMap  <String,Object> ();
needScript = true;
returnDom = false;
result = null;
inFile = null;
inData = null;
exceptionErrs = false;
messages = false;
outFileName = null;
outPrintWriter = null;
scriptFile = null;
i_scriptDir = null;
scriptMap = null;
saxReaderStack = new Stack <XMLReader> ();
rootName = null;
trace = false;

  outPrintWriter = new XtpipesPrintWriter( out );
  logWriter = (log==null)? (new PrintWriter( System.err )) : log;
  mainMethod(args);
  outPrintWriter.flush();
}

   public static void xtpipes(String [] args,
                           XtpipesPrintWriter out,
                           PrintWriter log)
                                                throws Exception {
  map = new HashMap  <String,Object> ();
needScript = true;
returnDom = false;
result = null;
inFile = null;
inData = null;
exceptionErrs = false;
messages = false;
outFileName = null;
outPrintWriter = null;
scriptFile = null;
i_scriptDir = null;
scriptMap = null;
saxReaderStack = new Stack <XMLReader> ();
rootName = null;
trace = false;

  outPrintWriter = out;
  logWriter = (log==null)? (new PrintWriter( System.err )) : log;
  mainMethod(args);
  outPrintWriter.flush();
}

   public static Document getDOM(String args[])
                               throws Exception {
  map = new HashMap  <String,Object> ();
needScript = true;
returnDom = false;
result = null;
inFile = null;
inData = null;
exceptionErrs = false;
messages = false;
outFileName = null;
outPrintWriter = null;
scriptFile = null;
i_scriptDir = null;
scriptMap = null;
saxReaderStack = new Stack <XMLReader> ();
rootName = null;
trace = false;

   returnDom = true;
   mainMethod(args);
   Document dom = null;
   if( result == null ){
     System.err.println(
       "--- xtpipes warning --- getDOM without <return name=\"...\"> from 4xt file: "
         + scriptFile );
   } else {
      try{
         byte [] bytes = result.getBytes("UTF-8");
         InputStream is =  new ByteArrayInputStream( bytes );
         dom = domBuilder.parse (is);
      } catch ( org.xml.sax.SAXParseException e ){
         if( Xtpipes.trace ){
            Xtpipes.logWriter.println(
               "\n---------------------------------------------------\n"
                        + result +
               "\n---------------------------------------------------\n" );
         }
         String s = "";
         for( int n=0; n<args.length; n++ ){
  if( args[n].charAt(0)!='-' ){
     s += "  input file: " + args[n] + ".";  break;
  }
  else if( args[n].equals("-s")
           || args[n].equals("-S")
           || args[n].equals("-i")
           || args[n].equals("-o")
           || args[n].equals("-d") ){ n++;  }
}

         if( scriptFile != null ){ s += "    script file: " + scriptFile; }
         instructionErr( null,
            "parsing error: " + e.getMessage() +s, 21 );
      } catch ( Exception e ){
         instructionErr( null, e.toString(), 5 );
      }
      if( ml2xmlClassObj != null ){
  Class<?> [] argTypes = { };
  Method m = ml2xmlClassObj.getMethod( "closeFiles", argTypes );
  Object parmValues[] = new Object[0];
  m.invoke( null, parmValues );
}

   }
   return dom;
}
public static Document getDOM(String s, String args[])
                                           throws Exception {
   map = new HashMap  <String,Object> ();
needScript = true;
returnDom = false;
result = null;
inFile = null;
inData = null;
exceptionErrs = false;
messages = false;
outFileName = null;
outPrintWriter = null;
scriptFile = null;
i_scriptDir = null;
scriptMap = null;
saxReaderStack = new Stack <XMLReader> ();
rootName = null;
trace = false;

   returnDom = true;
   inData = s;
   mainMethod(args);
   Document dom = null;
   if( result == null ){
     System.err.println(
       "--- xtpipes warning --- getDOM without"
         + " <return name=\"...\"> from 4xt file: "
         + scriptFile );
   } else {
      try{
         byte [] bytes = result.getBytes("UTF-8");
         InputStream is =  new ByteArrayInputStream( bytes );
         dom = domBuilder.parse (is);
      } catch ( org.xml.sax.SAXParseException e ){
         instructionErr( null, "improper xml: " + e.getMessage()
           + "\n code starts with: "
           + result.substring(0, Math.min(100,result.length()))
         , 17 );
      } catch ( Exception e ){
         instructionErr( null, e.toString(), 6 );
      }
      if( ml2xmlClassObj != null ){
  Class<?> [] argTypes = { };
  Method m = ml2xmlClassObj.getMethod( "closeFiles", argTypes );
  Object parmValues[] = new Object[0];
  m.invoke( null, parmValues );
}

   }
   return dom;
}
public static Document getDOM(String args[], PrintWriter log)
                               throws Exception {
   logWriter = (log==null)? new PrintWriter( System.err ) : log;
   return getDOM(args);
}
public static Document getDOM(String s, String args[], PrintWriter log)
                                           throws Exception {
   logWriter = (log==null)? (new PrintWriter( System.err )) : log;
   return getDOM(s, args);
}

   private static void mainMethod(String args[]) throws Exception {
  try{
    String xtpipes_call =
     "   xtpipes (2009-01-27-22:19)"
   + "\n   Command line options: "
   + "\n     java xtpipes [-trace] [-help] [-m] [-E] [-s script_file]"
   +                                               " [-S script_map]"
   + "\n                  [-i script_dir] [-o out_file] "
   + "\n                  [-x...ml2xml_arg...]  "
   +                     "(-d in_data | in_file)"
   + "\n     -m        messages printing mode"
   + "\n     -E        error messages into exception calls"
   + "\n     in_data   XML data directly into the command line\n"
;

    boolean help=false;
for( int n=0; n<args.length; n++ ){
  if( args[n] == null ){}
else if( args[n].equals("") ){}
else if( args[n].charAt(0)!='-' ){ inFile = args[n]; }
else if( args[n].equals("-m") ){
  messages = true;
  logWriter.println(
     "xtpipes (2009-01-27-22:19)"
     + "\n java.version: "    + System.getProperty("java.version")
     + "\n java.class.path: " + System.getProperty("java.class.path")
     + "\n os.name: "         + System.getProperty("os.name")
     + "\n user.home: "       + System.getProperty("user.home")
     + "\n user.dir: "        + System.getProperty("user.dir")
);
for( int k=0; k<args.length; k++ ){
  logWriter.println( "     " + args[k] );
}

}
else if( args[n].equals("-s") ){
  n++;
if( n < args.length ){ scriptFile=args[n]; }
else {
  System.err.println(
      "--- Error --- Missing field for -s argument" );
  inFile = null; inData = null;  break;
}

}
else if( args[n].equals("-S") ){
  n++;
if( n < args.length ){ scriptMap=args[n]; }
else {
  System.err.println(
      "--- Error --- Missing field for -S argument" );
  inFile = null; inData = null;  break;
}

}
else if( args[n].equals("-i") ){
  n++;
if( n < args.length ){
   i_scriptDir=args[n];
} else {
  System.err.println(
      "--- Error --- Missing field for -i argument" );
  inFile = null; inData = null; break;
}

}
else if( args[n].equals("-o") ){
  n++;
if( n < args.length ){
   outFileName = args[n];
} else {
   System.err.println(
       "--- Error --- Missing field for -o argument" );
   inFile = null; inData = null; break;
}

}
else if( args[n].startsWith("-x") ){
  if( args[n].substring(2).equals("") ){
   if( ml2xml == null ){ ml2xml = new String[0]; }
} else {
   if( ml2xml == null ){
      ml2xml = new String[1];
   } else {
      String [] m2x = new String [ml2xml.length + 1];
      for(int cnt=0; cnt < ml2xml.length; cnt++){
        m2x[cnt] = ml2xml[cnt];
      }
      ml2xml = m2x;
   }
   ml2xml[ ml2xml.length - 1 ] = args[n].substring(2);
}

}
else if( args[n].equals("-E") ){
  exceptionErrs = true;
}
else if( args[n].equals("-d") ){
  n++;
if( n < args.length ){
   inData = args[n];
} else {
   System.err.println(
       "--- Error --- Missing field for -d argument" );
   inFile = null; inData = null; break;
}

}
else if( args[n].equals("-trace") ){ trace=true; }
else if( args[n].equals("-help") ){ help=true; }
else { if( !exceptionErrs ){
  for(int i=n+1; i<args.length; i++ ){
    if( args[i].equals("-E") ){ exceptionErrs = true; }
} }
instructionErr( null,
     "Improper argument: " + args[n] + "\n" + xtpipes_call, 26 );
 }

}
if( outPrintWriter == null ){
   outPrintWriter = new XtpipesPrintWriter(System.out,true);
}

if( !returnDom ){
   if( help || ((inFile == null) && (inData == null)) ){
     System.err.println( xtpipes_call );

     if( (inFile == null) && (inData == null) ){
        System.exit(0);
}  } }

new FileInfo(logWriter, i_scriptDir, trace);
if( inFile != null ){
   inputObject = new InputObject( inFile, logWriter );
   if( inputObject.getInputStream() == null ){
      instructionErr( null, "Could not find or open file: " + inFile, 28 );
   }
   inFile = inputObject.getFilename();
} else {
   inputObject = new InputObject( inData.getBytes("UTF-8"), logWriter );
}
inputObject.buildProfile( trace );

  } catch (Exception e){
     instructionErr( null, e.getMessage(), e.getStackTrace(), 31 );
  }
  try {
     DocumentBuilderFactory domFactory =
           DocumentBuilderFactory.newInstance();
     domFactory.setValidating(true);
     DocumentBuilder validatingDomBuilder =
                     domFactory.newDocumentBuilder();
     validatingDomBuilder.setEntityResolver(new XtpipesEntityResolver()
);
     validatingDomBuilder.setErrorHandler(new ErrorHandler() {
    public void warning(SAXParseException e) throws SAXParseException {
      showSpecifics("warning",e);
    }
    public void error(SAXParseException e) throws SAXParseException {
      showSpecifics("error",e);
    }
    public void fatalError(SAXParseException e) throws SAXParseException {
      showSpecifics("fatal error",e);
    }
    public void showSpecifics(String s, SAXParseException e)
                                                throws SAXParseException {
      String err =   "--- xtpipes " + s + " 24 --- " + e.getSystemId()
                     + " line " + e.getLineNumber()
                     + " col "  + e.getColumnNumber()
                     + " : "    + e.getMessage() ;
      if( exceptionErrs ) { throw new SAXParseException(
                                     err, (org.xml.sax.Locator) null ); }
      else {
         System.err.println( err );
         System.exit(1);
}   } }
);
     fc = TransformerFactory.newInstance();
identityTransformer =  fc.newTransformer();
identityTransformer.setErrorListener(
    new ErrorListener() {
   public void warning(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   public void error(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   public void fatalError(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   void showSpecifics(TransformerException e)
                                               throws  TransformerException{
     String err = e.getMessage() ;
     String loc = e.getLocationAsString();
     if( loc != null ){ err = loc + ": " + err; }
     throw new TransformerException(err);
}  }
 );
saxFactory = SAXParserFactory.newInstance();
saxFactory.setValidating(true);
domFactory.setValidating(false);
domBuilder = domFactory.newDocumentBuilder();
saxFactory = SAXParserFactory.newInstance();
saxFactory.setValidating(false);
Class <?> cls = Xtpipes.class;
Class<?> [] argTypes = { Node.class, String.class };
method = cls.getMethod( "execute", argTypes );

     if( scriptMap != null ){
   try{
      String f = FileInfo.searchFile( scriptMap );
      if( f == null ){
         throw new java.io.FileNotFoundException( scriptMap );
      } else {
         scriptMap = f;
      }
      XMLReader saxReader;
if( saxReaderStack.empty() ){
   SAXParser saxParser = saxFactory.newSAXParser();
   saxReader = saxParser.getXMLReader();
   saxReader.setEntityResolver(new org.xml.sax.EntityResolver() {
   public InputSource resolveEntity(
                          String publicId, String systemId) {
     if( (new File(systemId)).exists() ){
        return new org.xml.sax.InputSource( systemId );
      }
      StringReader strReader = new StringReader("");
      return new org.xml.sax.InputSource(strReader);
   }
});

} else {
   saxReader = (XMLReader) saxReaderStack.pop();
}

      saxReader.setContentHandler( new DefaultHandler(){
         private Stack <Boolean> condition = new Stack <Boolean> ();
public void startDocument () {
   condition.push( new Boolean(true) );
}
public void startElement(String ns, String sName,
                        String qName, Attributes atts) {
   if( condition == null ){ return; }
   if( Xtpipes.trace ){
   String s =  "<" + qName + "\n";
   for(int i=0; i<atts.getLength(); i++ ){
      String name = atts.getQName(i);
      s += " " + name + "=\"" + atts.getValue(i) + "\"";
   }
   s += ">" ;
   Xtpipes.logWriter.println( s );
}

   boolean cond = ((Boolean) condition.peek()).booleanValue();
   if( qName.equals("when") ){
      if( cond ){ String name = atts.getValue("name");
String value = atts.getValue("value");
if( name.equals("system-id") ){
   cond = value.equals(inputObject.getSystemId());
}
else
if( name.equals("public-id") ){
   cond = value.equals(inputObject.getPublicId());
}
else
if( name.equals("dtd-root") ){
   cond = value.equals(inputObject.getDtdRoot());
}
else
if( name.equals("root") ){
   cond = value.equals(inputObject.getRoot());
}
else
if( name.equals("ext") ){
   cond = inputObject.getFilename().endsWith("." + value);
}
else
if( name.equals("prefix") ){
   name = inputObject.getFilename();
   if( name != null ){
      int i = name.lastIndexOf('/');
      if( (i != -1) && ((i+1) < name.length()) ){
         name = name.substring(i+1);
      }
      i = name.lastIndexOf('\\');
      if( (i != -1) && ((i+1) < name.length()) ){
         name = name.substring(i+1);
      }
      cond = name.startsWith(value);
}  }
else
if( name.equals("meta-type") ){
   cond = value.equals(inputObject.getMetaType());
}
else
if( name.equals("content-type") ){
   cond = value.equals(inputObject.getContentType());
}
 }
   }
   else
   if( qName.equals("command-line") ){
      if( scriptFile != null ){
        if( trace ){
   Xtpipes.logWriter.println( " Found script file in map: "
                                + Xtpipes.scriptFile );
}

        condition = null;
        return;
   }  }
   else
   if( qName.equals("processing-instruction") ){
      if( cond ){
         String s = inputObject.getXtpipes();
         if( s != null ){
           Xtpipes.scriptFile = s;
           if( trace ){
   Xtpipes.logWriter.println( " Found script file in map: "
                                + Xtpipes.scriptFile );
}

           condition = null;
           return;
   }  }  }
   else
   if( qName.equals("select") ){
      if( cond ){
        Xtpipes.scriptFile = atts.getValue("name");
        if( trace ){
   Xtpipes.logWriter.println( " Found script file in map: "
                                + Xtpipes.scriptFile );
}

        condition = null;
        return;
   }  }
   condition.push( new Boolean(cond) );
}
public void endElement(String ns, String sName, String qName) {
   if( condition == null ){ return; }
   if( Xtpipes.trace ){
   String s =  "</" + qName + ">";
   Xtpipes.logWriter.println( s );
}

   condition.pop();
}

        } );
      InputStream inputStream =
           (InputStream) (new File(scriptMap).toURI().toURL().openStream());
      saxReader.parse( new InputSource(inputStream) );
      saxReaderStack.push( saxReader );
   } catch( java.io.FileNotFoundException e ){
      instructionErr( null,
                      "File not found: " + e.getMessage()
                      + "; command line option -i",
                      33 );
   } catch( Exception e ){
      instructionErr( null, e.toString(), e.getStackTrace(), 27 );
   }
}
if( scriptFile == null ){
    scriptFile = "xtpipes-default.4xt";
}

     while( needScript ){
       if( scriptFile == null ){
          instructionErr( null, "Missing 4xt script file name", 32 );
       }
       String f = FileInfo.searchFile( scriptFile );
if( f == null ){
   throw new java.io.FileNotFoundException( scriptFile );
} else {
   scriptFile = f;
}

       Document script = validatingDomBuilder.parse(scriptFile);
       if( trace ){
   logWriter.println( "(" + scriptFile + ")" );
}

       execute( script.getFirstChild() );
     }
     if( outFileName != null ){
   outPrintWriter.close();
}

      Xtpipes.logWriter.flush();
  } catch( org.xml.sax.SAXParseException e ){
     String s = "Improper file " + scriptFile + ": " + e.getMessage();
     instructionErr( null, s, 2 );
  } catch( java.io.FileNotFoundException e ){
     String s;
     if( scriptFile.equals( e.getMessage() ) ){
        s =  "Could not find file: " + scriptFile;
     } else {
        s = "Problem at script " + scriptFile + ": Could not find file "
                                                     + e.getMessage();
     }
     instructionErr( null, s, 3 );
  } catch( Exception e ){
     String s = "Problems at file: " + scriptFile + "\n   " + e;
     instructionErr( null, s, 4 );
} }

   private static void execute( Node node ) throws Exception {
  while( node != null ){
    if( node.getNodeType()==Node.ELEMENT_NODE ){
      String instruction = node.getNodeName();
      if( trace ){
   logWriter.print( "[##] = xtpipes => " + instruction );
   if( node.hasAttributes() ){
      NamedNodeMap attributes = node.getAttributes();
      for(int i=0; i < attributes.getLength(); i++ ){
         Node attr = attributes.item(i);
         logWriter.print( " "   + attr.getNodeName()
                         + "=\"" + attr.getNodeValue() + "\"" );
   } }
   logWriter.println(); logWriter.flush();
}

      if( instruction.equals( "xtpipes" ) ){
         // String errMsg = "";
needScript = false;
if( node.hasChildNodes() ){
  if( outFileName != null ){
   try {
      FileWriter fw = new FileWriter( outFileName );
      outPrintWriter = new XtpipesPrintWriter( fw );
      returnToFile = true;
   } catch(Exception e){
      instructionErr( null, e.toString(), 12 );
}  }

  if( node.hasAttributes() ){
   Node attr = node.getAttributes()
                   .getNamedItem( "signature" );
   if ( (attr != null) && messages ) {
   logWriter.println( attr.getNodeValue() );
}

   attr = node.getAttributes()
                   .getNamedItem( "preamble" );
   if( (attr != null)
       && attr.getNodeValue().equals( "yes" ) ){
      // BufferedReader br = null;
try {
   String s;
   boolean front = true;
   rootName = "<" + ((rootName==null)? inputObject.getRoot() : rootName);
   if( inData == null ){
      // FileReader fr = new FileReader(inFile);
// BufferedReader in = new BufferedReader(fr);
URLConnection connection =
             new URL(inFile).openConnection();
connection.setRequestProperty("User-Agent",
                "["
              + System.getProperty("os.name")
              + " / "
              + System.getProperty("os.arch")
              + "]"
              + "["
              + System.getProperty("java.version")
              + " - "
              + System.getProperty("java.vendor")
              + "]"
);
InputStream inputStream = connection.getInputStream();
BufferedReader in = new BufferedReader (
                        new InputStreamReader ( inputStream ) );

while (  ((s = in.readLine()) != null) && front ) {
   int i = s.indexOf( rootName );
   if( i > -1 ){
      front = false;
      s = s.substring(0,i);
   }
   outPrintWriter.println(s);
   returnToFile = false;
}
in.close();

   } else {
      int i = inData.indexOf( rootName );
if( i > -1 ){
   front = false;
   s = inData.substring(0,i);
} else { s = ""; }
outPrintWriter.println(s);
returnToFile = false;

   }
} catch (Exception e) {
   System.err.println(
        "--- Error --- Couldn't copy preamble: " + e);
}

} }

   execute( node.getFirstChild() );
} else {
   if( inData == null ){
   /*
errMsg = "Searching <?xtpipes file=\"...\"?>  in "
                      + inFile + ": ";
*/
scriptFile = inputObject.getXtpipes();
rootName = inputObject.getRoot();
needScript = true;

} else {
   scriptFile = inputObject.getXtpipes();
rootName = inputObject.getRoot();
needScript = true;

}

}

      } else if( instruction.equals( "set" ) ){
         String name = node.getAttributes().getNamedItem( "name" )
                                  .getNodeValue();
Node cdata = node.getFirstChild();
while( cdata.getNodeType() != Node.CDATA_SECTION_NODE ){
   cdata = cdata.getNextSibling();
}
String code = cdata.getNodeValue().trim();
map.put( name, (Object) code );

      } else if( instruction.equals( "get" ) ){
         try {
  String name = node.getAttributes()
                  .getNamedItem( "name" ).getNodeValue();
  String file = node.getAttributes()
               .getNamedItem( "file" ).getNodeValue();
  StreamSource in   = new StreamSource( new File(file) );
  ByteArrayOutputStream baos = new ByteArrayOutputStream();
  identityTransformer.transform( in, new StreamResult(baos) );
  byte [] bytes = baos.toByteArray();
  map.put( name, (Object) new String(bytes) );
} catch( Exception e ){
   instructionErr( node, e.toString(), 14 );
}

      } else if( instruction.equals( "print" ) ){
         String name = node.getAttributes()
                 .getNamedItem( "name" ).getNodeValue();
String xml = (String) map.get(name);
if( node.getAttributes().getNamedItem( "file" )==null ){
   if( !Xtpipes.trace ){
      logWriter.println( "[##]  = xtpipes => print: " + scriptFile );
   }
   logWriter.println( XtpipesUni.toUni(xml, "") );
} else {
   String file = node.getAttributes()
                 .getNamedItem( "file" ).getNodeValue();
   try{
       FileWriter fw = new FileWriter( file );
       XtpipesPrintWriter out = new XtpipesPrintWriter( fw );
       out.println( xml );
       out.close();
   } catch(Exception e){
       instructionErr( node, e.toString(),15 );
}  }

      } else if( instruction.equals( "return" ) ){
         String name = node.getAttributes()
                 .getNamedItem( "name" ).getNodeValue();
result = (String) map.get(name);
if( returnToFile ){
   outPrintWriter.println(result);
}

      } else if( instruction.equals( "if" ) ){
         try{
   String xml = node.getAttributes()
                 .getNamedItem( "xml" ).getNodeValue();
String dtd = node.getAttributes()
                 .getNamedItem( "dtd" ).getNodeValue();
// String root = node.getAttributes()
//                 .getNamedItem( "root" ).getNodeValue();
String doc = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
             + "<!DOCTYPE doc [\n"
             + (String) map.get(dtd)
             + "\n]>\n"
             + (String) map.get(xml) ;

   byte [] bytes = doc.getBytes("UTF-8");
   ByteArrayInputStream bais = new ByteArrayInputStream( bytes );
   InputSource in = new InputSource( bais );
   SAXParser saxParser = saxFactory.newSAXParser();
   XMLReader xmlReader = saxParser.getXMLReader();
   xmlReader.parse( in );
   if( trace ){
   logWriter.print( "--> true" );
}

   execute( node.getFirstChild() );
} catch ( Exception e ){ if( trace ){
   logWriter.print( "--> true" );
}
 }

      } else if( instruction.equals( "xslt" ) ){
         try{
   Node xmlNode = node.getAttributes().getNamedItem( "xml" );
StreamSource inDoc = null;
String xml;
if( xmlNode == null ){
   if( inData == null ){
   inDoc = new StreamSource( new File(inFile) );
} else {
   byte [] bytes = inData.getBytes("UTF-8");
   ByteArrayInputStream bais = new ByteArrayInputStream( bytes );
   inDoc = new StreamSource( bais );
}

} else {
   xml = xmlNode.getNodeValue();
   String doc = (String) map.get(xml);
   if( doc!=null ){
      byte [] bytes = doc.getBytes("UTF-8");
      ByteArrayInputStream bais = new ByteArrayInputStream( bytes );
      inDoc = new StreamSource( bais );
}  }

   String xslt = node.getAttributes()
                  .getNamedItem( "xsl" ).getNodeValue();
String templates = (String) map.get(xslt);
byte [] bytes = templates.getBytes("UTF-8");
ByteArrayInputStream bais =  new ByteArrayInputStream( bytes );
StreamSource inXslt = new StreamSource( bais );

   Node nameNode = node.getAttributes().getNamedItem("name");
StreamResult outDoc;
CharArrayWriter caos = null;
if( nameNode == null ){
   outDoc = new StreamResult(outPrintWriter);
   returnToFile = false;
} else {
   caos = new CharArrayWriter();
   outDoc  = new StreamResult(caos);
}

   errMssg = null;
   fc.setErrorListener( new ErrorListener() {
   public void warning(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   public void error(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   public void fatalError(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   void showSpecifics(TransformerException e)
                                               throws  TransformerException{
     String err = e.getMessage() ;
     String loc = e.getLocationAsString();
     if( loc != null ){ err = loc + ": " + err; }
     err = "XSL stylesheet problem: " + err;
     if( errMssg == null ){ errMssg = err; }
     throw new TransformerException(err);
}  }
 );
   Transformer transformer = fc.newTransformer( inXslt );
   transformer.setErrorListener( new ErrorListener() {
   public void warning(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   public void error(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   public void fatalError(TransformerException e) throws TransformerException {
     showSpecifics(e);
   }
   void showSpecifics(TransformerException e)
                                               throws  TransformerException{
     String err = e.getMessage() ;
     String loc = e.getLocationAsString();
     if( loc != null ){ err = loc + ": " + err; }
     if( errMssg == null ){ errMssg = err; }
     err = "XML document prblem: " + err;
     throw new TransformerException(err);
}  }
 );
   transformer.transform(inDoc, outDoc );
   if( nameNode != null ){
   String name = nameNode.getNodeValue();
   char [] chars = caos.toCharArray() ;
   map.put( name, (Object) new String(chars) );
}


} catch ( javax.xml.transform.TransformerException e ){
   if( Xtpipes.trace ){ e.printStackTrace(); }
   instructionErr( node,
                   e.getMessage()
                   +
                   ((errMssg==null)? "" : ("; " +errMssg))
                   , 37);
} catch ( Exception e ){
   if( Xtpipes.trace ){ e.printStackTrace(); }
   instructionErr( node, (errMssg==null)? e.toString()
                                        : e.toString() + "; " + errMssg
                       , 16 );
}

      } else if( instruction.equals( "dom" ) ){
         try{
   Node xmlNode = node.getAttributes().getNamedItem( "xml" );
Document dom;
if( xmlNode == null ){
   if( inData == null ){
   dom = domBuilder.parse( new File(inFile) );
} else {
   byte [] bytes = inData.getBytes("UTF-8");
   InputStream is =  new ByteArrayInputStream( bytes );
   dom = domBuilder.parse (is);}

} else {
   String xml = xmlNode.getNodeValue();
   String doc = (String) map.get(xml);
   if( doc == null ){
      instructionErr( node, "improper xml attribute value", 18 );
   }
   byte [] bytes = doc.getBytes("UTF-8");
   InputStream is = new ByteArrayInputStream( bytes );
   dom = domBuilder.parse (is);
}

   String className = node.getAttributes()
             .getNamedItem( "class" ).getNodeValue();
String methodName = node.getAttributes()
             .getNamedItem( "method" ).getNodeValue();
Class <?> cls = Class.forName( className );
Class<?>  [] argTypes = { Node.class };
Method m = cls.getMethod( methodName, argTypes );
Object parmValues[] = new Object[1];
parmValues[0] = dom;
m.invoke( null, parmValues );

   Node nameNode = node.getAttributes().getNamedItem("name");
StreamResult outDoc;
CharArrayWriter caos = null;
if( nameNode == null ){
   outDoc = new StreamResult(outPrintWriter);
   returnToFile = false;
} else {
   caos = new CharArrayWriter();
   outDoc  = new StreamResult(caos);
}

cleanXmlns(dom);
DOMSource domSource = new DOMSource(dom);
try{
   identityTransformer.transform( domSource, outDoc );
} catch ( javax.xml.transform.TransformerException e ){
  String s = Xtpipes.trace?
      (
        "\n------------------------ xml code ------------------------\n"
      + serialize( dom )
      + "\n----------------------------------------------------------\n"
      )
      : "";
   instructionErr( node, e.getMessage() + s, 35 );
}
if( nameNode != null ){
  String name = nameNode.getNodeValue();
  char [] chars = caos.toCharArray() ;
  String domString = new String(chars);
  Node dcl = node.getAttributes().getNamedItem( "dcl" );
if(  ((dcl == null) || (dcl.getNodeValue().equals("no") ))
     &&
       (domString.length() > 7)
     &&
       domString.startsWith("<?xml")
     &&
       !Character.isLetterOrDigit( domString.charAt(5) )
){
    domString = domString.substring( domString.indexOf("?>") + 2 );
}

  map.put( name, (Object) domString );
}

} catch ( NoSuchMethodException e ){
   instructionErr( node,
       "could not find method: " + e.getMessage(), 18 );
} catch ( java.lang.reflect.InvocationTargetException e ){
   if( Xtpipes.trace ){ e.printStackTrace(); }
   instructionErr( node, e.getCause().toString(), 36);
} catch ( Exception e ){
   if( Xtpipes.trace ){ e.printStackTrace(); }
   instructionErr( node, e.toString(), 20 );
}

      } else if( instruction.equals( "sax" ) ){
         String errMsg = "";
try{
   Node xmlNode = node.getAttributes().getNamedItem( "xml" );
InputSource inputSource=null;
String xml = null;
if( xmlNode == null ){
   if( inData == null ){
   xml = inFile;
} else {
   byte [] bytes = inData.getBytes("UTF-8");
   ByteArrayInputStream bais = new ByteArrayInputStream( bytes );
   inputSource = new InputSource( bais );
}

} else {
   xml = xmlNode.getNodeValue();
   String doc = (String) map.get(xml);
   if( doc!=null ){
      byte [] bytes = doc.getBytes("UTF-8");
      ByteArrayInputStream bais = new ByteArrayInputStream( bytes );
      inputSource = new InputSource( bais );
}  }

   String [] className = node.getAttributes()
                     .getNamedItem( "content-handler" )
                     .getNodeValue()
                     .split(",");

   Class<?> [] argTypes = {
         PrintWriter.class, HashMap.class, Method.class,
         PrintWriter.class, boolean.class };
Node nameNode = node.getAttributes().getNamedItem("name");
PrintWriter out;
CharArrayWriter caos = null;
if( nameNode == null ){
   out = outPrintWriter;
   returnToFile = false;
} else {
   caos = new CharArrayWriter();
   out = new PrintWriter( caos );
}

Object parmValues[] = new Object[5];
parmValues[0] = out;
HashMap <String,Object> scripts = new HashMap <String,Object> ();
Node script = node.getFirstChild();
while( script != null ){
  if( script.getNodeType()==Node.ELEMENT_NODE ){
     String element = script.getAttributes().getNamedItem( "element" )
                                       .getNodeValue();
     if( scripts.containsKey(element) ){
        System.err.println(
           "--- Warning --- redfining script: " + element );
     }
     scripts.put( element, (Object) script );
  }
  script = script.getNextSibling();
}
  parmValues[1] = scripts;
parmValues[2] = method;
parmValues[3] = Xtpipes.logWriter;
parmValues[4] = (Object) Xtpipes.trace;
Class<?> cls = Class.forName( className[0].trim() );
Constructor<?> c = cls.getConstructor( argTypes );
Object ch = (Object) c.newInstance( parmValues );

   XMLReader saxReader;
if( saxReaderStack.empty() ){
   SAXParser saxParser = saxFactory.newSAXParser();
   saxReader = saxParser.getXMLReader();
   saxReader.setEntityResolver(new org.xml.sax.EntityResolver() {
   public InputSource resolveEntity(
                          String publicId, String systemId) {
     if( (new File(systemId)).exists() ){
        return new org.xml.sax.InputSource( systemId );
      }
      StringReader strReader = new StringReader("");
      return new org.xml.sax.InputSource(strReader);
   }
});

} else {
   saxReader = (XMLReader) saxReaderStack.pop();
}

   XMLReader reader = saxReader;
   for( int i=1; i<className.length; i++ ){
   argTypes = new Class [3];
   argTypes[0] = PrintWriter.class;
   argTypes[1] = PrintWriter.class;
   argTypes[2] = boolean.class;
   parmValues = new Object[3];
   parmValues[0] = out;
   parmValues[1] = Xtpipes.logWriter;
   parmValues[2] = (Object) Xtpipes.trace;
   errMsg = "Class.forName( " + className[i].trim() + ") " ;
   cls = Class.forName( className[i].trim() );
   errMsg = "get-constructor "
            + className[i].trim()
            + "( PrintWriter, PrintWriter, boolean ) " ;
   c = cls.getConstructor( argTypes );
   errMsg = "get-object "
            + className[i].trim()
            + "( PrintWriter, PrintWriter, boolean ) " ;
   if( (cls.getModifiers() % 2) != 1 ){
      errMsg += "; class not defined to be public. ";
   }
   XMLFilter filter = (XMLFilter) c.newInstance( parmValues );
   errMsg = "set-parent "
            +  className[i].trim()
            + "( PrintWriter, PrintWriter, boolean ) " ;
   filter.setParent(saxReader);
   saxReader = filter;
}

   errMsg = "setContentHandler( "
            + className[0].trim() + " )";
   saxReader.setContentHandler( (org.xml.sax.ContentHandler) ch );
   Node lexAttr = node.getAttributes()
                  .getNamedItem( "lexical-handler" );
if( lexAttr != null ){
   String lexName = lexAttr.getNodeValue();
   argTypes = new Class[3];
   argTypes[0] = Class.forName( className[0].trim() );
   argTypes[1] = PrintWriter.class;
   argTypes[2] = boolean.class;
   parmValues = new Object[3];
   parmValues[0] = ch;
   parmValues[1] = Xtpipes.logWriter;
   parmValues[2] = (Object) Xtpipes.trace;
   errMsg = "Class.forName( " + lexName.trim() + ") " ;
   cls = Class.forName( lexName.trim() );
   errMsg = "get-constructor " +
                lexName.trim() +
                "( " + className[0].trim() + " ) " ;
   c = cls.getConstructor( argTypes );
   errMsg = "get-object " +
               lexName.trim() + "( ... ) " ;
   Object xh = (Object) c.newInstance( parmValues );
   errMsg = "set lexical handler " + lexName.trim() + " ";
   saxReader.setProperty(
       "http://xml.org/sax/properties/lexical-handler",
       (org.xml.sax.ext.LexicalHandler) xh
    );
}

   saxReader.setEntityResolver(new org.xml.sax.EntityResolver() {
   public InputSource resolveEntity(
                          String publicId, String systemId) {
     if( (new File(systemId)).exists() ){
        return new org.xml.sax.InputSource( systemId );
      }
      StringReader strReader = new StringReader("");
      return new org.xml.sax.InputSource(strReader);
   }
});

   if( inputSource==null ){
       errMsg = "While parsing file " + xml + ": ";
InputStream inputStream = null;
if( Xtpipes.ml2xml == null ){
   if( Xtpipes.trace ){
       Xtpipes.logWriter.println(
         "No request for ml2xml configuration (command line option -x)" );
   }
   try{
    inputStream = (InputStream) (new File(xml).toURI().toURL().openStream());
} catch ( java.io.FileNotFoundException ioe ){
    try{
       URL url = null;
       try {
           url = new URL(xml);
       } catch ( java.net.MalformedURLException fnf ){
           url = new File(xml).toURI().toURL();
       }
       inputStream = (InputStream) (url.openStream());
    } catch ( java.io.FileNotFoundException fnf ){
        inputStream = (InputStream)
           (
              new File( new File(xml).toURI().toURL().toString() )
              . toURI().toURL()
              . openStream()
           );
}   }

} else {
   try{
   ml2xmlClassObj = Class.forName( "ml2xml.Ml2xml" );
} catch (java.lang.ClassNotFoundException cnf ){
   instructionErr( null, "Class not found: ml2xml.Ml2xml", 25 );
}
Class<?> [] argTyp = { String.class, String[].class };
Constructor<?> con = ml2xmlClassObj.getConstructor( argTyp );
try{
   if( Xtpipes.trace ){
   String s = "Calling: ml2xml.Ml2xml(inputStream,"
              + "new String[]{" ;
   for(int i=0; i < Xtpipes.ml2xml.length; i++){
      s += ((i==0)? "\"" : ", \"") + Xtpipes.ml2xml[i] + "\"";
   }
   s += "})";
   Xtpipes.logWriter.println( s );
}

   inputStream = (InputStream) con.newInstance(
         new Object[]{xml, ml2xml}
      );
} catch(java.lang.reflect.InvocationTargetException ite){
   String s = "Problem at: ml2xml.Ml2xml(" + xml + ","
              + "new String[]{" ;
   for(int i=0; i < Xtpipes.ml2xml.length; i++){
      s += ((i==0)? "\"" : ", \"") + Xtpipes.ml2xml[i] + "\"";
   }
   s += "})";
   instructionErr( null, s + "; " + ite.getCause(), 38);
}

}
saxReader.parse( new InputSource(inputStream) );
if( Xtpipes.ml2xml != null ){
   Class<?> [] argTyp = {};
   Method m = ml2xmlClassObj . getMethod( "closeFiles", argTyp );
   m.invoke( null, new Object[0] );
}


   } else {
       errMsg = "xtpipes sax parsing error";
       saxReader.parse( inputSource );
   }
   if( nameNode != null ){
   String name = nameNode.getNodeValue();
   char [] chars = caos.toCharArray() ;
   map.put( name, (Object) new String(chars) );
}

   saxReaderStack.push( reader );
} catch ( java.io.FileNotFoundException e ){
   instructionErr( node, errMsg
                   + "could not find file: " + e.getMessage(), 19 );
} catch ( ClassNotFoundException e ){
   instructionErr( node, errMsg
                   + " class not found: "
                   + e.getMessage() + "\n classpath = "
                   + System.getProperty("java.class.path")
                   + " ---", 22 );
} catch ( java.lang.reflect.InvocationTargetException e ){
   instructionErr( node, errMsg + ": " + e.getCause(), 23 );
} catch ( Exception e ){
   Xtpipes.logWriter.flush();
   e.printStackTrace();
   instructionErr( node, errMsg + ": " + e.toString(), 29 );
}

      } else {
         instructionErr( node, "Improper instruction: " + instruction, 11 );
    } }
    node = node.getNextSibling();
} }

   public static String execute( Node node, String xml )
                                      throws Exception {
  String name = ".";
  String old = (String) map.get(name);
  map.put( name, (Object) xml );
  execute( node.getFirstChild() );
  String s = (String) map.get(name);
  if( old != null ){ map.put( name, (Object) old ); }
  return s;
}

   private static void instructionErr( Node node, String e, int num )
                                     throws Exception {
   String err = "--- xtpipes error " + num + " --- ";
   if( node != null ){
      err += "At <" + node.getNodeName();
      NamedNodeMap attr = node.getAttributes();
      for(int i=0; i<attr.getLength(); i++){
         Node nd = attr.item(i);
         err += " " +
             nd.getNodeName()  + "=\"" +
             nd.getNodeValue() + "\"" ;
      }
      err += " > : " ;
   }
   err += e;
   if( ml2xmlClassObj != null ){
  Class<?> [] argTypes = { };
  Method m = ml2xmlClassObj.getMethod( "closeFiles", argTypes );
  Object parmValues[] = new Object[0];
  m.invoke( null, parmValues );
}

   Xtpipes.logWriter.flush();
   if( exceptionErrs ) { throw new Exception( err );  }
   else {
      System.err.println( err );
      System.exit(1);
   }
}
private static void instructionErr( Node node, String e,
                                StackTraceElement[] st, int num )
                                     throws Exception {
   Xtpipes.logWriter.println(
      "--- xtpipes error " + num + " --- " + e
   );
   for(int i=st.length-1; i>=0; i-- ){
      Xtpipes.logWriter.println( st[i].toString() );
   }
   instructionErr( node, e, num );
}

   static String serialize( Node root ){
   if( root.getNodeType() == Node.TEXT_NODE) {
         return root.getNodeValue();
   }
   if( root.getNodeType() == Node.ELEMENT_NODE) {
      String ser = "";
      String tagName = root.getNodeName();
      ser += "<" + tagName;
      NamedNodeMap attributes = root.getAttributes();
for(int i = 0; i < attributes.getLength(); i++) {
   Attr attribute = (Attr) attributes.item(i);
   ser += "\n" + attribute.getName() + "=\""
               + attribute.getValue() + "\" ";
}

      ser += "\n>";
      NodeList children = root.getChildNodes();
if(children.getLength() > 0) {
   for(int i = 0; i < children.getLength(); i++) {
      ser += serialize(children.item(i));
}  }

      ser += "</" + tagName + ">";
      return ser;
   }
   if( root.getNodeType() == Node.DOCUMENT_NODE) {
      String ser = "";
      NodeList children = root.getChildNodes();
if(children.getLength() > 0) {
   for(int i = 0; i < children.getLength(); i++) {
      ser += serialize(children.item(i));
}  }

      return ser;
   }
   if( root == null ){ return "null"; }
   return "";
}

   static ArrayList<String> nsName, nsValue;
static void cleanXmlns( Node root ){
   if( root.getNodeType() == Node.ELEMENT_NODE) {
      int top = nsName.size();
      ArrayList<Attr> remove = new ArrayList<Attr>();
NamedNodeMap attributes = root.getAttributes();
for(int i = 0; i < attributes.getLength(); i++) {
   Attr attribute = (Attr) attributes.item(i);
   String name = attribute.getName();
   if( name.startsWith("xmlns") ){
     if( (name.length() == 5) || (name.charAt(5) == ':') ){
        String value = attribute.getValue();
        boolean bool = false;
for(int k=nsName.size(); k>0; ){
  k--;
  if( ((String) nsName.get(k)) . equals(name) ){
     bool = ((String) nsValue.get(k)) . equals(value);
     break;
} }

        if( bool ){ remove.add(attribute);
        } else { nsName.add(name); nsValue.add(value); }
}  } }
for(int i=remove.size(); i>0; ){
   i--;
  ((Element) root).removeAttributeNode( (Attr) remove.get(i) );
}
remove = null;

      NodeList children = root.getChildNodes();
if(children.getLength() > 0) {
   for(int i = 0; i < children.getLength(); i++) {
      cleanXmlns(children.item(i));
}  }

       for(int i=nsName.size(); i>top; ){
         i--;
         nsName.remove(i);
         nsValue.remove(i);
       }
   } else if( root.getNodeType() == Node.DOCUMENT_NODE) {
      nsName = new ArrayList<String>();
      nsValue = new ArrayList<String>();
      NodeList children = root.getChildNodes();
if(children.getLength() > 0) {
   for(int i = 0; i < children.getLength(); i++) {
      cleanXmlns(children.item(i));
}  }

      nsName = null;
      nsValue = null;
}  }

}
class XtpipesEntityResolver implements  org.xml.sax.EntityResolver {
   public InputSource resolveEntity(String publicID, String systemID)
                                                    throws SAXException {
   if( Xtpipes.trace ){
      Xtpipes.logWriter.println( "Resolving: publicID = \" " + publicID
                + "\"  systemID = \"" + systemID + "\"" );
   }
   String file = FileInfo.searchFile( systemID );
   if( file != null ){
     try{
        file = new File(file).toURI().toURL().toString();
        return new InputSource( file );
     } catch( java.net.MalformedURLException mfe){
        throw new SAXException(
          "--- xtpipes error 30 --- improper file name: " + file  );
   } }
   return null;
}

}


