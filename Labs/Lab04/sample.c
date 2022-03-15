
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>


long get_number_memory(char *st);
long get_number(char *st);
long get_number_sscanf(char *st);

int main(int argc, char *argv[]){

        char *st = "a b 4 c d";

        long num1 = get_number(st);

        printf("%li\n", num1);

        long num2 = get_number_memory(st);

        printf("%li\n", num2);

        long num3 = get_number_sscanf(st);

        printf("%li\n", num3);

}

long get_number(char *st){
        // Uses indexing

        if (isdigit(st[4])){
                char * temp = &st[4];
                long num = strtol(temp, &temp, 10);
                return num;
        }

        return -1.11111111111111111;
}

long get_number_sscanf(char *st){
        // Uses the sscanf function

        long temp;
        char *discard;
        int success = sscanf(st, "%*[^0123456789]%li", &temp);

        if (success == 1){
                // printf(" works %li", temp);
                return temp;
        } else {
                printf("Failed");
                return -1.1111111111111111111111111;
        }
}
