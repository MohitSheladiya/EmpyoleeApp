#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <occi.h>
#include <iomanip>
#include <string>

using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct Employee {
    int employeeNumber;
    char lastName[50];
    char firstName[50];
    char extension[10];
    char email[100];
    char officecode[10];
    int  reportsTo;
    char jobTitle[50];
};

//returns an int value; selected by the user from the menu
int menu(void) {

    //displaying title
    cout << "******* HR Menu *******" << endl;
    cout << "1) Find Employee" << endl;
    cout << "2) Employees Report" << endl;
    cout << "3) Add Employee" << endl;
    cout << "4) Update Employee" << endl;
    cout << "5) Remove Employee" << endl;
    cout << "0) Exit" << endl;
    cout << "Enter an option (0-5): ";
    int choice;
    cin >> choice;
    return choice;
}

//returns 0 if the employee does not exist
int findEmployee(Connection* conn, int employeeNumber, Employee* emp) {
    int	empNum = 0;
    string firstName;
    string lastName;
    string extension;
    string email;
    string officeCode;
    int	reportsTo = 0;
    string jobTitle;
    int i = 0;
    int flag = 0;
    Statement* s = conn->createStatement();

    try {
        s->setSQL("SELECT * FROM employees where employeenumber = :employeeN");
        s->setInt(1, employeeNumber);
        ResultSet* r = s->executeQuery();
        while (r->next()) {
            empNum = r->getInt(1);
            lastName = r->getString(2);
            firstName = r->getString(3);
            extension = r->getString(4);
            email = r->getString(5);
            officeCode = r->getString(6);
            reportsTo = r->getInt(7); //stores the employee ID of the manager
            jobTitle = r->getString(8);
            i++;
        }
        if (i > 0) {
            flag = 1;
            emp->employeeNumber = empNum;
            strcpy(emp->firstName, firstName.c_str());
            strcpy(emp->lastName, lastName.c_str());
            strcpy(emp->extension, extension.c_str());
            strcpy(emp->email, email.c_str());
            strcpy(emp->officecode, officeCode.c_str());
            emp->reportsTo = reportsTo;
            strcpy(emp->jobTitle, jobTitle.c_str());

        }
        else flag = 0;
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    return flag;
    conn->terminateStatement(s);
}

//diplay employee
void displayEmployee(Connection* conn, Employee emp) {
    cout << endl;
    cout << "-------------- Employee Information -------------" << endl;
    cout << "Employee Number: " << emp.employeeNumber << endl;
    cout << "Last Name: " << emp.lastName << endl;
    cout << "First Name: " << emp.firstName << endl;
    cout << "Extension: " << emp.extension << endl;
    cout << "Email: " << emp.email << endl;
    cout << "Office Code: " << emp.officecode << endl;
    cout << "Manager ID: " << emp.reportsTo << endl;
    cout << "Job Title: " << emp.jobTitle << endl;
}

//displays all employees' information if exist
void displayAllEmployees(Connection* conn) {

    Statement* s = conn->createStatement();

    try {
        s->setSQL("SELECT e1.employeeNumber,e1.lastname, e1.firstname, e1.email, o.phone, e1.extension, e2.lastname, e2.firstname FROM offices o, employees e1 LEFT JOIN employees e2 on e1.reportsTo = e2.employeeNumber WHERE e1.officeCode = o.officeCode ORDER BY e1.employeeNumber");
        ResultSet* r = s->executeQuery();
        cout << "------   ---------------    ----------------------------------  -------------------  ---------   -------------------" << endl;
        cout << "ID       Employee Name      Email                               Phone                Extension   Manager Name       " << endl;
        cout << "-----    ---------------    ----------------------------------  -------------------  ---------   -------------------" << endl;
        while (r->next()) {
            int empNum = r->getInt(1);
            string lastname = r->getString(2);
            string firstname = r->getString(3);
            string email = r->getString(4);
            string phone = r->getString(5);
            string extension = r->getString(6);
            string managerLname = r->getString(7);
            string managerFname = r->getString(8);
            string empname = firstname + ' ' + lastname;
            string managername = managerFname + ' ' + managerLname;
            cout << left << setw(9) << empNum << setw(19) << empname << setw(36) << email << setw(21) << phone << setw(12) << extension << setw(17) << managername << endl;
        }
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    conn->terminateStatement(s);
}

//gets the info about the employee from the user and stores
void insertEmployee(struct Employee* emp) {
    strcpy(emp->officecode, "1");
    emp->reportsTo = 1002;
    cout << endl;
    cout << "-------------- New Employee Information -------------" << endl;
    cout << "Employee Number: ";
    cin >> emp->employeeNumber;
    cout << "Last Name: ";
    cin >> emp->lastName;
    cout << "First Name: ";
    cin >> emp->firstName;
    cout << "Extension: ";
    cin >> emp->extension;
    cout << "Email: ";
    cin >> emp->email;
    cout << "Office Code: " << emp->officecode << endl;
    cout << "Manager ID: " << emp->reportsTo << endl;
    cout << "Job Title: ";
    cin >> emp->jobTitle;
}

//inserts to the employees table
void insertEmployee(Connection* conn, struct Employee emp) {

    Statement* s = conn->createStatement();

    try {
        s->setSQL("INSERT INTO employees VALUES (:1,:2,:3,:4,:5,:6,:7,:8)");
        s->setInt(1, emp.employeeNumber);
        s->setString(2, emp.lastName);
        s->setString(3, emp.firstName);
        s->setString(4, emp.extension);
        s->setString(5, emp.email);
        s->setString(6, emp.officecode);
        s->setInt(7, emp.reportsTo);
        s->setString(8, emp.jobTitle);
        s->executeUpdate();
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    conn->commit();
    conn->terminateStatement(s);
}


void updateEmployee(Connection* conn, int employeeNumber, Employee emp) {
    Statement* s = conn->createStatement("UPDATE EMPLOYEES SET EXTENSION = :1 WHERE EMPLOYEENUMBER = : 2");
    string extension;
    cout << "Last Name: " << emp.lastName << endl;
    cout << "First Name: " << emp.firstName << endl;
    cout << "Extension: ";
    cin >> extension;

    try {
        s->setString(1, extension);
        s->setInt(2, employeeNumber);
        s->executeUpdate();
        conn->commit();
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    conn->terminateStatement(s);
}

void deleteEmployee(Connection* conn, int employeeNumber) {
    Statement* s = conn->createStatement();
    try {
        s->setSQL("DELETE FROM employees WHERE employeeNumber = :1");
        s->setInt(1, employeeNumber);
        s->executeUpdate();
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    conn->commit();
    conn->terminateStatement(s);

}

int menu();
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp);
void displayEmployee(Connection* conn, Employee emp);
void displayAllEmployees(Connection* conn);
void insertEmployee(struct  Employee* emp);
void insertEmployee(Connection* conn, struct Employee emp);
void updateEmployee(Connection* conn, int employeeNumber, Employee emp);
void deleteEmployee(Connection* conn, int employeeNumber);

int main(void) {

    //declaring the environment and connection variables
    Environment* env = nullptr;
    Connection* conn = nullptr;
    //defining and initializing the variables
    string user = "dbs211_211zb05";
    string pass = "31548157";
    string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";

    //creating the environment and the connection
    env = Environment::createEnvironment(Environment::DEFAULT);
    conn = env->createConnection(user, pass, constr);

    struct Employee emp = { 0 };
    int empNum;
    int flag = 0;

    while (flag == 0) {
        int selection = menu();
        //when the user chooses 1
        if (selection == 1) {
            cout << "Enter Employee Number: ";
            cin >> empNum;
            int check = findEmployee(conn, empNum, &emp);
            if (check == 1) {
                displayEmployee(conn, emp);
                cout << endl;
            }
            else {
                cout << "Employee " << empNum << " does not exist." << endl;
                cout << endl;
            }
        }
        else if (selection == 2) {
            displayAllEmployees(conn);
            cout << endl;
        }
        else if (selection == 3) {
            insertEmployee(&emp);
            int check2 = findEmployee(conn, emp.employeeNumber, &emp);
            if (check2 == 0) {
                insertEmployee(conn, emp);
                cout << endl;
                cout << "The new employee is added successfully." << endl;
                cout << endl;
            }
            else {
                cout << endl;
                cout << "An employee with the same employee number exists." << endl;
                cout << endl;
            }
        }
        else if (selection == 4) {
            cout << "Employee Number: ";
            cin >> empNum;
            int check3 = findEmployee(conn, empNum, &emp);
            if (check3 == 1) {
                updateEmployee(conn, empNum, emp);
                cout << "The employee's extension is updated successfully." << endl;
                cout << endl;
            }
            else {
                cout << "The employee with ID " << empNum << " does not exist." << endl;
                cout << endl;
            }
        }
        else if (selection == 5) {
            cout << "Employee Number: ";
            cin >> empNum;
            int check4 = findEmployee(conn, empNum, &emp);
            if (check4 == 1) {
                deleteEmployee(conn, empNum);
                cout << "The employee with ID " << empNum << " is deleted successfully." << endl;
                cout << endl;
            }
            else {
                cout << "The employee with ID " << empNum << " does not exist." << endl;
                cout << endl;
            }
        }
        else if (selection == 0) {
            flag = 1;
        }
    }
    //terminating and closing the connection to the environment
    env->terminateConnection(conn);
    Environment::terminateEnvironment(env);
    return 0;
}