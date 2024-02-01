/*-----Header Files------*/

#include <stdio.h> //Header file required for user I/O
#include <stdlib.h> //Header file required for exit() library function
#include <string.h> //Header file required for string operations
#include "Animation.c" //Animation.c has the code for the progress bar
#include <time.h> //Header file required for getting the time from user's pc

/*-----Constants---------*/
#define MAX_PRD 30

/*-----Global Variables----*/
int count = 0, my_index; //Count, the global variable for total number of products, my_index, the global variable for storing a searched product
char id[10], new_id[10]; //Global variables for storing product ID
int invoiceNumber = 1000; //Used to generate unique invoice number

/*--Necessary informations related to a product--*/

struct prd
{

    char prd_id[10]; //A unique id for each product
    char prd_name[20]; //Name of the product
    int prd_quantity; //Quantity of the product
    int prd_sold; //Number of products sold
    int prd_totalsold; //Number of units sold
    float prd_cpu; //Cost per unit of the product
    float prd_rp; //Retail price of the product
    float prd_vat; //Value added tax
    float prd_dc; //If any discount is applicable for the product
};

/*--For showing month--*/
char *months[] =
{
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

/*--Function Prototypes--*/
void loadPrd(struct prd *products, int *count);
void savePrd(const struct prd *products, int count);
void copyProducts(struct prd *source, struct prd *destination, int count);
void sortPrd(struct prd *products, int count);
int prd_checker(struct prd *products, int count, const char *id);
void header();
void menu();
void chartPrd(struct prd *products, int count);
void outPrd(struct prd *products, int count);
void generateReceipt(struct prd *tempProducts, int count);
void topPrd(struct prd *products, int count);
int quit();
void sellPrd(struct prd *products, int count, const char *id);
void remPrd(struct prd *products, int count, const char *id);
void modPrd(struct prd *products, int count, const char *id);
void addPrd(struct prd *products, int index);
void showPrd(struct prd *products, int count);

/*--Main function of the program--*/
int main() {
    struct prd products[MAX_PRD]; //MAX_PRD different product information can be stored
    struct prd tempProducts[MAX_PRD]; //Temporary product information for sorting sold quantities
    int count = 0; //Keeps count of the number of products
    header(); //displays the logo and animation of the progress bar of the program
    int opt, exit_opt; //Variable for getting user input of options
    loadPrd(products, &count);
    back:
    do {
        menu(); //Shows the main menu
        scanf("%d", &opt); //User input of the option
        system("cls"); //Clears screen each time a new option is choosen
        switch (opt) {//Condition for running various functions
            case 1: //Add product function
                addPrd(products, count);
                count++;
                break;
            case 2: //Modify product function
                showPrd(products, count);
                printf("\tEnter Product ID: ");
                scanf("%s", id);
                modPrd(products, count, id);
                break;
            case 3: //Remove product function
                showPrd(products, count);
                printf("\tEnter Product ID: ");
                scanf("%s", id);
                remPrd(products, count, id);
                break;
            case 4: //Show products function
                showPrd(products, count);
                break;
            case 5: //Sell product function
                showPrd(products, count);
                printf("\tEnter Product ID: ");
                scanf("%s", id);
                sellPrd(products, count, id);
                break;
            case 6: //Highest sold products function
                topPrd(products, count);
                break;
            case 7: //Displaying Out-of-stock products
                outPrd(products,count);
                break;
            case 8: //Generates a products vs quantity sold histogram
                chartPrd(products, count);
                break;
            case 9: //Generates a receipt for total number of products sold
                copyProducts(products, tempProducts, count);
                generateReceipt(tempProducts, count);
                break;
            case 10: //Exiting out of the program
                printf("\n\tDo you want to exit?\a\n\t1.Yes\n\t2.No\n\tEnter your choice: ");
                scanf("%d", &exit_opt);
                switch(exit_opt) {
                    case 1:
                        savePrd(products, count);
                        quit();
                        break;
                    case 2:
                        goto back;
                        break;
                    default:
                        printf("\tInvalid Option Number\n");
                        break;
                }
            default: //If option is not valid
                printf("\tInvalid Option Number\n");
                break;
            }
    } //The loop runs until user enters 10 which is exit
    while (opt != 10);
    return 0;//Successful running of the program returns 0
}

/*---------------------Helper Functions--------------------------*/

/*--Used for importing data from products.txt--*/
void loadPrd(struct prd *products, int *count) {
    FILE *file = fopen("products.txt", "r");
    if (!file) {
        printf("\t\aAlert: product.txt import failed!\n");
        return;
    }
    *count = 0;
    while(fscanf(file, " %16[^0-9] %9s  %d  %f  %f  %f  %f", products[*count].prd_name, products[*count].prd_id, &products[*count].prd_quantity,
                 &products[*count].prd_cpu, &products[*count].prd_rp, &products[*count].prd_vat, &products[*count].prd_dc) == 7) {
        products[*count].prd_sold = 0;
        products[*count].prd_totalsold = 0;

        (*count)++;
    }
    fclose(file);
}

/*--Used for exporting data from products.txt--*/
void savePrd(const struct prd *products, int count) {
    FILE *file = fopen("products.txt", "w");
    if (!file) {
        printf("\t\aAlert: product.txt export failed!\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        if(strcmp(products[i].prd_id, "") == 0) { //It skips the removed product
            continue;
        }
        fprintf(file, " %-16s  %-9s  %-9d  %-14.2f  %-13.2f  %-5.2f  %-9.2f\n",
               products[i].prd_name, products[i].prd_id, products[i].prd_quantity,
               products[i].prd_cpu, products[i].prd_rp, products[i].prd_vat, products[i].prd_dc);
    }
    fclose(file);
}

/*--Function to duplicate the product data structure--*/
void copyProducts(struct prd *source, struct prd *destination, int count) {
    for (int i = 0; i < count; i++) {
        destination[i] = source[i]; //Copies all the product database to another temporary database to sort them
    }
}

/*--Function to sort the tempProduct structure based on totalsold--*/
void sortPrd(struct prd *products, int count) {
    for (int i = 0; i < count - 1; i++) {//Implements bubble sort algorithm to sort the totalsold array
        for (int j = 0; j < count - i - 1; j++) {
            if (products[j].prd_totalsold < products[j + 1].prd_totalsold) {
                struct prd temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }
}

/*--Function to check whether a product exists based on id provided--*/
int prd_checker(struct prd *products, int count, const char *id) {
    for(int i = 0; i < count; i++) {//Loop for scrolling through the IDs of the products
        if(strcmp(products[i].prd_id, id) == 0) {//Compares the user input ID with already existing product IDs
            my_index = i; //If user provided ID found then stores the my_index of that ID
            return 1; //Successfully finding the ID returns 1
        }
    }
    return 0; //If ID is not found returns 0
}

/*--Displays the starting ASCII art and loading animation--*/
void header() {
    system("color 90");
    printf("\n\t /$$$$$$     /$$      /$$      /$$$$$$ \n");
    printf("\t|_  $$_/    | $$$    /$$$     /$$__  $$\n");
    printf("\t  | $$      | $$$$  /$$$$    | $$  \\__/\n");
    printf("\t  | $$      | $$ $$/$$ $$    |  $$$$$$ \n");
    printf("\t  | $$      | $$  $$$| $$     \\____  $$\n");
    printf("\t  | $$      | $$\\  $ | $$     /$$  \\ $$\n");
    printf("\t /$$$$$$ /$$| $$ \\/  | $$ /$$|  $$$$$$/\n");
    printf("\t|______/|__/|__/     |__/|__/ \\______/ \n");
    printf("\n");
    mainP(); //Progress bar animation linked by "Animation.c"
    printf("\n\t WELCOME TO INVENTORY MANAGEMENT SYSTEM\n");
}

/*--Displays the main menu of the program--*/
void menu() {
    system("color B0"); //Background: Light Aqua, Foreground: Black
     // Header for the menu
    printf("\n");
    printf("\t========================================\n");
    printf("\t|..............MAIN MENU...............|\n");
    printf("\t========================================\n");

    // Options available in the menu
    printf("\t| 1. Add New Product                   |\n");
    printf("\t| 2. Modify Product Details            |\n");
    printf("\t| 3. Remove Product                    |\n");
    printf("\t| 4. View All Products                 |\n");
    printf("\t| 5. Sell Products                     |\n");
    printf("\t| 6. View Top Selling Product          |\n");
    printf("\t| 7. View Out-of-Stock Products        |\n");
    printf("\t| 8. Generate Products vs Sales Graph  |\n");
    printf("\t| 9. Generate Receipt                  |\n");
    printf("\t|10. Exit                              |\n");
    // Footer for the menu
    printf("\t========================================\n");
    printf("\tEnter your choice: "); // Asking for user input
}


/*--------------------Principal Functions--------------------*/

/*--Function for showing products vs sold amount graph--*/
void chartPrd(struct prd *products, int count) {
    FILE *pipe = popen("\"C:/Program Files/gnuplot/bin/gnuplot.exe\" -persistent", "w"); //Used to open gnuplot
    //1. Opens the gnuplot.exe and -persistent allows multiple commands to be sent
    if (pipe == NULL) { //If can't open shows an error message
        printf("Error opening Gnuplot pipe.\n");
        return;
    }
    //Commands written to gnuplot.exe to plot the graph
    //Command for configuring various settings for the plot, such as histogram styles, labels, box width, output format etc
    fprintf(pipe, "set style data histogram\n");
    fprintf(pipe, "set style histogram clustered gap 1\n");
    fprintf(pipe, "set style fill pattern\n");
    fprintf(pipe, "set style fill solid border -1\n");
    fprintf(pipe, "set boxwidth 0.4\n");
    fprintf(pipe, "set xtic rotate by -45\n");
    fprintf(pipe, "set ytics 1\n");
    fprintf(pipe, "set xlabel 'Products (Names)'\n");
    fprintf(pipe, "set ylabel 'Quantity Sold (Units)'\n");
    fprintf(pipe, "set offsets 0.7, 0.7, 3, 0\n");
    fprintf(pipe, "set terminal pngcairo enhanced font 'Verdana,8'\n");  // Set terminal to PNG format
    fprintf(pipe, "set output 'chart.png'\n");  // Output file name
    FILE *dataFile = fopen("data.txt", "w");  // Open a file to write data and store it for later use
    if (dataFile == NULL) { //If can't open the file it prints an error message
        printf("Error opening file.\n");
        pclose(pipe);
        return;
    }
    //Iterates through the product names and their sold quantities
    for (int i = 0; i < count; ++i) {
        if (products[i].prd_totalsold > 0) {
            fprintf(dataFile, "%s %d\n", products[i].prd_name, products[i].prd_totalsold); //Writes the informations to the data.txt
        }
    }
    fclose(dataFile);
    //Generates chart.png histogram chart using values from data.txt
    fprintf(pipe, "plot 'data.txt' using 2:xtic(1) with boxes fillstyle pattern 3 title 'Quantity Sold'\n");
    //Makes sure all the commands are sent
    fflush(pipe);
    //Closes the gnuplot.exe
    pclose(pipe);
    printf("\tSuccessfully generated chart.png\n");
}

/*--Function to check whether a product is out of stock--*/
void outPrd(struct prd *products, int count) {
    printf("\n\t=======================================\n");
    printf("\t..........OUT_OF_STOCK_PRODUCT.........\n");
    printf("\t=======================================\n");
    printf("\t| PROD. NAME           |   QUANTITY   |\n");
    printf("\t=======================================\n");
    for(int i=0; i<count; i++) {//Loop for scrolling through all the products
        if(strcmp(products[i].prd_id, "") == 0) {//It skips the removed product
            continue;
        }
        if(products[i].prd_quantity==0) {//Checks whether any product has zero quantity
            printf("\t| %-20s | %-10s |\n", products[i].prd_name,"OUT OF STOCK"); //Prints "OUT OF STOCK" and the product name
        }
    }
    printf("\t=======================================\n");
}

/*--Function to generate receipt of the sold goods--*/
void generateReceipt(struct prd *tempProducts, int count)
{
    FILE *receiptFile; //Necessary variable for file generation
    char fileName[30];
    sprintf(fileName, "IMS_Receipt_%d.txt", invoiceNumber); //Receipt name with its invoice number
    receiptFile = fopen(fileName, "w");
    if (receiptFile == NULL) {
        printf("\tError creating receipt file.\n"); //If writing to receipt file is failed displays this error message
        return;
    }
    //Code snippet for displaying the current time in 12 hours format
    time_t currentTime;
    struct tm *localTime;
    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    char am_pm[3];
    int hours12 = localTime->tm_hour % 12;
    if (hours12 == 0) hours12 = 12;
    //For getting AM or PM
    if (localTime->tm_hour >= 12) strcpy(am_pm, "PM");
    else strcpy(am_pm, "AM");

    //Written to receipt txt file
    fprintf(receiptFile, "-----------------------------------------------------\n");
    fprintf(receiptFile, "              INVENTORY MANAGEMENT SYSTEM\n");
    fprintf(receiptFile, "-----------------------------------------------------\n\n");

    fprintf(receiptFile, "Date: %s %02d, %d\t\tTime: %02d:%02d %s\n", months[localTime->tm_mon], localTime->tm_mday, localTime->tm_year + 1900, hours12, localTime->tm_min, am_pm);
    fprintf(receiptFile, "Invoice Number: %d\n\n", invoiceNumber++);

    fprintf(receiptFile, "-----------------------------------------------------\n");
    fprintf(receiptFile, "Product Name        Qty    Unit Price    Total Price\n");
    fprintf(receiptFile, "-----------------------------------------------------\n");

    float subtotal = 0.0; //Variable for total price of products
    float vat = 0.0; //Variable for calculating VAT for a product
    float dis = 0.0; //Variable for calculating discount for a product


    for (int i = 0; i < count; i++) {
        if(tempProducts[i].prd_sold == 0) continue; //If product sold is zero then its excluded from the receipt
        else {
            fprintf(receiptFile, "%-20s %-7d %-13.2f %-13.2f\n", tempProducts[i].prd_name, tempProducts[i].prd_sold, tempProducts[i].prd_rp, tempProducts[i].prd_sold * tempProducts[i].prd_rp);
            subtotal += tempProducts[i].prd_sold * tempProducts[i].prd_rp;
            vat += (tempProducts[i].prd_vat / 100.0) * (tempProducts[i].prd_rp * tempProducts[i].prd_sold); //Total VAT is calculate
            dis += (tempProducts[i].prd_dc / 100.0) * (tempProducts[i].prd_rp * tempProducts[i].prd_sold); //Total discount is calculated
        }
    }
    fprintf(receiptFile, "-----------------------------------------------------\n");

    float total = subtotal + vat - dis; //Calculating total price of the all the products
    //Prints the Subtotal, VAT, Discount, Total
    fprintf(receiptFile, "Subtotal:                                 %.2ftk\n", subtotal);
    fprintf(receiptFile, "VAT      :                                  %.2ftk\n", vat);
    fprintf(receiptFile, "(-) Discount:                               %.2ftk\n", dis);
    fprintf(receiptFile, "Total (incl. VAT):                        %.2ftk\n\n", total);

    fprintf(receiptFile, "-----------------------------------------------------\n\n");
    fprintf(receiptFile, "                    Powered by IMS\n");
    fprintf(receiptFile, "         (Inventory Management System Soft.)\n"); //Shows the credit of the program

    fclose(receiptFile);
    printf("\tReceipt generated successfully: %s\n", fileName); //Successful generation of receipt shows a message
}

/*--Function to display the highest sold product descending order--*/
void topPrd(struct prd *products, int count) {
    struct prd tempProducts[count]; //Creates a temporary product database
    copyProducts(products, tempProducts, count); // Make a copy of the products database
    sortPrd(tempProducts, count); // Sort the copied product database based on the highest sales in descending order
    printf("\n\t=======================================\n");
    printf("\t..........TOP SELLING PRODUCT..........\n");
    printf("\t=======================================\n");
    printf("\t| PROD. NAME             | UNITS SOLD |\n"); //Header for displaying top selling products
    printf("\t=======================================\n");

    for (int i = 0; i < count; i++) {
        if (tempProducts[i].prd_totalsold > 0) {//Condition for not displaying products with zero sales
            printf("\t| %-22s | %-10d |\n", tempProducts[i].prd_name, tempProducts[i].prd_totalsold); //Perfectly aligns the informations
        }
    }
    printf("\t=======================================\n");
}

/*--Function for exiting out of the program--*/
int quit() {
    printf("\n\n\tThanks for using\n\t\tInventory Management System(IMS).\n\n\tCredits: \n\t1. Md. Samiul Alam Rezon (202322033)\n\t2. Lauhe Mahfuz Siam (202322034)\n\t3. Md. Adnan Sadik (202322035)\n\t4. Safwan Ibn Ahmad (202322009)\n\t5. Arnab Nag (202322047)\n\t6. Jubair Haider (202322046)\n\t7. Kazi Tahsin Shafayet (202322038)\n\t8. Indillah Mursalin (202322042)\n\t9. Silvia Rahman Shamanta (202322041)\n");
    //Used to display the credits of the project team members
    exit(0); //Stdlib function for exiting out of a running program
}

/*--Function to sell products from the inventory--*/
void sellPrd(struct prd *products, int count, const char *id) {
    if(prd_checker(products, count, id)) {//Checks whether product with provided ID exist or not
        printf("\tEnter Quantity: ");
        scanf("%d", &products[my_index].prd_sold); //User input for the amount of quantity to be sold
        if (products[my_index].prd_sold <= products[my_index].prd_quantity && products[my_index].prd_sold != 0) {//Condition for checking whether theres stock available
            products[my_index].prd_quantity -= products[my_index].prd_sold; //Reduces the product quantity by the quantity sold by the user
            products[my_index].prd_totalsold += products[my_index].prd_sold; // Increases the total sold by the quantity sold by the user
            printf("\tSuccessfully Sold %d %s\n", products[my_index].prd_sold, products[my_index].prd_name); //Shows a successfully sold message
        }
        else {
            printf("\tInsufficient Stock!\n"); //If amount to be sold is greater than the quantity available displays this message
        }
    }
    else {
        printf("\tProduct ID not found!\n"); //If the provided product is not found within the existing products displays this message
    }
}

/*--Function to remove already existing product in the inventory--*/
void remPrd(struct prd *products, int count, const char *id) {
    int flag = prd_checker(products, count, id); //Checks whether ID exist or not
    if (flag) {//If ID exist prd_checker returns 1 and if not exist it returns 0
        printf("\tProduct with ID %s Removed Successfully!\n", id); //Displays product with provided ID is successfully removed
        strcpy(products[my_index].prd_id, ""); //Removal works by replacing the product ID with an empty string which is ""
        count--;
    }
    else {
        printf("\tProduct ID not found!\n"); //If provided product ID not found shows this message
    }
}

/*--Function to modify the details of the product--*/
void modPrd(struct prd *products, int count, const char *id) {
    int found = 0; //Decision variable initially set to false
    for (int i = 0; i < count; i++) {
        if (strcmp(products[i].prd_id, id) == 0) {//Checks whether user input id exist or not
            found = 1; //If ID found sets decision variable to true
            printf("\n\t=======================================\n");
            printf("\t...........MODIFY PRODUCT..............\n"); //Product details modification header
            printf("\t=======================================\n");

            fflush(stdin);
            printf("\tEnter New Name(e.g.Whipped Cream): ");
            scanf("%[^\n]s", products[i].prd_name); //User input of new product name

            printf("\tEnter New Quantity: ");
            scanf(" %d", &(products[i].prd_quantity)); //User input of new product quantity

            printf("\tEnter New Cost Per Unit: ");
            scanf(" %f", &(products[i].prd_cpu)); //User input of new product cost per unit

            printf("\tEnter New Retail Price: ");
            scanf("%f", &(products[i].prd_rp)); //User input of new product retail price

            printf("\tEnter New Value Added Tax(%%VAT): ");
            scanf("%f", &(products[i].prd_vat)); //User input of new Value Added Tax(VAT)

            printf("\tEnter New Discount Amount(%%): ");
            scanf("%f", &(products[i].prd_dc)); //User input of new discount amountt

            printf("\tProduct Details Updated Successfully!\n"); //Success message of updated product details
            break; //Breaks out of the loop which is used for checking product ids'
        }
    }
    if (!found) {//If ID not found flag becomes false
        printf("\tProduct ID not found!\n"); //Displays message that no product of provided ID exist
    }
}

/*--Function to add new product with a unique id--*/
void addPrd(struct prd *products, int index) {
    printf("\n\t=======================================\n");
    printf("\t..............PRODUCT ENTRY............\n"); //Product entry header
    printf("\t=======================================\n");

    printf("\tEnter Product ID(e.g.1001): ");
    scanf("%s", new_id); //Gets user input of product id
    while(prd_checker(products, index, new_id)) {//Checks whether a product with same ID exists or not
        printf("\tSame ID Found!\n\tEnter Unique ID: ");
        scanf("%s", new_id); //If same ID found asks for a unique ID
    }
    strcpy(products[index].prd_id, new_id); //After getting a unique ID copies the value from new_id string to products[index].prd_id

    fflush(stdin); //Clears the input buffer
    printf("\tEnter Product Name(e.g.Whipped Cream): ");
    scanf("%[^\n]s", products[index].prd_name); //User input of product name

    printf("\tEnter Quantity: ");
    scanf("%d", &(products[index].prd_quantity)); //User input of product quantity

    products[index].prd_sold = 0; //Initializes the product sold to zero
    products[index].prd_totalsold = 0; //Initializes the sales accumulated to zero

    printf("\tEnter Cost Per Unit: ");
    scanf("%f", &(products[index].prd_cpu)); //User input of the buying price of product

    printf("\tEnter Retail Price: ");
    scanf("%f", &(products[index].prd_rp)); //User input of the selling price of product

    printf("\tEnter Value Added Tax(%%VAT): ");
    scanf("%f", &(products[index].prd_vat)); //User input of Value Added Tax(VAT) of the product

    printf("\tEnter Discount Amount(%%): ");
    scanf("%f", &(products[index].prd_dc)); //User input of product discount

    printf("\n\tProduct %s is successfully added.\n", products[index].prd_id);
}

/*--Function to show already existing products--*/
void showPrd(struct prd *products, int count) {
    printf("\n\t\t\t             =======================================\n");
    printf("\t\t\t             |...........PRODUCT INVENTORY.........|\n"); //Headers for showing product informations
    printf("\t\t\t             =======================================\n");
    printf("=================================================================================================================\n");
    printf("| PROD. ID | PROD. NAME      | QUANTITY | COST PER UNIT | RETAIL PRICE | VAT  | DISCOUNT |\n");
    printf("=================================================================================================================\n");

    for (int i = 0; i < count; i++) {
        if(strcmp(products[i].prd_id, "") == 0) {//It skips the removed product
            continue;
        }
        printf("| %-9s| %-16s| %-9d| %-14.2f| %-13.2f| %-5.2f| %-9.2f|\n",
               products[i].prd_id, products[i].prd_name, products[i].prd_quantity,
               products[i].prd_cpu, products[i].prd_rp, products[i].prd_vat, products[i].prd_dc); //Product related informations are displayed with proper allignment
    }
    printf("=================================================================================================================\n");
}
