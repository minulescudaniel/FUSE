# FileSystem Based On FUSE API 
## Scopul proiectului
Prin acest proiect, dorim sa aducem utilizatorilor o modalitate user-friendly de a crea un sistem de fisiere intr-un mod safe, fara a avea contact direct cu kernelul si a modifica in vreun fel codul acestuia.
Acestea fiind spuse, dorim sa implementam eficient functionalitatile de creare, modificare si stergere a fisiereleor si directoarelor ce alacatuiesc un sistem de fisiere din sistemul de operare Linux.
## Cum ne propunem sa implementam?🤔
In vederea realizarii acestui proiect, vom aborda concepte precum:
* superblock : `folosit pentru a retine metadate despre sistemul de fisiere`
* i-node: `metadate (path, owner,block size, access time,modified time etc.)`
* lista pentru a retine i-node-urile: `o listă care conține inodurile din sistem`
* dentry: `o evidenta a ierarhiei fisierelor in directoare`
* bitmap: `modalitate prin care se tine evidenta de blocuri de memorie libere pentru a plasa i-node-ul sau continutul unui nou fisier`
## Structura
Vom folosi i-node-uri, lista de i-node-uri, blocuri de date, bitmap-uri.
Tabela de i-noduri ale fisierelor va fi populata la momentul montarii sistemului de fisiere.Atunci cand se doreste crearaea unui nou fisier, se alege primul bloc disponibil din blocul de date pe baza bitmap-ului creat pentru a tine evidenta disponibilitatii blocurilor.
Atunci cand un fisier este eliminat, acest lucru se va observa si in blocurile destinate listei de i-noduri si bitmap-ului ce face referire la gradul de ocupare al blocurilor de date din sistem.
## Operatiile folosite in FUSE
* Crearea unui fisier/director
* Stergerea unui fisier/director
* Vizualizarea datelor despre un fisier/director
* Posibilitatea de a naviga prin directoarele sistemului de fisiere
* Functionalitatea de modificare a permisiunilor unui fisier
* Redenumirea unui fisier/director
* Persistenta sistemului de fisiere dupa demontare
* Functionalitate de afisare a atributelor unui fisier

# IMPORTANT!!!
Se pot observa posibilele erori ce pot aparea pe parcursul rularii programului, utlizand optiunea "-d", in optiunile montarii.


# Bibliografie
  `https://www.maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/`
  `https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201001/homework/fuse/fuse_doc.html`
  `https://libfuse.github.io/doxygen/structfuse__operations.html`
