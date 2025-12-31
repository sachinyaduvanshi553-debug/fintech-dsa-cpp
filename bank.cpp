#include <iostream>
#include <fstream>   // Added for file handling
using namespace std;

/* ---------------- CONSTANTS ---------------- */
#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100

/* ---------------- STRUCTURES ---------------- */

// Linked List Node for Transaction History
struct Transaction {
    int id;
    float amount;
    char type;               // 'D' = Deposit, 'W' = Withdraw
    Transaction* next;
};

// Account Structure
struct Account {
    int accountNumber;
    float balance;
    Transaction* history;    // Head of linked list
};

// Stack Node (Undo Feature)
struct StackNode {
    int accNo;
    float amount;
    char type;
};

// Queue Node (Transaction Processing)
struct QueueNode {
    int accNo;
    float amount;
    char type;
};

/* ---------------- GLOBAL VARIABLES ---------------- */

Account accounts[MAX_ACCOUNTS];
int totalAccounts = 0;

// Stack
StackNode undoStack[MAX_TRANSACTIONS];
int top = -1;

// Queue
QueueNode transactionQueue[MAX_TRANSACTIONS];
int front = 0, rear = -1;

// Transaction ID Counter
int transactionID = 1;

/* ---------------- FILE EXPORT FUNCTIONS ---------------- */

// Save account to JSON file for MongoDB import
void saveAccountToFile(int accNo, float balance) {
    ofstream fout("accounts.json", ios::app);
    fout << "{ \"account\": " << accNo
         << ", \"balance\": " << balance << " }\n";
    fout.close();
}

// Save transaction to JSON file for MongoDB import
void saveTransactionToFile(int accNo, float amt, char type) {
    ofstream fout("transactions.json", ios::app);
    fout << "{ \"account\": " << accNo
         << ", \"amount\": " << amt
         << ", \"type\": \"" << type << "\" }\n";
    fout.close();
}

/* ---------------- UTILITY FUNCTIONS ---------------- */

// Search account using Linear Search
int findAccount(int accNo) {
    for (int i = 0; i < totalAccounts; i++) {
        if (accounts[i].accountNumber == accNo)
            return i;
    }
    return -1;
}

/* ---------------- ACCOUNT MANAGEMENT ---------------- */

void createAccount() {
    if (totalAccounts >= MAX_ACCOUNTS) {
        cout << "Account limit reached!\n";
        return;
    }

    cout << "Enter Account Number: ";
    cin >> accounts[totalAccounts].accountNumber;

    accounts[totalAccounts].balance = 0.0;
    accounts[totalAccounts].history = NULL;

    totalAccounts++;
    cout << "Account created successfully.\n";

    // Export to JSON
    saveAccountToFile(accounts[totalAccounts-1].accountNumber,
                      accounts[totalAccounts-1].balance);
}

/* ---------------- STACK FUNCTIONS ---------------- */

void pushUndo(int accNo, float amount, char type) {
    top++;
    undoStack[top].accNo = accNo;
    undoStack[top].amount = amount;
    undoStack[top].type = type;
}

/* ---------------- QUEUE FUNCTIONS ---------------- */

void enqueueTransaction(int accNo, float amount, char type) {
    rear++;
    transactionQueue[rear].accNo = accNo;
    transactionQueue[rear].amount = amount;
    transactionQueue[rear].type = type;
}

/* ---------------- LINKED LIST ---------------- */

void addTransactionHistory(Account &acc, float amount, char type) {
    Transaction* newTransaction = new Transaction;
    newTransaction->id = transactionID++;
    newTransaction->amount = amount;
    newTransaction->type = type;
    newTransaction->next = acc.history;
    acc.history = newTransaction;
}

/* ---------------- PROCESS TRANSACTION ---------------- */

void processTransaction() {
    if (front > rear) {
        cout << "No pending transactions.\n";
        return;
    }

    QueueNode t = transactionQueue[front++];
    int index = findAccount(t.accNo);

    if (index == -1) {
        cout << "Invalid Account.\n";
        return;
    }

    if (t.type == 'D') {
        accounts[index].balance += t.amount;
    } else {
        if (accounts[index].balance < t.amount) {
            cout << "Insufficient balance!\n";
            return;
        }
        accounts[index].balance -= t.amount;
    }

    pushUndo(t.accNo, t.amount, t.type);
    addTransactionHistory(accounts[index], t.amount, t.type);

    cout << "Transaction successful.\n";

    // Export transaction to JSON
    saveTransactionToFile(t.accNo, t.amount, t.type);
}

/* ---------------- BANK OPERATIONS ---------------- */

void depositMoney() {
    int acc;
    float amt;

    cout << "Enter Account Number: ";
    cin >> acc;
    cout << "Enter Amount: ";
    cin >> amt;

    enqueueTransaction(acc, amt, 'D');
    processTransaction();
}

void withdrawMoney() {
    int acc;
    float amt;

    cout << "Enter Account Number: ";
    cin >> acc;
    cout << "Enter Amount: ";
    cin >> amt;

    enqueueTransaction(acc, amt, 'W');
    processTransaction();
}

/* ---------------- UNDO FEATURE ---------------- */

void undoLastTransaction() {
    if (top < 0) {
        cout << "No transaction to undo.\n";
        return;
    }

    StackNode last = undoStack[top--];
    int index = findAccount(last.accNo);

    if (last.type == 'D')
        accounts[index].balance -= last.amount;
    else
        accounts[index].balance += last.amount;

    cout << "Last transaction undone successfully.\n";
}

/* ---------------- DISPLAY HISTORY ---------------- */

void showTransactionHistory() {
    int acc;
    cout << "Enter Account Number: ";
    cin >> acc;

    int index = findAccount(acc);
    if (index == -1) {
        cout << "Account not found.\n";
        return;
    }

    Transaction* temp = accounts[index].history;

    cout << "\nTransaction History:\n";
    while (temp != NULL) {
        cout << "ID: " << temp->id
             << " | Amount: " << temp->amount
             << " | Type: " << (temp->type == 'D' ? "Deposit" : "Withdraw")
             << endl;
        temp = temp->next;
    }
}

/* ---------------- FRAUD DETECTION ---------------- */

void detectFraud() {
    int acc;
    cout << "Enter Account Number: ";
    cin >> acc;

    int index = findAccount(acc);
    if (index == -1) {
        cout << "Account not found.\n";
        return;
    }

    Transaction* temp = accounts[index].history;
    int withdrawalCount = 0;

    while (temp != NULL && withdrawalCount <= 3) {
        if (temp->type == 'W')
            withdrawalCount++;
        temp = temp->next;
    }

    if (withdrawalCount > 3)
        cout << "⚠ Fraud Alert: Multiple withdrawals detected!\n";
    else
        cout << "No fraud detected.\n";
}

/* ---------------- MAIN MENU ---------------- */

int main() {
    int choice;

    do {
        cout << "\n========== SMART BANK SYSTEM ==========\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Undo Last Transaction\n";
        cout << "5. View Transaction History\n";
        cout << "6. Fraud Detection\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: createAccount(); break;
            case 2: depositMoney(); break;
            case 3: withdrawMoney(); break;
            case 4: undoLastTransaction(); break;
            case 5: showTransactionHistory(); break;
            case 6: detectFraud(); break;
            case 0: cout << "Thank you for using Smart Bank System.\n"; break;
            default: cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
