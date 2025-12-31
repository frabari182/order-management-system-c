#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ADDREC "aggiungi_ricetta"
#define REMREC "rimuovi_ricetta"
#define RIF "rifornimento"
#define ORD "ordine"
#define LIM 20
#define LMAX 20
#define HSIZE 65537

//STRUTTURA PER RICETTARIO
typedef struct ingred{  
    char nome_ingred[LMAX];
    int qta;
    struct ingred * next;
}ilist_t;

typedef struct recipe_s{  
    char nome_recipe[LMAX];
    int peso;
    ilist_t * ingred;
    int num;
    int * ptr;
    struct recipe_s * next;
}recipe_t;

//STRUTTURA PER MAGAZZINO
typedef struct scorte{
    int qnta;
    int exp;
    struct scorte * next;
}scorte_t;

typedef struct alimento_s{ 
    char nome_alim[LMAX];
    int quanto;
    scorte_t * scorte;
    struct alimento_s * next;
}alimento_t;

//STRUTTURA PER CORRIERE
typedef struct coda_s{
    char nome_ord[LMAX];
    int peso;
    int num;
    int on;
    int time;
    struct coda_s * next;
}coda_t;

typedef struct{
    coda_t * head;
    coda_t * tail;
}queue;

//VARIABILI GLOBALI
int ris;
queue * q;

//PROTOTIPI
char * addrec(char *, recipe_t **, recipe_t *);   
char * remrec(char *, recipe_t **);
char * rif(char *, int, alimento_t **, recipe_t **, recipe_t *, alimento_t *);
char * ordine(char *, recipe_t **, alimento_t **, recipe_t *, int);
ilist_t * push(ilist_t *, char[], int);
int find(recipe_t *, char[]);
int hash(char[]);
recipe_t * push2(recipe_t *, char[], ilist_t *, int, int, int *);
recipe_t * find2(recipe_t *, char[]);
void destroy_r(recipe_t *);
void destroy_i(ilist_t *);
void destroy_mag(alimento_t *);
void destroy_camion(coda_t *);
alimento_t * find_rif(alimento_t *, char[]);
alimento_t * bucket_alim(alimento_t *, char[], int, int);
scorte_t * push_rif(scorte_t *, int, int);
scorte_t * insertinorder(scorte_t *, int, int);
recipe_t * find_ord(recipe_t *, char[]);
void enqueue(char[], int, int, int, int);
int find_coda(coda_t *, char[]);
void corriere(int);
coda_t * push_camion(coda_t *, coda_t *);
void display_coda(coda_t *);
coda_t * quicksort(coda_t *, coda_t *);
coda_t * dividi(coda_t *, coda_t *);

int main (int argc, char * argv[])
{
    int molt, cap, i;
    int time, cor;
    char comm[LMAX+1];
    
    recipe_t * hash_table[HSIZE];   
    alimento_t * hash_rif[HSIZE];

    recipe_t * lista_rec=NULL;
    alimento_t * lista_rif=NULL;

    q=(queue *)malloc(sizeof(queue));
    if(q){
        q->tail=NULL;
        q->head=NULL;
    }

    for(i=0; i<HSIZE; i++)
        hash_table[i]=NULL;

    for(i=0; i<HSIZE; i++)
        hash_rif[i]=NULL;
    
    if(scanf("%d%d", &molt, &cap)==2);
    
    ris=scanf("%s", comm);

    time=-1;
    
    while(ris!=-1){
        time++;
        cor=time%molt;
        if(time!=0 && cor==0){
             corriere(cap);
        }

        if(!strcmp(comm, ADDREC)){
            addrec(comm, hash_table, lista_rec);
        }
        
        else if(!strcmp(comm, REMREC)){
            remrec(comm, hash_table);
        }
        
        else if(!strcmp(comm, RIF)){
            rif(comm, time, hash_rif, hash_table, lista_rec, lista_rif);
        }
        
        else if(!strcmp(comm, ORD)){
            ordine(comm, hash_table, hash_rif, lista_rec, time);
        }  
    }
    
    //GESTIONE EXTRA PER CORRIERE
    time++;
    cor=time%molt;
    if(cor==0){
        corriere(cap);
    }

    destroy_r(lista_rec);
    destroy_mag(lista_rif);
     
    return 0;
        
}

