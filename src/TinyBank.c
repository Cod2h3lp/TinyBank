  /* Simple implementation of a bank system. */ 


#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILENAME_SIZE 1024
#define MAX_LINE 2048

void log_current(char *credit_card, float withdraw_amount, char *gateway_name);
void log_deposit(char *credit_card, float withdraw_amount, char *gateway_name);

void transfer_log_current(char *credit_card, char *b_card, float transfer_amount);
void transfer_log_deposit(char *credit_card, char *b_card, float transfer_amount);

void loan_log_current(char *credit_card, char *b_card, float loan_amount); 
void loan_log_deposit(char *credit_card, char *b_card, float loan_amount);

void update_user_count(); 
int return_user_count(); 
void trim(char *s);
void sum_each_investor(char *investor);

char *token_to_card(char *token) {

    FILE* fp;
    char *card = NULL;
    char beneficiary_card[255];
    int bufferLength = 255;

    fp = fopen("auth.json", "r");
    while(fgets(beneficiary_card, bufferLength, fp))
    {
        char *ptr = strstr(beneficiary_card, token);
        if (ptr != NULL) 
        {   
            fclose(fp); 
            card = strtok(beneficiary_card, ":");
            return card;
        }
    }
    fclose(fp);

return NULL;
}


char *generate_token(int length) {    

    int mySeed = 25011984;
    int n; 
    char *string = "0123456789";
    size_t stringLen = strlen(string);        
    char *randomString = NULL;

    srand(time(NULL) * length + ++mySeed);

    if (length < 1) {
        length = 1;
    }

    randomString = malloc(sizeof(char) * (length +1));

    if (randomString) {
        short key = 0;

        for(n = 0;n < length;n++) {            
            key = rand() % stringLen;          
            randomString[n] = string[key];
        }

        randomString[length] = '\0';

        return randomString;        
    }
    else {
        printf("No memory");
        exit(1);
    }
}


int check_existence(char *credit_card, char *filename) {

    FILE* fp;
    char line[255];
    
    fp = fopen(filename, "r");

    while(fgets(line, 255, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {
             fclose(fp); 
             return 0; 

        }
    }

    fclose(fp);

    return 1;

}

void special_delete_line(char *ptr, char *filename) {

   char temp_filename[FILENAME_SIZE];
   char buffer[MAX_LINE]; 
   
   FILE *file, *temp;

   strcpy(temp_filename, filename);
   strcat(temp_filename, "temp");

    file = fopen(filename, "r");
    temp = fopen(temp_filename, "a");
    
    int status = 1;

    do {

    fgets(buffer, MAX_LINE, file);
     
    trim(buffer); 

    if (strcmp(buffer, ptr) != 0)
          fputs(buffer, temp);

    if (feof(file)) {
        status = 0; 
    }

    memset(buffer, 0, MAX_LINE);

   } while (status);


  fclose(file);
  fclose(temp);
  
  remove(filename);
  rename(temp_filename, filename);  
}

void delete_line(char *ptr, char *filename) {

   char temp_filename[FILENAME_SIZE];
   char buffer[MAX_LINE]; 
   
   FILE *file, *temp;

   strcpy(temp_filename, "temp____");
   strcat(temp_filename, filename);

    file = fopen(filename, "r");
    temp = fopen(temp_filename, "a");
    
    int status = 1;

    do {

    fgets(buffer, MAX_LINE, file);

    if (strcmp(buffer, ptr) != 0)
          fputs(buffer, temp);

    if (feof(file)) {
        status = 0; 
    }

    memset(buffer, 0, MAX_LINE);

   } while (status);


  fclose(file);
  fclose(temp);

  remove(filename);
  rename(temp_filename, filename);

  
  
}


void update_cash(char *credit_card, float amount, int operation)  {

    FILE* fp;
    FILE* fp_write;
    char string[100];
    char *balance_line = NULL;
    char new_cash[25] = {0};
    float saved_balance; 
    float total_balance; 


    fp = fopen("cash.json", "r");
    char line[255];

     while(fgets(line, 255, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {
             strcpy(string, ptr);
             balance_line = strtok(ptr, ":");
             strcpy(new_cash, balance_line); 
             balance_line = strtok(NULL, ":"); 
             saved_balance = atof(balance_line);

             if(operation == 1) {
                 total_balance = saved_balance - amount; 
             }

             if(operation == 2) {
                 total_balance = saved_balance + amount;
             }
            
             strcat(new_cash, ":"); 

             int len = snprintf(NULL, 0, "%f", total_balance);
             char *result = malloc(len + 1);
             snprintf(result, len + 1, "%f", total_balance);
             strcat(new_cash, result);
             printf("new_cash: %s\n", new_cash);

        }
    }

    fclose(fp);
    delete_line(string, "cash.json"); 

    fp_write = fopen("cash.json", "a"); 
    fwrite(new_cash , 1 , sizeof(new_cash) , fp_write );
    fwrite("\n", 1, sizeof(char), fp_write);
    fclose(fp_write);



}

void update_current_balance(char *credit_card, float balance) {

    FILE* fp;
    FILE* fp_write;

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    char new_balance[25] = {0};
    char string[100];

    fp = fopen("current.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcpy(new_balance, balance_line);
            strcat(new_balance, ":"); 
            
            int len = snprintf(NULL, 0, "%f", balance);
            char *result = malloc(len + 1);
            snprintf(result, len + 1, "%f", balance);

            strcat(new_balance, result);

        }
    }
    fclose(fp);

    printf("\nBalance line: %s\n", new_balance);
    delete_line(string, "current.json");

    fp_write = fopen("current.json", "a"); 
    fwrite(new_balance , 1 , sizeof(new_balance) , fp_write );
    fwrite("\n", sizeof(char), 1, fp_write);
    fclose(fp_write);

}

void update_deposit_balance(char *credit_card, float balance) {

    FILE* fp;
    FILE* fp_write;

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    char new_balance[25] = {0};
    char string[100];

    fp = fopen("deposit.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcpy(new_balance, balance_line);
            strcat(new_balance, ":"); 
            
            int len = snprintf(NULL, 0, "%f", balance);
            char *result = malloc(len + 1);
            snprintf(result, len + 1, "%f", balance);

            strcat(new_balance, result);

        }

    }
    fclose(fp);

    printf("\nBalance line: %s\n", new_balance);
    delete_line(string, "deposit.json");

    fp_write = fopen("deposit.json", "a"); 
    fwrite(new_balance , 1 , sizeof(new_balance) , fp_write );
    fwrite("\n", sizeof(char), 1, fp_write);
    fclose(fp_write);

}


