#include <stdio.h>

int main() {
   FILE *fp;

FILE * file_pointer;
char buffer[30], c, cnt[100];
//Read
file_pointer = fopen("file.txt", "r");
   fp = fopen("output.txt", "w+");
while ((fgets(cnt,100,file_pointer)) != NULL){
  printf("%s", cnt);
  fprintf(fp,cnt);
}



   //Write to new file



   fclose(fp);
   fclose(file_pointer);

}
