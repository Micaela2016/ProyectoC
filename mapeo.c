//#include <stdlib.h>
#include <stdio.h>

#include "mapeo.h"
#include "lista.h"
void fEliminar(tElemento);
void m_destruirAux(tMapeo m,void (*fEliminarC)(void *), void (*fEliminarV)(void *));
/**
 Inicializa un mapeo vacío, con capacidad inicial igual al MAX(10, CI).
 Una referencia al mapeo creado es referenciada en *M.
 A todo efecto, el valor hash para las claves será computado mediante la función fHash.
 A todo efecto, la comparación de claves se realizará mediante la función fComparacion.
 Finaliza indicando MAP_ERROR_MEMORIA si no es posible reservar memoria correspondientemente.
**/


void crear_mapeo(tMapeo * m, int ci, int (*fHash)(void *), int (*fComparacion)(void *, void *)){
     (*m) = (tMapeo)malloc(sizeof(struct mapeo)) ;

         if((*m)==NULL){
            exit(MAP_ERROR_MEMORIA);
         }

        (*m)->tabla_hash=(tLista*)malloc((sizeof(tLista)*(*m)->longitud_tabla));
        for(int i=0;i<10;i++){
            crear_lista(&((*m)->tabla_hash[i]));
        }

        (*m)->cantidad_elementos=0;
        (*m)->longitud_tabla=10;

        (*m)->comparador=fComparacion;
        (*m)->hash_code=fHash;
}

/**
 Inserta una entrada con clave C y valor V, en M.
 Si una entrada con clave C y valor Vi ya existe en M, modifica Vi por V.
 Retorna NULL si la clave C no existía en M, o Vi en caso contrario.
 Finaliza indicando MAP_ERROR_MEMORIA si no es posible reservar memoria correspondientemente.
**/
tValor m_insertar(tMapeo m, tClave c, tValor v){
    //Donde meto MAP:ERROR MEMORIA
    int num_hash=m->hash_code(&c);
    int hashC=num_hash%(m->longitud_tabla);
    tValor salida=NULL;

    if(m_recuperar(m,c)!=NULL){
        tPosicion paux=l_primera(tabla[hashC]);
        tEntrada taux=paux->elemento;
        int encontrado=0;
        while(paux->siguiente!=NULL && encontrado==0){

            if(m->comparador(c,taux->clave)==0){
                encontrado=1;
                salida=taux->valor;
                taux->valor=c;
            }

        }
    }
    else{
        tEntrada * ent=NULL;
        (*ent)=(tEntrada)malloc(sizeof(struct entrada));
        (*ent)->clave=&c;
        (*ent)->valor=&v;
        tLista lisAux=tabla[hashC];
        l_insertar(lisAux,l_fin(lisAux),(*ent));
        l_insertar(*(m->tabla_hash),l_fin(*(m->tabla_hash)),(*ent));


        /**************************************************
        ******************************************************
        ******************¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿**********
        ** No es conveniente que la entrada sea TEntrada ?????***
        ?????????????????????????????
        ********************************************************
        ========================================================
        tEntrada ent = (tEntrada) malloc(sizeof(struct entrada));
        if(ent == NULL){
            exit(MAP_ERROR_MEMORIA);
        }

        ent->clave = c;
        ent->valor = v;
        tLista lisAux=tabla[hashC];
        l_insertar(lisAux,l_fin(lisAux),(ent));


        ¿POR QUE SE ONGRESE DOS COSAS LA LA LISTA?

        =============================================

        */



    }

    int sobrecarga=(m->longitud_tabla*75)/100;
    if((m->cantidad_elementos)>sobrecarga){
       void* aux[m->cantidad_elementos];
       int cont=0;
       while(cont!=m->cantidad_elementos){
          for(int j=0;j<m->longitud_tabla;j++){
            tLista laux=tabla[j];
            while(l_longitud(laux)!=0){
                aux[j]=l_fin(laux)->elemento;
                l_eliminar(laux,l_fin(laux),fEliminar);
                cont++;
             }
          }
       }
       tabla[m->longitud_tabla*10];//PROBLEMA PARA REASIGNAR LA LONGITUD DEL ARREGLO

       m->longitud_tabla=10*10;
       for(int k=0;k<=m->cantidad_elementos;k++){
            tLista tl;
            crear_lista(&tl);
            tabla[k]=tl;
       }
       for(int h=0;h<m->cantidad_elementos;h++){
            tEntrada eaux=aux[h];
            int hc=m->hash_code(eaux->clave)%(m->longitud_tabla);
            tLista laux=tabla[hc];
            l_insertar(laux,l_fin(*(m->tabla_hash)),eaux);
            tabla[hc]=aux[h];
       }

    }
    return salida;
}




/**
 Elimina la entrada con clave C en M, si esta existe.
 La clave y el valor de la entrada son eliminados mediante las funciones fEliminarC y fEliminarV.
**/
void m_eliminar(tMapeo m, tClave c, void (*fEliminarC)(void *), void (*fEliminarV)(void *)){
    int hashC=m->hash_code(c)%(m->longitud_tabla);
    tLista laux=tabla[hashC];
    tPosicion pos=l_primera(laux);
    tEntrada eaux;
    int encontrado = 0;

    while(pos->siguiente!=NULL && encontrado==0){
        eaux=pos->elemento;
        if( m->comparador(c,eaux->clave)==0){
                encontrado=1;
                fEliminarC(eaux->clave);
                fEliminarV(eaux->valor);
                free(eaux);
        }
        pos=l_siguiente(laux,pos);
    }
}

/**
 Destruye el mapeo M, elimininando cada una de sus entradas.
 Las claves y valores almacenados en las entradas son eliminados mediante las funciones fEliminarC y fEliminarV.
**/
void m_destruir(tMapeo * m, void (*fEliminarC)(void *), void (*fEliminarV)(void *)){
     tMapeo mp=(*m);
     m_destruirAux(mp,fEliminarC,fEliminarV);
     free(mp);
}
extern void m_destruirAux(tMapeo m,void (*fEliminarC)(void *), void (*fEliminarV)(void *)){
    for(int i=0;i<m->longitud_tabla;i++){
        tLista laux=tabla[i];
        tPosicion pos=l_primera(laux);

        while(pos->siguiente!=NULL){
            tEntrada eaux=pos->elemento;
            fEliminarC(&(eaux->clave));
            fEliminarV(&(eaux->valor));

            pos=l_siguiente(laux,pos);
        }
        free(laux);
    }
}
/**
 Recupera el valor correspondiente a la entrada con clave C en M, si esta existe.
 Retorna el valor correspondiente, o NULL en caso contrario.
**/
extern tValor m_recuperar(tMapeo m, tClave c){
    tValor salida=NULL;
    int n_bloque=m->hash_code(c)%(m->longitud_tabla);
    tLista *lista_m=(m->tabla_hash);
    if (l_longitud(*(lista_m+n_bloque))!=0)
    {   tPosicion pos=l_primera(*(lista_m+n_bloque));
        int b_enc=1;
        while(pos!=l_fin(*(lista_m+n_bloque))&&(b_enc==1))
        {    tEntrada new_ent=l_recuperar(*(lista_m+n_bloque),pos);
            tClave * cc=new_ent->clave;
            if (m->comparador(cc,c)==0 )//0 igual - 1 distinto
                {   b_enc=0;
                    salida=new_ent->valor;
                }
            pos=l_siguiente(*(lista_m+n_bloque),pos);
        }
    }
    return salida;
}

//hago esta funcion aunque no hace nd por parametro
//extern void fEliminar(tElemento e){

//}