void corriere(int cap)
{
    coda_t * tmp;
    int lim;
    coda_t * h=NULL;
    coda_t * del;
    coda_t * el;
    
    lim = cap;
    while(q->head && q->head->on==1 && lim>=0){
        lim=lim - q->head->peso;
        if(lim>=0){
            h=push_camion(h, q->head);
            del=q->head;
            q->head=q->head->next;
            free(del);
        }
    }
        
    tmp=q->head;
    while(tmp && tmp->next && lim>=0){
        while(tmp->next && tmp->next->on==1 && lim>=0){
            lim=lim - tmp->next->peso;
            if(lim>=0){
                h=push_camion(h, tmp->next);
                del=tmp->next;
                tmp->next=tmp->next->next;
                free(del);
            }
        }
        tmp=tmp->next;
    }

    
    if(h==NULL)
        printf("camioncino vuoto\n");
    else{
        h=quicksort(h, NULL);
        el=h;
        for(;el;el=el->next){
            printf("%d ", el->time);
            printf("%s ", el->nome_ord);
            printf("%d\n", el->num);
        }
    }

    destroy_camion(h);

}

void destroy_camion(coda_t * h)
{
    coda_t * tmp;

    while(h){
		tmp=h;
		h=h->next;
		free(tmp);
	}
}

coda_t * push_camion(coda_t * h, coda_t * tmp)
{
    coda_t * n;

    n=(coda_t *)malloc(sizeof(coda_t));
    if(n){
        strcpy(n->nome_ord, tmp->nome_ord);
        n->num=tmp->num;
        n->on=tmp->on;
        n->peso=tmp->peso;
        n->time=tmp->time;
        n->next=h;
        h=n;
    }else
        printf("errore allocazione camion");

    return h;
}

coda_t * quicksort(coda_t * s, coda_t * e)
{
    coda_t * p;
    if(s == e)
        return s;
    p = s;
    s = dividi(s, e);
    s = quicksort(s, p);
    p->next = quicksort(p->next, e);
    return s;
}

coda_t * dividi(coda_t * s, coda_t * e)
{ 
    coda_t * p, *tmpin, *tmp, *pivot;
    
    if ( s == NULL )
        return NULL;
    pivot = s;
    p = s;
    while( p->next != e){
        if( ((p->next->peso) == (pivot->peso) && (p->next->time) < (pivot->time)) || (p->next->peso) > (pivot->peso)){
            tmp = p->next;
            tmpin = s;
            p->next = tmp->next;
            s = tmp;
            s->next = tmpin;
        } else
            p = p->next;
    }           
    
    return s; 
}



