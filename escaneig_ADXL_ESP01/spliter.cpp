#include "spliter.h"

string_t splits (char linia[])
{
  string_t t;
  int pos=0;
  int i=0;
  int j=0;
  int count=0;
  char caracter[15]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};

  for (i=0;linia[i]!='(';i++){/*if (linia[i]=='\0'){return t;}*/}
  for (j=(i+1);linia[j]!='\0';j++)
  {
    if (linia[j]==',')
    {
      if (count==1)
      {
        strcpy(t.nom,caracter);
        for (int c=0;caracter[c]!='\0';c++)
        {
          caracter[c]='\0';
        }
      }
      else if(count==2)
      {
        strcpy(t.potencia,caracter);
        
        for (int c=0;caracter[c]!='\0';c++)
        {
          caracter[c]='\0';
        }
      }
      count++;
      pos=0;
    }
    else
    {
      caracter[pos]=linia[j];
      pos++;
    }
  }
  return t;
}



/*int main(int argc, char const *argv[]) {
  char caracters[]={'+','C','W','L','A','P',':','(','4',',','O','N','O','3','6','8','C',',','-','9','0',',','"','d','c',':','5','3',':','7','c',':','1','5',':','2','9',':','9','d','"',',','1',',','-','3','6',',','0',')'};
  
  char caracters1[]={'+','C','W','L','A','P',':','4',',','O','N','O','3','6','8','C',',','-','9','0',',','"','d','c',':','5','3',':','7','c',':','1','5',':','2','9',':','9','d','"',',','1',',','-','3','6',',','0',')'};

  printf("%s\n",split(caracters1).nom);
  printf("%s\n",split(caracters1).potencia);
  return 0;
}*/
