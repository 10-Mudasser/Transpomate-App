#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>
using namespace std;

// User structure to hold login information
struct User {
    string username;
    string password;
    int role; // 1 for admin, 2 for user
};

// Node structure for route information
struct Node {
    string stop;
    string destination;
    string distanceUnit;
    int distance;
    int time;
    string timeUnit;
    Node* next;
    Node* prev;
};

// Ticket structure to hold ticket details
struct Ticket {
    string busNumber;
    string driverName;
    string departureTime;
    string from;
    string to;
    int seatNumber;
    string stop; // Starting stop for the ticket
    float fare;  // Fare for the ticket
};
struct FuelConsumptionNode {
    float fuelAmount; // Amount of fuel consumed
    FuelConsumptionNode* next; // Pointer to the next node in the list
};
// Feedback structure to hold passenger feedback
struct Feedback {
    string busNumber;
    string driverName;
    string passengerName;
    string feedbackText;
    int rating; // 1-5 scale rating
    string date; // Feedback date
};

class Bus {
public:
    // Admin
    string busn;
    string driver;
    string arrival;
    string depart;
    string from;
    string to;

    // User 
    string seat[8][4];
    Node* head;
    vector<Ticket> tickets;

    // Fuel 
    FuelConsumptionNode* fuelHead;
    vector<Feedback> feedbacks;
    float currentFuel;
    float farePerKm;

    Bus() {
        head = NULL; // Initialize head to null
        fuelHead = NULL; // Initialize fuel head to null
        currentFuel = 0; // Initialize current fuel to 0
        farePerKm = 5.0; // Set a default fare per kilometer (you can change this)
        initSeats(); // Initialize all seats as empty
    }

    // Admin 
    void installBus();
    void availableBuses();
    void showPredefinedStops();
    void addRoute();
    void editRoute();
    void removeRoute();
    void displayRoutes();
    void manageBusSchedule();
    bool editStopByName(Node* head, const string& stopName);
    bool removeStopByName(Node*& head, const string& stopName);
    void updateBusScheduleToFile();

    // User 

    void allotSeat();
    void initSeats();
    void showBusInfo();
    void searchBusInfo(const string& searchTerm);
    void generateTicket(int busIndex, int seatNumber);
    void displayTickets();


    // Manage fuel and feedback

    void addFuelConsumption(float amount); // Function to add fuel consumption record
    void monitorFuelConsumption(); // Function to monitor fuel consumption
    float calculateAverageFuelConsumption(); // Function to calculate average fuel consumption
    float calculateFare(int distance); // Function to calculate the fare based on distance
    int calculateDistance(const string& from, const string& to);
    void collectFeedback(string passengerName, int rating, string feedbackText, string date);
    // Show all feedback for the bus
    void afterTripFeedback();
    void displayFeedback();
};
// Admin interface 


// Array of Bus objects and user storage
Bus bus[10];
static int p = 0;
static int busCount = 0; // Count of buses

const int MAX_STOPS = 5;
vector<User> users; // Store registered users

vector<string> predefinedStops = {
    // Gujranwala Stops
    "Gujranwala City Center", "Gujranwala Railway Station", "Faisal Town", "GT Road",

    // Islamabad Stops
    "Blue Area", "F-6 Markaz", "D-12", "Sector G-11", "Rawalpindi Road",

    // Lahore Stops
    "Lahore Fort", "Badshahi Mosque", "Mall Road", "Liberty Market", "DHA Phase 5", "Gulberg"
};

// horizontal line * print 

void vline(char ch) {
    for (int i = 80; i > 0; i--)
        cout << ch;
}

void parseDistance(const string& input, int& distance, string& unit) {
    stringstream ss(input);
    ss >> distance >> unit; // km or m
}

void parseTime(const string& input, int& time, string& unit) {
    stringstream ss(input);
    ss >> time >> unit; // min or hr
}

void printHeader(const string& title) {
    cout << "\n";
    cout << "                                      _____              _                 _             \n";
    cout << "                                     |  __ \\            | |               | |            \n";
    cout << "                                     | |  | | ___   ___ | | ___   __ _  __| | ___  _ __  \n";
    cout << "                                     | |  | |/ _ \\ / _ \\| |/ _ \\ / ` |/ _` |/ _ \\| ' \\ \n";
    cout << "                                     | || | () | () | | () | (| | (| | () | | | | \n";
    cout << "                                     |/ \\/ \\/||\\/ \\,|\\,|\\/|| || \n";
    cout << "                                                                                         \n";
    cout << "                                                 XPRESS YOUR JOURNEY                     \n";
    cout << "                                     ----------------------------------------------------------\n";
}

void printSeparator() {
    cout << "\n-----------------------------------\n";
}
void displayMainMenu() {
    cout << "\n\t\t\t\t\t\t*"
        << "\n\t\t\t\t\t\t|1. Admin Login            |"
        << "\n\t\t\t\t\t\t|2. User Login             |"
        << "\n\t\t\t\t\t\t|3. Fuel Management        |" // Added fuel management option
        << "\n\t\t\t\t\t\t|4. Passengers Feedback    |"
        << "\n\t\t\t\t\t\t|5. Exit                   |"
        << "\n\t\t\t\t\t\t**"
        << "\n\n\t\t\t\t\t\tEnter your choice:";
}
bool authenticate(const string& username, const string& password, int role) {

    // Password 

    User admin = { "admin", "admin123", 1 };
    User user = { "user", "user123", 2 };

    users.push_back(admin);
    users.push_back(user);
    for (const auto& user : users) {
        if (user.username == username && user.password == password && user.role == role) {
            return true; // Successful authentication
        }
    }
    return false; // Authentication failed
}