char * rif(char comm[], int time, alimento_t * hash_rif[], recipe_t * hash_table[], recipe_t * lista_rec, alimento_t * lista_rif)
{
    
    char lotto[LMAX+1];
    int qta1, scad, index;
    int stop, idx, esci, num;
    int need, diff, val;
    int exit, i;
    
    alimento_t * el;
    alimento_t * p;
    recipe_t * ptr;
    ilist_t * h;
    ilist_t * head;
    scorte_t * del;
    scorte_t * punt;
    coda_t * coda_ordini=NULL;

    if(scanf("%s", lotto)==1);

    printf("rifornito\n");

    //GESTIONE AGGIORNAMENTO MAGAZZINO
    while(strcmp(lotto, ADDREC) && strcmp(lotto, REMREC) && strcmp(lotto, RIF) && strcmp(lotto, ORD) && ris!=-1){
        if(scanf("%d%d", &qta1, &scad)==1);
        
        if(scad>time){ // inserisco in magazzino solo se non è scaduto, altrimenti passo al prossimo alimento/comando
            index=hash(lotto);
            lista_rif=hash_rif[index]; 
            el=find_rif(lista_rif, lotto);  
            if(el==NULL){  // cerco il campo lotto: se non ce, lo metto all'inizio della lista; se ce, gestisco l'insert in order
                lista_rif=bucket_alim(lista_rif, lotto, qta1, scad);  
            }
            
            else{
                el->quanto+=qta1;
                el->scorte=insertinorder(el->scorte, qta1, scad);  // passo direttamente la lista delle scorte
            }
            
            hash_rif[index]=lista_rif; 
        }
        
        ris=scanf("%s", lotto); 
    
    
    }

    coda_ordini=q->head;

    idx=0;
    num=0;

    // CONTROLLO SE POSSO SBLOCCARE QUALCHE ORDINE DA ATTESA A PRONTO
    while(coda_ordini!=NULL){ 
        stop=-1;
        if(coda_ordini->on==0){  //se è un ordine in attesa
            index=hash(coda_ordini->nome_ord); // calcolo hash ordine in ricettario
            lista_rec=hash_table[index];
            ptr=find_ord(lista_rec, coda_ordini->nome_ord);
            if(ptr){ // se trova la ricetta 
                h=ptr->ingred; 
                head=h;
                stop=0;
                i=0;
                while(head && !stop){
                    //idx=hash(head->nome_ingred);  // calcolo hash ingrediente in magazzino
                    idx=*(ptr->ptr + i);
                    i++;
                    lista_rif=hash_rif[idx];
                    p=find_rif(lista_rif, head->nome_ingred);
                    if(p){ // se trova l'ingr, controllo se è scaduto
                        num=coda_ordini->num;
                        need=num * head->qta;
                        esci=0;
                        while(p->scorte && p->quanto >= need && esci==0){ // casi: 1. elimino tutte le scorte(scadute), 2. non ho o non ho piu la qta sufficiente per fare ordine, 3. esci va a 0
                            if(time >= p->scorte->exp){
                                diff=p->scorte->qnta;
                                del=p->scorte;
                                p->scorte=p->scorte->next;
                                free(del);
                                p->quanto=p->quanto-diff;
                            }
                            else{   // se incontro una scorta con scadenza maggiore del tempo, siccome la lista è ordinata, nessun altro è scaduto 
                                esci=1;
                            }
                        }
                        if(p->scorte==NULL || p->quanto<need)
                            stop=1;  // mi manca un ingr, non posso fare ordine
                        else
                            head=head->next;
            
                    }else
                        stop=1;
                }
            }
        }
        if(stop==0){ // CASO: ho tutti gli ingredienti necessari
            coda_ordini->on=1; // metto l'ordine in pronto
            i=0;
            //tolgo ingredienti utilizzati dal magazzino
            while(h){ // mi ero salvato in h la testa di lista della ricetta, la riscorro per eliminare adesso nel magazzino gli ingred usati
                val=h->qta * num;  
                //idx=hash(h->nome_ingred);

                idx=*(ptr->ptr + i);
                i++;

                lista_rif=hash_rif[idx];
                p=find_rif(lista_rif, h->nome_ingred);
                p->quanto=p->quanto-val; //sottraggo quanto ne ho preso al valore totale delle scorte, poi devo levare da ogni scorta
                punt=p->scorte;
                exit=0;
                while(exit==0 && punt){
                    if(punt->qnta>val){ //caso semplice: la prima scorta ha abbastanza roba da togliere 
                        punt->qnta= punt->qnta - val;
                        exit=1;
                    }
                    
                    else if(punt->qnta==val){ //ne ha uguale
                        del=punt;
                        punt=punt->next;
                        free(del);
                        del=NULL;
                        exit=1;
                        p->scorte=punt;
                    }
                    
                    else{ // se invece è minore devo eliminare la cella e passare alla prox
                        val=val-punt->qnta;
                        del=punt;
                        punt=punt->next;
                        free(del);
                        del=NULL;
                        p->scorte=punt;
                    }
                }
                
                h=h->next;

            
            }

            hash_rif[idx]=lista_rif;
    
        }// else lo lascio off, cioe nella coda di attesa: non ho ingred sufficienti

        coda_ordini=coda_ordini->next;

    }

    strcpy(comm, lotto);
    return comm;
}