char *current_balance(char *credit_card) {

    FILE* fp;
    int bufferLength = 255;
    char line[bufferLength];
    char *balance = NULL;

    fp = fopen("current.json", "r");
    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {   

            balance = strtok(ptr, ":"); 
            balance = strtok(NULL, ":");
            fclose(fp); 
            return balance;
        }
    }
    fclose(fp);

return NULL;

}

char *cash_balance(char *credit_card) {

    FILE* fp;
    int bufferLength = 255;
    char line[bufferLength];
    char *balance = NULL;

    fp = fopen("cash.json", "r");
    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {   

            balance = strtok(ptr, ":"); 
            balance = strtok(NULL, ":");
            fclose(fp); 
            return balance;
        }

        memset(line, 0, 255);
    }
    fclose(fp);

return NULL;

}

char *deposit_balance(char *credit_card) {

    FILE* fp;
    int bufferLength = 255;
    char line[bufferLength];
    char *balance = NULL;

    fp = fopen("deposit.json", "r");
    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, credit_card);
        if (ptr != NULL) 
        {   

            balance = strtok(ptr, ":"); 
            balance = strtok(NULL, ":");
            fclose(fp); 
            return balance;
        }
    }
    fclose(fp);

return NULL;

}



int authentication_json(char *string) {

    FILE* fp;
    int bufferLength = 255;
    char line[bufferLength];

    fp = fopen("auth.json", "r");
    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, string);
        if (ptr != NULL) 
        {
            fclose(fp);
            return 1;
            break;
        }
    }
    fclose(fp);

return 0;

}

void withdraw_deposit(char *credit_card, char *password) {

       int card_check = check_existence(credit_card, "current.json"); 
       if(card_check == 0) {
           printf("\nCard already registered as current.\n");
           exit(0);
       }
       
       char *token = generate_token(6); 
       char gateway[4]; 
       char gateway_name[50];

       char response[4];
       char pass_created[20];
       char verify[21] = {0}; 
       char record[21] = {0};
       char setup[21] = {0};
       char templance[6]; 

       float deposit_amount;
       float withdraw_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password); 
       
       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 != 1) {

           printf("Credit card not registered...\n");
           printf("Would you like to register it yes/no? ");
           scanf("%s", response);

           if(strstr("yes", response)) {
                printf("Please enter a password: "); 
                scanf("%s", pass_created);
                printf("Setting up password...\n");
                printf("Creating record...\n");
                
                strcat(record, credit_card); 
                strcat(record, ":");
                strcat(record, pass_created);
                strcat(record, ":"); 
                strcat(record, token);
                strcat(record, "\n");

                FILE *fp; 
                fp = fopen("auth.json", "a");
                fprintf(fp, record);
                fclose(fp);

                fp = fopen("deposit.json", "a"); 
                strcpy(setup, credit_card);
                strcat(setup, ":");
                strcat(setup, "0.000000");
                strcat(setup, "\n");
                fprintf(fp, setup);
                fclose(fp); 

                fp = fopen("cash.json", "a");
                fprintf(fp, setup); 
                fclose(fp); 

                update_user_count(); 

                sleep(2);

           }else if(strstr("no", response)) {
                printf("Exiting system...\n");
                exit(0);
           }
       }else if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0); 

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");
           char *balance_string = deposit_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string); 

           printf("\nType Withdraw amount: "); 
           scanf("%f", &withdraw_amount); 
           
           deposit_amount = atof(templance); 
           balance = deposit_amount - withdraw_amount;  
           update_deposit_balance(credit_card, balance);
           update_cash(credit_card, withdraw_amount, 2); 
           
           printf("\nAre you using a gateway? "); 
           scanf("%s", gateway); 
           
           if(strstr("yes", gateway)) {
         
              printf("\nGateway name: "); 
              scanf("%s", gateway_name); 
              
              log_deposit(credit_card, withdraw_amount, gateway_name);
           }else{
              log_deposit(credit_card, withdraw_amount, "default");
           }

           exit(0);


       }

}

void withdraw_current(char *credit_card, char *password) {
       
 
       int card_check = check_existence(credit_card, "deposit.json"); 
       
       if(card_check == 0) {
           printf("\nCard already registered as deposit...\n");
           exit(0);
       }
        
       char *token = generate_token(6); 

       char gateway[4]; 
       char gateway_name[50]; 

       char response[4];
       char pass_created[20];
       char verify[21] = {0}; 
       char record[21] = {0};
       char setup[21] = {0};
       char templance[6]; 

       float current_amount;
       float withdraw_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password); 
       
       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 != 1) {

           printf("Credit card not registered...\n");
           printf("Would you like to register it yes/no? ");
           scanf("%s", response);

           if(strstr("yes", response)) {
                printf("Please enter a password: "); 
                scanf("%s", pass_created);
                printf("Setting up password...\n");
                printf("Creating record...\n");
                
                strcat(record, credit_card); 
                strcat(record, ":");
                strcat(record, pass_created);
                strcat(record, ":"); 
                strcat(record, token);
                strcat(record, "\n");

                FILE *fp; 
                fp = fopen("auth.json", "a");
                fprintf(fp, record);
                fclose(fp);

                fp = fopen("current.json", "a"); 
                strcpy(setup, credit_card);
                strcat(setup, ":");
                strcat(setup, "0.000000");
                strcat(setup, "\n");
                fprintf(fp, setup);
                fclose(fp); 
                
                fp = fopen("cash.json", "a");
                fprintf(fp, setup); 
                fclose(fp); 
                
                update_user_count();

                sleep(2);

           }else if(strstr("no", response)) {
                printf("Exiting system...\n");
                exit(0);
           }
       }else if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0); 

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");
           char *balance_string = current_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string); 

           printf("\nType Withdraw amount: "); 
           scanf("%f", &withdraw_amount); 
           
           current_amount = atof(templance); 
           balance = current_amount - withdraw_amount;  

           update_current_balance(credit_card, balance);
           update_cash(credit_card, withdraw_amount, 2);
             
           printf("\nAre you using a gateway? "); 
           scanf("%s", gateway); 
           
           if(strstr("yes", gateway)) {
         
              printf("\nGateway name: "); 
              scanf("%s", gateway_name); 
              
              log_current(credit_card, withdraw_amount, gateway_name);
           }else{
              log_current(credit_card, withdraw_amount, "default");
           }

           exit(0);


       }

}


