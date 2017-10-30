/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->obj = malloc(sizeof(struct obj_t) *cap);  //alokuje miesto o velkosti struktury cluster_t * pocet objektov v nej
    c->capacity = cap; 
    c->size = 0; 
    
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);  //uvolnenie alokovanej pamate pre jeden objekt
    c-> size = 0; //pocet objektov v strukture cluster_t = 0
    c-> capacity = 0;
    c->obj = NULL;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
  if (c->size == c->capacity){ //ak sa pocet objektov v strukture == kapacite vola sa realloc
    resize_cluster(c, c->capacity + CLUSTER_CHUNK );  //realokuje sa pamat na 10 novych objektov
  }
  
  c->obj[c->size] = obj;  //na posledne miesto v strukture sa priradi obj
  c->size++;   //pocet objektov v strukture sa zvacsi o 1
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for (int i = 0; i < c2->size; i++)  //vykonava sa pokial neprejde vsetky prvky c2
    {
    	if (c2->size + c1->size == c1->capacity){  //ak pocet prvkov c1 + c2 presahuje kapacitu c1, resizuje sa c1 
        resize_cluster(c1,(c2->size + c1->size));
      }
      append_cluster(c1, c2->obj[i]);  //pridanie objektov zo shluku c2 do c1
    }  
      sort_cluster(c1);  //usporiadanie c1 podla ID
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    for (; idx < (narr - 1); idx++){  //cyklus od shluku, ktory sa ma vymazat

      carr[idx] = carr[idx + 1];  //posun vsetkych prvkov v poli
    }

  return narr -1;  //vratenie aktualneho poctu prvkov v poli
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float x = (o1->x - o2->x);  //odpocitaju sa x suradnice objektov
    float y = (o1->y - o2->y);  //odpocitaju sa y suradnice objektov
     
  return sqrtf((x*x)+(y*y));  //umocene vysledky sa scitaju a nasledne odmocnia
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    int i = 0;
    int j = 0;
    int vzdialenost = 0;
    float prvy = obj_distance(&c1->obj[0], &c2->obj[0]);  //vzdialenost prvych dvoch objektov, pouzita ako pociatocna

    for (; i < c1->size; i++)  //najprv sa beru vsetky objekty z c1
    { 
      for (; j < c2->size; j++)  //porovnava sa prvy objekt z c1 so vsetkymi z c2
      {
        vzdialenost = obj_distance(&c1->obj[i], &c2->obj[j]);  
        
        if (vzdialenost > prvy){  
          prvy = vzdialenost;  //najvacsia vzdialenost 
        }         
      }
    }       
    return prvy;  //vracia najvacsiu vzdialenost
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

  int i = 0;
  int j = 0;
  int vzdialenost;
  float najmensi = cluster_distance(&carr[0], &carr[1]);
  int adresa_prveho = 0;
  int adresa_druheho = 0;
  
  for (;i <= narr-1; i++)
  { 
	  for(j = i +1; j <= narr -1; j++)
	  {
      vzdialenost = cluster_distance(&carr[i], &carr[j]);
      
      if (vzdialenost <= najmensi){
      
        najmensi = vzdialenost;   
        adresa_prveho = i;
        adresa_druheho = j;    
      }
    }
  }
        *c1 = adresa_prveho;
        *c2 = adresa_druheho;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    
  FILE *objekty;
	objekty = fopen(filename, "r");
	
	if (objekty == NULL){
	  return 0;
	}
    int i;
    int id = 0;
    float x = 0.0;
    float y = 0.0;
    int count = 0;
    struct obj_t s;

    	if((fscanf(objekty,"count=%d\n",&count)) != 1){  //chybajuci udaj o pocte objektov
	  		fprintf(stderr, "count= nenajdene v subore\n");
	  		*arr = NULL;
	  		fclose(objekty);
	  		return 0;
	  	}
	
	  fscanf(objekty,"count=%d",&count);  //ziskanie poctu objektov v subore
	  
	  	if (count < 0){
	    	return 0;
	  	}  
	  
	*arr = malloc(sizeof(struct cluster_t) * count);
	
	for (i = 0; i < count; i++){ 

	  fscanf(objekty, "%d %f %f", &id, &x, &y);  //nacitanie po jednom riadku zo suboru
	    
	    if (((int)x - x) != 0 || ((int)y - y) != 0){  // osetrenie ak by suradnica v subore mala desatinny tvar
	    	fprintf(stderr, "Suradnica nemoze mat desatinny format\n");
	    	*arr = NULL;
	    	fclose(objekty);
	    	return 0;
	    }	
	  /* pomocna struktura do ktorej sa ukladaju nacitane suradnice a ID objektu*/

	  s.x = x;
	  s.y = y;
	  s.id = id; 
	 
	    if ((s.x < 0 || s.x > 1000) || (s.y < 0 || s.y > 1000)){  //podmienka pre spravnu velkost suradnic
	  		fprintf(stderr, "Neplatne suradnice objektu v subore\n");

	      return 0;
	    }
	    
	  init_cluster(*arr+i, CLUSTER_CHUNK); //inicializuje sa shluk pre nacitane suradnice z riadku
	  append_cluster(*arr+i, s);  
	 
	    if(malloc(sizeof(struct cluster_t) * i) == NULL){  //ak sa nepodari vytvorit miesto v pamataci pre shluk
	      *arr = NULL;
	      return 0;
	      }   
	}
	fclose(objekty);
	return i;  //vrati pocet shlukov
}	
/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
	// TUTO FUNKCI NEMENTE
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{

  int c1;
  int c2;
  int poc_shlukov = 0;
  char* float_check; 
  struct cluster_t *clusters;

    if (argc > 3){
      fprintf(stderr, "Prilis vela argumentov\n");
      return 1;
    }
    
    if (argc <= 1){
      fprintf(stderr, "IZP Projekt c.3 - Jednoducha shlukova analyza\n"
      		   "Igor Ignac - 1BIA - xignac00\n"
      		   "\n"
      		   "Spustite program poomocou ./proj3 SOUBOR [N]\n"
      		   " SOUBOR - nazov suboru z ktoreho sa nacitavaju data\n"
      		   "    [N] - argument definuje finalny pocet shlukov N > 0 \n");
      return 1;
    }

    if (argv[2] == NULL){
      poc_shlukov = 1;
    }
    
    else{  
      poc_shlukov = strtoul(argv[2], &float_check, 10);
       
        if(*float_check != '\0'){  //kontrola ci je argument N cele cislo a je typu int
       
        fprintf(stderr, "Argument N bol zadany v zlom formate, pozadovany je format int\n");
        return 1;
        }
		}
	  if (poc_shlukov <= 0){
	    fprintf(stderr,"Pocet vyslednych shlukov nemoze byt mensi alebo rovny 0\n");
	    return 1;
	  }
    
    int pocet_obj = load_clusters(argv[1],&clusters);
    
    if(fopen(argv[1], "r") == NULL){  //ak zadany subor neexistuje 
      fprintf(stderr,"Zadany subor neexistuje\n");
      return 1;
    }     
    
    if(pocet_obj < poc_shlukov){
    	fprintf(stderr,"Pocet shlukov nemoze byt vacsi ako pocet objektov v subore\n");
    	return 1;
    }

/*
 cyklus sa opakuje dokial sa nedosiahne hodnota premennej poc_shlukov 
 tato hodnota sa znizi po kazdom zluceni dvoch shlukov
*/  
  while (pocet_obj != poc_shlukov)  // pocet objektov sa musi rovnat poctu shlukov na konci cyklu
  {
    find_neighbours(clusters, pocet_obj, &c1, &c2);
    merge_clusters(clusters + c1, clusters + c2);
    pocet_obj = remove_cluster(clusters, pocet_obj, c2);  // odstranenim shluku sa zmensi pocet shlukov, cyklus sa nezacykli
  }
  print_clusters(clusters,pocet_obj);
  
  for (int i = 0; i < poc_shlukov; i++)  //uvolnenie pamate, podla poctu shlukov
  {
    clear_cluster(clusters + i);
    free((clusters +i)->obj);
  }   
    free(clusters);   

 return 0;  
}