int readInteger() {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) { // Check for input failure
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter an integer: ";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the newline from the buffer
            break; // Exit the loop if input is valid
        }
    }
    return value;
}


void Bus::installBus() {
    // Ensure we're not exceeding the array capacity of buses
    if (p >= 10) {
        cout << "Cannot install more buses. Capacity full.\n";
        return;
    }

    // Gather bus details from the user
    cout << "Enter bus number: ";
    cin >> bus[p].busn;

    // Check if the bus number already exists in the file
    ifstream busFile("buses.txt");
    string busNumber;
    bool busExists = false;

    if (busFile.is_open()) {
        while (getline(busFile, busNumber)) {
            // Compare with each bus number in the file
            if (busNumber == bus[p].busn) {
                busExists = true; // Bus number already exists
                break;
            }
            // Skip the next 5 lines in the file, which contain the details of the bus
            for (int i = 0; i < 5; i++) {
                getline(busFile, busNumber); // Skip the lines for driver, arrival, etc.
            }
        }
        busFile.close();
    }
    else {
        cerr << "Error: Unable to open buses file.\n";
        return;
    }

    if (busExists) {
        cout << "Bus number already exists in the file. Please enter a unique bus number.\n";
        return;
    }

    // Gather remaining bus details
    cout << "\nEnter Driver's name: ";
    cin.ignore(); // Clear the input buffer before using getline
    getline(cin, bus[p].driver);

    cout << "\nArrival time (e.g., 10:00 AM): ";
    getline(cin, bus[p].arrival);

    cout << "\nDeparture time (e.g., 5:00 PM): ";
    getline(cin, bus[p].depart);

    cout << "\nFrom (start location): ";
    getline(cin, bus[p].from);

    cout << "\nTo (end location): ";
    getline(cin, bus[p].to);

    // Add the bus details to the file
    ofstream outFile("buses.txt", ios::app); // Open file in append mode
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open buses file.\n";
        return;
    }

    outFile << bus[p].busn << "\n"
        << bus[p].driver << "\n"
        << bus[p].arrival << "\n"
        << bus[p].depart << "\n"
        << bus[p].from << "\n"
        << bus[p].to << "\n";
    outFile.close();

    // Increment the bus counter
    p++;

    cout << "Bus successfully installed and saved to file!" << endl;
}
void Bus::availableBuses() {
    // Open the file to read bus data
    ifstream busFile("buses.txt");
    if (!busFile.is_open()) {
        cerr << "Error: Unable to open buses file.\n";
        return;
    }

    // Read all bus details from the file
    string busNumber, driver, arrival, depart, from, to;

    // Check if there are any buses in the file
    bool busExists = false;
    while (getline(busFile, busNumber)) {
        // Bus exists, we read the remaining details
        busExists = true;
        getline(busFile, driver);
        getline(busFile, arrival);
        getline(busFile, depart);
        getline(busFile, from);
        getline(busFile, to);

        // Display the bus details
        cout << "Bus Number: " << busNumber << "\n";
        cout << "Driver: " << driver << "\n";
        cout << "Arrival Time: " << arrival << "\n";
        cout << "Departure Time: " << depart << "\n";
        cout << "From: " << from << "\n";
        cout << "To: " << to << "\n";
        cout << "----------------------------------------\n";
    }

    if (!busExists) {
        cout << "No buses available.\n";
    }

    busFile.close();
}


void Bus::showPredefinedStops() {
    cout << "\nPredefined Stops:\n";
    for (int i = 0; i < predefinedStops.size(); ++i) {
        cout << i + 1 << ". " << predefinedStops[i] << endl;
    }
}

void Bus::addRoute() {
    Node* newNode = new Node();

    // Display predefined stops for user to choose from
    showPredefinedStops();

    // Ask user to select a stop (by name)
    string stopChoice;
    bool validStop = false;

    // for enter input show any work or invalid 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do {
        cout << "\nEnter a stop name from the list: ";
        getline(cin, stopChoice);

        // Check if the entered stop is in the predefined stops list
        for (const auto& stop : predefinedStops) {
            if (stop == stopChoice) {
                newNode->stop = stopChoice; // Assign selected stop
                validStop = true;
                break;
            }
        }

        if (!validStop) {
            cout << "Invalid stop! Please choose a valid stop from the list.\n";
        }
    } while (!validStop);

    // Ask user to select a destination (by name)
    cout << "\nEnter a destination name from the list: ";
    getline(cin, newNode->destination);

    // Ask user for distance input
    string distanceInput;
    cout << "\nEnter distance (e.g., 50 km): ";
    getline(cin, distanceInput);
    parseDistance(distanceInput, newNode->distance, newNode->distanceUnit); // Parse distance and its unit

    // Ask user for time input
    string timeInput;
    cout << "\nEnter time to reach (e.g., 30 min): ";
    getline(cin, timeInput);
    parseTime(timeInput, newNode->time, newNode->timeUnit); // Parse time and its unit

    newNode->next = nullptr; // Initialize the next pointer to nullptr

    // If list is empty, set the new node as head
    if (head == nullptr) {
        newNode->prev = nullptr; // Previous pointer for the head is null
        head = newNode;          // Set head to the new node
    }
    else {
        Node* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next; // Traverse to the last node
        }
        temp->next = newNode; // Link the last node to the new node
        newNode->prev = temp; // Set the previous pointer of the new node
    }

    // Write the route details to a file
    ofstream routeFile("routes.txt", ios::app); // Open the file in append mode
    if (!routeFile.is_open()) {
        cerr << "Error: Unable to open routes file.\n";
        return;
    }

    routeFile << "Stop: " << newNode->stop << "\n"
        << "Destination: " << newNode->destination << "\n"
        << "Distance: " << newNode->distance << " " << newNode->distanceUnit << "\n"
        << "Time: " << newNode->time << " " << newNode->timeUnit << "\n"
        << "-----------------------------\n";

    routeFile.close();

    cout << "\nRoute added successfully and saved to file!\n";
}