void deposit_current(char *credit_card, char *password) {
       
       int card_check = check_existence(credit_card, "current.json"); 
       if(card_check == 0) {
           printf("\nCard already registered as current.\n");
           exit(0);
       }

       char *token = generate_token(6); 

       char response[4];
       char pass_created[20];
       char verify[21] = {0}; 
       char record[21] = {0};
       char setup[21] = {0};
       char templance[6]; 
       char current_lance[6]; 

       float current_amount;
       float deposit_amount;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password); 
       
       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 != 1) {

           printf("Credit card not registered...\n");
           printf("Would you like to register it yes/no? ");
           scanf("%s", response);

           if(strstr("yes", response)) {
                printf("Please enter a password: "); 
                scanf("%s", pass_created);
                printf("Setting up password...\n");
                printf("Creating record...\n");
                
                strcat(record, credit_card); 
                strcat(record, ":");
                strcat(record, pass_created);
                strcat(record, ":"); 
                strcat(record, token);
                strcat(record, "\n");

                FILE *fp; 
                fp = fopen("auth.json", "a");
                fprintf(fp, record);
                fclose(fp);

                fp = fopen("current.json", "a"); 
                strcpy(setup, credit_card);
                strcat(setup, ":");
                strcat(setup, "0.000000");
                strcat(setup, "\n");
                fprintf(fp, setup);
                fclose(fp); 
                
                fp = fopen("cash.json", "a");
                fprintf(fp, setup); 
                fclose(fp); 

                update_user_count();


                sleep(2);

           }else if(strstr("no", response)) {
                printf("Exiting system...\n");
                exit(0);
           }
       }else if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0); 

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = cash_balance(credit_card);
           strcpy(templance, balance_string);

           char *c_lance = current_balance(credit_card);
           strcpy(current_lance, c_lance);

           printf("\n[+] Current balance: %s\n", templance); 
          
           printf("\nType Deposit amount: "); 
           scanf("%f", &deposit_amount); 
           
           current_amount = deposit_amount + atof(current_lance);            
           
           update_current_balance(credit_card, current_amount);
           update_cash(credit_card, deposit_amount, 1); 
           

           exit(0);


       }

}

void deposit_deposit(char *credit_card, char *password) {

       int card_check = check_existence(credit_card, "current.json"); 
       if(card_check == 0) {
           printf("\nCard already registered as current.\n");
           exit(0);
       }

       char *token = generate_token(6); 

       char response[4];
       char pass_created[20];
       char verify[21] = {0}; 
       char record[21] = {0};
       char setup[21] = {0};
       char templance[6]; 
       char current_lance[6]; 

       float current_amount;
       float deposit_amount;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password); 
       
       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 != 1) {

           printf("Credit card not registered...\n");
           printf("Would you like to register it yes/no? ");
           scanf("%s", response);

           if(strstr("yes", response)) {
                printf("Please enter a password: "); 
                scanf("%s", pass_created);
                printf("Setting up password...\n");
                printf("Creating record...\n");
                
                strcat(record, credit_card); 
                strcat(record, ":");
                strcat(record, pass_created);
                strcat(record, ":"); 
                strcat(record, token);
                strcat(record, "\n");

                FILE *fp; 
                fp = fopen("auth.json", "a");
                fprintf(fp, record);
                fclose(fp);

                fp = fopen("deposit.json", "a"); 
                strcpy(setup, credit_card);
                strcat(setup, ":");
                strcat(setup, "0.000000");
                strcat(setup, "\n");
                fprintf(fp, setup);
                fclose(fp); 
                
                fp = fopen("cash.json", "a");
                fprintf(fp, setup); 
                fclose(fp); 

                update_user_count();

                sleep(2);

           }else if(strstr("no", response)) {
                printf("Exiting system...\n");
                exit(0);
           }
       }else if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0); 

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = cash_balance(credit_card);
           strcpy(templance, balance_string);

           char *c_lance = deposit_balance(credit_card);
           strcpy(current_lance, c_lance);

           printf("\n[+] Current balance: %s\n", templance); 
          
           printf("\nType Deposit amount: "); 
           scanf("%f", &deposit_amount); 
           
           current_amount = deposit_amount + atof(current_lance);            
           
           update_deposit_balance(credit_card, current_amount);
           update_cash(credit_card, deposit_amount, 1); 
           

           exit(0);
       }
}


void log_current(char *credit_card, float withdraw_amount, char *gateway_name) {

      FILE* fp;
      
      char path[50] = {0}; 
      char logs[100] = {0}; 

      strcpy(path, "log_c/extract/");
      strcat(path, credit_card); 
      strcat(path, ".json");

      int len = snprintf(NULL, 0, "%f", withdraw_amount);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", withdraw_amount);

      strcat(logs, "-");  
      strcat(logs, result); 
      strcat(logs, ":");
      strcat(logs, gateway_name); 
      
      printf("\nLog: %s\n", logs); 

      fp = fopen(path, "a");
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 

} 

void log_deposit(char *credit_card, float withdraw_amount, char *gateway_name) {

      FILE* fp;
      
      char path[50] = {0}; 
      char logs[100] = {0}; 

      strcpy(path, "log_d/extract/");
      strcat(path, credit_card); 
      strcat(path, ".json");

      int len = snprintf(NULL, 0, "%f", withdraw_amount);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", withdraw_amount);

      strcat(logs, "-");  
      strcat(logs, result); 
      strcat(logs, ":");
      strcat(logs, gateway_name); 
      
      printf("\nLog: %s\n", logs); 

      fp = fopen(path, "a");
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 

} 