char * ordine(char flag[], recipe_t * hash_table[], alimento_t * hash_rif[], recipe_t * lista_rec, int time)
{
    char ord[LMAX+1];
    char comm[LMAX+1];
    int num, index, i;
    int stop, sum, val;
    int exit, need;
    int diff, esci, true;
    
    recipe_t * el;
    ilist_t * h;
    ilist_t * head;
    alimento_t * lista_magaz;
    alimento_t * p;
    scorte_t * punt;
    scorte_t * del;
    
    
    if(scanf("%s%d", ord, &num)==2);

    index=hash(ord);
    lista_rec=hash_table[index];
    el=find_ord(lista_rec, ord);

    stop=0;
    sum=0;

    //AGGIORNAMENTO MAGAZZINO PER I LOTTI SCADUTI 
    if(el){ // se trova la ricetta
        printf("accettato\n");
        sum=num*el->peso;
        h=el->ingred; 
        head=h;
        i=0;
        while(head && !stop){
            //index=hash(head->nome_ingred);  
            index=*(el->ptr + i);
            i++;
            lista_magaz=hash_rif[index];  
            p=find_rif(lista_magaz, head->nome_ingred);
            if(p){ 
                need=num * head->qta;
                esci=0;
                while(p->scorte && p->quanto >= need && esci==0){ 
                    if(time >= p->scorte->exp){
                        diff=p->scorte->qnta;
                        del=p->scorte;
                        p->scorte=p->scorte->next;
                        free(del);
                        p->quanto=p->quanto-diff;
                    }
                    else{   
                        esci=1;
                    }
                }
                if(p->scorte==NULL || p->quanto<need)
                    stop=1; 
                else
                    head=head->next;
            
            }else
                stop=1;
        }
        
        if(stop==0){ // ho tutti gli ingred della ricetta el magazzino ->> coda pronti
            true=1;
            enqueue(ord, sum, true, num, time);
            i=0;
            //tolgo ingredienti utilizzati dal magazzino 
            while(h){ 
                val=h->qta * num;  
                //index=hash(h->nome_ingred);  
                index=*(el->ptr+i);
                i++;
                lista_magaz=hash_rif[index];  
                p=find_rif(lista_magaz, h->nome_ingred);
                
                p->quanto=p->quanto-val; 
                punt=p->scorte;
                exit=0;
                while(exit==0 && punt){
                    if(punt->qnta>val){ 
                        punt->qnta= punt->qnta - val; 
                        exit=1;
                    }
                    
                    else if(punt->qnta==val){
                        del=punt;
                        punt=punt->next;
                        free(del);
                        del=NULL;
                        exit=1;
                        p->scorte=punt;
                    }
                    
                    else{
                        val=val-punt->qnta; 
                        del=punt; 
                        punt=punt->next;
                        free(del);
                        del=NULL;
                        p->scorte=punt;
                    }
                }
                
                h=h->next;
                hash_rif[index]=lista_magaz;
            
            }

        }else if(stop==1){ // coda attesa
            true=0;
            enqueue(ord, sum, true, num, time); 
        }

    }else
        printf("rifiutato\n");


    ris=scanf("%s", comm);

    strcpy(flag, comm);
    return flag;
}

void enqueue(char s[], int sum, int pronto, int val, int tempo) 
{ // qui ci entro solo da ordine, da rifornimento cambio solo il campo on
    coda_t * n;
    
    n=(coda_t *)malloc(sizeof(coda_t));
    if(n){
            
            strcpy(n->nome_ord, s);
            n->time=tempo;
            n->peso=sum;
            n->num=val;
            n->next=NULL;
            n->on=pronto; // mette 0 o 1 a seconda che sia in attesa o pronto
            if(q->tail!=NULL){ 
                q->tail->next=n;
            }
            q->tail=n;
            q->tail->next=NULL;
            if(q->head == NULL){
                q->head=n;
            }
            
    }

}

scorte_t * insertinorder(scorte_t * h, int num1, int scad)
{
	scorte_t * pre;
    scorte_t * n;

    if(!h || (scad<h->exp)){ // gestione in testa
        n=(scorte_t *)malloc(sizeof(scorte_t));
        if(n){
		    n->qnta=num1;
            n->exp=scad;
		    n->next=h;
		    h=n;
        }
    }
    
    else if(scad==h->exp){ // se la testa ha scadenza uguale a quello nuovo; cambio solo la quantita
        h->qnta+=num1;
	}else{
		pre=h;
    
		while(pre->next && scad > pre->next->exp)
				pre=pre->next;
        
        if(pre->next == NULL){  // caso di pre next null
            n=(scorte_t *)malloc(sizeof(scorte_t));
            if(n){
		        n->qnta=num1;
                n->exp=scad;
                n->next=pre->next; 
		        pre->next=n; 
            }
        }else if(scad < pre->next->exp){
            n=(scorte_t *)malloc(sizeof(scorte_t));
            if(n){
		        n->qnta=num1;
                n->exp=scad;
                n->next=pre->next; 
		        pre->next=n; 
            }
        }else // caso in cui si è fermato perche era =
            pre->next->qnta = pre->next->qnta + num1;
    
    }

	return h;
}

