package tex4ht;
import org.w3c.dom.*;
public class HtSpk {
  public static void fracLevel(Node dom) {
   setFracLevel(dom.getFirstChild(), 0);
}
private static int setFracLevel(Node node, int cont) {
  int level = 0;
  String clName = null;
  if (node.hasChildNodes()) {
    if (node.hasAttributes()) {
      Node cl = node.getAttributes().getNamedItem("class");
      if (cl != null) { clName = cl.getNodeValue(); }
    }
    NodeList children = node.getChildNodes();
int max = 0;
for (int i = 0; i < children.getLength(); i++) {
   Node child = children.item(i);
   if (child.getNodeType() == Node.ELEMENT_NODE) {
      int d = setFracLevel(child,
           (clName != null) &&
            clName.equals("continuous-mfrac")?
            2 :
            ((clName != null) &&
              clName.equals("continuous-mfrac")?
                (cont-1) : cont)
        );
      if (d > max) { max = d; }
}  }
level += max;

    if( clName != null ){
      if( clName.equals("msub") || clName.equals("msup") ||
    clName.equals("msubsup")
) {
   return 0;
}

      if( clName.equals("continuous-mfrac") ) {
   if( cont > 0 ){
     Node child = node.getLastChild();
if (child.getNodeType() == Node.ELEMENT_NODE) {
   Node cls = child.getAttributes() .getNamedItem("class");
   if (cls != null) {
      String clsName = cls.getNodeValue();
      if ( clsName.equals("begin-end")) {
         node.removeChild( child  );
}  }  }

   } else { for (int i = 0; i < children.getLength(); i++) {
  Node child = children.item(i);
  if (child.getNodeType() == Node.ELEMENT_NODE) {
    Node cls = child.getAttributes()
                   .getNamedItem("class");
    if (cls != null) {
       String clsName = cls.getNodeValue();
       if ( clsName.equals("begin-end")) {
          child = child.getFirstChild();
          String s = child.getNodeValue();
          s = s.replaceFirst("begin", "begin continued");
          s = s.replaceFirst("end", "end continued");
          ((org.w3c.dom.Text) child).setData(s);
} } } }
 } 
   return 0;
}

      if (clName.equals("mfrac")) {
        if( cont > 0 ){
          Node child = node.getLastChild();
if (child.getNodeType() == Node.ELEMENT_NODE) {
   Node cls = child.getAttributes() .getNamedItem("class");
   if (cls != null) {
      String clsName = cls.getNodeValue();
      if ( clsName.equals("begin-end")) {
         node.removeChild( child  );
}  }  }

        } else if( level > 0 ){
          for (int i = 0; i < children.getLength(); i++) {
  Node child = children.item(i);
  if (child.getNodeType() == Node.ELEMENT_NODE) {
    Node cls = child.getAttributes()
                    .getNamedItem("class");
    if (cls != null) {
      String clsName = cls.getNodeValue();
      if (clsName.equals("begin-end")) {
         child = child.getFirstChild();
         String s = child.getNodeValue();
         String bg = "", ov = "", en = "";
         for(int j=0; j<level; j++){
            bg += " begin "; ov += " over "; en += " end ";
         }
         s = s.replaceFirst("begin", bg + "begin");
         s = s.replaceFirst("over", ov + "over");
         s = s.replaceFirst("end", en + "end");
         ((org.w3c.dom.Text) child).setData(s);
} } } }

        }
        level++;
  } } }
  return level;
}

  public static void scriptLevel(Node dom) {
   setScriptLevel(dom.getFirstChild(), "");
}
private static void setScriptLevel(Node node, String prefix) {
  String clName = null;
  if (node.hasChildNodes()) {
    if (node.hasAttributes()) {
      Node cl = node.getAttributes().getNamedItem("class");
      if (cl != null) {
        clName = cl.getNodeValue();
        if( clName.equals("mrow-sub")
            ||
            clName.equals("mrow-super")
        ){
          if( !prefix.equals("") ){
  Node child = node.getFirstChild();
if( (child.getNodeType() == Node.ELEMENT_NODE)
    &&
    child.hasAttributes()
){
   Node cls = child.getAttributes().getNamedItem("class");
   if (cls != null) {
      String clsName = cls.getNodeValue();
      if ( clsName.equals("begin-script")
           ||
           clsName.equals("mid-script")
      ) {
         child = child.getFirstChild();
         String s = child.getNodeValue();
         ((org.w3c.dom.Text) child).setData( prefix + s );
}  }  }

}
if( clName.equals( "mrow-sub" ) ){ prefix += " sub "; }
else
if( clName.equals( "mrow-super" ) ){ prefix += " super "; }

        } else if(
           clName.equals("msqrt")
        ){  prefix = ""; }
    } }
    NodeList children = node.getChildNodes();
for (int i = 0; i < children.getLength(); i++) {
   Node child = children.item(i);
   if (child.getNodeType() == Node.ELEMENT_NODE) {
      setScriptLevel(child, prefix);
}  }

} }

  public static void rootLevel(Node dom) {
   setRootLevel(dom.getFirstChild());
}
private static int setRootLevel( Node node ){
  int level = 0;
  String clName = null;
  if (node.hasChildNodes()) {
    if (node.hasAttributes()) {
      Node cl = node.getAttributes().getNamedItem("class");
      if (cl != null) { clName = cl.getNodeValue(); }
    }
    NodeList children = node.getChildNodes();
int max = 0;
for (int i = 0; i < children.getLength(); i++) {
  Node child = children.item(i);
  if (child.getNodeType() == Node.ELEMENT_NODE) {
    int d = setRootLevel(child);
    if( d > max ){ max = d; }
} }
level += max;

    if( clName != null ){
      if( clName.equals("msub") || clName.equals("msup") ||
    clName.equals("msubsup")
) {
   return 0;
}

      if( clName.equals("msqrt") || clName.equals("root") ){
        for (int i = 0; i < children.getLength(); i++) {
  Node child = children.item(i);
  if (child.getNodeType() == Node.ELEMENT_NODE) {
    Node cls = child.getAttributes()
                    .getNamedItem("class");
    if (cls != null) {
      String clsName = cls.getNodeValue();
      if( clsName.equals("begin-root")
          || clsName.equals("mid-root")
          || clsName.equals("end-root")
      ){
         child = child.getFirstChild();
         String s = child.getNodeValue();
         String nested = "";
         for(int j=0; j<level; j++){
            nested += " nested ";
         }
         ((org.w3c.dom.Text) child).setData( nested + s);
} } } }

        level++;
  } } }
  return level;
}

}

