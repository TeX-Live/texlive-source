package tex4ht;
/**********************************************************/ 
/* OoUtilities.java                      2009-03-11-03:09 */
/* Copyright (C) 2006--2009    Eitan M. Gurari            */
/*                                                        */
/* This work may be distributed and/or modified under the */
/* conditions of the LaTeX Project Public License, either */
/* version 1.3 of this license or (at your option) any    */
/* later version. The latest version of this license is   */
/* in                                                     */
/*   http://www.latex-project.org/lppl.txt                */
/* and version 1.3 or later is part of all distributions  */
/* of LaTeX version 2003/12/01 or later.                  */
/*                                                        */
/* This work has the LPPL maintenance status "maintained".*/
/*                                                        */
/* This Current Maintainer of this work                   */
/* is Eitan M. Gurari.                                    */
/*                                                        */
/*                             gurari@cse.ohio-state.edu  */
/*                 http://www.cse.ohio-state.edu/~gurari  */
/**********************************************************/


import org.w3c.dom.*;
public class OoUtilities {
  public static void mtable(Node dom) {
      Node mtr, mtd, d;
      int cols = 0;
  Node node = dom.getFirstChild();
   if (node.hasChildNodes()) {
   mtr = node.getLastChild();
   while( mtr != null){
      if( mtr.getNodeType() == Node.ELEMENT_NODE ){
         if (mtr.hasChildNodes()) {
           mtd = mtr.getLastChild();
           int count = 0;
           while( mtd != null){
             if( mtd.getNodeType() == Node.ELEMENT_NODE ){
                count++;
                d = mtd.getFirstChild();
                if( d != null ){
                   boolean remove = true;
for(Node i=d; i!=null; i=i.getNextSibling() ){
   if( (i.getNodeType() == Node.ELEMENT_NODE)
       && !i.getNodeName().equals("math:mspace") ) {
      remove = false; break;
}  }
if( remove ){
   while( d != null ){
     mtd.removeChild(d);
     d = mtd.getFirstChild();
}  }

                }
                if( d != null ){
                   if(
    (d.getNodeType() == Node.TEXT_NODE)
    && d.getNodeValue().trim().equals("")
){
    d.getParentNode().removeChild(d);
    d = null;
}

             }  }
             d = mtd;
             mtd = mtd.getPreviousSibling();
             if(
    (d.getNodeType() == Node.TEXT_NODE)
    && d.getNodeValue().trim().equals("")
){
    d.getParentNode().removeChild(d);
    d = null;
}

             if( (d != null)
                  && (d.getNodeType() == Node.ELEMENT_NODE) ){
               if( (d.getNextSibling()==null)
    && (d.getFirstChild()==null)  ){
   d.getParentNode().removeChild(d);
   d = null;
}

               if( d == null ){ count--; }
           } }
           if( count > cols ){ cols = count; }
      } }
      d = mtr;
      mtr = mtr.getPreviousSibling();
      if(
    (d.getNodeType() == Node.TEXT_NODE)
    && d.getNodeValue().trim().equals("")
){
    d.getParentNode().removeChild(d);
    d = null;
}

      if( d != null ){
         if( (d.getNextSibling()==null)
    && (d.getFirstChild()==null)  ){
   d.getParentNode().removeChild(d);
   d = null;
}

}  }  }

   if (node.hasChildNodes()) {
   mtr = node.getFirstChild();
   while( mtr != null){
      if( mtr.getNodeType() == Node.ELEMENT_NODE ){
         int count = 0;
         if (mtr.hasChildNodes()) {
           mtd = mtr.getFirstChild();
           while( mtd != null){
             if( mtd.getNodeType() == Node.ELEMENT_NODE ){
                mtr.insertBefore( ((Document) dom).createTextNode("\n"), mtd );

                count++;
             }
             mtd = mtd.getNextSibling();
           }
         }
         if( count < cols ){
            for(int i = count; i < cols; i++){
  mtr.appendChild( ((Document) dom).createElement("math:mtd") );
}

      }  }
      mtr = mtr.getNextSibling();
}  }

}

