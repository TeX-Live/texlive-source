package tex4ht;
/* XhtmmlUtilities.java (2026-05-11-09:59), generated from tex4ht-xhtmml-xtpipes.tex
   Copyright (C) 2009-2010 TeX Users Group
   Copyright (C) 2008-2009 Eitan M. Gurari
%
% This work may be distributed and/or modified under the
% conditions of the LaTeX Project Public License, either
% version 1.3c of this license or (at your option) any
% later version. The latest version of this license is in
%   https://www.latex-project.org/lppl.txt
% and version 1.3c or later is part of all distributions
% of LaTeX version 2005/12/01 or later.
%
% This work has the LPPL maintenance status "maintained".
%
% The Current Maintainer of this work
% is the TeX4ht Project <https://tug.org/tex4ht>.
%
% If you modify this program, changing the
% version identification would be appreciated. */

import org.w3c.dom.*;
public class XhtmmlUtilities {
  public static void p(Node dom) {
   Node pNode = dom.getFirstChild();
   if( pNode.hasChildNodes() ){
      boolean drop = true;
      Node child = pNode.getFirstChild();
      while( child != null ){
         short type = child.getNodeType();
         if(
             (type == Node.ELEMENT_NODE)
           ||
             (type == Node.CDATA_SECTION_NODE)
           ||
             (type == Node.TEXT_NODE)
           &&
             !((Text) child).getWholeText().trim().equals("")
         ){
            drop = false; break;
         }
         child = child.getNextSibling();
      }
      if( drop ){
         dom.removeChild( pNode );
      }
   } else {
      dom.removeChild( pNode );
   }
}

  public static void li(Node dom) {
   Node liNode = dom.getFirstChild();
   Node liChild;
   boolean hasBlock = false, hasInline = false;
   liChild = liNode.getFirstChild();
while( liChild != null ){
   short type = liChild.getNodeType();
   if( (type == Node.ELEMENT_NODE)
&&
(
   liChild.getNodeName().equals("p")
   ||
   liChild.getNodeName().equals("ol")
   ||
   liChild.getNodeName().equals("ul")
   ||
   liChild.getNodeName().equals("div")
   ||
   liChild.getNodeName().equals("table")
)
 ){  hasBlock = true; }
   else if(  type == Node.TEXT_NODE ){
      if( !((Text) liChild).getWholeText().trim().equals("") ){
         hasInline = true;
   }  }
   else if(
       (type != Node.COMMENT_NODE)
       &&
       (type != Node.PROCESSING_INSTRUCTION_NODE )
   ){
      hasInline = true;
   }
   liChild = liChild.getNextSibling();
}

   if( hasBlock && hasInline ){
      liChild = liNode.getLastChild();
Element g = ((Document) dom).createElement("p");
g.setAttribute("class", "noindent");
while( liChild != null ){
   short type = liChild.getNodeType();
   if( (type == Node.ELEMENT_NODE)
&&
(
   liChild.getNodeName().equals("p")
   ||
   liChild.getNodeName().equals("ol")
   ||
   liChild.getNodeName().equals("ul")
   ||
   liChild.getNodeName().equals("div")
   ||
   liChild.getNodeName().equals("table")
)
 ){
      if( g.hasChildNodes() ){
   Node nextChild = liChild.getNextSibling();
   if( nextChild == null  ){
      liNode.appendChild( g );
   } else {
      liNode.insertBefore( g, nextChild );
   }
   g = ((Document) dom).createElement("p");
   g.setAttribute("class", "noindent");
}

      liChild = liChild.getPreviousSibling();
   } else {
      Node nextChild = liChild;
      liChild = liChild.getPreviousSibling();
      nextChild = liNode.removeChild(nextChild);
type = nextChild.getNodeType();
if(
     (type != Node.COMMENT_NODE)
   &&
   (
     (type != Node.TEXT_NODE)
   ||
     !((Text) nextChild).getWholeText().trim().equals("")
   )
){
   if( g.hasChildNodes() ){
      g.insertBefore( nextChild, g.getFirstChild() );
   } else {
      g.appendChild( nextChild );
} }

}  }

      if( liNode.hasChildNodes() ){
  liNode.insertBefore( g, liNode.getFirstChild() );
} else {
  liNode.appendChild( g );
}

   }
}

  public static void table(Node dom) {
   Node tableNode = dom.getFirstChild();
   if( tableNode.hasChildNodes() ){
      Node trChild = tableNode.getLastChild();
      while( (trChild != null)
             &&
             !trChild.getNodeName().equals("tr") ){
         trChild = trChild.getPreviousSibling();
      }
      if( (trChild != null) && trChild.hasChildNodes() ){
         Node tdChild = trChild.getLastChild();
         while( (tdChild != null)
                &&
                !tdChild.getNodeName().equals("td") ){
            tdChild = tdChild.getPreviousSibling();
         }
         if( !tdChild.hasChildNodes() ){
            tableNode.removeChild( trChild );
         } else {
            Node child = tdChild.getFirstChild();
            if(
               (child.getNodeType() == Node.TEXT_NODE)
               &&
               ((Text) child).getWholeText().trim().equals("")
               &&
               (child.getNextSibling() == null)
            ){
               tableNode.removeChild( trChild );
}  }  }  } }

