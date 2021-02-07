MODULE Fifo;
IMPORT Out; (*  := WCout; *)
VAR
TYPE 
	LONGINT = INTEGER;
  Node* = POINTER TO NodeDesc;
  NodeDesc = RECORD
  	key* : LONGINT;
  	next : Node
  END;
  FIFO* = RECORD
  		first*, last* : Node
  		END;
  slur* = POINTER TO slurlist; 	  
  slurlist* = RECORD(NodeDesc)
  id, start, stop : CHAR;
  ps, voice, measure, note : LONGINT;
  END;
 PROCEDURE smallfree*(used : SET) : LONGINT;
	  (* finds the lowest INTEGER included in "used" *)
	  VAR i : LONGINT;
	  BEGIN
	  i := -1; 
	  REPEAT INC(i); UNTIL (i IN used);
	  RETURN i;
	  END smallfree;

 PROCEDURE Enqueue*( VAR q: FIFO;  n: Node );  
	(* puts Tag "n" into the list "q" *)
	BEGIN 
		n.next := NIL;  
		IF q.first # NIL THEN q.last.next := n ELSE q.first := n END;  
		q.last := n
	END Enqueue;  
 PROCEDURE DequeuedNode*(VAR q: FIFO) : Node;
 VAR n : Node;
 BEGIN
 n := q.first;
 IF n # NIL THEN q.first := n.next END;
 RETURN n
 END DequeuedNode; 
 
PROCEDURE testfifo*;
VAR m, n : Node; q : FIFO; i : LONGINT; used : SET;
BEGIN
i := 0; 
WHILE i < 10 DO 
		NEW(n); 
		n.key := i; 
		Enqueue(q,n); 
		INC(i) 
END;

m := DequeuedNode(q); i := 0;
WHILE m # NIL DO 
	Out.Int(i,5); 
	Out.Char("|");
	Out.Int(m.key,5);
	m := DequeuedNode(q); 
	INC(i); 
END;
i := 0; 
used := {1,2,3};Out.Ln(); Out.String("1,2,3"); Out.Int(smallfree(used),5);

used := {0,2,3,4};Out.Ln(); Out.String("2,3,4"); Out.Int(smallfree(used),5);


END testfifo;
END Fifo.testfifo
System.Free Fifo
