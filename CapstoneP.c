#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

// Structures
typedef struct {
    char fullName[50];
    char username[20];
    char phone[15];
    char password[20];
    char role; // 'A' for Admin, 'P' for Passenger
} User;

typedef struct {
    int id;
    char name[50];
    char type[10]; // Bus, Car, Bike
    char route[50];
    char time[10];
    float fare;
} Vehicle;

typedef struct {
    char id[5]; 
    char status; 
} Seat;

typedef struct {
    int bookingId;
    char username[20];
    int vehicleId;
    char seatId[5]; 
    char date[20];
    float finalPrice;
} Booking;

typedef struct {
    char name[100];
    float distance;
    float fare;
} Route;

typedef struct {
    char* name;
    Route routes[9];
} District;

typedef struct {
    char* name;
    District districts[9];
} Division;

// Function Prototypes
void registration(char role);
void login(char role);
void adminMenu();
void passengerMenu(char* username);
void addVehicle();
void deleteVehicle();
void viewAllRides();
void setDiscounts();
void viewRevenueDashboard();
void buyTickets(char* username);
void seatSelection(int vehicleId, char* username, char* date, float fare);
void viewTicket(int bookingId);
void getPassword(char *password);
void initializeRouteFiles();
void initializeBookingIdFile();
void showLoginRegisterMenu(char role);
void carBooking(char* username);
void cancelBooking(char* username);
void bikeBooking(char* username);
float applyManualDiscount(float originalFare);
void get_terminal_size(int *width, int *height);
void print_centered(const char *text, int width);
void print_centered_no_newline(const char *text, int width);


// Global variables
User currentUser;
const char* cities[] = {
    "Dhaka-Jamalpur", "Dhaka-Rangpur", "Dhaka-Rajshahi", "Dhaka-Chittagong", "Dhaka-Sylhet",
    "Dhaka-Khulna", "Dhaka-Mymensingh", "Dhaka-Barishal", "Dhaka-Kurigram", "Dhaka-Tangail", "Dhaka-Natore"
};
const int num_cities = 11;

Division divisions[8];

void initializeData() {
    // Dhaka Division
    divisions[0].name = "Dhaka Division";
    divisions[0].districts[0] = (District){"Dhaka", {
        {"Gulshan-Banani", 5, 25}, {"Motijheel-Dhanmondi", 8, 40}, {"Uttara-Mirpur", 12, 60}, {"New Market-Old Dhaka", 4, 20}, {"Savar-Ashulia", 15, 75}, {"Keraniganj-Jatrabari", 10, 50}, {"Mohammadpur-Farmgate", 6, 30}, {"Tejgaon-Badda", 7, 35}, {"Ramna-Paltan", 3, 15}
    }};
    divisions[0].districts[1] = (District){"Narayanganj", {
        {"Chashara-Fatullah", 6, 30}, {"Siddhirganj-Rupganj", 14, 70}, {"Bandar-Sonargaon", 18, 90}, {"Araihazar-Gognagar", 25, 125}, {"Pagla-Kanchpur", 12, 60}, {"Madanganj-Tarabo", 10, 50}, {"Nitaiganj-Panchdona", 20, 100}, {"Baidyer Bazar-Langalbandh", 16, 80}, {"Adamjee-Demra", 9, 45}
    }};
   
    divisions[0].districts[8] = (District){"Tangail", {
        {"TangailTown-Elenga", 8, 40}, {"Kalihati-Ghatail", 18, 90}, {"Sakhipur-Dhanbari", 40, 200}, {"Gopalpur-Basail", 28, 140}, {"Madhupur-Bhuapur", 22, 110}, {"Nagarpur-Mirzapur", 30, 150}, {"Delduar-Dighulia", 12, 60}, {"Dhanbari-Garabari", 15, 75}, {"Fulbaria-Pakutia", 10, 50}
    }};


    // Chattogram Division
    divisions[1].name = "Chattogram Division";
   
}


void initializeRouteFiles() {
    FILE *file;

    file = fopen("dhaka_routes.txt", "r");
    if (file == NULL) { 
        file = fopen("dhaka_routes.txt", "w");
        if (file != NULL) {
            Route dhakaRoutes[] = {
                {"Gulshan - Banani", 5, 50},
                {"Dhanmondi - Mirpur", 8, 80},
                {"Motijheel - Old Dhaka", 6, 60},
                {"Uttara - Pallabi", 7, 70},
                {"Banani - Bashundhara", 6, 60},
                {"Mirpur - Mohammadpur", 5, 50},
                {"Gabtoli - Kuril", 10, 100},
                {"New Market - Elephant Road", 4, 40},
                {"Gulshan - Baridhara", 7, 70},
                {"Shantinagar - Farmgate", 5, 50},
                {"Mohakhali - Dhaka University", 6, 60},
                {"Kamlapur Railway Station - Dhaka University", 7, 70}
            };
            int numDhakaRoutes = sizeof(dhakaRoutes) / sizeof(dhakaRoutes[0]);
            fwrite(dhakaRoutes, sizeof(Route), numDhakaRoutes, file);
            fclose(file);
        }
    } else {
        fclose(file);
    }

    // Check for Chittagong routes
    file = fopen("chittagong_routes.txt", "r");
    if (file == NULL) { 
        file = fopen("chittagong_routes.txt", "w");
        if (file != NULL) {
            Route chittagongRoutes[] = {
                {"Agrabad - Halishahar", 7, 56},
                {"Chittagong Railway Station - CU", 12, 96},
                {"Chittagong Railway Station - CUET", 11, 88},
                {"Chittagong Airport - Halishahar", 10, 80},
                {"Halishahar - CEPZ", 8, 64},
                {"Pahartali - Nasirabad", 5, 40},
                {"Mehedibag - Kalurghat", 6, 48},
                {"Muradpur - Agrabad", 3, 24},
                {"Oxygen - Anderkilla", 7, 56},
                {"Nasirabad - Chawk Bazar", 6, 48},
                {"CU - CEPZ", 10, 80}
            };
            int numChittagongRoutes = sizeof(chittagongRoutes) / sizeof(chittagongRoutes[0]);
            fwrite(chittagongRoutes, sizeof(Route), numChittagongRoutes, file);
            fclose(file);
        }
    } else {
        fclose(file);
    }
}

void initializeBookingIdFile() {
    FILE *file = fopen("booking_id.txt", "r");
    if (file == NULL) { 
        file = fopen("booking_id.txt", "w");
        if (file != NULL) {
            fprintf(file, "0");
            fclose(file);
        }
    } else {
        fclose(file);
    }
}

void get_terminal_size(int *width, int *height) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
#endif
}

void print_centered(const char *text, int width) {
    int len = strlen(text);
    int padding = (width - len) / 2;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s\n", text);
}

void print_centered_no_newline(const char *text, int width) {
    int len = strlen(text);
    int padding = (width - len) / 2;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s", text);
}