void Bus::editRoute() {
    string routeName;
    bool routeFound = false;

    // Ask user if they want to proceed with editing the route
    char proceed;
    cout << "\nDo you want to proceed with editing the route? (y/n): ";
    cin >> proceed;
    cin.ignore();  // To clear the newline character from the input buffer

    if (proceed == 'n' || proceed == 'N') {
        cout << "Route edit cancelled.\n";
        return; // Exit without making any changes
    }

    // Display the existing routes
    cout << "\nExisting Routes:\n";
    displayRoutes();  // Assuming you have a displayRoutes function that shows all routes

    // Ask the user to select the route to edit
    cout << "\nEnter the name of the route you want to edit: ";
    getline(cin, routeName);

    // Convert routeName to lowercase for case-insensitive comparison
    transform(routeName.begin(), routeName.end(), routeName.begin(), ::tolower);

    // Search for the route to edit
    Node* temp = head;
    while (temp != nullptr) {
        string stopLower = temp->stop;
        string destinationLower = temp->destination;

        // Convert both stop and destination to lowercase for case-insensitive comparison
        transform(stopLower.begin(), stopLower.end(), stopLower.begin(), ::tolower);
        transform(destinationLower.begin(), destinationLower.end(), destinationLower.begin(), ::tolower);

        if (stopLower == routeName || destinationLower == routeName) {
            routeFound = true;
            break;
        }
        temp = temp->next;
    }

    if (!routeFound) {
        cout << "Route not found.\n";
        return;
    }

    // Proceed with editing the route
    string newStop, newDestination;
    bool validStop = false, validDestination = false;

    do {
        cout << "\nEnter new stop name: ";
        getline(cin, newStop);

        for (const auto& stop : predefinedStops) {
            if (stop == newStop) {
                temp->stop = newStop;
                validStop = true;
                break;
            }
        }

        if (!validStop) {
            cout << "Invalid stop! Please choose a valid stop.\n";
        }
    } while (!validStop);

    do {
        cout << "\nEnter new destination name: ";
        getline(cin, newDestination);

        for (const auto& stop : predefinedStops) {
            if (stop == newDestination) {
                temp->destination = newDestination;
                validDestination = true;
                break;
            }
        }

        if (!validDestination) {
            cout << "Invalid destination! Please choose a valid destination.\n";
        }
    } while (!validDestination);

    // Ask user for new distance and time
    string distanceInput, timeInput;
    cout << "\nEnter new distance (e.g., 50 km): ";
    getline(cin, distanceInput);
    parseDistance(distanceInput, temp->distance, temp->distanceUnit);

    cout << "\nEnter new time to reach (e.g., 30 min): ";
    getline(cin, timeInput);
    parseTime(timeInput, temp->time, temp->timeUnit);

    // Update the route details in the file
    vector<Node> routes;
    Node* current = head;

    // Collect all routes in memory
    while (current != nullptr) {
        routes.push_back(*current);
        current = current->next;
    }

    // Overwrite the file with updated routes
    ofstream routeFile("routes.txt", ios::trunc); // Open file in truncate mode
    if (!routeFile.is_open()) {
        cerr << "Error: Unable to open routes file.\n";
        return;
    }

    for (const auto& route : routes) {
        routeFile << "Stop: " << route.stop << "\n"
            << "Destination: " << route.destination << "\n"
            << "Distance: " << route.distance << " " << route.distanceUnit << "\n"
            << "Time: " << route.time << " " << route.timeUnit << "\n"
            << "-----------------------------\n";
    }

    routeFile.close();

    cout << "\nRoute updated successfully and saved to file!\n";
}


void Bus::removeRoute() {
    string routeName;
    bool routeFound = false;

    // Ask the user if they want to remove a route
    char removeChoice;
    cout << "Do you want to remove a route? (y/n): ";
    cin >> removeChoice;
    cin.ignore();  // Clear the newline character from the input buffer

    if (removeChoice == 'n' || removeChoice == 'N') {
        cout << "Skipping remove operation.\n";
        return;  // Exit if the user doesn't want to remove
    }

    // If the user wants to remove, display the existing routes
    cout << "\nExisting Routes:\n";
    displayRoutes();  // Assuming you have a displayRoutes function that shows all routes

    // Ask the user to select the route to remove
    cout << "\nEnter the name of the route you want to remove: ";
    getline(cin, routeName);

    // Search for the route to remove
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->stop == routeName || temp->destination == routeName) {
            routeFound = true;
            break;
        }
        temp = temp->next;
    }

    if (!routeFound) {
        cout << "Route not found.\n";
        return;
    }

    // Handle removal of node from the linked list
    if (temp->prev != nullptr) {
        temp->prev->next = temp->next;
    }
    else {
        head = temp->next; // Update head if it's the first node
    }

    if (temp->next != nullptr) {
        temp->next->prev = temp->prev;
    }

    delete temp;  // Free memory of the removed route

    // Update the file to exclude the removed route
    ofstream routeFile("routes.txt", ios::trunc); // Open file in truncate mode
    if (!routeFile.is_open()) {
        cerr << "Error: Unable to open routes file.\n";
        return;
    }

    // Write all remaining routes back to the file
    Node* current = head;
    while (current != nullptr) {
        routeFile << "Stop: " << current->stop << "\n"
            << "Destination: " << current->destination << "\n"
            << "Distance: " << current->distance << " " << current->distanceUnit << "\n"
            << "Time: " << current->time << " " << current->timeUnit << "\n"
            << "-----------------------------\n";
        current = current->next;
    }

    routeFile.close();

    cout << "\nRoute removed successfully and saved to file!\n";
}