alimento_t * bucket_alim(alimento_t * h, char s[], int num1, int num2)
{
	alimento_t * n;
    scorte_t * lista_sco=NULL;  //creo la lista  

	n=(alimento_t *)malloc(sizeof(alimento_t));
    if(n){ 
		strcpy(n->nome_alim, s);
        n->quanto=num1;
        lista_sco=push_rif(lista_sco, num1, num2); 
        n->scorte=lista_sco; // metto la lista appena creata nel campo lista del bucket
		n->next=h; 
		h=n;  
	}

	return h;	
}

alimento_t * find_rif(alimento_t * h, char lotto[])
{
	alimento_t * el;

	el=h;
	while(el){
		if(!strcmp(el->nome_alim, lotto))
			return el;
		el=el->next;
	}
	return NULL;
}

recipe_t * find_ord(recipe_t * h, char s[])
{
	while(h!=NULL){
		if(strcmp(h->nome_recipe, s)==0){
			return h;
        }
		h=h->next;
	}

	return NULL;
}

scorte_t * push_rif(scorte_t * h, int num1, int num2)
{
	scorte_t * n;

	n=(scorte_t *)malloc(sizeof(scorte_t));
    if(n){ //creo la memoria //se n è diversa da NULL
        n->qnta=num1;
        n->exp=num2;
		n->next=h; // h è la testa di lista di lista_col
		h=n; //infine pongo la struttura dati ormai modificata uguale alla testa (cioe la nuova struttura diventa la testa della lista) 
	}

	return h;	
}


char * addrec(char comm[], recipe_t * hash_table[], recipe_t * lista_col) 
{
    char rec[LMAX+1];
    char ingr[LMAX+1];
    int qta, index, sum;
    int num, idx, i;
    int * v;
    ilist_t * el;
    
    
    if(scanf("%s", rec)==1);

    index=hash(rec); // calcolo hash ricetta
    lista_col=hash_table[index]; 

    // CERCO LA RICETTA NEL RICETTARIO
    if(!find(lista_col, rec)){ // se returna 0 vuol dire che non c'è: posso quindi mettere ricetta
        sum=0;
        ilist_t * h=NULL; // creo lista ingred
        if(scanf("%s", ingr)==1);
        num=0;
        
        while(strcmp(ingr, ADDREC) && strcmp(ingr, REMREC) && strcmp(ingr, RIF)  && strcmp(ingr, ORD) && ris!=-1){
            if(scanf("%d", &qta)==1);
            //idx=hash(ingr);
            //*(v+num)=idx;
            num++;
            sum=sum+qta;
            h=push(h, ingr, qta);
            ris=scanf("%s", ingr); 
        }
        v=(int *)malloc(sizeof(int)*(num));
        i=0;
        el=h;
        while(el){
            idx=hash(el->nome_ingred);
            *(v+i)=idx;
            i++;
            el=el->next;
        }
        //v=(int *)malloc(sizeof(int));
        lista_col=push2(lista_col, rec, h, sum, num, v); // metto il bucket creato nel ricettario
        hash_table[index]=lista_col;
        printf("aggiunta\n");

    }else{ // se la trovo passo all'altro comando
        if(scanf("%s", ingr)==1);
        while(strcmp(ingr, ADDREC) && strcmp(ingr, REMREC) && strcmp(ingr, RIF)  && strcmp(ingr, ORD) && ris!=-1){
            if(scanf("%d", &qta)==1);
            ris=scanf("%s", ingr);
        }
        printf("ignorato\n");
    }
    
    strcpy(comm, ingr);
    return comm;

}       


