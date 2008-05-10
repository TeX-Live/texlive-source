package tex4ht;
import org.w3c.dom.*;
public class HtJsml {
  private static void insertLevelPrefix(Node node, int level){
   if( level == 0 ){ return; }
   if (node.getNodeType() == Node.ELEMENT_NODE) {
     if( node.getNodeName().equals( "level" ) ){
        Node attr = node.getAttributes().getNamedItem("prefix");
if( attr != null ){
  String prefix = attr.getNodeValue();
  String  s = "";
  for(int j=0; j<level; j++){
     s += prefix + " ";
  }
  ((org.w3c.dom.Element) node).setAttribute( "depth", ""+level);
  Node child = node.getFirstChild();
  if( child != null ){
     node.insertBefore( dom.createTextNode(s), child );
} }

     } else {
        NodeList children = node.getChildNodes();
        for (int i = 0; i < children.getLength(); i++) {
           Node child = children.item(i);
           insertLevelPrefix(child, level);
}  }  } }
private static void setContinuedNote(Node node){
   if (node.getNodeType() == Node.ELEMENT_NODE) {
     if( node.getNodeName().equals( "level" ) ){
        Node attr = node.getAttributes().getNamedItem("continued");
if (attr != null) {
  node = node.getFirstChild();
  if( node != null ){
     ((org.w3c.dom.Text) node).setData( attr.getNodeValue() );
} }

     } else {
        NodeList children = node.getChildNodes();
        for (int i = 0; i < children.getLength(); i++) {
           Node child = children.item(i);
           setContinuedNote(child);
}  }  } }

  private static Document dom;
public static void mnGroup(Node d) {
  dom = (Document) d;
  setMnGroup(dom.getFirstChild());
}
private static void setMnGroup(Node node) {
  if( node.getNodeName().equals( "mn-group" ) ){
         boolean bool = false;
    NodeList children = node.getChildNodes();
int n = children.getLength();
char [] digit = new char[n];
for (int i = 0; i < n; i++) {
  Node child = children.item(i).getFirstChild();
  if( child == null ){
     digit[i] = 'x';
  } else if( child.getNodeType() != Node.TEXT_NODE ){
     digit[i] = 'x';
  } else {
     String s = child.getNodeValue();
     if( s.length() != 1 ){
       digit[i] = 'x';
     } else {
       char ch = s.charAt(0);
       if(      (ch >= '0') && (ch <= '9') ){
          digit[i] = '0'; bool = true;
       } else if( (ch == '.') || (ch == ',') ){ digit[i] = ch; }
       else                                 { digit[i] = 'x';}
} }  }

    if( bool ){
      bool = false;
for (int i = 0; i < n; i++) {
  if( digit[i] == '.' ){
     for (; i < n; i++) {
       if( digit[i] == ',' ){
          bool = true; break;
     } }
     break;
} }

      if( !bool ){
  for (int i = 0; i < n; i++) {
    if( digit[i] == ',' ){
       if( ( ((i+3) >= n)
             || (digit[i+1] != '0')
             || (digit[i+2] != '0')
             || (digit[i+3] != '0')
           )
           ||
           (
             ((i+4) < n) && (digit[i+4] == '0')
           )
           ||
           (
             (i>3) && (digit[i-4] == '0')
           )
       ){  bool = true; break;
       } else { i += 3; }
} } }

      if( bool ){
  for (int i = 0; i < n; i++) {
    if( digit[i] == ',' ){ digit[i] = 'x'; }
} }

      bool = false;
for (int i = 0; i < n; i++) {
  if( (digit[i] == 'x')
      || (digit[i] == ',') ){ bool = false; }
  else if( digit[i] == '.' ){
    if( bool ){
       for (int j = 0; j < n; j++) {
         if( (digit[j] == '.') || (digit[j] == ',') ){
            digit[j] = 'x';
       } }
       break;
    }
    bool = true;
} }

      if( digit[n-1] == '.' ){ digit[n-1] = 'x'; }

    }
    Node parent = node.getParentNode();
Element g = dom.createElement( "mn-group-s" );
Element cur = dom.createElement( "mn-group" );
for(int i=0;  i<n; i++ ){
   Node child = node.getFirstChild();
   node.removeChild( child );
   if( digit[i] == 'x' ){
      if( cur.hasChildNodes() ){ g.appendChild( cur ); }
      g.appendChild( child );
      cur = dom.createElement( "mn-group" );
   } else {
      cur.appendChild( child );
}  }
if( cur.hasChildNodes() ){ g.appendChild( cur ); }
parent.replaceChild( g, node );

  } else if (node.hasChildNodes()) {
     NodeList children = node.getChildNodes();
     for (int i = 0; i < children.getLength(); i++) {
        Node child = children.item(i);
        if (child.getNodeType() == Node.ELEMENT_NODE) {
           setMnGroup(child);
} }  }  }