void Bus::displayRoutes() {
    if (head == NULL) {
        // If no routes in memory, attempt to read from the file
        ifstream routeFile("routes.txt");
        if (!routeFile.is_open()) {
            cout << "No routes available and unable to open file.\n";
            return;
        }

        cout << "Routes from file:\n";
        string line;
        while (getline(routeFile, line)) {
            cout << line << endl;
        }
        routeFile.close();
        return;
    }

    // If routes are available in the linked list, display them
    Node* temp = head;
    int routeNumber = 1;
    cout << "Routes in memory:\n";
    while (temp != NULL) {
        cout << routeNumber++ << ". Stop: " << temp->stop
            << ", Destination: " << temp->destination
            << "  Distance: " << temp->distance << " " << temp->distanceUnit
            << "  Time: " << temp->time << " " << temp->timeUnit << endl;
        temp = temp->next;
    }
}
void Bus::manageBusSchedule() {
    ifstream busScheduleFile("bus_schedule.txt");
    if (!busScheduleFile.is_open()) {
        cerr << "Error: Unable to open bus schedule file.\n";
        return;
    }

    for (int i = 0; i < p; i++) {  // Loop through all buses
        cout << "Bus " << bus[i].busn << " Schedule:\n";
        cout << "Driver: " << bus[i].driver << "\n";
        cout << "Arrival Time: " << bus[i].arrival << "\n";
        cout << "Departure Time: " << bus[i].depart << "\n";
        cout << "Total Seats: " << 32 << "\n"; // Assuming all buses have 32 seats
        cout << "Stops: \n";

        // Display stops for the bus from file
        Node* temp = bus[i].head; // Start from the head of the route
        int stopNumber = 1;
        while (temp != nullptr) {
            cout << stopNumber << ". Stop: " << temp->stop << " | Destination: "
                << temp->destination << " | Distance: " << temp->distance
                << " km | Time: " << temp->time << " mins\n";
            temp = temp->next; // Move to the next stop
            stopNumber++;
        }

        // Ask if user wants to edit a stop
        cout << "Do you want to edit any stops? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            cout << "Enter the stop name to edit: ";
            string stopName;
            cin.ignore();  // To clear any leftover newline characters
            getline(cin, stopName);
            if (!editStopByName(bus[i].head, stopName)) {
                cout << "Stop not found.\n";
            }
        }

        // Ask if user wants to remove a stop
        cout << "Do you want to remove any stops? (y/n): ";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            cout << "Enter the stop name to remove: ";
            string stopName;
            cin.ignore();  // To clear any leftover newline characters
            getline(cin, stopName);
            if (!removeStopByName(bus[i].head, stopName)) {
                cout << "Stop not found.\n";
            }
        }

        cout << "------------------------\n";
    }

    busScheduleFile.close();

    // After editing, save the updated bus schedule back to the file
    ofstream busScheduleOut("bus_schedule.txt");
    if (!busScheduleOut.is_open()) {
        cerr << "Error: Unable to open bus schedule file for writing.\n";
        return;
    }

    // Write bus data and stops to the file
    for (int i = 0; i < p; i++) {
        busScheduleOut << bus[i].busn << endl;  // Bus number
        busScheduleOut << bus[i].driver << endl; // Driver's name
        busScheduleOut << bus[i].arrival << endl;  // Arrival time
        busScheduleOut << bus[i].depart << endl;  // Departure time

        // Write stops for the bus
        Node* temp = bus[i].head;
        while (temp != nullptr) {
            busScheduleOut << temp->stop << endl;
            busScheduleOut << temp->destination << endl;
            busScheduleOut << temp->distance << endl;
            busScheduleOut << temp->time << endl;
            temp = temp->next;
        }
    }

    busScheduleOut.close();
}

bool Bus::editStopByName(Node* head, const string& stopName) {
    Node* temp = head;
    while (temp != nullptr) {
        displayRoutes();
        if (temp->stop == stopName) {
            cout << "Editing stop: " << temp->stop << "\n";
            cout << "Enter new stop name: ";
            getline(cin, temp->stop);

            cout << "Enter new destination: ";
            getline(cin, temp->destination);

            cout << "Enter new distance (km): ";
            cin >> temp->distance;

            cout << "Enter new time (mins): ";
            cin >> temp->time;

            cout << "Stop updated successfully!\n";
            return true;  // Stop found and updated
        }
        temp = temp->next;  // Move to the next stop
    }
    return false;  // Stop not found
}

bool Bus::removeStopByName(Node*& head, const string& stopName) {
    if (head == nullptr) {
        cout << "No stops to remove.\n";
        return false;
    }

    Node* temp = head;

    // If the stop to be removed is the first node
    if (head->stop == stopName) {
        head = temp->next; // Move the head to the next stop
        delete temp;
        cout << "Stop removed successfully!\n";
        return true;
    }

    // Find the stop to be removed
    while (temp != nullptr && temp->next != nullptr) {
        if (temp->next->stop == stopName) {
            Node* toDelete = temp->next;
            temp->next = temp->next->next;  // Remove the stop from the list
            delete toDelete;
            cout << "Stop removed successfully!\n";
            return true;
        }
        temp = temp->next;
    }

    return false;  // Stop not found
}