void transfer_current_current(char *credit_card, char *password) {
       
       int card_check = check_existence(credit_card, "deposit.json"); 
       
       if(card_check == 0) {
           printf("\nCard already registered as deposit...\n");
           exit(0);
       }

       char token[6]; 
       char verify[21] = {0};
       char templance[6];
       char newlance[6];
       char bcard[25]; 
       
       float transfer_amount;
       float current_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password);


       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = current_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string);

           printf("\nType Transfer amount: "); 
           scanf("%f", &transfer_amount); 
           
           current_amount = atof(templance); 
           balance = current_amount - transfer_amount;  
           
           update_current_balance(credit_card, balance);

           current_amount = 0.0;
           balance = 0.0;  

           printf("\nEnter account token: "); 
           scanf("%s", token); 
           
           char *beneficiary_card = token_to_card(token); 
           strcpy(bcard, beneficiary_card);  
            
           char *new_string = current_balance(beneficiary_card);
           strcpy(newlance, new_string);
           printf("\n[+] Beneficiary balance: %s\n", newlance);
           
           current_amount = atof(newlance); 
           balance = current_amount + transfer_amount;

           update_current_balance(bcard, balance);
           transfer_log_current(credit_card, bcard, transfer_amount);

           
       }
}

void transfer_current_deposit(char *credit_card, char *password) {
        
       int card_check = check_existence(credit_card, "deposit.json"); 
       
       if(card_check == 0) {
           printf("\nCard already registered as deposit...\n");
           exit(0);
       }

       char token[6]; 
       char verify[21] = {0};
       char templance[6];
       char newlance[6];
       char bcard[25]; 
       
       float transfer_amount;
       float current_amount;
       float deposit_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password);


       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = current_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string);

           printf("\nType Transfer amount: "); 
           scanf("%f", &transfer_amount); 
           
           current_amount = atof(templance); 
           balance = current_amount - transfer_amount;  
           
           update_current_balance(credit_card, balance);

           current_amount = 0.0;
           balance = 0.0;  

           printf("\nEnter account token: "); 
           scanf("%s", token); 
           
           char *beneficiary_card = token_to_card(token); 
           strcpy(bcard, beneficiary_card);  
            
           char *new_string = deposit_balance(beneficiary_card);
           strcpy(newlance, new_string);
           printf("\n[+] Beneficiary balance: %s\n", newlance);
           
           deposit_amount = atof(newlance); 
           balance = deposit_amount + transfer_amount;

           update_deposit_balance(bcard, balance);
           transfer_log_current(credit_card, bcard, transfer_amount);
       }

}


void transfer_deposit_current(char *credit_card, char *password) {
        
       int card_check = check_existence(credit_card, "current.json"); 
       if(card_check == 0) {
           printf("\nCard already registered as current.\n");
           exit(0);
       }

       char token[6]; 
       char verify[21] = {0};
       char templance[6];
       char newlance[6];
       char bcard[25]; 
       
       float transfer_amount;
       float current_amount;
       float deposit_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password);


       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = deposit_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string);

           printf("\nType Transfer amount: "); 
           scanf("%f", &transfer_amount); 
           
           current_amount = atof(templance); 
           balance = current_amount - transfer_amount;

           update_deposit_balance(credit_card, balance);

           current_amount = 0.0;
           balance = 0.0; 

           printf("\nEnter account token: "); 
           scanf("%s", token); 

           char *beneficiary_card = token_to_card(token); 
           strcpy(bcard, beneficiary_card);  
            
           char *new_string = current_balance(beneficiary_card);
           strcpy(newlance, new_string);
           printf("\n[+] Beneficiary balance: %s\n", newlance);

           deposit_amount = atof(newlance); 
           balance = deposit_amount + transfer_amount;

           update_current_balance(bcard, balance);
           transfer_log_deposit(credit_card, bcard, transfer_amount);

       }
}    

void transfer_deposit_deposit(char *credit_card, char *password) {
       
       int card_check = check_existence(credit_card, "current.json"); 
       if(card_check == 0) {
           printf("\nCard already registered as current.\n");
           exit(0);
       }
       
       char token[6]; 
       char verify[21] = {0};
       char templance[6];
       char newlance[6];
       char bcard[25]; 
       
       float transfer_amount;
       float current_amount;
       float deposit_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password);


       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = deposit_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string);

           printf("\nType Transfer amount: "); 
           scanf("%f", &transfer_amount); 
           
           current_amount = atof(templance); 
           balance = current_amount - transfer_amount;

           update_deposit_balance(credit_card, balance);

           current_amount = 0.0;
           balance = 0.0; 

           printf("\nEnter account token: "); 
           scanf("%s", token); 

           char *beneficiary_card = token_to_card(token); 
           strcpy(bcard, beneficiary_card);  
            
           char *new_string = deposit_balance(beneficiary_card);
           strcpy(newlance, new_string);
           printf("\n[+] Beneficiary balance: %s\n", newlance);

           deposit_amount = atof(newlance); 
           balance = deposit_amount + transfer_amount;

           update_deposit_balance(bcard, balance);
           transfer_log_deposit(credit_card, bcard, transfer_amount);

       }
}    


void transfer_log_current(char *credit_card, char *b_card, float transfer_amount) {
       
      FILE* fp;
      
      char path[50] = {0}; 
      char logs[100] = {0}; 

      strcpy(path, "log_c/transfers/");
      strcat(path, credit_card); 
      strcat(path, ".json");

      int len = snprintf(NULL, 0, "%f", transfer_amount);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", transfer_amount);

      strcat(logs, credit_card);
      strcat(logs, " -> "); 
      strcat(logs, b_card);
      strcat(logs, ":");    
      strcat(logs, result);  
      
      printf("\nLog: %s\n", logs); 

      fp = fopen(path, "a");
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 
      
      fp = fopen("admin/transfers.json", "a"); 
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 



}