int main() {
    initializeData();
    initializeRouteFiles();
    initializeBookingIdFile();
    int choice;

    while (1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("\n\n\n\n");
        printf("                                 ---Transport Booking & Management System---\n");
        printf("                               -----------------------------------------------\n");
        printf("                                               Select your role:\n");
        printf("                                                   1. Admin\n");
        printf("                                                   2. Passenger\n");
        printf("                                                   3. Exit\n");
        printf("                                ----------------------------------------------\n");
        printf("                                                Enter your choice: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }

        switch (choice) {
            case 1:
                showLoginRegisterMenu('A');
                break;
            case 2:
                showLoginRegisterMenu('P');
                break;
            case 3:
                printf("\nThank you for using the system.\n");
                exit(0);
            default:
                printf("\nInvalid choice. Please try again.\n");
                getch();
        }
    }

    return 0;
}

void showLoginRegisterMenu(char role) {
    int choice;
    while(1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("\n\n\n\n");
        printf("                                                 --- %s Menu ---\n", (role == 'A' ? "Admin" : "Passenger"));
        printf("                                   ----------------------------------------------------\n");
        printf("                                                      1. Login\n");
        printf("                                                      2. Register\n");
        printf("                                                      3. Back to Main Menu\n");
        printf("                                   ----------------------------------------------------\n");
        printf("                                                     Enter your choice: ");

        fflush(stdout);
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }
        printf("\n");

        switch (choice) {
            case 1:
                login(role);
                break;
            case 2:
                registration(role);
                break;
            case 3:
                return;
            default:
                print_centered("Invalid choice. Please try again.", 120);
                getch();
        }
    }
}


void getPassword(char *password) {
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r') {
        if (ch == '\b') {
            if (i > 0) {
                printf("\b \b");
                i--;
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

void registration(char role) {
    User newUser;
    FILE *file;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("\n\n\n\n");
    printf("                                                     --- Registration ---\n");
    printf("                                -------------------------------------------------------------------\n");
    printf("                                           Enter Full Name: ");
    fflush(stdout);
    scanf(" %49[^\n]", newUser.fullName);

    printf("                                           Enter Username: ");
    fflush(stdout);
    scanf("%s", newUser.username);

    while(1){
        printf("                                       Enter Phone Number (11 digits): ");
        fflush(stdout);
        scanf("%s", newUser.phone);
        if (strlen(newUser.phone) == 11) {
            break;
        } else {
            printf("                                   Invalid phone number. Please enter 11 digits.\n");
        }
    }

    printf("                                           Enter Password: ");
    fflush(stdout);
    getPassword(newUser.password);
    printf("                                   ------------------------------------------------------------------\n\n");

    newUser.role = role;

    file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        getch();
        return;
    }

    fwrite(&newUser, sizeof(User), 1, file);
    fclose(file);

    printf("                                                  --Registration Complete!--\n");
    getch();
}

void login(char role) {
    char username[20];
    char password[20];
    User user;
    FILE *file;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("\n\n\n\n");
    printf("                                          --- Login ---\n");
    printf("                ----------------------------------------------------------------------\n");
    printf("                                      Enter Username: ");
    fflush(stdout);
    scanf("%s", username);

    printf("                                      Enter Password: ");
    fflush(stdout);
    getPassword(password);
    printf("               ------------------------------------------------------------------------\n\n");

    file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("                              No users registered yet.\n");
        getch();
        return;
    }

    int found = 0;
    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0 && (user.role == role)) {
            currentUser = user;
            found = 1;
            break;
        }
    }
    fclose(file);

    if (found) {
        printf("                                       Login Successfully!\n");
        getch();
        if (currentUser.role == 'A') {
            adminMenu();
        } else if (currentUser.role == 'P') {
            passengerMenu(currentUser.username);
        }
    } else {
        printf("                       Incorrect Username or Password for the selected role.\n");
        getch();
    }
}

void adminMenu() {
    int choice;
    while(1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("\n\n\n\n");
        printf("                                                  --- Admin Menu ---\n");
        printf("                                     ----------------------------------------------------\n");
        printf("                                                      1. Add Vehicle\n");
        printf("                                                      2. Delete Vehicle\n");
        printf("                                                      3. View All Rides\n");
        printf("                                                      4. Set Discounts\n");
        printf("                                                      5. View Revenue Dashboard\n");
        printf("                                                      6. Logout\n");
        printf("                                     ---------------------------------------------------\n");
        printf("                                                    Enter your choice: ");

        fflush(stdout);
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }
        printf("\n");

        switch (choice) {
            case 1:
                addVehicle();
                break;
            case 2:
                deleteVehicle();
                break;
            case 3:
                viewAllRides();
                break;
            case 4:
                setDiscounts();
                break;
            case 5:
                viewRevenueDashboard();
                break;
            case 6:
                return;
            default:
                print_centered("                      Invalid choice. Please try again.", 80);
                getch();
        }
    }
}

void passengerMenu(char* username) {
    int choice;
    int width, height;
    while(1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        get_terminal_size(&width, &height);
        print_centered("-----------------------------------------", width);
        print_centered("--- Passenger Menu ---", width);
        print_centered("-----------------------------------------", width);
        print_centered("1. Buy Tickets", width);
        print_centered("-----------------------------------------", width);
        print_centered("2. View Your Ticket", width);
        print_centered("-----------------------------------------", width);
        print_centered("3. Cancel Booking", width);
        print_centered("-----------------------------------------", width);
        print_centered("4. Logout", width);
        print_centered("-----------------------------------------", width);
        print_centered_no_newline("Enter your choice: ", width);
        fflush(stdout);
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }
        printf("\n");

        switch (choice) {
            case 1:
                buyTickets(username);
                break;
            case 2:
                print_centered("To view a ticket, you need a booking ID.", width);
                int bookingId;
                print_centered_no_newline("Enter your booking ID: ", width);
                fflush(stdout);
                scanf("%d", &bookingId);
                printf("\n");
                viewTicket(bookingId);
                break;
            case 3:
                cancelBooking(username);
                break;
            case 4:
                return; 
            default:
                print_centered("Invalid choice.", width);
                getch();
        }
    }
}