// calling all functions 

void promptUserForCredentials(int role) {
    string username, password;
    cout << (role == 1 ? "Admin" : "User") << " Username: ";
    cin >> username;
    cout << (role == 1 ? "Admin" : "User") << " Password: ";
    cin >> password;

    if (authenticate(username, password, role)) {
        cout << "\n" << (role == 1 ? "Admin" : "User") << " login successful!\n";
        cin.ignore(); // Wait for Enter press

        // Clear the screen and show next view
        system("cls");
        system("color 4");
        if (role == 1) {
            // Admin functionalities
            int adminChoice;
            do {
                cout << "\n\t\t\t\t\t\t**"
                    << "\n\t\t\t\t\t\t|1. Install Bus           |"
                    << "\n\t\t\t\t\t\t|2. Show Buses            |"
                    << "\n\t\t\t\t\t\t|3. Manage Routes         |"
                    << "\n\t\t\t\t\t\t|4. Manage Bus Schedule   |"
                    << "\n\t\t\t\t\t\t|5. Exit Admin Menu       |"
                    << "\n\t\t\t\t\t\t*"
                    << "\n\n\t\t\t\t\t\tChoose an option: ";
                cin >> adminChoice;
                switch (adminChoice) {
                case 1:
                    bus[p].installBus();
                    break;
                case 2:
                    bus[0].availableBuses();
                    break;
                case 3:
                    bus[0].addRoute();
                    bus[0].editRoute();
                    bus[0].removeRoute();
                    bus[0].displayRoutes();
                    break;
                case 4: {
                    int numPassengers;
                    cout << "Enter the number of passengers: ";
                    cin >> numPassengers;
                    bus[0].manageBusSchedule();
                    break;
                }
                case 5:
                    cout << "\nExiting Admin Menu.\n";
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
                }
                // Pause to view output
                if (adminChoice != 5) {
                    cout << "Press Enter to return to the menu...";
                    cin.ignore();
                    cin.get();
                    system("cls");
                }
            } while (adminChoice != 5);
        }

        // User expalin by 2nd memeber

        else {
            // Clear the screen and show next view
            system("cls");
            system("color 46");
            int userChoice;
            do {
                cout << "\n\t\t\t\t\t\t*"
                    << "\n\t\t\t\t\t\t|1. Reserve Seat           |"
                    << "\n\t\t\t\t\t\t|2. Show Bus Information   |"
                    << "\n\t\t\t\t\t\t|3. Search Bus Information |"
                    << "\n\t\t\t\t\t\t|4. View Tickets           |"
                    << "\n\t\t\t\t\t\t|5. Enter your Feedback    |"
                    << "\n\t\t\t\t\t\t|6. Exit User Menu         |"
                    << "\n\t\t\t\t\t\t**"
                    << "\n\n\t\t\t\t\t\tChoose an option: ";
                cin >> userChoice;
                switch (userChoice) {
                case 1:
                    bus[0].allotSeat(); // Assuming seat allotment is handled for the first bus
                    break;
                case 2:
                    bus[0].showBusInfo();
                    break;
                case 3: {
                    string searchTerm;
                    cout << "Enter bus number or destination to search: ";
                    cin >> searchTerm;
                    bus[0].searchBusInfo(searchTerm);
                    break;
                }
                case 4:
                    bus[0].displayTickets(); // View tickets option
                    break;
                case 5:
                    bus[0].afterTripFeedback();
                    break;
                case 6:
                    cout << "\nExiting User Menu\n";
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
                }
            } while (userChoice != 6);
        }
    }
    else {
        cout << "\n" << (role == 1 ? "Admin" : "User") << " login failed! Please try again.\n";
    }
}

// Ending my working 

// 2nd memeber:

// User interface 


void Bus::initSeats() {
    ifstream inFile("seats.txt");  // Open the file for reading
    if (inFile.is_open()) {
        // Read the seat availability from the file
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 4; j++) {
                inFile >> seat[i][j];  // Load the seat status from the file
            }
        }
        inFile.close();  // Close the file after reading
    }
    else {
        // If the file doesn't exist or can't be opened, initialize seats as "Available"
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 4; j++) {
                seat[i][j] = "Available";  // Initialize all seats as available
            }
        }
        // Write the initial seat arrangement to the file
        ofstream outFile("seats.txt");  // Open the file for writing
        if (outFile.is_open()) {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 4; j++) {
                    outFile << seat[i][j] << " ";  // Write seat status to file
                }
                outFile << endl;  // Newline for the next row of seats
            }
            outFile.close();  // Close the file after writing
        }
    }
}
void Bus::searchBusInfo(const string& searchTerm) {
    ifstream inFile("buses.txt");  // Open the file containing bus information
    bool found = false;

    if (inFile.is_open()) {
        string busNumber, fromCity, toCity;

        // Read buses from the file
        while (inFile >> busNumber >> fromCity >> toCity) {
            // Search for the term in bus number, from city, or to city
            if (busNumber == searchTerm || fromCity == searchTerm || toCity == searchTerm) {
                // Bus found, show its information (you would implement showBusInfo based on your class)
                cout << "Bus Number: " << busNumber << endl;
                cout << "From: " << fromCity << endl;
                cout << "To: " << toCity << endl;
                found = true;
            }
        }
        inFile.close();  // Close the file after reading
    }
    else {
        cout << "Error opening the file!\n";
    }

    if (!found) {
        cout << "No bus found with the given search term.\n";
    }
}