void transfer_log_deposit(char *credit_card, char *b_card, float transfer_amount) {
       
      FILE* fp;
      
      char path[50] = {0}; 
      char logs[100] = {0}; 

      strcpy(path, "log_d/transfers/");
      strcat(path, credit_card); 
      strcat(path, ".json");

      int len = snprintf(NULL, 0, "%f", transfer_amount);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", transfer_amount);

      strcat(logs, credit_card);
      strcat(logs, " -> "); 
      strcat(logs, b_card);
      strcat(logs, ":");    
      strcat(logs, result);  
      
      printf("\nLog: %s\n", logs); 

      fp = fopen(path, "a");
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 

      fp = fopen("admin/transfers.json", "a"); 
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp);



}

void update_user_count() {

    FILE* fp;
    FILE* fp_write;

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    int count; 
    char new_balance[25] = {0};
    char string[100];

    fp = fopen("loan.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, "users");
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcat(new_balance, balance_line);
            strcat(new_balance, ":");
            balance_line = strtok(NULL, ":"); 
            count = atoi(balance_line) + 1; 

            int len = snprintf(NULL, 0, "%d", count);
            char *result = malloc(len + 1);
            snprintf(result, len + 1, "%d", count);
            
            strcat(new_balance, result); 
            printf("new_balance: %s\n", new_balance);
          
        }
    }
    fclose(fp);

    delete_line(string, "loan.json");
    fp_write = fopen("loan.json", "a"); 
    fprintf(fp_write, new_balance);
    fprintf(fp_write, "\n");
    fclose(fp_write);

      
}

float return_initial_value() {

    FILE* fp;

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    int count; 
    char new_balance[25] = {0};
    char string[100];

    fp = fopen("loan.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, "initial_value");
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcat(new_balance, balance_line);
            strcat(new_balance, ":");
            balance_line = strtok(NULL, ":"); 
            count = atof(balance_line);
            fclose(fp);
            return count; 
          
        }
    }
    fclose(fp);
    return 1; 
}


int return_user_count() {

    FILE* fp;

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    int count; 
    char new_balance[25] = {0};
    char string[100];

    fp = fopen("loan.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, "users");
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcat(new_balance, balance_line);
            strcat(new_balance, ":");
            balance_line = strtok(NULL, ":"); 
            count = atoi(balance_line);
            fclose(fp); 
            return count; 
          
        }
    }
    fclose(fp);
    return 1; 

}

void reset_user_value() {

    FILE* fp;
    FILE* fp_write;

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    
   // int count = return_user_count(); 

    int count = return_user_count(); 

    char new_balance[25] = {0};
    char string[100];

    fp = fopen("loan.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, "users");
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcat(new_balance, balance_line);
            strcat(new_balance, ":");
            balance_line = strtok(NULL, ":");  

            int len = snprintf(NULL, 0, "%d", count);
            char *result = malloc(len + 1);
            snprintf(result, len + 1, "%d", count);
            strcat(new_balance, result);
            strcat(new_balance, "\n"); 
            printf("new_balance: %s\n", new_balance);
          
        }
    }
    fclose(fp);

    delete_line(string, "loan.json");
    fp_write = fopen("loan.json", "a"); 
    fprintf(fp_write, new_balance);
    fprintf(fp_write, "\n");
    fclose(fp_write);
}


float get_max_loan() {

    int number_of_users = return_user_count();
    float initial_value = return_initial_value(); 
    
    printf("Number of users: %d\n", number_of_users);
    printf("initial_value: %f\n", initial_value);
    
    float max_loan = initial_value / number_of_users; 
    printf("max_loan: %f\n", max_loan);  

    return max_loan; 
}

float get_max_current_invest(char *credit_card) {

    char templance[6];  
    float current_amount; 
    
    char *balance_string = current_balance(credit_card);
    strcpy(templance, balance_string); 
    printf("\n[+] Current balance: %s\n", balance_string); 
    current_amount = atof(templance);
    return current_amount; 
    
}

float get_max_deposit_invest(char *credit_card) {

    char templance[6];  
    float current_amount; 
    
    char *balance_string = deposit_balance(credit_card);
    strcpy(templance, balance_string); 
    printf("\n[+] Current balance: %s\n", balance_string); 
    current_amount = atof(templance);
    return current_amount; 
    
}

void update_loan(float max_loan, float loan) {
    
    FILE* fp;
    FILE* fp_write;

    float loaned = max_loan - loan; 
    float total_loan = max_loan + loaned; 

    int bufferLength = 255;
    char line[bufferLength];
    char *balance_line = NULL;
    char new_balance[25] = {0};
    char string[100];

    fp = fopen("loan.json", "r");

    while(fgets(line, bufferLength, fp))
    {
        char *ptr = strstr(line, "initial_value");
        if (ptr != NULL) 
        {
            strcpy(string, ptr);
            balance_line = strtok(ptr, ":");
            strcat(new_balance, balance_line);
            strcat(new_balance, ":");
            balance_line = strtok(NULL, ":"); 

            int len = snprintf(NULL, 0, "%f", total_loan);
            char *result = malloc(len + 1);
            snprintf(result, len + 1, "%f", total_loan);
            
            strcat(new_balance, result); 
            printf("new_balance: %s\n", new_balance);
          
        }
    }
    fclose(fp);

    delete_line(string, "loan.json");
    fp_write = fopen("loan.json", "a"); 
    fprintf(fp_write, new_balance);
    fprintf(fp_write, "\n");
    fclose(fp_write);

}

void add_loan_current(char *credit_card, char *password) {
    
       char verify[21] = {0};
       char newlance[6];
       char bcard[25]; 

       float current_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password);


       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           float max_loan = get_max_loan();
           float loaned; 

           printf("Loan Available: %f\n", max_loan);
           printf("\n"); 

           printf("Loan: "); 
           scanf("%f", &loaned); 
           printf("Loaned: %f\n", loaned); 

           update_loan(max_loan, loaned);
           reset_user_value();

           strcpy(bcard, credit_card);  
            
           char *new_string = current_balance(credit_card);
           strcpy(newlance, new_string);
           printf("\n[+] Current balance: %s\n", newlance);
           
           current_amount = atof(newlance); 
           balance = current_amount + loaned;

           update_current_balance(bcard, balance);
           loan_log_current(credit_card, "TinyBank", loaned);
           
       }

}