  public static void table(Node dom) {
      Node tblRow, tblCell, d;
//      int cols = 0;
   Node node = dom.getFirstChild();
   
   tblRow = node.getLastChild();
while( (tblRow != null)
//       && (tblRow.getNodeType() == Node.TEXT_NODE)
//       && tblRow.getNodeValue().trim().equals("")
       && tblRow.getTextContent().trim().equals("")
){
   node.removeChild(tblRow);
   tblRow = node.getLastChild();
}

   if( (tblRow != null) && (tblRow.getPreviousSibling() != null) ){
     boolean bool = false;
if( tblRow.getNodeName().equals("table:table-row")
    && tblRow.hasAttributes()
){
   NamedNodeMap attributes = tblRow.getAttributes();
   Node styleAttr = attributes.getNamedItem( "table:style-name" );
   String style = (styleAttr==null)? null
                                   : styleAttr.getNodeValue();
   if( (style != null)
       && (   style.equals("hline-row")
           || style.equals("cline-row")
          )
   ){
     bool = true;
}  }

     if( !bool ){
        tblCell = tblRow.getFirstChild();
while( tblCell != null){
  d = tblCell.getNextSibling();
  if( justSpace(tblCell) ){  tblRow.removeChild(tblCell);  }
  tblCell = d;
}

        
        tblCell = tblRow.getFirstChild();
        if( (tblCell != null)
            && (tblCell.getNextSibling() == null)
            && justSpace(tblCell)
        ){
          node.removeChild(tblRow);
   } }  }
   int n = 0;
tblRow = node.getFirstChild();
while( tblRow != null ){
   if(
       (tblRow.getNodeType() == Node.ELEMENT_NODE)
     &&
        tblRow.getNodeName().equals("table:table-row")
   ){
      int m = 0;
tblCell = tblRow.getFirstChild();
while( tblCell != null ){
   if(
       (tblCell.getNodeType() == Node.ELEMENT_NODE)
     &&
        tblCell.getNodeName().equals("table:table-cell")
   ){
      m++;
      Node child = tblCell.getLastChild();
while( child != null ){
   Node prevChild = child.getPreviousSibling();
   if(
       (child.getNodeType() == Node.ELEMENT_NODE)
     &&
        child.getNodeName().equals("text:p")
   ){
     Node sibling = child.getPreviousSibling();
     while( (sibling != null)
            &&
            (sibling.getNodeType() != Node.ELEMENT_NODE)
     ){
       sibling = sibling.getPreviousSibling();
     }
     if( sibling == null ){
       sibling = child.getNextSibling();
       while( (sibling != null)
              &&
              (sibling.getNodeType() != Node.ELEMENT_NODE)
       ){
          sibling = sibling.getNextSibling();
     } }
     if( (sibling != null)
         && child.getTextContent().trim().equals("")
     ){
         tblCell.removeChild(child);
   } }
   child = prevChild;
}

   }
   tblCell = tblCell.getNextSibling();
}

      if( m > n ){ n = m; }
   }
   tblRow = tblRow.getNextSibling();
}

   tblRow = node.getFirstChild();
while( tblRow != null ){
   d = tblRow.getNextSibling();
   if(
       (tblRow.getNodeType() == Node.ELEMENT_NODE)
     &&
        tblRow.getNodeName().equals("table:table-column")
   ){
      n--;
      if( n < 0 ){
        tblRow.getParentNode().removeChild(tblRow);
  }  }
  tblRow = d;
}

}

  static boolean justSpace(Node node){
   if( node == null ){ return true; }
   if( node.getNodeType() == Node.TEXT_NODE ){
       if( !node.getNodeValue().trim().equals("") ){ return false; }
   } else {
       if( node.getNodeType() == Node.ELEMENT_NODE ){
          String nm = node.getNodeName();
          if(
                !nm.equals("table:table-cell")
             && !nm.equals("text:p")
          ){
             return false;
       }  }
   }
   if(!justSpace( node.getNextSibling() )){ return false; }
   if(!justSpace( node.getFirstChild() )){ return false; }
   return true;
}

}