void addVehicle() {
    Vehicle newVehicle;
    FILE *file;
    FILE *idFile;
    int lastId = 0;
    int i, route_choice;
    int width, height;

    idFile = fopen("vehicle_id.txt", "r");
    if (idFile != NULL) {
        fscanf(idFile, "%d", &lastId);
        fclose(idFile);
    }

    newVehicle.id = lastId + 1;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);
    print_centered("--- ADD VEHICLE ---", width);
    printf("\n");

    int vehicleTypeChoice;
    print_centered("| Select Vehicle Type:                              |", width);
    print_centered("|---------------------------------------------------|", width);
    print_centered("| 1. Bus                                            |", width);
    print_centered("| 2. Car                                            |", width);
    print_centered("| 3. Bike                                           |", width);
    print_centered("|---------------------------------------------------|", width);
    print_centered_no_newline("Enter your choice: ", width);
    fflush(stdout);
    if (scanf("%d", &vehicleTypeChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        vehicleTypeChoice = 0;
    }
    printf("\n");

    switch(vehicleTypeChoice) {
        case 1:
            strcpy(newVehicle.type, "Bus");
            print_centered_no_newline("Enter Vehicle Name: ", width);
            fflush(stdout);
            scanf(" %49[^\n]", newVehicle.name);
            break;
        case 2:
            strcpy(newVehicle.type, "Car");
            strcpy(newVehicle.name, "Car");
            break;
        case 3:
            strcpy(newVehicle.type, "Bike");
            strcpy(newVehicle.name, "Bike");
            break;
        default:
            print_centered("Invalid vehicle type.", width);
            getch();
            return;
    }

    if (strcasecmp(newVehicle.type, "Bike") == 0) {
        int cityChoice;
        print_centered("Choose City for the Bike Route", width);
        print_centered("-----------------------------------------", width);
        print_centered("1. Dhaka City", width);
        print_centered("-----------------------------------------", width);
        print_centered("2. Chittagong City", width);
        print_centered("-----------------------------------------", width);

        print_centered_no_newline("Enter your choice: ", width);
        fflush(stdout);
        if (scanf("%d", &cityChoice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            cityChoice = 0;
        }
        printf("\n");

        if (cityChoice != 1 && cityChoice != 2) {
            print_centered("Invalid city choice.", width);
            getch();
            return;
        }

        Route routes[50];
        int numRoutes = 0;
        char* fileName = (cityChoice == 1) ? "dhaka_routes.txt" : "chittagong_routes.txt";

        FILE *routeFile = fopen(fileName, "r");
        if (routeFile != NULL) {
            while(fread(&routes[numRoutes], sizeof(Route), 1, routeFile)) {
                numRoutes++;
            }
            fclose(routeFile);
        }

        print_centered("--- Available Routes ---", width);
        print_centered("-----------------------------------------", width);

        for (i = 0; i < numRoutes; i++) {
            char route_str[100];
            sprintf(route_str, "%d. %s", i + 1, routes[i].name);
            print_centered(route_str, width);
        }
        print_centered("---------------------------------------------", width);

        int bikeRouteChoice;
        print_centered_no_newline("Enter route choice, or 0 to add a new route: ", width);
        fflush(stdout);
        if (scanf("%d", &bikeRouteChoice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            bikeRouteChoice = 0;
        }
        printf("\n");

        if (bikeRouteChoice == 0) {
            Route newRoute;
            print_centered_no_newline("Enter new route name (e.g., LocationA - LocationB): ", width);
            fflush(stdout);
            scanf(" %99[^\n]", newRoute.name);
            print_centered_no_newline("Enter distance (KM): ", width);
            fflush(stdout);
            scanf("%f", &newRoute.distance);
            print_centered_no_newline("Enter fare: ", width);
            fflush(stdout);
            scanf("%f", &newRoute.fare);

            routeFile = fopen(fileName, "a");
            if (routeFile != NULL) {
                fwrite(&newRoute, sizeof(Route), 1, routeFile);
                fclose(routeFile);
                print_centered("New route added successfully.", width);
                strcpy(newVehicle.route, newRoute.name);
            } else {
                print_centered("Error adding new route.", width);
                getch();
                return;
            }
        } else if (bikeRouteChoice > 0 && bikeRouteChoice <= numRoutes) {
            strcpy(newVehicle.route, routes[bikeRouteChoice - 1].name);
        } else {
            print_centered("Invalid route choice.", width);
            getch();
            return;
        }

    } else {
        print_centered("| Select a route:                                   |", width);
        print_centered("|---------------------------------------------------|", width);
        for(i=0; i<num_cities; i++){
            char city_str[100];
            sprintf(city_str, "%d. %s", i+1, cities[i]);
            print_centered(city_str, width);
        }
        print_centered("|---------------------------------------------------|", width);
        print_centered_no_newline("Enter your choice: ", width);
        fflush(stdout);
        if (scanf("%d", &route_choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            route_choice = 0;
        }
        printf("\n");

        if(route_choice > 0 && route_choice <= num_cities){
            strcpy(newVehicle.route, cities[route_choice-1]);
        } else {
            print_centered("Invalid route choice.", width);
            getch();
            return;
        }
    }

    if (strcasecmp(newVehicle.type, "Bike") != 0) {
        print_centered_no_newline("Enter Time (e.g., 10:30): ", width);
        fflush(stdout);
        scanf("%s", newVehicle.time);
    } else {
        strcpy(newVehicle.time, "N/A");
    }

    print_centered_no_newline("Enter Fare: ", width);
    fflush(stdout);
    scanf("%f", &newVehicle.fare);

    file = fopen("vehicles.txt", "a");
    if (file == NULL) {
        print_centered("Error opening file.", width);
        getch();
        return;
    }
    fwrite(&newVehicle, sizeof(Vehicle), 1, file);
    fclose(file);

    idFile = fopen("vehicle_id.txt", "w");
    if (idFile != NULL) {
        fprintf(idFile, "%d", newVehicle.id);
        fclose(idFile);
    }

    print_centered("Vehicle added successfully!", width);
    getch();
}

void viewAllRides() {
    Vehicle vehicle;
    FILE *file = fopen("vehicles.txt", "r");
    int width, height;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    if (file == NULL) {
        print_centered("No vehicles available.", width);
        getch();
        return;
    }

    print_centered("--- All Rides ---", width);
    printf("\n");
    printf("%-5s%-20s%-10s%-50s%10s%10s\n", "ID", "Name", "Type", "Route", "Time", "Fare");
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    while (fread(&vehicle, sizeof(Vehicle), 1, file)) {
        printf("%-5d%-20s%-10s%-50s%10s%10.2f\n", vehicle.id, vehicle.name, vehicle.type, vehicle.route, vehicle.time, vehicle.fare);
    }
    fclose(file);
    getch();
}


void deleteVehicle() {
    int id_to_delete;
    int width, height;
    viewAllRides();
    get_terminal_size(&width, &height);
    print_centered_no_newline("\nEnter ID of vehicle to delete: ", width);
    fflush(stdout);
    if (scanf("%d", &id_to_delete) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        print_centered("Invalid input.", width);
        getch();
        return;
    }

    Vehicle vehicles[1000];
    int vehicle_count = 0;
    FILE *file = fopen("vehicles.txt", "r");
    if (file == NULL) {
        print_centered("No vehicles found to delete.", width);
        getch();
        return;
    }
    while (fread(&vehicles[vehicle_count], sizeof(Vehicle), 1, file)) {
        vehicle_count++;
    }
    fclose(file);

    int vehicle_found = 0;
    for (int i = 0; i < vehicle_count; i++) {
        if (vehicles[i].id == id_to_delete) {
            vehicle_found = 1;
            break;
        }
    }

    if (!vehicle_found) {
        char msg[50];
        sprintf(msg, "Vehicle with ID %d not found.", id_to_delete);
        print_centered(msg, width);
        getch();
        return;
    }

    Booking bookings[1000];
    int booking_count = 0;
    file = fopen("bookings.txt", "r");
    if (file != NULL) {
        while (fread(&bookings[booking_count], sizeof(Booking), 1, file)) {
            booking_count++;
        }
        fclose(file);
    }

    int id_map[1000] = {0};
    Vehicle updated_vehicles[1000];
    int updated_vehicle_count = 0;
    int new_id = 1;

    for (int i = 0; i < vehicle_count; i++) {
        if (vehicles[i].id != id_to_delete) {
            id_map[vehicles[i].id] = new_id;
            updated_vehicles[updated_vehicle_count] = vehicles[i];
            updated_vehicles[updated_vehicle_count].id = new_id;
            updated_vehicle_count++;
            new_id++;
        }
    }

    Booking updated_bookings[1000];
    int updated_booking_count = 0;
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].vehicleId != id_to_delete) {
            if (id_map[bookings[i].vehicleId] != 0) {
                bookings[i].vehicleId = id_map[bookings[i].vehicleId];
            }
            updated_bookings[updated_booking_count++] = bookings[i];
        }
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        print_centered("Error creating temp file for vehicles.", width);
        getch();
        return;
    }
    fwrite(updated_vehicles, sizeof(Vehicle), updated_vehicle_count, tempFile);
    fclose(tempFile);
    remove("vehicles.txt");
    rename("temp.txt", "vehicles.txt");

    tempFile = fopen("temp_bookings.txt", "w");
    if (tempFile == NULL) {
        print_centered("Error creating temp file for bookings.", width);
        getch();
        return;
    }
    fwrite(updated_bookings, sizeof(Booking), updated_booking_count, tempFile);
    fclose(tempFile);
    remove("bookings.txt");
    rename("temp_bookings.txt", "bookings.txt");

    // Update vehicle_id.txt
    FILE *idFile = fopen("vehicle_id.txt", "w");
    if (idFile != NULL) {
        fprintf(idFile, "%d", updated_vehicle_count);
        fclose(idFile);
    }

    print_centered("Vehicle deleted and all IDs have been re-sequenced.", width);
    getch();
}