void add_loan_deposit(char *credit_card, char *password) {

       char verify[21] = {0};
       char templance[6];
       char bcard[25]; 
       
       float deposit_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password);


       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           float max_loan = get_max_loan();
           float loaned; 

           printf("Loan Available: %f\n", max_loan);
           printf("\n"); 

           printf("Loan: "); 
           scanf("%f", &loaned); 
           printf("Loaned: %f\n", loaned); 

           update_loan(max_loan, loaned);
           reset_user_value();

           char *balance_string = deposit_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string); 

           strcpy(bcard, credit_card);  

           deposit_amount = atof(templance); 
           balance = deposit_amount + loaned;

           update_deposit_balance(bcard, balance);
           loan_log_deposit(credit_card, "TinyBank", loaned); 

           
       }

} 

void get_loan(char *credit_card, char *password) {
  
  
    int card_check = check_existence(credit_card, "current.json"); 
    if(card_check == 0) {
           add_loan_current(credit_card, password); 
    }else{
           add_loan_deposit(credit_card, password); 
    }

    
}

void loan_log_current(char *credit_card, char *b_card, float loan_amount) {
       
      FILE* fp;
      
      char path[50] = {0}; 
      char logs[100] = {0}; 

      strcpy(path, "log_c/loans/");
      strcat(path, credit_card); 
      strcat(path, ".json");

      int len = snprintf(NULL, 0, "%f", loan_amount);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", loan_amount);

      strcat(logs, b_card);
      strcat(logs, " -> "); 
      strcat(logs, credit_card);
      strcat(logs, ":");    
      strcat(logs, result);  
      
      printf("\nLog: %s\n", logs); 

      fp = fopen(path, "a");
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 

      fp = fopen("admin/loans.json", "a"); 
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp);

}

void loan_log_deposit(char *credit_card, char *b_card, float loan_amount) {
       
      FILE* fp;
      
      char path[50] = {0}; 
      char logs[100] = {0}; 

      strcpy(path, "log_d/loans/");
      strcat(path, credit_card); 
      strcat(path, ".json");

      int len = snprintf(NULL, 0, "%f", loan_amount);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", loan_amount);

      strcat(logs, b_card);
      strcat(logs, " -> "); 
      strcat(logs, credit_card);
      strcat(logs, ":");    
      strcat(logs, result);  
      
      printf("\nLog: %s\n", logs); 

      fp = fopen(path, "a");
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp); 

      fp = fopen("admin/loans.json", "a"); 
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp);

}

void mutual_log(char *credit_card, float invested) {

      FILE* fp;
       
      char logs[100] = {0}; 

      int len = snprintf(NULL, 0, "%f", invested);
      char *result = malloc(len + 1);
      snprintf(result, len + 1, "%f", invested);

      strcat(logs, credit_card);
      strcat(logs, ":");    
      strcat(logs, result);  
      
      printf("\nLog: %s\n", logs); 

      fp = fopen("funds/mutual/mutual_fund.json", "a"); 
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp);

      memset(logs, 0, 100);
      strcpy(logs, credit_card);
      fp = fopen("funds/mutual/investors.json", "a"); 
      fprintf(fp, logs);
      fprintf(fp, "\n");
      fclose(fp);


}


void mutual_current(char *credit_card, char *password) {

       char verify[21] = {0}; 
       char templance[6]; 

       float current_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password); 
       
       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card); 

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           float max_to_invest = get_max_current_invest(credit_card);
           float invested; 

           printf("Investiment Available: %f\n", max_to_invest);
           printf("\n"); 

           char *balance_string = current_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string); 

           printf("\nType invest amount: "); 
           scanf("%f", &invested); 
           
           current_amount = atof(templance); 
           balance = current_amount - invested;  

           update_current_balance(credit_card, balance);
           mutual_log(credit_card, invested);

       }
}


void mutual_deposit(char *credit_card, char *password) {

 
       char verify[21] = {0}; 
       char templance[6]; 

       float deposit_amount;
       float balance;

       strcat(verify, credit_card); 
       strcat(verify, ":");
       strcat(verify, password); 
       
       int status_card = authentication_json(verify);
       int status_card2 = authentication_json(credit_card);

       if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

       }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");

           char *balance_string = deposit_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Current balance: %s\n", balance_string); 

           float max_to_invest = get_max_deposit_invest(credit_card);
           float invested; 

           printf("Investiment Available: %f\n", max_to_invest);
           printf("\n"); 

           printf("\nType invest amount: "); 
           scanf("%f", &invested); 
           
           deposit_amount = atof(templance); 
           balance = deposit_amount - invested;  
           update_deposit_balance(credit_card, balance);
           mutual_log(credit_card, invested);

      }

}

void invest_mutual_fund(char *credit_card, char *password) {

    int card_check = check_existence(credit_card, "current.json"); 
    if(card_check == 0) {
           mutual_current(credit_card, password); 
    }else{
           mutual_deposit(credit_card, password); 
    }

}

int remove_duplicate(char arr[][20], int n)
{
    
    
    int i, j, k, size;
    size = n;  


    for ( i = 0; i < size; i ++)  
    {  

        for ( j = i + 1; j < size; j++)  
        {  
            // use if statement to check duplicate element  
            if (!strcmp(arr[i], arr[j]))  
            {  
                // delete the current position of the duplicate element  
                for ( k = j; k < size - 1; k++)  
                {  
                   strcpy(arr[k], arr[k+1]);
                    
                }  
                // decrease the size of array after removing duplicate element  
                size--;  
                  
            // if the position of the elements is changes, don't increase the index j  
                j--;      
            }  
        }  
    } 
    return size;
}

