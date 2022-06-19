1. Make sure, that named pipes do exist, e.g.
   rm f0
   rm f1
   makfifo f0 f1
2. Make sure, that no protocol files will be overwritten.
3. Execute the following command in the terminal window:
   ./ecuser logic.kb prot1.txt < f0 > f1 & ./ec < f1 > f0
4. Execute the following command in the terinal window:
   ./ecuser logic.kb prot2.txt < f0 > f1 & ./eco < f1 > f0
5. Now compare both files:
   diff prot1.txt prot2.txt

If ecuser cannot communicate successfully with the specific variant 
of the ec program, it may hang.

The first run creates a reference protocol file, here prot1.txt.
It should be created with the reference version of ec (compile EC.cpp).
The second run creates another protocol file for a new version
of ec, here prot2.txt.
If both protocol files do not differ, both versions of ec came to
exactly the same conclusions. In this case, the inference process
was not changed substantially.
