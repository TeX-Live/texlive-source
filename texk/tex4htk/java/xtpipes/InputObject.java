package xtpipes;
/*
InputObject.java (2009-01-27-22:19)
*/
import java.io.PrintWriter;
import java.net.URL;
import java.net.URLConnection;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.InputStream;


public class InputObject{
      InputStream inputStream = null;
URLConnection connection = null;
String filename = null;
static PrintWriter log;
String dtdRoot = null,
      publicId = null,
      systemId = null,
       xtpipes = null,
           url = null,
      metaType = null,
   contentType = null,
          root = null;

   public InputObject( String filename, PrintWriter log ){
   InputObject.log = log;
   filename = filename.trim();
   try{
      inputStream = getInputStream(filename);
   } catch (Exception exp0){
      if( !filename.startsWith( "http://" ) ){
         try{
            String name = "http://" + filename;
            inputStream = getInputStream( name );
            filename = name;
         } catch (Exception exp1){
            try{
               String name = FileInfo.cleanPath(filename);
               inputStream = getInputStream( name );
               filename = name;
            } catch (Exception exp2){ inputStream = null; }
   }  }  }
   this.filename = filename;
}
public InputObject( byte [] bytes, PrintWriter log ){
   InputObject.log = log;
   inputStream = new ByteArrayInputStream( bytes );
}

   private java.io.InputStream getInputStream(
                                      String filename )
                                   throws java.io.IOException{
   if( filename == null ){ return null; }
   URL url;
   java.io.InputStream inputStream = null;
//   String loadingError = "Failed to get requested file.";
   try {
      url = new File(filename).toURI().toURL();
      inputStream =  getInputStream( url );
   } catch (Exception ie) {
       try {
           url = new URL(filename);
           inputStream =  getInputStream( url );
       } catch (java.io.FileNotFoundException ife) {
           throw new java.io.IOException(
              "File not found: " + filename);
       } catch (Exception ife) {
           throw new java.io.IOException(ife + "\n" + ie);
   }   }
   return inputStream;
}

   private java.io.InputStream getInputStream( URL url )
                            throws java.io.FileNotFoundException,
                                             java.io.IOException {
   java.io.InputStream inputStream = null;
   String errMssg = "";
   try{
      connection = null;
      connection = url.openConnection();
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
      inputStream = connection.getInputStream();
   } catch(java.io.FileNotFoundException ve){
      errMssg = "File not found: " + url;
      throw new java.io.FileNotFoundException(
                 "--- Ml2xml input error --- " + errMssg );
   } catch (javax.net.ssl.SSLHandshakeException ve){
      errMssg = "SSL Handshake Exception: " + ve.getMessage();
      throw new javax.net.ssl.SSLHandshakeException(
                 "--- Ml2xml input error --- " + errMssg );
   } catch (java.net.UnknownHostException ve){
      errMssg = "Unknown Host Exception: " + ve.getMessage();
      throw new java.net.UnknownHostException(
                   "--- Ml2xml input error --- " + errMssg );
   }
   return inputStream;
}