void clean_investors() {

    FILE* fp; 
    char string[255];
    int bufferLength = 255;
    int counter = 0; 

    fp = fopen("funds/mutual/investors.json", "r");
    while(fgets(string, bufferLength, fp))
    {
       counter++; 
    }

    fclose(fp);
    
    counter = counter -1; 
    int count = 0; 
    char cards[counter][20]; 

    fp = fopen("funds/mutual/investors.json", "r");
    while(fgets(string, bufferLength, fp))
    {
        strcpy(&cards[count][0], string);
        if(count < counter) {
            count++;           
        }
    }

    fclose(fp);

    printf("%d\n", count);
    count = count + 1;

    int size = remove_duplicate(cards, count);
    int i; 
    remove("funds/mutual/investors.json");
    fp = fopen("funds/mutual/investors.json", "w");
   
    for(i = 0; i < size; i++) {
        fprintf(fp, cards[i]);
         
    }

    fclose(fp);
}

void trim(char *s)
{
  int i = strlen(s) - 1;
  
  while (i >= 0)
  {
    if (s[i] == ' ' || s[i] == '\n' ||
        s[i] == '\t') i--;
    else break;
  }
 
  s[i + 1] = '\0';
}


float sum_all_money() {

    FILE* fp; 
    FILE *fr; 

    char investor[255];
    int bufferLength = 255;


    char string[255]; 

    char *balance_line = NULL;
    char *balance = NULL;

    float sum = 0;
    float value;
    

    fp = fopen("funds/mutual/investors.json", "r");

     while(fgets(investor, bufferLength, fp))
    {     
          fr = fopen("funds/mutual/temp_mutual.json", "r");
          trim(investor); 

          while(fgets(string, bufferLength, fr)) {

               char *ptr = strstr(string, investor);
                if (ptr != NULL) {
                        
                        balance_line = strtok(ptr, ":");
                        balance = strtok(NULL, ":");
                        value = atof(balance);
                        sum = sum+value;

                }
                
                    
           }
           fclose(fr);
    
      }
      fclose(fp);  
    

  remove("funds/mutual/mutual_fund.json");
  rename("funds/mutual/temp_mutual.json", "funds/mutual/mutual_fund.json");
   
   return sum;
}


void gather_each_investor() {
    
    FILE* fp;

    char investor[255];
    int bufferLength = 255;


    fp = fopen("funds/mutual/investors.json", "r");

     while(fgets(investor, bufferLength, fp))
    {     
          trim(investor); 
          sum_each_investor(investor);
    }
    fclose(fp);


}


void sum_each_investor(char *investor) {

    FILE* fr; 
    FILE* fp; 

    char string[255]; 
    int bufferLength = 255;
    char *balance_line = NULL;
    char *balance = NULL;

    float sum;
    float value;
    char full_string[100];
    
    strcpy(full_string, investor); 

    fr = fopen("funds/mutual/mutual_fund.json", "r"); 
    
    while(fgets(string, bufferLength, fr)) {
                
                char *ptr = strstr(string, investor);
                if (ptr != NULL) {
                        
                        balance_line = strtok(ptr, ":");
                        strcpy(full_string, balance_line); 
                        balance = strtok(NULL, ":");
                        value = atof(balance);
                        sum = sum+value; 
                        
                }
               
          } 

    fclose(fr);

    fp = fopen("funds/mutual/temp_mutual.json", "a");
    
    strcat(full_string, ":");

    int len = snprintf(NULL, 0, "%f", sum);
    char *result = malloc(len + 1);
    snprintf(result, len + 1, "%f", sum);

    strcat(full_string, result);
    fprintf(fp, full_string);
    fprintf(fp, "\n");
    fclose(fp);

}

void invest(char *investor, float invested, float sum) {

     float percentage = 5; 
     float result; 
     float x;
     char new_cash[25] = {0};
     float y;   
     float z; 

     x = invested * 100 / sum; 
     result = sum + (sum * percentage) / 100;
     float rest = result - sum;      
     y = rest * x / 100;  

     z = invested + y; 
     float earned = z; 

     strcpy(new_cash, investor); 
     strcat(new_cash, ":"); 

     int len = snprintf(NULL, 0, "%f", earned);
     char *result2 = malloc(len + 1);
     snprintf(result2, len + 1, "%f", earned);
     strcat(new_cash, result2);
     printf("new_cash = %s\n", new_cash);
    
     
     FILE *fp; 
     fp = fopen("funds/mutual/temp_mutual.json", "a");
     fprintf(fp, new_cash);
     fprintf(fp, "\n");
     fclose(fp); 

     
      
} 


void start_investing(float sum) {
    
    FILE* fp;

    char string[255];
    float invested;
    int bufferLength = 255;
    char *balance_line = NULL;

    char investor[20]; 


    fp = fopen("funds/mutual/mutual_fund.json", "r");

     while(fgets(string, bufferLength, fp))
    {     
        
             balance_line = strtok(string, ":");
             strcpy(investor, balance_line); 
             balance_line = strtok(NULL, ":"); 
             invested = atof(balance_line);
             
             invest(investor, invested, sum);
         
    }
    fclose(fp);

    remove("funds/mutual/mutual_fund.json");
    rename("funds/mutual/temp_mutual.json", "funds/mutual/mutual_fund.json");


}

void withdraw_mutual_investment(char *credit_card, char *password) {

    
    FILE *fp;   
    char string[255];
    int bufferLength = 255; 
    char *balance_line = NULL;
    float invested;
    int card_type; 

    int card_check = check_existence(credit_card, "current.json"); 

    if(card_check == 0) {
          card_type = 1;  
    }else{
          card_type = 2;
    }

    
    fp = fopen("funds/mutual/mutual_fund.json", "r"); 
    
    while(fgets(string, bufferLength, fp))
    {     
        
             balance_line = strtok(string, ":");

             if(!strcmp(balance_line, credit_card)) {
                  
                  balance_line = strtok(NULL, ":");
                  invested = atof(balance_line);
                  printf("invested: %f\n", invested);                 
             }
             
         
    }
    fclose(fp);

    char verify[21] = {0};
    char templance[6];
       
    float current_amount;
    float balance;

    strcat(verify, credit_card); 
    strcat(verify, ":");
    strcat(verify, password);


    int status_card = authentication_json(verify);
    int status_card2 = authentication_json(credit_card); 

    if(status_card == 0 && status_card2 == 1){

           printf("\nWrong password for card: %s\n", credit_card);
           exit(0);

    }else if(status_card == 1 && status_card2 == 1) {

           printf("\nAuthentication sucessfull, Proceeding..\n");
           
          if(card_type == 1) {

            char *balance_string = current_balance(credit_card);
            strcpy(templance, balance_string); 
            printf("\n[+] Current balance: %s\n", balance_string);
           
            current_amount = atof(templance); 
            balance = current_amount + invested;  
           
            update_current_balance(credit_card, balance);

          }else if(card_type == 2) {

           char *balance_string = deposit_balance(credit_card);
           strcpy(templance, balance_string); 
           printf("\n[+] Deposit balance: %s\n", balance_string);
           
           current_amount = atof(templance); 
           balance = current_amount + invested;

           update_deposit_balance(credit_card, balance);


          }

      }

    special_delete_line(credit_card, "funds/mutual/investors.json");
    clean_investors(); 
    gather_each_investor(); 

    remove("funds/mutual/mutual_fund.json");
    rename("funds/mutual/temp_mutual.json", "funds/mutual/mutual_fund.json");

}

