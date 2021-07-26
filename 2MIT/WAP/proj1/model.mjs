/**
 * FIT VUT
 * 
 * Author: Jakub Sadílek
 * Date: 7.3.2021
 */

import { Calculator, AddCommand, SubCommand, MulCommand, DivCommand, NegCommand, SetCommand, ResetCommand } from './library.mjs';
import * as readline from 'readline';

const calculator = new Calculator();        // Global instance of calculator

/**
 * Function prints help istructions.
 */
const printHelp = () => {
    console.log("List of available operations (all <values> must be numeric):");
    console.log("+ help");
    console.log("+ quit");
    console.log("+ undo");
    console.log("+ redo");
    console.log("+ neg");
    console.log("+ reset");
    console.log("+ set <value>");
    console.log("+ add <value>");
    console.log("+ sub <value>");
    console.log("+ mul <value>");
    console.log("+ div <value>");
    console.log("+ sim <value>\n");
}

/**
 * Function prints error message on bad input from user.
 */
const errorMsg = () => {
    console.log("Unknown operation or bad parameter! For help type 'help'.");
}

/**
 * Function choose a random operation for simulation.
 * 
 * @returns Operation name as a string.
 */
const randomOperation = () => {
    const OPERATIONS = ["undo", "redo", "neg", "add", "sub", "mul", "div", "set", "reset"];

    return OPERATIONS[Math.floor(Math.random() * OPERATIONS.length)];
}

/**
 * Function generate a random value as positive integer from 1 up to 99.
 * 
 * @returns Random value as integer.
 */
const randomNumber = () => {
    return Math.floor(Math.random() * Math.floor(99)) + 1;
}

/**
 * Function simulate input from user. It's used for demonstration.
 * 
 * @param iterations Number of commands to process.
 */
const doSimulation = (iterations) => {
    console.log("----------------------");
    console.log("Begin of simulation...");
    console.log("----------------------");

    calculator.execute(new ResetCommand());     // Reset the calculator
    console.log("Initial value: " + calculator.getValue());
    
    for (let i = 0; i < iterations; i++) {
        const operation = randomOperation();    // Get random operation
        const value = randomNumber();           // Get random value

        processOperation(operation, value);
    }

    console.log("----------------------");

    calculator.execute(new ResetCommand());     // Reset the calculator
    console.log("\nInitial value: " + calculator.getValue() + "\n");
}

/**
 * Function synchronously reads user input from terminal.
 * 
 * @returns Line from terminal as string.
 */
const readOperation = async () => {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    return new Promise(resolve => rl.question('Type operation: ', (line) => { rl.close(); resolve(line); }));
}

/**
 * Function obtains user's input and decode operation from it. Then returns operation as list.
 * 
 * @param line Line from terminal as string.
 * @returns Operation as list -> ["operation", "value"], if error is detected returns 1.
 */
const encodeOperation = (line) => {
    const OPERATIONS_WITH_VAL = ["add", "sub", "mul", "div", "set", "sim"];

    let operation = line.trim();
    operation = operation.toLowerCase();
    operation = operation.replace(/\s+/gi, ' ');
    operation = operation.split(/\s/);

    if (OPERATIONS_WITH_VAL.includes(operation[0])) {       // Operations with value
        let regex = new RegExp('^-?\\d+\\.?\\d*$');         // Regex for numbers

        if (regex.test(operation[1]) && operation.length === 2) {
            operation[1] = parseFloat(operation[1]);

            if (operation[0] === "div" && operation[1] === 0) {     // Division by zero error
                return 1;
            }

            return operation;
        }
        else {
            return 1;
        }
    }
    if (operation.length > 1) {
        return 1;
    }

    operation.push("");
    return operation;
}

/**
 * Functions obtains operation and value, then invokes corresponding  command.
 * 
 * @param operation Name of operation as string.
 * @param value Value for operation.
 */
const processOperation = (operation, value) => {
    switch (operation) { 
        case "undo":
            calculator.undo();
            console.log("Undo\t\t Result: " + calculator.getValue());
            break;
        case "redo":
            calculator.redo();
            console.log("Redo\t\t Result: " + calculator.getValue());
            break;
        case "add":
            calculator.execute(new AddCommand(value));
            console.log("Add(" + value + ")\t\t Result: " + calculator.getValue());
            break;
        case "sub":
            calculator.execute(new SubCommand(value));
            console.log("Sub(" + value + ")\t\t Result: " + calculator.getValue());
            break;
        case "mul":
            calculator.execute(new MulCommand(value));
            console.log("Mul(" + value + ")\t\t Result: " + calculator.getValue());
            break;
        case "div":
            calculator.execute(new DivCommand(value));
            console.log("Div(" + value + ")\t\t Result: " + calculator.getValue());
            break;
        case "neg":
            calculator.execute(new NegCommand());
            console.log("Neg\t\t Result: " + calculator.getValue());
            break;
        case "set":
            calculator.execute(new SetCommand(value));
            console.log("Set(" + value + ")\t\t Result: " + calculator.getValue());
            break;
        case "reset":
            calculator.execute(new ResetCommand());
            console.log("Reset\t\t Result: " + calculator.getValue());
            break;
        case "sim":
            doSimulation(value);
            break;
        case "help":
            printHelp();
            break;
        default:
            errorMsg();
            break;
    }
}

const main = async () => {
    printHelp();

    console.log("Initial value: " + calculator.getValue() + "\n");
    while (1) {             // Infinite loop until user terminates the calculator.
        let line = await readOperation();
        line = encodeOperation(line);

        if (line === 1) {
            errorMsg();
            continue;
        }
        if (line[0] === "quit") {       // Termination condition
            console.log("Good bye.")
            break;
        }

        processOperation(line[0], line[1]);
    }
}

main();
