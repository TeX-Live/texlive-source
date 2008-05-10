package tex4ht;
/**********************************************************/ 
/* OoUtilities.java                      2008-02-20-10:22 */
/* Copyright (C) 2006--2008    Eitan M. Gurari            */
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
      Node mtr, mtd, d;
//      int cols = 0;
   Node node = dom.getFirstChild();
   
   mtr = node.getLastChild();
while( (mtr != null)
       && (mtr.getNodeType() == Node.TEXT_NODE)
       && mtr.getNodeValue().trim().equals("")             ){
   node.removeChild(mtr);
   mtr = node.getLastChild();
}

   if( (mtr != null) && (mtr.getPreviousSibling() != null) ){
     boolean bool = false;
if( mtr.getNodeName().equals("table:table-row")
    && mtr.hasAttributes()
){
   NamedNodeMap attributes = mtr.getAttributes();
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
        mtd = mtr.getFirstChild();
while( mtd != null){
  d = mtd.getNextSibling();
  if( justSpace(mtd) ){  mtr.removeChild(mtd);  }
  mtd = d;
}

        
        mtd = mtr.getFirstChild();
        if( (mtd != null)
            && (mtd.getNextSibling() == null)
            && justSpace(mtd)
        ){
          node.removeChild(mtr);
   } }  }
   int n = 0;
mtr = node.getFirstChild();
while( mtr != null ){
   if(
       (mtr.getNodeType() == Node.ELEMENT_NODE)
     &&
        mtr.getNodeName().equals("table:table-row")
   ){
      int m = 0;
mtd = mtr.getFirstChild();
while( mtd != null ){
   if(
       (mtd.getNodeType() == Node.ELEMENT_NODE)
     &&
        mtd.getNodeName().equals("table:table-cell")
   ){
      m++;
   }
   mtd = mtd.getNextSibling();
}

      if( m > n ){ n = m; }
   }
   mtr = mtr.getNextSibling();
}

   mtr = node.getFirstChild();
while( mtr != null ){
   d = mtr.getNextSibling();
   if(
       (mtr.getNodeType() == Node.ELEMENT_NODE)
     &&
        mtr.getNodeName().equals("table:table-column")
   ){
      n--;
      if( n < 0 ){
        mtr.getParentNode().removeChild(mtr);
  }  }
  mtr = d;
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