void Bus::showBusInfo() {
    // Display information about the bus
    cout << "\n======================== Show Total Information  ========================\n";
    cout << "Bus No: " << busn << "                                                             |\n";
    cout << "Driver: " << driver << "                                                          |\n";
    cout << "Arrival: " << arrival << "                                                        |\n";
    cout << "Departure: " << depart << "                                                     |\n";
    cout << "From: " << from << "                                                               |\n";
    cout << "To: " << to << "                                                                 |\n";
    cout << "=========================================================================\n";

    // Write the bus information to a file
    ofstream busFile("bus_info.txt", ios::app); // Open in append mode
    if (!busFile.is_open()) {
        cerr << "Error: Unable to open bus info file for writing.\n";
        return;
    }

    busFile << "======================== Bus Information ========================\n";
    busFile << "Bus No: " << busn << "\n";
    busFile << "Driver: " << driver << "\n";
    busFile << "Arrival: " << arrival << "\n";
    busFile << "Departure: " << depart << "\n";
    busFile << "From: " << from << "\n";
    busFile << "To: " << to << "\n";
    busFile << "=================================================================\n";

    busFile.close();
    cout << "Bus information saved to file successfully!\n";
}


void Bus::allotSeat() {
    // Display bus number and available seats
    cout << "Bus Number: " << busn << "\n";
    cout << "Available Seats:\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            cout << "Seat " << i * 4 + j + 1 << ": " << seat[i][j] << "\n";
        }
    }

    // Ask user to choose a seat
    int seatNumber;
    cout << "Choose a seat number: ";
    cin >> seatNumber;

    int row = (seatNumber - 1) / 4;
    int col = (seatNumber - 1) % 4;

    if (row >= 0 && row < 8 && col >= 0 && col < 4) {
        if (seat[row][col] == "Available") {
            seat[row][col] = "Booked"; // Mark the seat as booked
            cout << "Seat " << seatNumber << " has been booked successfully.\n";

            // Save updated seating information to file
            ofstream seatFile("seating_info.txt", ios::app); // Open in append mode
            if (!seatFile.is_open()) {
                cerr << "Error: Unable to open seating info file.\n";
                return;
            }

            // Write booking details to file
            seatFile << "Bus Number: " << busn << "\n";
            seatFile << "Seat " << seatNumber << ": Booked\n";
            seatFile.close();

            // Optionally generate a ticket
            generateTicket(p - 1, seatNumber);
        }
        else {
            cout << "Sorry, this seat is already booked.\n";
        }
    }
    else {
        cout << "Invalid seat number!\n";
    }
}


void Bus::generateTicket(int busIndex, int seatNumber) {
    // Create a new ticket object
    Ticket newTicket;
    newTicket.busNumber = bus[busIndex].busn;
    newTicket.driverName = bus[busIndex].driver;
    newTicket.departureTime = bus[busIndex].depart;
    newTicket.from = bus[busIndex].from;
    newTicket.to = bus[busIndex].to;
    newTicket.seatNumber = seatNumber;

    // Determine the starting stop for the ticket
    Node* current = bus[busIndex].head;
    if (current != NULL) {
        newTicket.stop = current->stop; // Assume the first stop for the ticket
    }

    // Calculate the fare based on the distance (assuming distance is in kilometers)
    int distance = 0;
    current = bus[busIndex].head;

    // Traverse through the bus stops and calculate the total distance
    while (current != NULL && current->destination != newTicket.to) {
        distance += current->distance;  // Add distance between stops
        current = current->next;
    }

    // Debugging: Check if distance is calculated correctly
    cout << "Calculated Distance: " << distance << " km" << endl;

    // If distance is zero, we can't calculate the fare properly
    if (distance == 0) {
        cout << "Error: Unable to calculate fare, distance is zero!" << endl;
        return;
    }

    // Calculate the fare based on the distance and fare per km
    float fare = bus[busIndex].calculateFare(distance);
    newTicket.fare = fare;

    // Debugging: Check the calculated fare
    cout << "Calculated Fare: " << fare << " PRK" << endl;

    // Add the new ticket to the tickets vector
    tickets.push_back(newTicket);

    // Output the ticket details to the console
    cout << "Ticket generated!" << endl;
    cout << "Bus Number: " << newTicket.busNumber << endl;
    cout << "Driver: " << newTicket.driverName << endl;
    cout << "From: " << newTicket.from << " To: " << newTicket.to << endl;
    cout << "Seat: " << newTicket.seatNumber << endl;
    cout << "Fare: " << newTicket.fare << " PRK " << endl;

    // Save the ticket details to a file
    ofstream ticketFile("tickets.txt", ios::app); // Open in append mode
    if (!ticketFile.is_open()) {
        cerr << "Error: Unable to open tickets file.\n";
        return;
    }

    ticketFile << "Ticket Details:\n";
    ticketFile << "Bus Number: " << newTicket.busNumber << "\n";
    ticketFile << "Driver: " << newTicket.driverName << "\n";
    ticketFile << "From: " << newTicket.from << " To: " << newTicket.to << "\n";
    ticketFile << "Departure Time: " << newTicket.departureTime << "\n";
    ticketFile << "Seat: " << newTicket.seatNumber << "\n";
    ticketFile << "Fare: " << newTicket.fare << " PRK\n";
    ticketFile << "-----------------------------\n";

    ticketFile.close();
}