char * remrec(char comm[], recipe_t * hash_table[])
{
    char rec[LMAX+1];
    char flag[LMAX+1];
    
    recipe_t * h=NULL;
    recipe_t * del;
    ilist_t * tmp;
    recipe_t * el;
    int index;
    
    if(scanf("%s", rec)==1);
    
    index=hash(rec); // calcolo hash ricetta
    h=hash_table[index];

    if(h){ // se la ricetta è nel ricettario
        if(find_coda(q->head, rec)==0){ // se la ricetta non è un ordine in coda attesa/pronti, RIMUOVI
            if(!strcmp(h->nome_recipe, rec)){ // gestione testa
                tmp=h->ingred;
                destroy_i(tmp);
                del=h;
                h=h->next;
                free(del);
                printf("rimossa\n");
            }else if(find2(h, rec)){ // el è il predecessore del nodo da eliminare
                    el=find2(h, rec);
                    tmp=el->next->ingred;
                    destroy_i(tmp);
                    del=el->next;
                    el->next=el->next->next;
                    free(del);
                    printf("rimossa\n");
            }else
                printf("non presente\n");
            hash_table[index]=h;
        }else
            printf("ordini in sospeso\n");
    }else
        printf("non presente\n");
    
    ris=scanf("%s", flag);
    strcpy(comm, flag);

    return comm;
}

// SCORRO LA CODA ORDINI
int find_coda(coda_t * h, char s[])
{
    int stop;

    stop=0;
    while(h && stop==0){
        if(!strcmp(h->nome_ord, s)){
            stop=1;
        }
        h=h->next;
    }

    return stop;
}

// LISTA INGREDIENTI RICETTA
ilist_t * push(ilist_t * h, char s[], int num)
{
	ilist_t * n;

	n=(ilist_t *)malloc(sizeof(ilist_t));
    if(n){ 
		strcpy(n->nome_ingred, s); 
        n->qta=num;  
		n->next=h;   
		h=n;  
	}

	return h;	
}

// CERCO NEL RICETTARIO
int find(recipe_t * h, char s[]) 
{
	recipe_t * el;
	int trovato;

	el=h;
	trovato=0;
	while(el && !trovato){  
        if(!strcmp(el->nome_recipe, s))
			trovato=1;
		el=el->next;
	}

	return trovato;
}

// LISTA RICETTARIO
recipe_t * push2(recipe_t * h, char s[], ilist_t * listain, int sum, int num, int * v)
{
	recipe_t * n;//devo fare bucket di tipo recipe_t

	n=(recipe_t *)malloc(sizeof(recipe_t));
    if(n){ //creo la memoria //se n è diversa da NULL
		strcpy(n->nome_recipe, s);
        n->ptr=v;
        n->num=num;
        n->peso=sum;
        n->ingred=listain; //listain è la lista degli ingr gia completata
		n->next=h; // h è la testa di lista di lista_col
		h=n; //infine pongo la struttura dati ormai modificata uguale alla testa (cioe la nuova struttura diventa la testa della lista) 
	}

	return h;	
}

// SCORRO IL RCIETTARIO
recipe_t * find2(recipe_t * h, char s[])  // ho gia controllato la testa che non è
{
	recipe_t * el;
    recipe_t * pre;
    
    pre=h;
    el=pre->next;
	while(el){
		if(!strcmp(el->nome_recipe, s))
			return pre; // returno il predecessore
        pre=pre->next;
		el=el->next;
	}
	return NULL;
}

int hash(char s[])
{
    unsigned int hash;
    
    hash=0;
    unsigned int tmp;

    while((tmp=*s++))
        hash = tmp + (hash << 6) + (hash << 16) - hash;

    return hash % HSIZE;
    
}

void destroy_r(recipe_t * l2) //ricette
{
	recipe_t * tmp;
	while(l2){
		tmp=l2;
		l2=l2->next;
		free(tmp);
	}
}

void destroy_mag(alimento_t * l2) //magazzino
{
	alimento_t * tmp;
	while(l2){
		tmp=l2;
		l2=l2->next;
		free(tmp);
	}
}

void destroy_i(ilist_t * l2) // ingr
{
	ilist_t * tmp;
	while(l2){
		tmp=l2;
		l2=l2->next;
		free(tmp);
	}
}

