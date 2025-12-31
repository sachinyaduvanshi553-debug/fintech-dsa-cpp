const fs = require("fs");
const mongoose = require("mongoose");

mongoose.connect("mongodb://127.0.0.1:27017/smartbank");

const Account = mongoose.model("Account", {
  account: Number,
  balance: Number
});

const Transaction = mongoose.model("Transaction", {
  account: Number,
  amount: Number,
  type: String
});

// Save Accounts
if (fs.existsSync("accounts.json")) {
  const accData = fs.readFileSync("accounts.json", "utf8").trim().split("\n");
  accData.forEach(line => {
    const obj = JSON.parse(line);
    Account.create(obj);
  });
}

// Save Transactions
if (fs.existsSync("transactions.json")) {
  const txnData = fs.readFileSync("transactions.json", "utf8").trim().split("\n");
  txnData.forEach(line => {
    const obj = JSON.parse(line);
    Transaction.create(obj);
  });
}

console.log("Data inserted into MongoDB");