void Bus::displayTickets() {
    cout << "\n======================== Booked Tickets ========================\n";

    // Open the file in read mode
    ifstream ticketFile("tickets.txt");
    if (!ticketFile.is_open()) {
        cerr << "Error: Unable to open tickets file.\n";
        return;
    }

    // Check if the file is empty
    ticketFile.seekg(0, ios::end); // Move to the end of the file
    if (ticketFile.tellg() == 0) { // Check if the file size is 0
        cout << "No tickets booked yet.\n";
        ticketFile.close();
        return;
    }

    // Reset the file pointer and read ticket data line by line
    ticketFile.seekg(0, ios::beg); // Move to the beginning of the file
    string line;
    while (getline(ticketFile, line)) {
        cout << line << "\n";
    }

    ticketFile.close();

    cout << "=================================================================\n";
}
// 3rd memeber

// Fuel mangement 

float Bus::calculateFare(int distance) {
    if (distance <= 0) {
        cout << "Error: Invalid distance for fare calculation!" << endl;
        return 0;
    }
    return farePerKm * distance;  // Fare calculation based on fare per km and distance
}

int Bus::calculateDistance(const string& from, const string& to) {
    // You can create a method to calculate distance based on stops
    // For example, if you have the distance in the route information (Node structure), 
    // you could look it up like this:

    Node* current = head; // Start from the first stop
    while (current != NULL) {
        if (current->stop == from) {
            // Find the distance between 'from' and 'to'
            Node* temp = current;
            while (temp != nullptr) {
                if (temp->stop == to) {
                    return temp->distance;  // Return the distance found
                }
                temp = temp->next;
            }
        }
        current = current->next;
    }

    return 0;  // Default return value if no distance is found
}



void Bus::addFuelConsumption(float amount) {
    FuelConsumptionNode* newNode = new FuelConsumptionNode();
    newNode->fuelAmount = amount; // Store the fuel amount
    newNode->next = NULL; // Initialize next to nullptr

    if (fuelHead == NULL) {
        fuelHead = newNode; // Set head to new node if list is empty
    }
    else {
        FuelConsumptionNode* temp = fuelHead;
        while (temp->next != NULL) {
            temp = temp->next; // Traverse to the end of the list
        }
        temp->next = newNode; // Add the new node at the end
    }
    currentFuel += amount; // Update current fuel level
}

void Bus::monitorFuelConsumption() {
    float averageFuel = calculateAverageFuelConsumption(); // Get average fuel consumption
    cout << "Average fuel consumption: " << averageFuel << " liters\n";
    if (currentFuel > averageFuel) {
        cout << "Fuel consumption is normal.\n";
    }
    else {
        cout << "Warning: Current fuel level is below the average consumption\n";
    }
}


float Bus::calculateAverageFuelConsumption() {
    float totalFuel = 0;
    int count = 0;
    FuelConsumptionNode* current = fuelHead;
    while (current != NULL) {
        totalFuel += current->fuelAmount;
        count++;
        current = current->next;
    }
    return count > 0 ? totalFuel / count : 0; // Return average or 0 if no records
}