void reinvest(float sum) {

     float percentage = 10; 
     float result;
     float saved_balance; 
     char string[255];
     char *balance_line = NULL;
     char new_cash[25] = {0};
     char delete[100] = {0};
    
     result = sum + (sum * percentage) / 100;
     
     FILE *fp; 

     fp = fopen("loan.json", "r");

     while(fgets(string, 255, fp))
    {   
            
        char *ptr = strstr(string, "initial_value");
        if (ptr != NULL) 
        {   
            strcpy(delete, ptr);

            balance_line = strtok(ptr, ":");
            strcpy(new_cash, balance_line);

            balance_line = strtok(NULL, ":"); 
            saved_balance = atof(balance_line);
            int totall = result + saved_balance;

            int len = snprintf(NULL, 0, "%d", totall);
            char *total = malloc(len + 1);
            snprintf(total, len + 1, "%d", totall);

            strcat(new_cash, ":"); 
            strcat(new_cash, total); 
        }
         
    }
    fclose(fp);
    
    delete_line(delete, "loan.json");
    
    fp = fopen("loan.json", "a");
    fprintf(fp, new_cash);
    fprintf(fp, "\n");
    fclose(fp);
 

}

int main(void)
{
	char credit_card[16];
    char password[4];
    int card_len; 
    int brand = 0;
    int operation;
    int type;

    printf("======================\n");
    printf("    Card Brands \n"); 
    printf("======================\n"); 
    printf("1 = Mastercard\n"); 
    printf("2 = Visa\n"); 
    printf("3 = Amex\n");
    printf("======================\n\n");

    printf("Type your card brand: ");
    scanf("%d", &brand); 
    
    switch(brand) {

    	case 1: card_len = 16; 
    	break;
    	case 2: card_len = 16;
    	break;
    	case 3: card_len = 15;
    	break;
    	default: printf("Invalid brand...");
    	exit(0); 
    } 

    printf("\nType your card number: ");
    scanf("%s", credit_card); 

    if(card_len == strlen(credit_card)) {

    	printf("\n\nAccepted..\n");
    }else{
    	printf("Invalid card, try another one...");
        exit(0);
    }

    printf("\n\n======================\n");
    printf("     Operation \n"); 
    printf("======================\n"); 
    printf("1 = withdraw\n"); 
    printf("2 = Deposit\n"); 
    printf("3 = Transfer\n");
    printf("4 = Loan    \n");
    printf("5 = Invest  \n"); 
    printf("6 = withdraw investment\n");
    printf("======================\n\n");

    printf("Type your operation: ");
    scanf("%d", &operation);
    
    if(operation == 1 || operation == 2) {

         printf("\n======================\n");
         printf("     Account type \n"); 
         printf("======================\n"); 
         printf("1 = current account\n"); 
         printf("2 = deposit account\n"); 
         printf("======================\n\n");
         
         printf("Type your account type: ");
         scanf("%d", &type);

    }else if(operation == 3) {

         printf("\n======================\n");
         printf("     Transfer type \n"); 
         printf("======================\n"); 
         printf("1 = current current \n"); 
         printf("2 = current deposit\n");
         printf("3 = deposit current\n"); 
         printf("4 = deposit deposit\n"); 
         printf("======================\n\n");

         printf("Type your transfer type: ");
         scanf("%d", &type);
    }

    if(operation == 4) {

        printf("\n======================\n");
         printf("     Loan type \n"); 
         printf("======================\n"); 
         printf("1 = Loan from TinyBank \n");
         printf("======================\n\n");

         printf("Type your transfer type: ");
         scanf("%d", &type);
    }

    if(operation == 5 || operation == 6) {

         printf("\n======================\n");
         printf("   Investiment type \n"); 
         printf("======================\n"); 
         printf("1 = mutual funds \n"); 
         printf("====================\n\n");

         printf("Type your Investiment type: ");
         scanf("%d", &type);
    }


    printf("\nProvide the card password: "); 
    scanf("%s", password); 

    
    if(operation == 1 && type == 1) {
                
        withdraw_current(credit_card, password); 

    }

    if(operation == 1 && type == 2){

       withdraw_deposit(credit_card, password);  

    }

    if(operation == 2 && type == 1) {

        deposit_current(credit_card, password);
    }

    if(operation == 2 && type == 2) {

        deposit_deposit(credit_card, password);
    }

    if(operation == 3 && type == 1) {
        

        transfer_current_current(credit_card, password);

    }

    if(operation == 3 && type == 2) {

       transfer_current_deposit(credit_card, password);

    }

    if(operation == 3 && type == 3) {

       transfer_deposit_current(credit_card, password);

    }

    if(operation == 3 && type == 4) {

       transfer_deposit_deposit(credit_card, password);

    }

    if(operation == 4 && type == 1) {
       
       get_loan(credit_card, password); 
    }

    if(operation == 5 && type == 1) {

       invest_mutual_fund(credit_card, password); 
       clean_investors();
       gather_each_investor();
       float sum = sum_all_money();
       start_investing(sum); 
       reinvest(sum);
    }

    if(operation == 6 && type == 1) {
         
         withdraw_mutual_investment(credit_card, password);
    }

	return 0;
}
