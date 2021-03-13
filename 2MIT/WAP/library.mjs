/**
 * FIT VUT
 * 
 * Author: Jakub Sadílek
 * Date: 7.3.2021
 */

/**
 * Class calculator which receiving Commands to operate.
 * 
 * @class Calculator
 */
export class Calculator {

    constructor() {
        this.value = 0;         // result
        this.stash = [];        // returned commands by operation undo (redo)
        this.history = [];      // commands which had effect on result (undo)
    }

    execute(command) {
        this.history.push(command);
        this.value = command.execute(this.value);
    }

    undo() {
        if (this.history.length !== 0) {
            const command = this.history.pop();
            this.stash.push(command);
            this.value = command.undo(this.value);
        }
    }

    redo() {
        if (this.stash.length !== 0) {
            const command = this.stash.pop();
            this.history.push(command);
            this.value = command.execute(this.value);
        }
    }

    getValue() {
        return this.value;
    }
}

/**
 * Abstract class Command.
 * 
 * @class Command
 */
class Command {

    constructor(value) {
        if (this.constructor === Command) {
            throw new Error ("Abstract class cannot be instantiated.");
        }
        
        this.value = value;
    }

    execute() {
        throw new Error ("Method 'Execute()' must be implemented.");
    }
}


export class AddCommand extends Command {

    execute(currentValue) {
        return currentValue + this.value;
    }

    undo(currentValue) {
        return currentValue - this.value;
    }
}

export class SubCommand extends Command {

    execute(currentValue) {
        return currentValue - this.value;
    }

    undo(currentValue) {
        return currentValue + this.value;
    }
}

export class MulCommand extends Command {

    execute(currentValue) {
        return currentValue * this.value;
    }

    undo(currentValue) {
        return currentValue / this.value;
    }
}

export class DivCommand extends Command {

    execute(currentValue) {
        return currentValue / this.value;
    }

    undo(currentValue) {
        return currentValue * this.value;
    }
}

export class NegCommand extends Command {

    execute(currentValue) {
        return currentValue * (-1);
    }

    undo(currentValue) {
        return currentValue * (-1);
    }
}

export class SetCommand extends Command {

    execute(currentValue) {
        this.oldValue = currentValue;
        return this.value;
    }

    undo() {
        return this.oldValue;
    }
}

export class ResetCommand extends Command {

    execute(currentValue) {
        this.oldValue = currentValue;
        return 0;
    }

    undo() {
        return this.oldValue;
    }
}
