.entry K
.entry LOOP
.extern W
.extern R
MAIN: mov @r0,@r1
      cmp -5,K
      sub W , @r2
      not K
	prn S
	cmp @r1,@r3
LOOP:   jmp R
	inc @r3
	prn K	
	jsr S
	cmp 2,S

END: rts
K: .data -1
S: .string "ab"

