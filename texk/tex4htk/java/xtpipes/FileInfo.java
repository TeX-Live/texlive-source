package xtpipes;
/*
FileInfo.java (2009-01-27-22:19)
*/
import java.io.File;
import java.io.PrintWriter;

public class FileInfo{
     static String [] classPaths = null;
static String [] scriptPaths = null;
static java.util.HashMap <String,String> registry =
                                  new java.util.HashMap <String,String>();
static String slash = System.getProperty("file.separator");
static String ii_scriptDir;
static PrintWriter log;
static boolean trace;

   public FileInfo(PrintWriter log, String iii_scriptDir, boolean trace){
      FileInfo.log = log;
      FileInfo.ii_scriptDir = iii_scriptDir;
      FileInfo.trace = trace;
      classPaths = FileInfo.getPaths( System.getProperty("java.class.path") );
if( iii_scriptDir != null ){
   scriptPaths = FileInfo.getPaths( iii_scriptDir );
}

      
   }
   public static String searchFile( String file ){
   String key = ((ii_scriptDir == null)? "" : ii_scriptDir )
                + "!" + file;
   String result = (String) registry.get( key );
   if( result == null ){
      for(int i=0; i<2; i++){
         if( trace ){
            log.println( "Searching: " + file );
         }
         if( (new File(file)).exists() ){
            result = ( file.indexOf(slash) == -1 )?
                         (System.getProperty("user.dir") + slash + file)
                        :
                         file;
         }
         else {
            if( ii_scriptDir != null ){
               int k = scriptPaths.length;
while( k>0 ){
  k--;
  if( trace ){
    log.println( "Searching: " + file
                   + ", recursively in directory: " + scriptPaths[k] );
  }
  result = searchDirectory( new File(scriptPaths[k]), file);
  if( result != null ){ break; }
}
String s = ii_scriptDir + file;
if( (new File( s )).exists() ){ result = s; }

            }
            if( result == null ){
               int k = classPaths.length;
String toFile = "xtpipes" + slash + "lib" + slash + file;
while( k>0 ){
  k--;
  String s =  classPaths[k] + toFile;
  if( trace ){ log.println( "Searching: " + s ); }
  if( new File(s).exists() ){ result = s; break; }
}

         }  }
         if( result != null ){ break; }
         file =  new File(file).getName();
      }
      if( result != null ){
        result = FileInfo.cleanPath(result);
        registry.put(key, result);
      }
   }
   if( trace ){
      if( result == null ){
         log.println(
            "Directory paths from xtpipes command line option -i: "
                                            + ii_scriptDir );
      } else { log.println( "Found: " + result + "\n" ); }
      log.flush();
   }
   return result;
}

   static String [] getPaths( String dirs ){
      String [] paths = null;
   paths = dirs.split( System.getProperty("path.separator") );
   int k = paths.length;
   while( k>0 ){
      k--;
      paths[k] = cleanPath( paths[k] );

      int len = paths[k].length();
      if( (len>1) && (paths[k].lastIndexOf(slash + ".") == (len-1)) ){
         paths[k]  = paths[k].substring(0,len-1);
      } else if( (len>0) && ((len-1) != paths[k].lastIndexOf( slash )) ){
         paths[k] += slash;
   }  }
   return paths;
}

   public static String cleanPath( String path ){
     String slash = System.getProperty("file.separator");
     String userDir = System.getProperty( "user.dir" );
    if( (path.length() > 0) && (path.charAt(0) == '~') ){
    if( (path.length() == 1) || (path.charAt(1) != '~') ){
      path = System.getProperty( "user.home" )
                             + path.substring(1);
  } }

    if( path.startsWith("..") ){
     path = userDir.substring(0,
               Math.max(0,Math.max(
                 userDir.lastIndexOf("/")
                 ,
                 userDir.lastIndexOf("\\")
               )))
            + path.substring(2);
  }
  if( path.startsWith(".") ){
     path = userDir + slash + path.substring(1);
  }

    int i;
  while(
    ((i=path.indexOf("/..")) != -1)
    ||
    ((i=path.indexOf("\\..")) != -1)
  ){
    String s = path.substring(0,i);
    int j = Math.max(s.lastIndexOf("/"), s.lastIndexOf("\\"));
    path = path.substring(0,j) + path.substring(i+3);
  }
  while(
    ((i=path.indexOf("/.")) != -1)
    ||
    ((i=path.indexOf("\\.")) != -1)
  ){
    String s = path.substring(0,i);
    int j = Math.max(s.indexOf("/"), s.indexOf("\\"));
    path = path.substring(0,j) + path.substring(i+2);
  }

  return path;
}

   static String searchDirectory(File dir, String file) {
    String result = null;
    if( dir.isDirectory() ){
       String [] children = dir.list();
       for (int i=0; i<children.length; i++) {
          result = searchDirectory( new File(dir,children[i]), file);
          if( result != null ) { break; }
       }
    } else {
       String s = dir.toString();
       if( s.equals(file) || s.endsWith(slash + file) ){
           result = s;
        }
     }
     return result;
}

}