  public static void fracLevel(Node d) {
   dom = (Document) d;
   setFracLevel(dom.getFirstChild(), 0);
}
private static int setFracLevel(Node node, int cont) {
  int level = 0;
  if (node.hasChildNodes()) {
        String ndName = node.getNodeName();
    int prevCont = cont;
    String clValue = null;
    if (ndName.equals("mfrac")) {
      if (node.hasAttributes()) {
   Node cl = node.getAttributes().getNamedItem("class");
   if (cl != null) { clValue = cl.getNodeValue(); }
}

      if( (clValue != null)
    && clValue.equals("continued-mfrac") ) {
  cont = 2;
} else { cont--; }

    }
    NodeList children = node.getChildNodes();
int max = 0;
for (int i = 0; i < children.getLength(); i++) {
   Node child = children.item(i);
   if (child.getNodeType() == Node.ELEMENT_NODE) {
      int d = setFracLevel(child,
                           cont
);
      if (d > max) { max = d; }
}  }
level += max;

    if(    ndName.equals("msub")
    || ndName.equals("msup")
    || ndName.equals("msubsup")
) {
   return 0;
}

    if (ndName.equals("mfrac")) {
      if( (clValue != null) && clValue.equals("word-frac")
) {
   return 0;
}

      if( cont > 0 ){
          if ( prevCont == 0 ){
             Node child = node.getFirstChild();
if (child.getNodeType() == Node.ELEMENT_NODE) {
   Node cls = child.getAttributes() .getNamedItem("class");
   if (cls != null) {
      String clsName = cls.getNodeValue();
      if ( clsName.equals("begin-end")) {
         setContinuedNote(child);
}  }  }

          }
          Node child = node.getLastChild();
if (child.getNodeType() == Node.ELEMENT_NODE) {
   Node cls = child.getAttributes() .getNamedItem("class");
   if (cls != null) {
      String clsName = cls.getNodeValue();
      if ( clsName.equals("begin-end")) {
         node.removeChild( child  );
}  }  }

          level = 0;
      } else if ( prevCont > 0 ){
          Node child = node.getLastChild();
if (child.getNodeType() == Node.ELEMENT_NODE) {
   Node cls = child.getAttributes() .getNamedItem("class");
   if (cls != null) {
      String clsName = cls.getNodeValue();
      if ( clsName.equals("begin-end")) {
         setContinuedNote(child);
}  }  }

          level = 0;
      } else
        if( level > 0 ){
          for (int i = 0; i < children.getLength(); i++) {
   Node child = children.item(i);
   if (child.getNodeType() == Node.ELEMENT_NODE) {
      Node cls = child.getAttributes()
                      .getNamedItem("class");
      if (cls != null) {
         String clsName = cls.getNodeValue();
         if (clsName.equals("begin-end")) {
            insertLevelPrefix(child, level);
}  }  }  }

        }
        level++;
  } }
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
if( clName.equals( "mrow-sub" ) ){
    prefix += " sub ";
} else if( clName.equals( "mrow-super" ) ){
    prefix += " super ";
}

    } } }
    String ndName = node.getNodeName();
    if(
      ndName.equals("msqrt")
      ||
      ndName.equals("mroot")
    ){  prefix = ""; }
    NodeList children = node.getChildNodes();
for (int i = 0; i < children.getLength(); i++) {
   Node child = children.item(i);
   if (child.getNodeType() == Node.ELEMENT_NODE) {
      setScriptLevel(child, prefix);
}  }

} }

  public static void rootLevel(Node d) {
   dom = (Document) d;
   setRootLevel(d.getFirstChild());
}
private static int setRootLevel( Node node ){
  int level = 0;
//  String clName = null;
  if (node.hasChildNodes()) {
    NodeList children = node.getChildNodes();
int max = 0;
for (int i = 0; i < children.getLength(); i++) {
  Node child = children.item(i);
  if (child.getNodeType() == Node.ELEMENT_NODE) {
    int d = setRootLevel(child);
    if( d > max ){ max = d; }
} }
level += max;

    String ndName =  node.getNodeName();
    if( ndName.equals("msub") || ndName.equals("msup") ||
    ndName.equals("msubsup")
) {
   return 0;
}

    if( ndName.equals("msqrt") || ndName.equals("mroot") ){
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
         insertLevelPrefix(child, level);
} } } }

        level++;
  } }
  return level;
}

}

