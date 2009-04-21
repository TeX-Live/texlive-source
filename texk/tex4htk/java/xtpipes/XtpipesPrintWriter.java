package xtpipes;
/*
XtpipesPrintWriter.java (2009-01-27-22:19)
*/
import java.io.*;
public class XtpipesPrintWriter extends PrintWriter {
   public XtpipesPrintWriter() {
     super(System.out, true);
   }
   public XtpipesPrintWriter (PrintStream ps, boolean b){
     super(ps, b);
   }
   public XtpipesPrintWriter (OutputStream ps, boolean b){
     super(ps, b);
   }
   public XtpipesPrintWriter (FileWriter fw){
     super(fw);
   }
   public XtpipesPrintWriter (Writer wr){
     super(wr);
   }
   public void print(String str) {
     super.print( XtpipesUni.toUni(str, "") );
   }
   public void println(String str) {
     super.println( XtpipesUni.toUni(str, "") );
}  }

