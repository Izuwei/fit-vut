/**
 * FIT VUT
 * 
 * Author: Jakub Sadílek
 * Date: 7.3.2021
 */

/**
 * Abstract class Invoker.
 * 
 * @class Invoker
 */
class Invoker {

    constructor() {
        if (this.constructor === Invoker) {
            throw new Error ("Abstract class cannot be instantiated.");
        }
    }

    execute() {     // Action
        throw new Error ("Method 'Execute()' must be implemented.");
    }
}

/**
 * Class calculator which invokes Commands to operate.
 * 
 * @class Calculator
 */
export class Calculator extends Invoker {

    constructor() {
        super();

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

/**
 * Command implementing addition operation.
 * 
 * @class AddCommand
 */
export class AddCommand extends Command {

    execute(currentValue) {
        return currentValue + this.value;
    }

    undo(currentValue) {
        return currentValue - this.value;
    }
}

/**
 * Command implementing subtraction operation.
 * 
 * @class SubCommand
 */
export class SubCommand extends Command {

    execute(currentValue) {
        return currentValue - this.value;
    }

    undo(currentValue) {
        return currentValue + this.value;
    }
}

/**
 * Command implementing multiplication operation.
 * 
 * @class MulCommand
 */
export class MulCommand extends Command {

    execute(currentValue) {
        return currentValue * this.value;
    }

    undo(currentValue) {
        return currentValue / this.value;
    }
}

/**
 * Command implementing division operation.
 * 
 * @class DivCommand
 */
export class DivCommand extends Command {

    execute(currentValue) {
        return currentValue / this.value;
    }

    undo(currentValue) {
        return currentValue * this.value;
    }
}

/**
 * Command implementing negation operation.
 * 
 * @class NegCommand
 */
export class NegCommand extends Command {

    execute(currentValue) {
        return currentValue * (-1);
    }

    undo(currentValue) {
        return currentValue * (-1);
    }
}

/**
 * Command implementing set operation.
 * 
 * @class SetCommand
 */
export class SetCommand extends Command {

    execute(currentValue) {
        this.oldValue = currentValue;
        return this.value;
    }

    undo() {
        return this.oldValue;
    }
}

/**
 * Command implementing reset operation.
 * 
 * @class ResetCommand
 */
export class ResetCommand extends Command {

    execute(currentValue) {
        this.oldValue = currentValue;
        return 0;
    }

    undo() {
        return this.oldValue;
    }
}
