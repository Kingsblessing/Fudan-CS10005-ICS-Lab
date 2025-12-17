/*
name: 杨添燚
id: 24300240173
*/
#include "cachelab.h"
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<unistd.h>
#define MAXN 30
char input[MAXN];
int hit=0,miss=0,evict=0;
typedef struct {
    int vaild;//有效位
    unsigned int tag;//标签
    int count;//用于LRU计数
}Cache;

void load(int count,unsigned int setindex,unsigned int tag,unsigned int E,Cache *cache){
    for(int i=0;i<E;i++){
        int index=setindex*E+i;
        if(cache[index].vaild==1&&cache[index].tag==tag){
            hit++;
            cache[index].count=count;
            return;
        }
    }
    miss++;
    for(int i=0;i<E;i++){
        int index=setindex*E+i;
        if(cache[index].vaild==0){
            cache[index].vaild=1;
            cache[index].tag=tag;
            cache[index].count=count;
            return;
        }
    }
    int lru_index=0;
    int min_count=cache[setindex*E].count;
    for(int i=1;i<E;i++){
        int index=setindex*E+i;
        if(cache[index].count<min_count){
            min_count=cache[index].count;
            lru_index=i;
        }
    }
    evict++;
    int index=setindex*E+lru_index;
    cache[index].tag=tag;
    cache[index].count=count;
    return;
}

int hextodec(char c){
    if(c>='0'&&c<='9') return c-'0';
    else if(c>='a'&&c<='f') return c-'a'+10;
    else if(c>='A'&&c<='F') return c-'A'+10;
    return 0;
}

int main(int argc,char *argv[])
{
    int opt=0;
    unsigned int s=0,E=0,b=0;
    unsigned int s_pow=0;
    char *t=NULL;
    while((opt=getopt(argc,argv,"hvs:E:b:t:"))!=-1){
        switch(opt){
            case 's':
                s=atoi(optarg);
                s_pow=1<<s;
                break;
            case 'E':
                E=atoi(optarg);
                break;
            case 'b':
                b=atoi(optarg);
                break;
            case 't':
                t=optarg;
                break;
            default:
                fprintf(stderr,"Invalid option:-%c\n",optopt);
                return 0;
        }
    }
    Cache *cache=(Cache *)malloc(sizeof(Cache)*E*s_pow);
    for(int i=0;i<E*s_pow;i++){
        cache[i].vaild=0;
        cache[i].tag=0;
        cache[i].count=0;
    }
    FILE *fp=fopen(t,"r");
    if(fp==NULL){
        fprintf(stderr,"Could not open file:%s\n",t);
        return 0;
    }
    int count=0;
    char input[MAXN];
    while(fgets(input,MAXN,fp)){
        count++;
        char op;
        unsigned address=0;
        int size;
        if(sscanf(input," %c %x,%d",&op,&address,&size)!=3) continue;
        if(op=='I') continue;
        unsigned int setindex=(address>>b)&(s_pow-1);
        unsigned int tag=address>>(b+s);
        switch(op){
            case 'L':
            case 'S':
                load(count,setindex,tag,E,cache);
                break;
            case 'M':
                load(count,setindex,tag,E,cache);
                hit++;
                break;
            default:
                fprintf(stderr, "Invalid operation: %c\n", op);
                break;
        }
    }
    fclose(fp);
    free(cache);
    printSummary(hit,miss,evict);
    return 0;
}