void carBooking(char* username) {
    int width, height;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    print_centered("Welcome! Please follow the steps below to book your car ride:", width);
    print_centered("---------------------------------------------------------------------------------------------------------", width);
    printf("\n");
    print_centered("Choose Car Type", width);
    print_centered("Select the type of car you want to book:", width);
    print_centered("------------------------------------------------------------------", width);
    printf("\n");
    print_centered("1. Private Car", width);
    printf("\n");
    print_centered("2. Family Car", width);
    printf("\n");
    print_centered("-------------------------------------------------------------------", width);
    printf("\n");
    print_centered_no_newline("Enter your choice: ", width);


    int carTypeChoice;
    char carTypeName[25];
    fflush(stdout);
    if (scanf("%d", &carTypeChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        carTypeChoice = 0;
    }
    printf("\n");

    if (carTypeChoice == 1) {
        strcpy(carTypeName, "Private Car");
    } else if (carTypeChoice == 2) {
        strcpy(carTypeName, "Family Car");
    } else {
        print_centered("Invalid car type choice.", width);
        getch();
        return;
    }

    int divisionChoice;
    printf("\n\n");
    print_centered("---Select City---", width);
    print_centered("-------------------------------------------------------------------------------", width);
    print_centered("Select the city where you would like to book your ride.", width);
    printf("\n");
    print_centered("--- Car Service Booking ---", width);
    print_centered("-------------------------------------------------", width);
    printf("\n");
    print_centered("Step 1: Select a division", width);
    printf("\n");
    print_centered("-------------------------------------------------", width);
    print_centered("1. Dhaka Division", width);
    print_centered("2. Chattogram Division", width);
    print_centered("-------------------------------------------------", width);
    printf("\n");
    print_centered_no_newline("Enter your choice: ", width);

    fflush(stdout);
    if (scanf("%d", &divisionChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        divisionChoice = 0;
    }
    printf("\n");

    if (divisionChoice < 1 || divisionChoice > 2) {
        print_centered("Invalid division choice.", width);
        getch();
        return;
    }

    int districtChoice;
    char dist_title[100];
    printf("\n\n");
    sprintf(dist_title, "Districts under %s", divisions[divisionChoice - 1].name);
    print_centered(dist_title, width);
    print_centered("-------------------------------------------------", width);
    
    if(divisionChoice == 2) { 
        print_centered("1. Chattogram - Coxbazar", width);
        print_centered("2. Chattogram - Bandarban", width);
        print_centered("3. Chattogram - Cumilla", width);
    } else {
        for (int i = 0; i < 3; i++) { 
            if (divisions[divisionChoice - 1].districts[i].name != NULL && strlen(divisions[divisionChoice - 1].districts[i].name) > 0) {
                char dist_str[100];
                sprintf(dist_str, "%d. %s", i + 1, divisions[divisionChoice - 1].districts[i].name);
                print_centered(dist_str, width);
            }
        }
    }
    print_centered("-------------------------------------------------", width);
    printf("\n");
    print_centered_no_newline("Enter your choice: ", width);

    fflush(stdout);
    if (scanf("%d", &districtChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        districtChoice = 0;
    }
    printf("\n");

    if (districtChoice < 1 || districtChoice > 3) {
        print_centered("Invalid district choice.", width);
        getch();
        return;
    }

    District selectedDistrict = divisions[divisionChoice - 1].districts[districtChoice - 1];
    int colorChoice;
    char route_title[100];
    printf("\n\n");
    sprintf(route_title, "-Routes in %s-", selectedDistrict.name);
    print_centered(route_title, width);
    print_centered("-------------------------------------------------", width);
    printf("\n");

    for (int i = 0; i < 9; i++) {
        if (selectedDistrict.routes[i].name != NULL && strlen(selectedDistrict.routes[i].name) > 0) {
            char route_str[200];
            sprintf(route_str, "%d. %s (%.0fKM - %.0fBDT)", i + 1, selectedDistrict.routes[i].name, selectedDistrict.routes[i].distance, selectedDistrict.routes[i].fare);
            print_centered(route_str, width);
            print_centered("-------------------------------------------------------------", width);
        }
    }
    printf("\n");
    print_centered_no_newline("Enter your choice: ", width);
    fflush(stdout);
    if (scanf("%d", &colorChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        colorChoice = 0;
    }
    printf("\n");

    if (colorChoice < 1 || colorChoice > 9 || selectedDistrict.routes[colorChoice - 1].name == NULL) {
        print_centered("Invalid route choice.", width);
        getch();
        return;
    }

    Route selectedRoute = selectedDistrict.routes[colorChoice - 1];
    char time[10], date[20];

    printf("\n");
    print_centered_no_newline("Enter Date (DD-MM-YYYY): ", width);
    fflush(stdout);
    scanf("%s", date);
    print_centered("-------------------------------------------------", width);
    printf("\n");
    print_centered_no_newline("Enter Time (e.g., 08:30 AM): ", width);
    fflush(stdout);
    scanf(" %9[^\n]", time);
    print_centered("-------------------------------------------------", width);

    printf("\n\n");
    print_centered("--- Confirm Booking & Apply Discount ---", width);
    print_centered("------------------------------------------------------------", width);
    printf("\n");
    char info[200];
    sprintf(info, "You have selected: %s", carTypeName);
    print_centered(info, width);
    sprintf(info, "Route: %s", selectedRoute.name);
    print_centered(info, width);
    sprintf(info, "Distance: %.0fKM", selectedRoute.distance);
    print_centered(info, width);
    sprintf(info, "Fare: %.0f BDT", selectedRoute.fare);
    print_centered(info, width);
    sprintf(info, "Date: %s", date);
    print_centered(info, width);
    sprintf(info, "Time: %s", time);
    print_centered(info, width);
    printf("\n");
    print_centered("----------------------------------------------------------", width);

    printf("\n\n");
    char confirm;
    print_centered_no_newline("Confirm Ticket (Y/N): ", width);
    fflush(stdout);
    scanf(" %c", &confirm);
    printf("\n");

    if (confirm == 'Y' || confirm == 'y') {
        Vehicle newVehicle;
        FILE *vehicleFile, *idFile;
        int lastId = 0;

        idFile = fopen("vehicle_id.txt", "r");
        if (idFile != NULL) {
            fscanf(idFile, "%d", &lastId);
            fclose(idFile);
        }
        newVehicle.id = lastId + 1;
        strcpy(newVehicle.name, carTypeName);
        strcpy(newVehicle.type, "Car");
        strcpy(newVehicle.route, selectedRoute.name);
        strcpy(newVehicle.time, time);
        newVehicle.fare = selectedRoute.fare;

        vehicleFile = fopen("vehicles.txt", "a");
        fwrite(&newVehicle, sizeof(Vehicle), 1, vehicleFile);
        fflush(vehicleFile);
        fclose(vehicleFile);

        idFile = fopen("vehicle_id.txt", "w");
        fprintf(idFile, "%d", newVehicle.id);
        fflush(idFile);
        fclose(idFile);

        Booking newBooking;
        FILE *bookingFile, *bookingIdFile;
        int lastBookingId = 0;

        bookingIdFile = fopen("booking_id.txt", "r");
        if (bookingIdFile != NULL) {
            fscanf(bookingIdFile, "%d", &lastBookingId);
            fclose(bookingIdFile);
        }

        newBooking.bookingId = lastBookingId + 1;
        strcpy(newBooking.username, username);
        newBooking.vehicleId = newVehicle.id;
        strcpy(newBooking.seatId, "N/A");
        strcpy(newBooking.date, date);
        newBooking.finalPrice = applyManualDiscount(selectedRoute.fare);

        bookingFile = fopen("bookings.txt", "a");
        fwrite(&newBooking, sizeof(Booking), 1, bookingFile);
        fflush(bookingFile);
        fclose(bookingFile);

        bookingIdFile = fopen("booking_id.txt", "w");
        fprintf(bookingIdFile, "%d", newBooking.bookingId);
        fflush(bookingIdFile);
        fclose(bookingIdFile);

        getch();
        viewTicket(newBooking.bookingId);
    } else {
        print_centered("Booking cancelled.", width);
        getch();
    }
}

void buyTickets(char* username) {
    int vehicleTypeChoice;
    char chosenType[10];
    char date[20];
    int route_choice;
    int width, height;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    print_centered("--- Buy Tickets ---", width);
    print_centered("----------------------------------------------------------", width);
    print_centered("Choose Vehicle Type:", width);
    print_centered("----------------------------------------------------------", width);
    print_centered("1. Bus", width);
    print_centered("----------------------------------------------------------", width);
    print_centered("2. Car", width);
    print_centered("----------------------------------------------------------", width);
    print_centered("3. Bike", width);
    print_centered("----------------------------------------------------------", width);
    print_centered_no_newline("Enter your choice: ", width);
    fflush(stdout);
    scanf("%d", &vehicleTypeChoice);
    printf("\n");

    switch(vehicleTypeChoice) {
        case 1: strcpy(chosenType, "Bus"); break;
        case 2:
            carBooking(username);
            return;
        case 3:
            bikeBooking(username);
            return;
        default:
            print_centered("Invalid vehicle type.", width);
            getch();
            return;
    }


    Vehicle vehicle;
    FILE *file = fopen("vehicles.txt", "r");
    if (file == NULL) {
        print_centered("No vehicles available.", width);
        getch();
        return;
    }

    char available_routes[100][50];
    int num_available_routes = 0;
    int i, route_exists;

    while (fread(&vehicle, sizeof(Vehicle), 1, file)) {
        if (strcasecmp(vehicle.type, chosenType) == 0) {
            route_exists = 0;
            for(i=0; i<num_available_routes; i++){
                if(strcmp(available_routes[i], vehicle.route) == 0){
                    route_exists = 1;
                    break;
                }
            }
            if(!route_exists){
                strcpy(available_routes[num_available_routes], vehicle.route);
                num_available_routes++;
            }
        }
    }
    fclose(file);

    if(num_available_routes == 0){
        char msg[100];
        sprintf(msg, "No routes available for %s.", chosenType);
        print_centered(msg, width);
        getch();
        return;
    }

    print_centered("----------------------------------------------------------", width);
    char title[100];
    sprintf(title, "--- Available Routes for %s ---", chosenType);
    print_centered(title, width);
    print_centered("----------------------------------------------------------", width);
    for(i=0; i<num_available_routes; i++){
        char route_str[100];
        sprintf(route_str, "%d. %s", i+1, available_routes[i]);
        print_centered(route_str, width);
        print_centered("----------------------------------------------------------", width);
    }
    print_centered_no_newline("Enter your choice: ", width);
    fflush(stdout);
    scanf("%d", &route_choice);
    printf("\n");

    if(route_choice <= 0 || route_choice > num_available_routes){
        print_centered("Invalid route choice.", width);
        getch();
        return;
    }
    char* selected_route = available_routes[route_choice-1];

    print_centered("----------------------------------------------------------", width);
    print_centered_no_newline("Date (DD-MM-YYYY): ", width);
    fflush(stdout);
    scanf("%s", date);
    printf("\n");

    file = fopen("vehicles.txt", "r");
    if (file == NULL) {
        print_centered("Error opening vehicles file.", width);
        getch();
        return;
    }

    print_centered("--- Available Rides ---", width);
    print_centered("-------------------------------------------------------------------------", width);
    printf("\n");
    printf("                                             ID      Name            Time    Fare\n");
    printf("                                      -------------------------------------------------\n");

    int found = 0;
    while (fread(&vehicle, sizeof(Vehicle), 1, file)) {
        if (strcmp(vehicle.route, selected_route) == 0 && strcasecmp(vehicle.type, chosenType) == 0) {
            printf("                                             %-7d %-15s %-7s %8.2f\n", vehicle.id, vehicle.name, vehicle.time, vehicle.fare);
            found = 1;
        }
    }
    fclose(file);

    if (!found) {
        print_centered("No rides found for the given route and vehicle type.", width);
        getch();
        return;
    }

    printf("                       -------------------------------------------------------------------------------\n");
    printf("\n");
    int vehicleId;
    print_centered_no_newline("Enter the ID of the vehicle you want to book: ", width);
    fflush(stdout);
    scanf("%d", &vehicleId);
    printf("\n");

    file = fopen("vehicles.txt", "r");
    found = 0;
    while (fread(&vehicle, sizeof(Vehicle), 1, file)) {
        if (vehicle.id == vehicleId) {
            found = 1;
            break;
        }
    }
    fclose(file);

    if (!found) {
        print_centered("Invalid Vehicle ID.", width);
        getch();
        return;
    }


    if (strcasecmp(vehicle.type, "Bus") == 0) {
        seatSelection(vehicleId, username, date, vehicle.fare);
    } else {
        Booking newBooking;
        FILE *bookingFile;
        FILE *bookingIdFile;
        int lastBookingId = 0;

        bookingIdFile = fopen("booking_id.txt", "r");
        if (bookingIdFile != NULL) {
            fscanf(bookingIdFile, "%d", &lastBookingId);
            fclose(bookingIdFile);
        }

        newBooking.bookingId = lastBookingId + 1;
        strcpy(newBooking.username, username);
        newBooking.vehicleId = vehicleId;
        strcpy(newBooking.seatId, "N/A");
        strcpy(newBooking.date, date);

        //Manual discount
        newBooking.finalPrice = applyManualDiscount(vehicle.fare);


        bookingFile = fopen("bookings.txt", "a");
        fwrite(&newBooking, sizeof(Booking), 1, bookingFile);
        fclose(bookingFile);

        bookingIdFile = fopen("booking_id.txt", "w");
        fprintf(bookingIdFile, "%d", newBooking.bookingId);
        fclose(bookingIdFile);


        print_centered("Your ticket is confirmed.", width);
        char msg[100];
        sprintf(msg, "Booking ID: %d", newBooking.bookingId);
        print_centered(msg, width);
        sprintf(msg, "Vehicle: %s", vehicle.name);
        print_centered(msg, width);
        sprintf(msg, "Route: %s", vehicle.route);
        print_centered(msg, width);
        sprintf(msg, "Date: %s", newBooking.date);
        print_centered(msg, width);
        sprintf(msg, "Fare: %.2f", newBooking.finalPrice);
        print_centered(msg, width);


        int choice;
        print_centered("1. View Your Ticket", width);
        print_centered("2. Go to Home", width);
        print_centered("3. Exit", width);
        print_centered_no_newline("Enter your choice: ", width);
        fflush(stdout);
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }
        printf("\n");
        if (choice == 1) {
            viewTicket(newBooking.bookingId);
        } else if (choice == 3) {
            exit(0);
        }
    }
}

void seatSelection(int vehicleId, char* username, char* date, float fare) {
    const int TOTAL_SEATS = 36; 
    Seat seats[TOTAL_SEATS];
    int seatCount = 0;
    int width, height;

    // Initialize seats
    for (char row = 'A'; row <= 'I'; row++) {
        for (int num = 1; num <= 4; num++) {
            if (seatCount < TOTAL_SEATS) {
                sprintf(seats[seatCount].id, "%c%d", row, num);
                seats[seatCount].status = 'A'; 
                seatCount++;
            }
        }
    }

    Booking booking;
    FILE *file = fopen("bookings.txt", "r");
    if (file != NULL) {
        while (fread(&booking, sizeof(Booking), 1, file)) {
            if (booking.vehicleId == vehicleId && strcmp(booking.date, date) == 0) {
                for (int i = 0; i < seatCount; i++) {
                    if (strcmp(seats[i].id, booking.seatId) == 0) {
                        seats[i].status = 'B'; 
                        break;
                    }
                }
            }
        }
        fclose(file);
    }

    get_terminal_size(&width, &height);
    int num_tickets;
    print_centered_no_newline("How many tickets do you want to book (1-4)? ", width);
    fflush(stdout);
    if (scanf("%d", &num_tickets) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        num_tickets = 0;
    }
    printf("\n");


    if (num_tickets < 1 || num_tickets > 4) {
        print_centered("Invalid number of tickets. Please choose between 1 and 4.", width);
        getch();
        return;
    }

    char selected_seats[4][5];
    int tickets_booked = 0;

    while (tickets_booked < num_tickets) {
        // Display seat map
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        get_terminal_size(&width, &height);
        print_centered("--- Seat Selection ---", width);
        print_centered("-----------------------------------------", width);
        print_centered("Available Seats:", width);
        int current_seat = 0;
        for (char row = 'A'; row <= 'I'; row++) {
            int padding = (width - 20) / 2;
            for(int p=0; p<padding; p++) printf(" ");
            for (int num = 1; num <= 4; num++) {
                if (seats[current_seat].status == 'B') {
                    printf("X   ");
                } else {
                    printf("%c%d  ", row, num);
                }
                if (num == 2) {
                    printf("     "); 
                }
                current_seat++;
            }
            printf("\n");
        }
        print_centered("---------------------------------------------------------------------", width);

        char seatId[5];
        char prompt[100];
        sprintf(prompt, "Enter seat %d to book (e.g., C2): ", tickets_booked + 1);
        print_centered_no_newline(prompt, width);
        fflush(stdout);
        scanf("%s", seatId);
        printf("\n");
        print_centered("----------------------------------------------------------------------", width);

        int seatIndex = -1;
        for (int i = 0; i < seatCount; i++) {
            if (strcasecmp(seats[i].id, seatId) == 0) {
                seatIndex = i;
                break;
            }
        }

        if (seatIndex == -1) {
            print_centered("Invalid seat ID.", width);
            getch();
            continue;
        }

        if (seats[seatIndex].status == 'B') {
            print_centered("Seat already booked. Please choose another seat.", width);
            getch();
            continue;
        }

        int already_selected = 0;
        for (int i = 0; i < tickets_booked; i++) {
            if (strcasecmp(selected_seats[i], seatId) == 0) {
                print_centered("You have already selected this seat.", width);
                already_selected = 1;
                getch();
                break;
            }
        }
        if (already_selected) {
            continue;
        }


        strcpy(selected_seats[tickets_booked], seats[seatIndex].id);
        seats[seatIndex].status = 'B';
        tickets_booked++;
    }

    print_centered("--- Confirm Your Booking ---", width);
    print_centered("------------------------------------------------------------------------", width);
    char msg[200];
    sprintf(msg, "You have selected the following %d seat(s): ", num_tickets);
    for (int i = 0; i < num_tickets; i++) {
        strcat(msg, selected_seats[i]);
        strcat(msg, " ");
    }
    print_centered(msg, width);

    float totalFare = fare * num_tickets;
    sprintf(msg, "Total Fare: %.2f", totalFare);
    print_centered(msg, width);

    char confirm;
    print_centered_no_newline("Confirm Booking (Y/N): ", width);
    fflush(stdout);
    scanf(" %c", &confirm);
    printf("\n");
    print_centered("--------------------------------------------------------------------------------------------------------", width);

    if (confirm == 'Y' || confirm == 'y') {
        float finalPrice = applyManualDiscount(totalFare);

        FILE *bookingIdFile;
        int lastBookingId = 0;

        bookingIdFile = fopen("booking_id.txt", "r");
        if (bookingIdFile != NULL) {
            fscanf(bookingIdFile, "%d", &lastBookingId);
            fclose(bookingIdFile);
        }

        int newBookingId = lastBookingId + 1;
        float pricePerTicket = finalPrice / num_tickets;

        // Book the seats
        for (int i = 0; i < num_tickets; i++) {
            Booking newBooking;
            FILE *bookingFile;

            newBooking.bookingId = newBookingId;
            strcpy(newBooking.username, username);
            newBooking.vehicleId = vehicleId;
            strcpy(newBooking.seatId, selected_seats[i]);
            strcpy(newBooking.date, date);
            newBooking.finalPrice = pricePerTicket;

            bookingFile = fopen("bookings.txt", "a");
            if (bookingFile == NULL) {
                print_centered("Error opening bookings file.", width);
                 for (int j = 0; j < num_tickets; j++) {
                    for (int k = 0; k < seatCount; k++) {
                        if (strcasecmp(seats[k].id, selected_seats[j]) == 0) {
                            seats[k].status = 'A';
                            break;
                        }
                    }
                }
                getch();
                return;
            }
            fwrite(&newBooking, sizeof(Booking), 1, bookingFile);
            fflush(bookingFile);
            fclose(bookingFile);
        }

        bookingIdFile = fopen("booking_id.txt", "w");
        if (bookingIdFile != NULL) {
            fprintf(bookingIdFile, "%d", newBookingId);
            fflush(bookingIdFile);
            fclose(bookingIdFile);
        }

        print_centered("Booking Confirmed!", width);
        getch();
        viewTicket(newBookingId);

    } else {
        print_centered("Booking cancelled.", width);
        for (int i = 0; i < num_tickets; i++) {
            for (int j = 0; j < seatCount; j++) {
                if (strcasecmp(seats[j].id, selected_seats[i]) == 0) {
                    seats[j].status = 'A';
                    break;
                }
            }
        }
        getch();
    }
}

void viewTicket(int bookingId) {
    Booking bookings[10]; 
    int booking_count = 0;
    Booking temp_booking;
    FILE *bookingFile = fopen("bookings.txt", "r");
    int width, height;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    if (bookingFile == NULL) {
        print_centered("No bookings found.", width);
        getch();
        return;
    }

    while (fread(&temp_booking, sizeof(Booking), 1, bookingFile)) {
        if (temp_booking.bookingId == bookingId) {
            if (booking_count < 10) {
                bookings[booking_count++] = temp_booking;
            }
        }
    }
    fclose(bookingFile);

    if (booking_count == 0) {
        print_centered("Booking ID not found.", width);
        getch();
        return;
    }

    Vehicle vehicle;
    FILE *vehicleFile = fopen("vehicles.txt", "r");
    if (vehicleFile == NULL) {
        print_centered("Could not retrieve vehicle details.", width);
        getch();
        return;
    }

    int vehicleFound = 0;
    while (fread(&vehicle, sizeof(Vehicle), 1, vehicleFile)) {
        if (vehicle.id == bookings[0].vehicleId) {
            vehicleFound = 1;
            break;
        }
    }
    fclose(vehicleFile);

    if (!vehicleFound) {
        print_centered("Could not retrieve vehicle details.", width);
        getch();
        return;
    }

    print_centered("--- Your Ticket ---", width);
    print_centered("---------------------", width);
    char info[200];
    sprintf(info, "Booking ID: %d", bookings[0].bookingId);
    print_centered(info, width);
    print_centered("---------------------", width);
    sprintf(info, "Passenger: %s", bookings[0].username);
    print_centered(info, width);
    print_centered("---------------------", width);
    sprintf(info, "Vehicle: %s (%s)", vehicle.name, vehicle.type);
    print_centered(info, width);
    print_centered("---------------------", width);
    sprintf(info, "Route: %s", vehicle.route);
    print_centered(info, width);
    print_centered("---------------------", width);
    sprintf(info, "Date: %s", bookings[0].date);
    print_centered(info, width);
    print_centered("---------------------", width);
    sprintf(info, "Time: %s", vehicle.time);
    print_centered(info, width);
    print_centered("---------------------", width);

    float totalFare = 0;
    if (strcmp(bookings[0].seatId, "N/A") != 0) {
        char seats_str[100] = "Seat Number(s): ";
        for (int i = 0; i < booking_count; i++) {
            strcat(seats_str, bookings[i].seatId);
            strcat(seats_str, " ");
            totalFare += bookings[i].finalPrice;
        }
        print_centered(seats_str, width);
        print_centered("---------------------", width);
    } else {
        totalFare = bookings[0].finalPrice;
    }

    sprintf(info, "Fare: %.2f", totalFare);
    print_centered(info, width);
    print_centered("---------------------", width);
    getch();
}

float applyManualDiscount(float originalFare) {
    float discountPercentage = 0;
    char choice;
    int width, height;
    get_terminal_size(&width, &height);

    FILE *file = fopen("discounts.txt", "r");
    if (file != NULL) {
        fscanf(file, "%f", &discountPercentage);
        fclose(file);

        if (discountPercentage > 0) {
            printf("\n");
            char msg[200];
            sprintf(msg, "A global discount of %.2f%% is available. Do you want to use it? (Y/N): ", discountPercentage);
            print_centered_no_newline(msg, width);
            fflush(stdout);
            scanf(" %c", &choice);
            printf("\n");

            if (choice == 'Y' || choice == 'y') {
                float discountAmount = originalFare * (discountPercentage / 100);
                float finalPrice = originalFare - discountAmount;
                print_centered("--------------------------------------------------------------------------------------", width);
                char inner_msg[200];
                sprintf(inner_msg, "Discount of %.2f%% applied.", discountPercentage);
                print_centered(inner_msg, width);
                sprintf(inner_msg, "New fare: %.2fBDT", finalPrice);
                print_centered(inner_msg, width);
                print_centered("-------------------------------------------------------------------------------------", width);
                getch();
                return finalPrice;
            }
        }
    }

    return originalFare;
}

void cancelBooking(char* username) {
    int bookingId;
    int width, height;
    get_terminal_size(&width, &height);

    print_centered_no_newline("Enter the booking ID to cancel: ", width);
    fflush(stdout);
    scanf("%d", &bookingId);
    printf("\n");

    Booking booking;
    FILE *file = fopen("bookings.txt", "r");
    FILE *tempFile = fopen("temp_bookings.txt", "w");

    if (file == NULL || tempFile == NULL) {
        print_centered("Error opening files.", width);
        getch();
        return;
    }

    int found = 0;
    while (fread(&booking, sizeof(Booking), 1, file)) {
        if (booking.bookingId == bookingId && strcmp(booking.username, username) == 0) {
            found = 1; // Mark as found but don't write to temp file
        } else {
            fwrite(&booking, sizeof(Booking), 1, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("bookings.txt");
        rename("temp_bookings.txt", "bookings.txt");
        char msg[100];
        sprintf(msg, "Booking with ID %d has been successfully canceled.", bookingId);
        print_centered(msg, width);
    } else {
        remove("temp_bookings.txt");
        print_centered("Booking not found or you do not have permission to cancel it.", width);
    }
    getch();
}


void bikeBooking(char* username) {
    int width, height;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    print_centered("--- Welcome to your Bike Ride booking! ---", width);
    print_centered("-----------------------------------------------------", width);
    print_centered("Please follow the steps below to get started:", width);
    printf("\n");
    print_centered("---Choose City---", width);
    print_centered("--------------------------------------------------------------", width);
    print_centered("Please choose the city where you want to grab your ride:", width);
    print_centered("-----------------------------------------------------------------------------------------------", width);
    print_centered("1. Dhaka City", width);
    print_centered("2. Chittagong City", width);
    print_centered("----------------------------------------", width);
    print_centered_no_newline("Enter your choice: ", width);

    int cityChoice;
    fflush(stdout);
    if (scanf("%d", &cityChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        cityChoice = 0;
    }
    printf("\n");

    if (cityChoice != 1 && cityChoice != 2) {
        print_centered("Invalid city choice.", width);
        getch();
        return;
    }

    Route routes[50];
    int numRoutes = 0;
    char* fileName = (cityChoice == 1) ? "dhaka_routes.txt" : "chittagong_routes.txt";

    FILE *file = fopen(fileName, "r");
    if (file != NULL) {
        while(fread(&routes[numRoutes], sizeof(Route), 1, file)) {
            numRoutes++;
        }
        fclose(file);
    }

    if (numRoutes == 0) {
        print_centered("No routes available for the selected city.", width);
        getch();
        return;
    }

    print_centered("\nSelect Destination Route", width);
    for (int i = 0; i < numRoutes; i++) {
        char route_str[100];
        sprintf(route_str, "%d. %s", i + 1, routes[i].name);
        print_centered(route_str, width);
        print_centered("-------------------------------------------------------", width);
    }

    int routeChoice;
    print_centered_no_newline("Enter your choice: ", width);
    fflush(stdout);
    if (scanf("%d", &routeChoice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        routeChoice = 0;
    }
    printf("\n");

    if (routeChoice <= 0 || routeChoice > numRoutes) {
        print_centered("Invalid route choice.", width);
        getch();
        return;
    }

    Route selectedRoute = routes[routeChoice - 1];

    char time[10], date[20];
    print_centered_no_newline("Enter Date (DD-MM-YYYY): ", width);
    fflush(stdout);
    scanf("%s", date);
    print_centered_no_newline("Enter Time (e.g., 08:30 AM): ", width);
    fflush(stdout);
    scanf(" %9[^\n]", time);

    print_centered("--- Confirm Ticket ---", width);
    char info[200];
    sprintf(info, "Route: %s", selectedRoute.name);
    print_centered(info, width);
    sprintf(info, "Distance: %.0fKM", selectedRoute.distance);
    print_centered(info, width);
    sprintf(info, "Fare: %.0f BDT", selectedRoute.fare);
    print_centered(info, width);
    sprintf(info, "Date: %s", date);
    print_centered(info, width);
    sprintf(info, "Time: %s", time);
    print_centered(info, width);

    char confirm;
    print_centered_no_newline("Confirm Ticket (Y/N): ", width);
    fflush(stdout);
    scanf(" %c", &confirm);
    printf("\n");

    if (confirm == 'Y' || confirm == 'y') {
        Vehicle newVehicle;
        FILE *vehicleFile, *idFile;
        int lastId = 0;

        idFile = fopen("vehicle_id.txt", "r");
        if (idFile != NULL) {
            fscanf(idFile, "%d", &lastId);
            fclose(idFile);
        }
        newVehicle.id = lastId + 1;
        strcpy(newVehicle.name, "Booked Bike");
        strcpy(newVehicle.type, "Bike");
        strcpy(newVehicle.route, selectedRoute.name);
        strcpy(newVehicle.time, time);
        newVehicle.fare = selectedRoute.fare;

        vehicleFile = fopen("vehicles.txt", "a");
        fwrite(&newVehicle, sizeof(Vehicle), 1, vehicleFile);
        fflush(vehicleFile);
        fclose(vehicleFile);

        idFile = fopen("vehicle_id.txt", "w");
        fprintf(idFile, "%d", newVehicle.id);
        fflush(idFile);
        fclose(idFile);

        Booking newBooking;
        FILE *bookingFile, *bookingIdFile;
        int lastBookingId = 0;

        bookingIdFile = fopen("booking_id.txt", "r");
        if (bookingIdFile != NULL) {
            fscanf(bookingIdFile, "%d", &lastBookingId);
            fclose(bookingIdFile);
        }

        newBooking.bookingId = lastBookingId + 1;
        strcpy(newBooking.username, username);
        newBooking.vehicleId = newVehicle.id;
        strcpy(newBooking.seatId, "N/A");
        strcpy(newBooking.date, date);
        newBooking.finalPrice = applyManualDiscount(selectedRoute.fare);

        bookingFile = fopen("bookings.txt", "a");
        fwrite(&newBooking, sizeof(Booking), 1, bookingFile);
        fflush(bookingFile);
        fclose(bookingFile);

        bookingIdFile = fopen("booking_id.txt", "w");
        fprintf(bookingIdFile, "%d", newBooking.bookingId);
        fflush(bookingIdFile);
        fclose(bookingIdFile);

        print_centered("Ticket Confirmed!", width);
        getch();
        viewTicket(newBooking.bookingId);
    } else {
        print_centered("Booking cancelled.", width);
        getch();
    }
}


void viewRevenueDashboard() {
    FILE *bookingFile, *vehicleFile;
    Booking booking;
    Vehicle vehicle;
    int width, height;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    Vehicle vehicles[1000];
    int numVehicles = 0;
    vehicleFile = fopen("vehicles.txt", "r");
    if (vehicleFile != NULL) {
        while (fread(&vehicle, sizeof(Vehicle), 1, vehicleFile)) {
            if (numVehicles < 1000) {
                vehicles[numVehicles++] = vehicle;
            }
        }
        fclose(vehicleFile);
    }

    float totalRevenue = 0;
    float busRevenue = 0, carRevenue = 0, bikeRevenue = 0;
    float totalTodayRevenue = 0;
    float busTodayRevenue = 0, carTodayRevenue = 0, bikeTodayRevenue = 0;

    // Get today's date
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char todayDate[20];
    strftime(todayDate, sizeof(todayDate), "%d-%m-%Y", tm_info);

    bookingFile = fopen("bookings.txt", "r");
    if (bookingFile == NULL) {
        print_centered("No bookings found.", width);
        getch();
        return;
    }

    while (fread(&booking, sizeof(Booking), 1, bookingFile)) {
        totalRevenue += booking.finalPrice;

        // Find vehicle type
        char vehicleType[10] = "Unknown";
        for (int i = 0; i < numVehicles; i++) {
            if (vehicles[i].id == booking.vehicleId) {
                strcpy(vehicleType, vehicles[i].type);
                break;
            }
        }

        // Revenue breakdown
        if (strcasecmp(vehicleType, "Bus") == 0) {
            busRevenue += booking.finalPrice;
        } else if (strcasecmp(vehicleType, "Car") == 0) {
            carRevenue += booking.finalPrice;
        } else if (strcasecmp(vehicleType, "Bike") == 0) {
            bikeRevenue += booking.finalPrice;
        }

        // Today's revenue
        if (strcmp(booking.date, todayDate) == 0) {
            totalTodayRevenue += booking.finalPrice;
            if (strcasecmp(vehicleType, "Bus") == 0) {
                busTodayRevenue += booking.finalPrice;
            }
            else if (strcasecmp(vehicleType, "Car") == 0) {
                carTodayRevenue += booking.finalPrice;
            }
            else if (strcasecmp(vehicleType, "Bike") == 0) {
                bikeTodayRevenue += booking.finalPrice;
            }
        }
    }
    fclose(bookingFile);

    print_centered("--- Revenue Dashboard ---", width);
    print_centered("", width); // New blank line
    print_centered("--- Lifetime Revenue ---", width);
    print_centered("--------------------------------------------------", width);
    char info[200];
    sprintf(info, "Total Revenue: %.2f", totalRevenue);
    print_centered(info, width);
    sprintf(info, "  - Bus:    %.2f", busRevenue);
    print_centered(info, width);
    sprintf(info, "  - Car:    %.2f", carRevenue);
    print_centered(info, width);
    sprintf(info, "  - Bike:   %.2f", bikeRevenue);
    print_centered(info, width);
    print_centered("-----------------------------------------------------------", width);
    sprintf(info, "Total Today's Revenue: %.2f", totalTodayRevenue);
    print_centered(info, width);
    sprintf(info, "  - Bus:    %.2f", busTodayRevenue);
    print_centered(info, width);
    sprintf(info, "  - Car:    %.2f", carTodayRevenue);
    print_centered(info, width);
    sprintf(info, "  - Bike:   %.2f", bikeTodayRevenue);
    print_centered(info, width);
    print_centered("--------------------------", width);
    getch();
}


void setDiscounts() {
    int width, height;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    get_terminal_size(&width, &height);

    float discountPercentage;
    print_centered_no_newline("Enter the global discount percentage (0-100): ", width);
    fflush(stdout);
    scanf("%f", &discountPercentage);

    if (discountPercentage < 0 || discountPercentage > 100) {
        print_centered("Invalid discount percentage. Please enter a value between 0 and 100.", width);
        getch();
        return;
    }

    FILE *file = fopen("discounts.txt", "w");
    if (file == NULL) {
        print_centered("Error opening discount file.", width);
        getch();
        return;
    }

    fprintf(file, "%.2f", discountPercentage);
    fclose(file);

    char msg[100];
    sprintf(msg, "Global discount of %.2f%% has been set.", discountPercentage);
    print_centered(msg, width);
    getch();
}