  public static void mrow(Node dom) {
   barsIntoFenced( dom );
   digitsIntoNumbers( dom );
}

  public static void math(Node dom) {
   barsIntoFenced( dom );
   digitsIntoNumbers( dom );
}

  static void barsIntoFenced(Node dom) {
   Node rightBarNode = null;
   Node mrowNode = dom.getFirstChild();
   Node mrowChild = mrowNode.getLastChild();
   while( mrowChild != null ){
      if(
           mrowChild.getNodeName().equals("msub")
           ||
           mrowChild.getNodeName().equals("msup")
           ||
           mrowChild.getNodeName().equals("msubsup")
      ){
         Node firstChild = mrowChild.getFirstChild();
         if(
             (firstChild.getChildNodes().getLength() == 1)
             &&
             firstChild.getTextContent().equals( "|" )
         ){
            rightBarNode = mrowChild;
      }  }
      else
      if(
         (rightBarNode != null)
         &&
         mrowChild.getNodeName().equals("mo")
         &&
         (mrowChild.getChildNodes().getLength() == 1)
         &&
         mrowChild.getTextContent().equals( "|" )
      ){
        if(mrowChild.getNextSibling() != rightBarNode){
            rightBarNode.removeChild( rightBarNode.getFirstChild() );
Node sub = rightBarNode.getFirstChild();
Node mfenced = ((Document) dom).createElement( "mfenced" );
rightBarNode.insertBefore( mfenced, sub );
((Element) mfenced).setAttribute("open","|");
((Element) mfenced).setAttribute("close","|");
((Element) mfenced).setAttribute("separator","");
Node node = mrowChild.getNextSibling();
while( node != rightBarNode ){
   Node next = node.getNextSibling();
   mrowNode.removeChild( node );
   mfenced.appendChild( node );
   node = next;
}
mrowNode.removeChild( mrowChild );
mrowChild = mrowNode.getLastChild();

        }
        rightBarNode = null;
      }
      mrowChild = mrowChild.getPreviousSibling();
}  }

  static void digitsIntoNumbers(Node dom){
   Node mrowNode = dom.getFirstChild();
   Node mrowChild = mrowNode.getFirstChild();
   short state = 0
;
   Node fromNode = null,
          toNode = null;
   while( mrowChild != null ){
      switch( state ){
         case 0
:
              if( mrowChild.getNodeName().equals("mn") ){
                 state = 1
;
                 fromNode = mrowChild;
                 toNode = mrowChild;
              }
            break;
         case 1
:
              boolean bool = true;
              if( mrowChild.getNodeName().equals("mn") ){
                 toNode = mrowChild;
                 bool = ( mrowChild.getNextSibling() == null );
              }
              else
              { String s = null;
Node base;
if(
    ( mrowChild.getNodeName().equals("msub")
      ||
      mrowChild.getNodeName().equals("msup")
      ||
      mrowChild.getNodeName().equals("msubsup")
    )
    &&
    ((s = (base = mrowChild.getFirstChild())
          . getTextContent()).length() == 1 )
    &&
    s.replaceAll("[0-9]","").equals("")
){
   for( Node node = fromNode; ; node = node.getNextSibling()){
  String str = node.getTextContent();
  if( str.length() > 1 ){ bool = false; break; }
  if( !str.replaceAll("[0-9]","").equals("") ){ bool = false; break; }
  if( node == toNode ){ break; }
}

   Node next;
String num = "";
Node node = fromNode;
while( node != toNode ){
  num += node.getTextContent();
  next = node.getNextSibling();
  mrowNode.removeChild( node );
  node = next;
}
num += node.getTextContent();

   mrowNode.removeChild( node );
   num += s;
   Node mn = ((Document) dom).createElement("mn");
   mn.appendChild( ((Document) dom).createTextNode(num) );
   mrowChild.replaceChild( mn, base);

   bool = false;
   state = 0
;
   fromNode = null;
   toNode = null;
}
 }
              if( bool )
              { for( Node node = fromNode; ; node = node.getNextSibling()){
  String str = node.getTextContent();
  if( str.length() > 1 ){ bool = false; break; }
  if( !str.replaceAll("[0-9]","").equals("") ){ bool = false; break; }
  if( node == toNode ){ break; }
}

                if( bool ){
                   Node next;
String num = "";
Node node = fromNode;
while( node != toNode ){
  num += node.getTextContent();
  next = node.getNextSibling();
  mrowNode.removeChild( node );
  node = next;
}
num += node.getTextContent();

                   node.replaceChild( ((Document) dom).createTextNode(num),
                                      node.getFirstChild()
                                    );
                }
                state = 0
;
                fromNode = null;
                toNode = null;
      }       }
      mrowChild = mrowChild.getNextSibling();
}  }

}

