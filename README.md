![FUSE](https://i.postimg.cc/59RjwV85/fuse-logo.png)

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

   Astfel, putem "simula" ierarhia folderelor si a fisierelor. Operatiile pe care le efectuam asupra structurilor de date, respectiv file si directory, sunt descrise in functiile prezente in sursa.c.

## Cum s-a realizat persistenata atat a fisierelor si a directoarelor create de utilzator, cat si a modificarilor ulterioare?

* Dupa ce am pus bazele logicii sistemului de fisiere si directoare, impreuna cu functiile necesare, am observat faptul ca datele se pierd o data cu demontarea sistemului de fisiere.
* Persistenta sistemului se bazeaza pa 3 fisiere text( _Atribute_fisiere.txt_ , _Atribute_directoare.txt_ , _Inode.txt_), si un director (_/home/[user]/Desktop/Memory/_).

* **Atribute_fisiere.txt si Atribute_directoare.txt**
  * Aceste 2 fisiere au rolul de a retine informatiile despre fisierele si directoarele sistemului.( **Atat pe cele nou create cat si pe cele deja create**)
  * **Am ales sa lucram cu 2 fisiere diferite, deoarece este mai usor de gestionat posibilele erori ce pot aparea pe parcurs si de separat directoarele si fiserele!!!**


 * **Inode.txt**
   * Este utilizat cu scopul de a tine evidenta inode-urilor la nivelul sistemului. Atfel, putem obtine unicitatea acestora. Acest lucru este util atunci cand dorim persistenta continutului fisierelor.
   * De fiecare data cand se scrie un anumit continut intr-un fisier, acesta se salveaza automat intr-un fisier cu calea: **/home/[user]/Desktop/Memory/ValoareInode.txt**
   * **Valoarea pe care o contine este actualizat de fiecare data cand este creat un nou fisier sau director.**
     
## Functii folosite pentru gestionarea modificarilor datelor utilizate in vederea constituirii persistentei sistemului de fisiere.

* **void update_information(char*searched_path,char*new_path,mode_t permissions,int inode,int size,int nlinks,int gid,int uid,time_t atime,time_t mtime ,char type)**
  * Este utilizata pentru a actualiza datele din fisierele(_Atribute_directoare.txt_ si _Atribute_fisiere.txt_) folosite cu scopul de a crea persistenta datelor sistemului.
  * **char*searched_path**->se foloseste pentru a gesi directoul sau fisierul pe care dorim sa-l actualizam
  * **char*new_path**-> se utilizeaza atunci cand se doreste redenumirea unui director sau a unui fisier
  * etc.
  * **char*type** -> poate fi f sau d si determina directorul in care se vor scrie noile modificari (_Atribute_fisiere.txt_ sau _Atribute_directoare.txt_ ). Astfel, aceasta functie capata un caracter _polimorfic_.
  * Ca si principiu de functionare, se sterge continutul fisierului modificat si sunt introduse datele actualizate( un rol important il are functia **char*create_buffer_information(char*path,mode_t mode,int inode,int size,int nlinks,int gid,int uid,time_t atime,time_t mtime,char type)**, ce are ca scop crearea unui buffer cu informatiile despre un fisier sau director.

* **void reconstruct()**
  * Aceasta functie sta la baza persistentei sistemului de fisiere. Este apelata la montarea sistemului de fisiere, in functia *do_init()*.
  * Prin intermediul ei se reconstruiesc mai intai directoarele, apoi fisierele sistemului. Aici intervine functia ***void create_instance(char* buff, char type)**, care _parseaza_ informatiile din liniile din fisiere si le utilieaza in vederea crearii unui fisier sau unui director. 
  
 
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

# SETUP
* Pentru a putea monta sistemul de fisiere FUSE, este nevoie de existenta directorului *.vscode*, in care se vor regasi 3 fisiere de configurare: _launch.json_, _tasks.json_, _settings.json_.
* In folder-ul in care exista proiectul, e necesar sa existe urmatoarele 3 fisiere: _Atribute_fisiere.txt_, _Atribute_directoare.txt_, _Inode.txt_ ( ATENTIE! - TREBUIE SA CONTINA VALOAREA 0! - pentru a atribui inodul 0 directorului ROOT).
* Apoi se va rula din interfata vscode-ului sursa. Utilizatorul trebuie sa deschida un nou terminal in care se va face _cd_ pe MountPoint-ul generat.
* In final, se vor testa functionalitatile puse la dispozitie puse de sistemul de fisiere. Dupa terminarea sesiunii, utilizatorul trebuie sa inchida terminalul si sa demonteze sistemul de fisiere cu comanda _sudo umount -f MountPoint_


# Bibliografie
  `https://www.maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/`
  `https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201001/homework/fuse/fuse_doc.html`
  `https://libfuse.github.io/doxygen/structfuse__operations.html`
  `https://stackoverflow.com/`

  