   public void buildProfile( boolean trace ){
   if( trace ){
      log.println(
         "xtpipes (2009-01-27-22:19)"
         + "\n   java.version: "    + System.getProperty("java.version")
         + "\n   java.class.path: " + System.getProperty("java.class.path")
         + "\n   os.name: "         + System.getProperty("os.name")
         + "\n   user.home: "       + System.getProperty("user.home")
         + "\n   user.dir: "        + System.getProperty("user.dir")
           );
   }
   if( connection != null ){
     contentType = connection . getContentType();
url = connection . getURL() . toString();

   }
   int max = 8192;
int buffSize = 4096;
byte [] buff = new byte [ buffSize ];
int m = 0;
int length = 0;
int ch;
int type = 0
;
String token = null;
while( m < max ){
   try{
      int k = Math.min( max - m, buffSize );
      length = inputStream.read( buff, 0, k );
      if( length == -1 ){ break; }
      if( length == 0  ){ continue; }
   } catch (java.io.IOException e){
      System.err.println( "--- xtpipes error --- : " + e );
      break;
   }
   for(int i = 0 ; i < length; i++ ){
     switch( ch = buff[i] ){
   case  '<':  token = "";
               type = 1
;
               break;
   case  '>':  if( token != null ){
                  token = token . replaceAll( "\\s+", " ");
                  if( type == 9
 ){
   if( xtpipes == null ){
      int n = token.length();
      if( (n > 1) && (token.charAt( n - 1 ) == '?')
                  && (token.startsWith("xtpipes") ) ){
         String s = token . substring(7,n-1) . replaceAll( "\\s+", "");
         n = s.length();
         if( (n>6) && (s.startsWith("file="))
                   && (s.charAt(5) == s.charAt(n-1)) ){
           xtpipes = s.substring(6,n-1);
   }  }  }
} else if( type == 11
 ){
   if( metaType == null ){
      token = token . replaceAll( "\\s+", "");
      int k = token.indexOf("http-equiv");
      int n = token.indexOf("content");
      if( (k != -1) && (n != -1) ){
         if( token.length() > (Math.max(k,n)+3) ){
            if( token.substring(k+12).startsWith("Content-Type") ){
               token = token.substring(n+9);
               n = token.indexOf(";");
               if( n !=-1 ){ metaType = token.substring(0,n); }
   }  }  }  }
} else if( (type == 2
) && (root == null) ){
   root = token;
}

                  token = null;
               }
               break;
   case '\n':
   case  ' ':  if( token != null ){
                  if( type == 2
 ){
   if( token.equals("meta") ){
      if( metaType == null ){
         type = 11
;
         token = " ";
      } else {
         token = null;
      }
   } else {
      if( root == null ){
        root = token;
      }
      token = null;
   }
} else if( type == 4
 ){
   if( token.equals("DOCTYPE") ){
      type = 5
;
      token = " ";
   } else { token = null; }
} else if( type == 5
 ){
   if( !token.trim().equals("") ){
      dtdRoot = token.trim();
      token = " ";
      type = 6
;
   } else { token = null; }
} else if( type == 6
 ){
   if( !token.trim().equals("") ){
      token = token.trim();
      if( token.equals("PUBLIC") ){
         type = 7
;
         token = "";
      } else if( token.equals("SYSTEM") ){
         type = 8
;
         token = "";
      } else { token = null; }
   }
} else { token += ' '; }

               }
               break;
   case  '"':
   case '\'':  if( token == null ){ break; }
               if( !token.trim().equals("") ){
   if( token.trim().charAt(0) == ch ){
     if( type == 7
 ){
        publicId = token.trim().substring(1);
        type = 8
;
        token = "";
        break;
     }
     else if( type == 8
 ){
        systemId = token.trim().substring(1);
        token = null;
        break;
     }
} }

   default:    if( token != null ){
                  if( type == 3
 ){
                     if( ch == 'D' ){
                        type = 4
;
                        token += (char) ch;
                     } else { token = null; type = 0
; }
                  }
                  else
                  if( token.equals("") && (type == 1
) ){
                     switch( ch ){
   case '!': type = 3
;
             break;
   case '?': type = 9
 ;
             break;
   default:  if( Character.isLetter(ch)
                 && ((root == null) || (metaType == null)) ){
                type = 2
;
                token += (char) ch;
             } else { token = null; }
}

                  } else { token += (char) ch; }
}              }

     m++;
}  }

   if( trace ){
      log.println(
           " url = "           + url
         + "\n contentType = " + contentType
         + "\n publicId = "    + publicId
         + "\n systemId = "    + systemId
         + "\n xtpipes = "     + xtpipes
         + "\n root = "        + root
         + "\n dtdRoot = "     + dtdRoot
      );
}  }

   public InputStream getInputStream(){ return inputStream; }
   public String getFilename(){
      return (url == null)?
         ( (connection == null)? filename
                               :
                                 connection . getURL() . toString()
         )
       : url;
   }
   public String getContentType(){ return contentType; }
   public String getMetaType(){ return metaType; }
   public String getPublicId(){ return publicId; }
   public String getSystemId(){ return systemId; }
   public String getXtpipes(){ return xtpipes; }
   public String getRoot(){ return root; }
   public String getDtdRoot(){ return dtdRoot; }
}