void fuelManagementMenu() {
    int choice;
    float fuelAmount;
    // Clear the screen and show next view
    system("cls");

    system("color DE");
    do {
        cout << "\n\t\t\t\t\t\t**"
            << "\n\t\t\t\t\t\t|    Fuel Management Menu    |"
            << "\n\t\t\t\t\t\t| 1. Add Fuel Consumption    |"
            << "\n\t\t\t\t\t\t| 2. Monitor Fuel Consumption|"
            << "\n\t\t\t\t\t\t| 3. Calculate Average Fuel  |"
            << "\n\t\t\t\t\t\t|    Consumption             |"
            << "\n\t\t\t\t\t\t| 4. Back to Main Menu       |"
            << "\n\t\t\t\t\t\t**"
            << "\n\n\t\t\t\t\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter fuel amount (in liters): ";
            cin >> fuelAmount;
            bus[0].addFuelConsumption(fuelAmount); // Assuming working with the first bus
            break;
        case 2:
            bus[0].monitorFuelConsumption(); // Monitor fuel consumption
            break;
        case 3: {
            float average = bus[0].calculateAverageFuelConsumption();
            cout << "Average Fuel Consumption: " << average << " liters\n";
            break;
        }
        case 4:
            cout << "Returning to Main Menu...\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 4);
}

void Bus::collectFeedback(string passengerName, int rating, string feedbackText, string date) {
    // Create a new feedback entry
    Feedback newFeedback;
    newFeedback.busNumber = busn;
    newFeedback.driverName = driver;
    newFeedback.passengerName = passengerName;
    newFeedback.rating = rating;
    newFeedback.feedbackText = feedbackText;
    newFeedback.date = date;

    // Store the feedback in the vector
    feedbacks.push_back(newFeedback);

    // Write the feedback to a file (appending to keep previous feedback)
    ofstream feedbackFile("feedbacks.txt", ios::app);  // Open file in append mode
    if (!feedbackFile.is_open()) {
        cerr << "Error: Unable to open feedback file for writing.\n";
        return;
    }

    // Write the feedback data to the file
    feedbackFile << "Bus Number: " << newFeedback.busNumber << endl;
    feedbackFile << "Driver: " << newFeedback.driverName << endl;
    feedbackFile << "Passenger: " << newFeedback.passengerName << endl;
    feedbackFile << "Rating: " << newFeedback.rating << endl;
    feedbackFile << "Feedback: " << newFeedback.feedbackText << endl;
    feedbackFile << "Date: " << newFeedback.date << endl;
    feedbackFile << "--------------------------------------" << endl;

    feedbackFile.close();  // Close the file after writing

    cout << "Thank you for your feedback!\n";
}

void Bus::displayFeedback() {

    // Clear the screen and show the next view
    system("cls");

    system("color 0a");
    // Load feedback from file into the feedbacks vector if empty
    if (feedbacks.empty()) {
        ifstream feedbackFile("feedbacks.txt");
        if (feedbackFile.is_open()) {
            string line;
            Feedback tempFeedback;

            while (getline(feedbackFile, line)) {
                if (line.find("Bus Number: ") == 0) { // If it starts with "Bus Number:"
                    if (!tempFeedback.busNumber.empty()) {
                        feedbacks.push_back(tempFeedback);  // Save the last feedback
                    }
                    tempFeedback = Feedback();  // Create a new feedback entry
                    tempFeedback.busNumber = line.substr(12);  // Extract bus number
                }
                else if (line.find("Driver: ") == 0) {
                    tempFeedback.driverName = line.substr(8);  // Extract driver name
                }
                else if (line.find("Passenger: ") == 0) {
                    tempFeedback.passengerName = line.substr(11);  // Extract passenger name
                }
                else if (line.find("Rating: ") == 0) {
                    tempFeedback.rating = stoi(line.substr(8));  // Extract rating
                }
                else if (line.find("Feedback: ") == 0) {
                    tempFeedback.feedbackText = line.substr(10);  // Extract feedback text
                }
                else if (line.find("Date: ") == 0) {
                    tempFeedback.date = line.substr(6);  // Extract date
                }
            }
            // Push the last feedback after exiting the loop
            if (!tempFeedback.busNumber.empty()) {
                feedbacks.push_back(tempFeedback);
            }
            feedbackFile.close();  // Close the file after reading
        }
    }

    // Display the feedbacks
    if (feedbacks.empty()) {
        cout << "No feedback available yet.\n";
    }
    else {
        cout << "\n================== Passenger Feedback ==================\n";
        for (const auto& feedback : feedbacks) {
            cout << "Bus Number: " << feedback.busNumber << "\n";
            cout << "Driver: " << feedback.driverName << "\n";
            cout << "Passenger: " << feedback.passengerName << "\n";
            cout << "Rating: " << feedback.rating << "/5\n";
            cout << "Date: " << feedback.date << "\n";
            cout << "Feedback: " << feedback.feedbackText << "\n";
            cout << "-------------------------------------------------------\n";
        }
    }
}

void Bus::afterTripFeedback() {
    string passengerName, feedbackText, date;
    int rating;

    cout << "Enter your name: ";
    cin >> passengerName;

    cout << "Rate the trip (1-5): ";
    cin >> rating;

    cin.ignore();  // To ignore any leftover newline character in the buffer
    cout << "Enter your feedback: ";
    getline(cin, feedbackText);

    cout << "Enter the date of your trip (format: YYYY-MM-DD): ";
    cin >> date;

    // Collect feedback and store in a vector
    collectFeedback(passengerName, rating, feedbackText, date);

    // Now save the feedback to a file (feedbacks.txt)
    ofstream feedbackFile("feedbacks.txt", ios::app);  // Open the file in append mode
    if (feedbackFile.is_open()) {
        feedbackFile << "Bus Number: " << busn << "\n";
        feedbackFile << "Driver: " << driver << "\n";
        feedbackFile << "Passenger: " << passengerName << "\n";
        feedbackFile << "Rating: " << rating << "/5\n";
        feedbackFile << "Date: " << date << "\n";
        feedbackFile << "Feedback: " << feedbackText << "\n";
        feedbackFile << "-------------------------------------------------------\n";

        feedbackFile.close();  // Close the file after writing
        cout << "Thank you for your feedback! Your response has been saved.\n";
    }
    else {
        cout << "Unable to save feedback. Please try again later.\n";
    }
}


int main() {
    system("color 3");

    system("cls");
    cout << "\n";
    cout << "                                    ###############################################\n";
    cout << "                                    #                                             #\n";
    cout << "                                    #             WELCOME TO THE                  #\n";
    cout << "                                    #                                             #\n";
    cout << "                                    #       _______   ______   _    _   ______    #\n";
    cout << "                                    #      |__   __| |  __  | | |  | | |  __  \\   #\n";
    cout << "                                    #         | |    | |  | | | |  | | | |) |   #\n";
    cout << "                                    #         | |    | |  | | | |  | | |  _  /    #\n";
    cout << "                                    #         | |    | || | | || | | | \\ \\    #\n";
    cout << "                                    #         ||    ||  \\/  ||  \\_\\   #\n";
    cout << "                                    #                                             #\n";
    cout << "                                    #            TRANSPOMATE APP                  #\n";
    cout << "                                    #                                             #\n";
    cout << "                                    ###############################################\n";
    cout << "\n";

    cout << "\n";
    cout << "\n";


    // Wait for user to press Enter
    cout << "\n                                              Press Enter to continue...";
    cin.ignore(); // Wait for Enter press

    // Clear the screen and show next view
    system("cls");
    system("color 2");
    printHeader("Xpress Journey App");
    int choice;
    do {
        displayMainMenu();
        choice = readInteger(); // Use safe input method
        switch (choice) {
        case 1:
            promptUserForCredentials(1); // Admin role

            break;
        case 2:
            promptUserForCredentials(2); // User role

            break;
        case 3:
            fuelManagementMenu(); // Fuel management function

            break;
        case 4:
            bus[0].displayFeedback();

            break;
        case 5:
            cout << "Exiting the application. Thank you!\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 5);
    return 0;
}