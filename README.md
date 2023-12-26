# FileSystem Based On FUSE API 

## Scopul proiectului
* Prin acest proiect, dorim sa aducem utilizatorilor o modalitate user-friendly de a crea un sistem de fisiere intr-un mod safe, fara a avea contact direct cu kernelul si a modifica in vreun fel codul acestuia.
* Acestea fiind spuse, dorim sa implementam eficient functionalitatile de creare, modificare si stergere a fisiereleor si directoarelor ce alacatuiesc un sistem de fisiere din sistemul de operare Linux.
    
## Cum ne propunem sa implementam?🤔
* Sistemul de fisiere bazat pe API-ul FUSE ii permite utilizatorului sa aibe control total asupra structurii de foldere si fisiere. 
* Astfel, Linux apare ca o interfata cu utilozatorul, fara a avea vreun efect asupra modului in care user-ul si-a construit ierarhia de directoare.
* Ne-am gandit sa folosim urmatoarea structura:

## struct file{
* int inode;
*  mode_t permissions;
* char*path;
* 	int gid;
* 	int uid;
* 	char*content;
* 	time_t accest;
* 	time_t mtime;
*    int size;
## };   

## struct directory{
* 	int inode;
* 	char*path;
* 	int gid;
* 	int uid;
* 	struct file*list_files[264];
* 	int number_of_directories_current_folder;
*   int number_of_files_current_folder;
* 	struct directory*list_directories[264];
* 	mode_t permissions;
* 	time_t accest;
* 	time_t mtime;
## };

   Astfel, putem "simula" ierarhia folderelor si a fisierelor. Operatiile pe care le efectuam asupra structurilor de date , respectiv file si directory , sunt descrise in functiile prezente in sursa.c.

## Cum s-a realizat persistenata atat a fisierelor si a directoarelor create de utilzator, cat si a modificarilor ulterioare?

* Dupa ce am pus bazele logicii sistemului de fisiere si directoare, impreuna cu functiile necesare, am observat faptul ca datele se pierd o data cu demontarea sistemului de fisiere.
* Persistenta sistemului se bazeaza pa 3 fisiere text( _Atribute_fisiere.txt_ , _Atribute_directoare.txt_ , _Inode.txt_), si un director (_/home/alex/Desktop/Memory/_).

* **Atribute_fisiere.txt si Atribute_directoare.txt**
  * Aceste 2 fisiere au rolul de a retine informatiile despre fisierele si directoarele sistemului.( **Atat pe cele nou create cat si pe cele deja create**)
  * **Am ales sa lucram cu 2 fisiere diferite, deoarece este mai usor de gestionat posibilele erori ce pot aparea pe parcurs si de separat directoarele si fiserele!!!**


 * **Inode.txt**
   * Este utilizat cu scopul de a tine evidenta inode-urilor la nivelul sistemului. Atfel, putem obtine unicitatea acestora. Acest lucru este util atunci cand dorim persistenta continutului fisierelor.
   * De fiecare data cand se scrie un anumit continut intr-un fisier, acesta se salveaza automat intr-un fisier cu calea: **/home/alex/Desktop/Memory/ValoareInode.txt**
   * **Valoarea pe care o contine este actualizat de fiecare data cand este creat un nou fisier sau director.**
     
## Functii folosite pentru gestionarea modificarilor datelor utilizate in vederea constituirii persistentei sistemului de fisiere.
   * 
 
## Operatiile folosite in FUSE
* Crearea unui fisier/director
* Stergerea unui fisier/director
* Vizualizarea datelor despre un fisier/director
* Posibilitatea de a naviga prin directoarele sistemului de fisiere
* Functionalitatea de modificare a permisiunilor unui fisier/director
* Redenumirea unui fisier/director
* Functionalitate de afisare a atributelor unui fisier
* Persistenta sistemului de fisiere dupa demontare

# IMPORTANT!!!
Se pot observa posibilele erori ce pot aparea pe parcursul rularii programului, utlizand optiunea "-d", in optiunile montarii.


# Bibliografie
  `https://www.maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/`
  `https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201001/homework/fuse/fuse_doc.html`
  `https://libfuse.github.io/doxygen/structfuse__operations.html`
  `https://stackoverflow.com/`
