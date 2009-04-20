package tex4ht;
/**********************************************************/ 
/* DbUtilities.java                      2008-11-14-02:41 */
/* Copyright (C) 2008          Eitan M. Gurari            */
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
public class DbUtilities {
  public static void cline(Node dom) {
   Node row, entry, para, nextrow;
   Node node = dom.getFirstChild();
   if( node != null ){
      row = node.getLastChild();
while(     (row != null)
        && ( (entry = row.getLastChild()) != null)
        && ( (para = entry.getFirstChild()) != null)
        && ( para.getNextSibling() == null)
        && ( para.getFirstChild() == null)
){
  node.removeChild(row);
  row = node.getLastChild();
}

      row = node.getFirstChild();
while( row != null ){
  if(    (row.getNodeType() == Node.ELEMENT_NODE)
      && ((Element) row).getAttribute("rowsep").equals("")
      && !((Element) row).getAttribute("role"  ).equals("cline")
      && ((nextrow = row.getNextSibling()) != null)
      && (nextrow.getNodeType() == Node.ELEMENT_NODE)
      && ((Element) nextrow).getAttribute("role"  ).equals("cline")
  ){
    boolean compatible = true;
Node entry1 = row.getFirstChild();
Node entry2 = nextrow.getFirstChild();
while( true ){
  if( (entry1 == null) || (entry2 == null) ){ break; }
  int range;
  try{
    range =
          Integer.parseInt( ((Element) entry1).getAttribute("nameend") )
          -
          Integer.parseInt( ((Element) entry1).getAttribute("namest") )
          +
          1;
  } catch( Exception e){ range = 1;}
  if( range > 1 ){
    String rowsep = ((Element) entry2).getAttribute("rowsep");
    while( --range > 0 ){
       entry2 = entry2.getNextSibling();
       if( entry2 == null ){
          compatible = false;
          break;
       }
       String value = ((Element) entry2).getAttribute("rowsep");
       if( !value.equals( rowsep ) ){
          compatible = false;
          break;
       }
    }
  }
  if( !compatible ){ break; }
  entry1 = entry1.getNextSibling();
  entry2 = entry2.getNextSibling();
}

    if( compatible ){
       entry1 = row.getFirstChild();
entry2 = nextrow.getFirstChild();
while( true ){
  if( (entry1 == null) || (entry2 == null) ){ break; }
  int range;
  try{
    range =
          Integer.parseInt( ((Element) entry1).getAttribute("nameend") )
          -
          Integer.parseInt( ((Element) entry1).getAttribute("namest") )
          +
          1;
  } catch( Exception e){ range = 1;}
  ((Element) entry1).setAttribute(
                       "rowsep",
                       ((Element) entry2).getAttribute("rowsep") );
  while( --range > 0 ){
     entry2 = entry2.getNextSibling();
  }
  entry1 = entry1.getNextSibling();
  entry2 = entry2.getNextSibling();
}

       node.removeChild(nextrow);
    }
  }
  row = row.getNextSibling();
}

   }
}

  public static void para(Node dom) {
   Node pNode = dom.getFirstChild();
   if( pNode.hasChildNodes() ){
      Node child = pNode.getFirstChild();
      if( child != null ){
         if(  (child.getNodeType() == Node.TEXT_NODE)
            &&
              ((Text) child).getWholeText().trim().equals("")
         ){
            pNode.removeChild( child );
   }  }  }
   if( pNode.hasChildNodes() ){
      Node child = pNode.getLastChild();
      if( child != null ){
         if(  (child.getNodeType() == Node.TEXT_NODE)
            &&
              ((Text) child).getWholeText().trim().equals("")
         ){
            pNode.removeChild( child );
   }  }  }
   if( pNode.hasChildNodes() ){
      Node child = pNode.getFirstChild();
      if(    (child != null)
          && (child.getNextSibling() == null)
          && (child.getNodeType() == Node.TEXT_NODE)
      ){
         String txt = ((Text) child).getWholeText();
         String trm = txt.trim();
         if( !trm.equals(txt) ){
            ((Text)child).replaceWholeText(trm);
   }  }  }
}

}

