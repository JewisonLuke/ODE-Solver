#include "Interpret.h"

int strSize(char *input, int newLine) {
    int inputIdxNum = INPUT_SIZE;
    for (int i = 0; i < INPUT_SIZE; i++) {
        if ((input[i] == '\n' && newLine) || (input[i] == '\0' && !newLine)) {
            inputIdxNum = i;
            break;
        }
    } //finds which value is the last in the array, then I don't have to search through all indexes that aren't filled

    return inputIdxNum;
}

int findIdx(char *input, int inputIdxNum, char op) {
    for (int i = 0; i < inputIdxNum; i++) {
        if (input[i] == op) {
            return i;
        }
    }
    return INPUT_SIZE;
}

int checkValidity(char *input, int inputIdxNum) {
    char validChars[numValids] = "1234567890xy- +/*^"; // last char is the \0 char
    int valid = 1;
    int numEqual = 0;
    int foundNum = 0;

    char validStart[] = "0123456789-xy(";
    int validStarts = 14;

    int parenCount = 0;
    char prevChar = '('; // because each side of the equation can start with (

    for (int i = 0; i < inputIdxNum; i++) {
        if (input[i] == ' ') {
            continue;
        }
        if (prevChar == '(' || prevChar == '=') {
            for (int j = 0; j < validStarts; j++) {
                if (input[i] == validStart[j]) {
                    break;
                }
                if (j == validStarts - 1) {
                    //printf("invalid %c\n", input[i]);
                    valid = 0;
                    break;
                }
            }
            if (!valid) {break;}
        }
        int validChar = 0;
        if (input[i] == '=') { // = checked separately
            numEqual++;
            if (parenCount != 0) {
                valid = 0;
                break; // can't have (3x = 5)
            }
            parenCount = 0;
            foundNum = 0;
        }
        else if (input[i] == '(') {
            parenCount++;
        }
        else if (input[i] == ')') {
            parenCount--;
            if (parenCount < 0) {
                valid = 0;
                break;
            }
        }
        else if (input[i] == '\'') { // checks '
            if (i == 0) {
                valid = 0;
                break;
            }
            if (input[i - 1] != '\'' && input[i - 1] != 'y') {
                valid = 0;
                break;
            }
        } else {
            for (int j = 0; j < numValids - 1; j++) {
                if (input[i] == validChars[j]) {
                    validChar = 1;
                    if (!foundNum && j > 13) {
                        valid = 0; // operator can't be first
                    } else if (j < 13) {
                        foundNum = 1;
                    }
                    break;
                }
            }
            if (!validChar) {
                valid = 0;
                break;
            }
        }
        prevChar = input[i];
    }

    if (prevChar == '=') {
        valid = 0;
    }

    if (numEqual != 1) {
        valid = 0;
    }
    if (parenCount != 0) {
        valid = 0;
    }

    return valid;
}

int findChunks(char *input, int inputIdxNum, int *chunks) {
    int numChunks = 0;
    int parenNum = 0;

    for (int i = 0; i < inputIdxNum; i++) {
        if (((input[i] == '+' || input[i] == '-' || input[i] == '=') && i != 0) && parenNum == 0) {
            chunks[numChunks] = i - 1;
            numChunks += 1;
        }
        if (input[i] == '(') {
            parenNum++;
        } else if (input[i] == ')') {
            parenNum--;
        }
    }
    chunks[numChunks] = inputIdxNum - 1; // mark the end of the last chunk

    return numChunks;
}

int equationType(char *input, int inputIdxNum) {
    int algebraic = 1; // assume no y until find one
    int derivative = 0;
    for (int i = 0; i < inputIdxNum; i++) {
        if (input[i] == 'y') {
            algebraic = 0;
            continue;
        }
        else if (input[i] == '\'') {
            derivative = 1;
            break; // already ensured there was a y before ', so if there is a ' it is ode
        }
    }
    if (derivative == 1) {
        return 3; // 3 output means ode
    }
    if (algebraic == 0) {
        return 2; // 2 means y(x) and graph
    }
    if (algebraic == 1) {
        return 1; // 1 means algebraic equation x = 3x + 2
    }
    return 0; // can't get here
}

void addChunk(char *input, char *oneSideInput, int start, int end, int idx) {
    while (start <= end) {
        oneSideInput[idx] = input[start];
        idx++;
        start++;
    }
    oneSideInput[idx] = '\0'; // so it is printable
    return;
}

int findHighChunk(char *input, int inputIdxNum, int *chunks, int numChunks, int eqType) {
    int chunkNum = -1; // initialize it to non-existent
    int idx = 0;
    int maxCount = 0;

    for (int i = 0; i <= numChunks; i++) {
        int orderCount = 0;
        if (i > 0) {
            idx = chunks[i-1] + 1;
        }
        for (int j = idx; j <= chunks[i]; j++) {
            if (input[j] == 'y' || input[j] == '\'') {
                orderCount++;
            }
        }
        if (orderCount > maxCount) {
            maxCount = orderCount;
            chunkNum = i;
        }
    }

    //printf("chunk: %d\n", chunkNum);
    return chunkNum;
}

int swapChunk(char *input, int inputIdxNum, int *chunks, int highestChunk) {
    char temp[INPUT_SIZE];
    int startIdx = 0;
    int endIdx = chunks[highestChunk];
    if (highestChunk > 0) {
        startIdx = chunks[highestChunk - 1] + 1;
    }
    int chunkLength = endIdx - startIdx + 1;

    //printf("%d %d %d\n", startIdx, endIdx, chunkLength);

    for (int i = startIdx; i <= endIdx; i++) {
        temp[i - startIdx] = input[i];
    } // copy to temp array
    for (int i = endIdx + 1; i < inputIdxNum; i++) {
        input[i - chunkLength] = input[i];
    }
    inputIdxNum -= chunkLength;
    input[inputIdxNum] = '=';  
    inputIdxNum++;
    input[inputIdxNum] = ' ';  
    inputIdxNum++;

    if (temp[0] == '-') {
        addChunk(temp, input, 1, chunkLength - 1, inputIdxNum);
        inputIdxNum += chunkLength - 1;
    }
    else if (temp[0] == '+') {
        temp[0] = '-';
        addChunk(temp, input, 0, chunkLength - 1, inputIdxNum);
        inputIdxNum += chunkLength;
    }
    input[inputIdxNum] = '\0';


    return inputIdxNum;
}

int eqOneSide(char *input, char *oneSideInput, int inputIdxNum, int eqType) {
    int parenCount = 0;
    int noParen = 1;
    int findOp = 0;
    int endParenIdx;
    for (int i = 0; i < inputIdxNum; i++) {
        if (input[i] == ' ') {
            continue;
        }
        else if (input[i] == '+' || input[i] == '-'||input[i] == '='||input[i] == ')') {
            noParen = 1;
            continue;
        }
        else if (input[i] == '(') {
            if (noParen) {
                parenCount = 0;
                findOp = 0;
                for (int j = i + 1; j < inputIdxNum; j++) {
                    if (input[j] == ' ') {
                        continue;
                    }
                    if (input[j] == '(' && !findOp) {
                        parenCount++;
                    }
                    else if (input[j] == ')') {
                        if (parenCount == 0) {
                            findOp = 1;
                            endParenIdx = j;
                        } else {
                            parenCount--;
                        }
                    } else if (findOp && (input[j] == '+' || input[j] == '-' || input[j] == '=' || input[j] == ')')) {
                        noParen = 1;
                        break;
                    } else if (findOp) {
                        noParen = 0;
                        break;
                    }
                }
                if (noParen) {
                    input[i] = ' ';
                    input[endParenIdx] = ' ';
                    for (int k = i; k < endParenIdx; k++) {
                        if (input[k] == '+') {
                            input[k] = '-';
                        } else if (input[k] == '-') {
                            input[k] = '+';
                        }
                    }
                }
                noParen = 1;
            }
        }
        else {
            noParen = 0;
        }
    }
    
    int equalIdx = findIdx(input, inputIdxNum,'=');
    int chunks[INPUT_SIZE/2];
    int numChunks = findChunks(input, inputIdxNum, chunks);
    int idx = 0;

    addChunk(input, oneSideInput, 0, chunks[0], idx);
    idx += chunks[0] + 1;
    addChunk(input, oneSideInput, chunks[1] + 1, chunks[2], idx);
    for (int i = 1; i <= numChunks; i++) {
        if (chunks[i] < equalIdx) {
            addChunk(input, oneSideInput, chunks[i-1] + 1, chunks[i], idx);
            idx += chunks[i] - chunks[i-1];
        } else {
            if (input[chunks[i-1] + 1] == '-') {
                oneSideInput[idx] = '+';
                idx++;
            } else if (input[chunks[i-1] + 1] == '+') {
                oneSideInput[idx] = '-';
                idx++;
            } else if (input[chunks[i-1] + 1] == '=') {
                if (input[chunks[i] + 1] == '-') {
                    int foundChar = 0;
                    for (int j = chunks[i-1]+2; j <= chunks[i]; j++) {
                        if (input[j] != ' ') {
                            foundChar = 1;
                            break;
                        }
                    }
                    if (!foundChar) { continue; }
                    oneSideInput[idx] = '-';
                    idx++;
                } else {
                    oneSideInput[idx] = '-';
                    idx++;
                }
            }
            addChunk(input, oneSideInput, chunks[i-1] + 2, chunks[i], idx);
            idx += chunks[i] - chunks[i-1] - 1;
        }
    }

    int oneSideIdxNum = strSize(oneSideInput, 0);
    int oneSideChunks[INPUT_SIZE/2];
    int oneSideNumChunks = findChunks(oneSideInput, oneSideIdxNum, oneSideChunks);
    int highestChunk = findHighChunk(oneSideInput, oneSideIdxNum, oneSideChunks, oneSideNumChunks, eqType); // find the y''' or y or highest order

    //printf("%s\n", oneSideInput);
    if (eqType != 1) {
        oneSideIdxNum = swapChunk(oneSideInput, oneSideIdxNum, oneSideChunks, highestChunk);
    } else {
        oneSideInput[oneSideIdxNum] = '=';
        oneSideIdxNum++;
        oneSideInput[oneSideIdxNum] = '\0';
    }

    char validChars[] = "0123456789xy')";
    int validChar;
    int maxChars = 14; // remove an accidental space at the end
    do {
        for (int i = 0; i < maxChars; i++) {
            if (oneSideInput[oneSideIdxNum - 1] == validChars[i]) {
                validChar = 1;
                break;
            }
            if (i == maxChars-1) {
                validChar = 0;
                oneSideIdxNum--;
                oneSideInput[oneSideIdxNum] = '\0';
            }
        }
    } while (!validChar);
    return oneSideIdxNum;
}

int typeChar(char c) {
    char nums[] = "0123456789";
    char ops[] = "+-*/^";
    char vars[] = "xy'";
    int type = -1;
    for (int i = 0; i < 10; i++) {
        if (c == nums[i]) {
            type = 1;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (c == ops[i]) {
            type = 2;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (c == vars[i]) {
            type = 3;
        }
    }
    return type;
}

int push(char c, char *stack, int *top) {
    if (*top == INPUT_SIZE) {
        return 0;
    }
    stack[*top] = c;
    (*top)++;
    //printf("push %c\n", c);
    return 1;
}

char pop(char *stack, int *top) {
    if (*top == 0) {
        return 0;
    }
    (*top)--;
    char c = stack[*top];
    //printf("pop %c\n", c);
    return c;
}

char oppositeOp(char op) {
    char validOps[] = "+-*/^";
    char oppositeOps[] = "-+/*r";
    int numOps = 5;
    char oppositeOp = '%';
    for (int i = 0; i < numOps; i++) {
        if (op == validOps[i]) {
            oppositeOp = oppositeOps[i];
        }
    }

    return oppositeOp;
}

void generatePostFix(char *input, int idxNum, char *postFix) {
    int isFirst = 1;
    int idx = 0;
    int equalIdx = findIdx(input, idxNum, '=');
    static int charType;
    static int postFixIdx = 0; // stack memory low
    char opStack[INPUT_SIZE];
    static int opStackTop = 0;
    static int parenOps[numParen];
    parenOps[0] = 0;
    static int parenNum = 0;
    static int popNext[numParen];
    popNext[0] = 0;
    static int lastCharOp = 1;
    static int popNum;
    static int lastChar = 0;
    static int lastCharEqual = 0;
    int parenStartIdx[numParen];
    int parenEndIdx[numParen];
    int skipParen[numParen];
    static int parenCount = 0;
    static int yIdx = -1;
    static int derivativeCount = -1; // ran out of stack memory
    static int unPop = 0;

    while (idx < idxNum) {
        //printf("%c %d %d\n", input[idx], idx, idxNum);
        if (input[idx] == ' ') {
            idx++;
            continue;
        }

        if (idx == equalIdx) {
            while (parenOps[parenNum] > 0) {
                postFix[postFixIdx] = pop(opStack, &opStackTop);
                postFixIdx++;
                parenOps[parenNum]--;
            }
            isFirst = 1;
            lastCharOp = 1;
            lastCharEqual = 1;
            idx++;
            continue;
        }

        if (idx < equalIdx) {
            if (isFirst && input[idx] == '-') {
                postFix[postFixIdx] = '0';
                postFixIdx++;
                push('-', opStack, &opStackTop);
                parenOps[parenNum]++;
                idx++;
                isFirst = 0;
                continue;
            } else {
                isFirst = 0;
            }
            charType = typeChar(input[idx]);
            if (charType == 1) { //number just add it to the post fix, but add a * operator if it wasn't after an operator
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                idx++;
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum] = 1;
                } // account for x2                
                lastCharOp = 0;
                continue;
            } else if (charType == 2) { // operator if it is a +,-, pop all of the current operators, then push the +,-
                if (input[idx] != '+' && input[idx] != '-') {
                    push(input[idx], opStack, &opStackTop);
                    parenOps[parenNum]++;
                    //printf("%c\n", input[idx]);
                    popNext[parenNum] = 1;
                    //printf("paren Num: %d\n", parenNum);
                    idx++;
                } else {
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        postFixIdx++;
                        parenOps[parenNum]--;
                    }
                    popNext[parenNum] = 0;
                    push(input[idx], opStack, &opStackTop);
                    idx++;
                    parenOps[parenNum]++;
                }
                lastCharOp = 1;
                continue;
            } else if (charType == 3) { // variable add the variable and all derivatives, and if it wasnt right before an operator, add a *
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                idx++;
                while(input[idx] == '\'') {
                    postFix[postFixIdx] = input[idx];
                    postFixIdx++;
                    idx++;
                }
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum] = 1;
                } // account for 2x
                lastCharOp = 0;
                continue;
            } else if (input[idx] == '(') { // if it wasn't an operator before, add *, then enter a parenthesis
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum]++;
                } // account for 2(x)
                parenNum++;
                parenOps[parenNum] = 0; // enter a parenthesis
                popNext[parenNum] = 0;
                idx++;
                lastCharOp = 1;
                isFirst = 1;
                continue;
            } else if (input[idx] == ')') { // pop all operators inside the parenthesis and leave parenthesis
                while (parenOps[parenNum] > 0) {
                    postFix[postFixIdx] = pop(opStack, &opStackTop);
                    postFixIdx++;
                    parenOps[parenNum]--;
                }
                parenNum--;
               if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                popNext[parenNum] = 0;
                idx++;
                lastCharOp = 0;
                continue;
            }
        } else {
            if (idx == idxNum - 1 && !lastCharEqual) { // last char
                lastChar = 1;
            }
            lastCharEqual = 0;
            if (isFirst && input[idx] == '-') {
                postFix[postFixIdx] = '0';
                postFixIdx++;
                push('-', opStack, &opStackTop);
                parenOps[parenNum]++;
                idx++;
                isFirst = 0;
                continue;
            } else {
                isFirst = 0;
            }
            charType = typeChar(input[idx]);
            //printf("idx %d\n", postFixIdx);
            if (parenNum == 0) {
                //printf("%c %d %d %d %c\n", input[idx], idx, idxNum - 1, lastCharEqual, input[idxNum-1]);
            if (charType == 1) { //number just add it to the post fix, but add a * operator if it wasn't after an operator
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                idx++;
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('/', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum] = 1;
                } // account for x2                
                lastCharOp = 0;
                continue;
            } else if (charType == 2) { // operator if it is a +,-, pop all of the current operators, then push the +,-
                if (input[idx] != '+' && input[idx] != '-') {
                    push(oppositeOp(input[idx]), opStack, &opStackTop);
                    parenOps[parenNum]++;
                    //printf("%c\n", input[idx]);
                    popNext[parenNum] = 1;
                    //printf("paren Num: %d\n", parenNum);
                    idx++;
                } else {
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        postFixIdx++;
                        parenOps[parenNum]--;
                    }
                    popNext[parenNum] = 0;
                    push(oppositeOp(input[idx]), opStack, &opStackTop);
                    idx++;
                    parenOps[parenNum]++;
                }
                lastCharOp = 1;
                continue;
            } else if (charType == 3) { // variable add the variable and all derivatives, and if it wasnt right before an operator, add a *
                if (input[idx] == 'y') {
                    if (popNext[parenNum]) {
                        //printf("%c %d\n", input[idx], idx);
                        if (!lastCharOp) {
                            push('/', opStack, &opStackTop);
                            parenOps[parenNum]++;
                            popNext[parenNum] = 1;
                        } // account for x2     
                        while (parenOps[parenNum] > 0) {
                            postFix[postFixIdx] = pop(opStack, &opStackTop);
                            popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                            parenOps[parenNum]--;
                            postFixIdx++;
                        }
                    }
                    idx++;
                    while(input[idx] == '\'') {
                        idx++;
                    }
                    continue;
                }                
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                idx++;
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('/', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum] = 1;
                } // account for 2x
                lastCharOp = 0;
                continue;
            }
            } if (input[idx] == '(') { // if it wasn't an operator before, add *, then enter a parenthesis
                if (parenNum == 0) {
                    if (!lastCharOp) {
                        postFix[postFixIdx] = '/';
                        //printf("%d\n", idx);
                        postFixIdx++;
                    } // account for 2(x)
                }
                parenStartIdx[parenCount] = idx;
                parenEndIdx[parenCount] = -1;
                parenNum++;
                parenCount++;
                idx++;
                continue;
            } else if (input[idx] == ')') { // pop all operators inside the parenthesis and leave parenthesis
                parenNum--;
                for (int endParen = parenCount - 1; endParen >= 0; endParen--) {
                    if (parenEndIdx[endParen] == -1) {
                        parenEndIdx[endParen] = idx;
                        break;
                    }
                }
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                idx++;
                lastCharOp = 0;
                continue;
            }
            idx++;
        }
    }
    
    // do the right side parentheses

    for (int i = 0; i < parenCount; i++) {
        skipParen[i] = 0;
    } // initialize them all to 0
    for (int i = 0; i < parenCount; i++) {
        if (skipParen[i] == 1) {
            continue;
        }
        //printf("start: %d\n", parenStartIdx[i]);
        //printf("end: %d\n", parenEndIdx[i]);
        isFirst = 1;
        lastCharOp = 1;
        int isStartChunk = 1;
        int isYChunk = 0;
        int j = 0;
        int numberParen = 0;
        parenNum = 0;
        popNext[parenNum] = 0;
        //printf("Equation: %s\n", input);
        for (idx = parenStartIdx[i] + 1; idx < parenEndIdx[i]; idx++) {
            if (idx >= idxNum) {
                parenEndIdx[i] = idx - 1;
                printf("broken\n");
                break;
            }
            if (input[idx] == 'y') {
                derivativeCount = 0;
                while (input[++idx] == '\'') {
                    derivativeCount++;
                }
            }
        }
        for (idx = parenStartIdx[i] + 1; idx < parenEndIdx[i]; idx++) {// start after the ( and end before the )
            if (idx >= idxNum) {
                printf("broken still\n");
                break;
            }
            if (input[idx] == ' ') {
                continue;
            }
            //printf("%d\n", idx);
            if (input[idx] == '+' || input[idx] == '-') { // lines commented in here because im not sure why I added them
                isStartChunk = 1;
                isYChunk = 0;
                while (parenOps[parenNum] > 0) {
                    char popChar = pop(opStack, &opStackTop);
                    //printf("%c %d\n", popChar, unPop);
                    if (popChar == '+' || popChar == '-') {
                        popChar = oppositeOp(popChar);
                    }
                    //printf("%d %d %c\n", idx, unPop, popChar);
                    /*if (unPop && (popChar == '+' || popChar == '-')) { // remove the +/- before the parenthesis
                        unPop = 0;
                        parenOps[parenNum]--;
                        continue;
                    }*/
                    postFix[postFixIdx] = popChar;
                    postFixIdx++;
                    parenOps[parenNum]--;
                }
            }
            if (isStartChunk) {
                j = idx + 1;
                int numberParenthesis = 0;
                while ((numberParenthesis > 0 || (input[j] != '+' && input[j] != '-')) && j < idxNum) {
                    if (input[j] == ' ') {
                        j++;
                        continue;
                    }
                    if (input[j] == '(') {
                        numberParenthesis++;
                    } else if (input[j] == ')') {
                        numberParenthesis--;
                    }
                    if (input[j] == 'y') {
                        int primeCount = 0;
                        while(input[++j] == '\'') {
                            primeCount++;
                        }
                        if (primeCount == derivativeCount) {
                            isYChunk = 1;
                            yIdx = idx;
                            //printf("yidx: %d %c\n", yIdx, input[yIdx]);
                            break;
                        }
                    }
                    j++;
                }
                isStartChunk = 0;
            }
            if (isYChunk) {
                //printf("working %d\n", idx);
                if (input[idx] == '(') {

                    if (!lastCharOp) {
                        //printf("%d %c\n", idx, input[idx]);
                        postFix[postFixIdx] = '/';
                        postFixIdx++;
                        if (!isFirst) {
                            unPop = 1;
                            //printf("here\n");
                        }
                    }
                    while (popNext[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        postFixIdx++;
                        popNext[parenNum]--;
                        parenOps[parenNum]--;
                    }
                    parenNum++;
                }
                if (input[idx] == ')') {
                    parenNum--;
                }
                if (parenNum != 0) {
                    continue;
                }
                if (typeChar(input[idx]) == 1) {
                    postFix[postFixIdx] = input[idx];
                    postFixIdx++;
                    if (popNext[parenNum]) {
                        //printf("%c %d\n", input[idx], idx);
                        while (parenOps[parenNum] > 0) {
                            postFix[postFixIdx] = pop(opStack, &opStackTop);
                            popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                            parenOps[parenNum]--;
                            postFixIdx++;
                        }
                    }
                    postFix[postFixIdx] = '/';
                    postFixIdx++;
                    if (!lastCharOp) {
                        push('/', opStack, &opStackTop);
                        parenOps[parenNum]++;
                        popNext[parenNum] = 1;
                    } // account for x2                
                    lastCharOp = 0;
                } else if (typeChar(input[idx]) == 2) {
                    if (input[idx] != '+' && input[idx] != '-') {
                        push(oppositeOp(input[idx]), opStack, &opStackTop);
                        parenOps[parenNum]++;
                        //printf("%c\n", input[idx]);
                        popNext[parenNum] = 1;
                        //printf("paren Num: %d\n", parenNum);
                    } else {
                        //printf("%d %c %d %d\n", idx, input[idx], parenNum, opStackTop);
                        while (parenOps[parenNum] > 0) {
                            //printf("popped %d %c %d %d\n", idx, input[idx], parenNum, opStackTop);
                            postFix[postFixIdx] = pop(opStack, &opStackTop);
                            //printf("%c\n", postFix[postFixIdx]);
                            postFixIdx++;
                            parenOps[parenNum]--;
                        }
                        popNext[parenNum] = 0;
                        if (input[idx] == '-') {
                            postFix[postFixIdx] = '0';
                            postFixIdx++;
                            postFix[postFixIdx] = '1';
                            postFixIdx++;
                            postFix[postFixIdx] = '-';
                            postFixIdx++;
                            postFix[postFixIdx] = '/';
                            postFixIdx++;
                        }
                        //printf("%d %c\n", idx, input[idx]);
                        parenOps[parenNum]++;
                    }
                    lastCharOp = 1;
                } else if (typeChar(input[idx]) == 3) {
                    postFix[postFixIdx] = input[idx];
                    postFixIdx++;
                    int orderCount = 0;
                    while(input[idx + 1] == '\'') {
                        postFix[postFixIdx] = input[idx+1];
                        postFixIdx++;
                        idx++;
                        orderCount++;
                    }
                    if (popNext[parenNum]) {
                        //printf("%c %d\n", input[idx], idx);
                        while (parenOps[parenNum] > 0) {
                            postFix[postFixIdx] = pop(opStack, &opStackTop);
                            popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                            parenOps[parenNum]--;
                            postFixIdx++;
                        }
                    }
                    if (orderCount == derivativeCount) {
                        continue;
                    }
                    postFix[postFixIdx] = '/';
                    postFixIdx++;
                    /*if (!lastCharOp) {
                        push('/', opStack, &opStackTop);
                        parenOps[parenNum]++;
                        popNext[parenNum] = 1;
                    } // account for 2x */
                    lastCharOp = 0;
                }
                continue;
            }
            else {
            //printf("idx: %d, %c, %d %d\n",idx, input[idx], parenNum, isFirst);
            if (isFirst && input[idx] == '-') {
                push('+', opStack, &opStackTop);
                parenOps[parenNum]++;
                isFirst = 0;
                continue;
            } else if (isFirst && parenNum == 0) {
                push('+', opStack, &opStackTop);
                parenOps[parenNum]++;
                //printf("%d %c %d + %d %d\n", idx, input[idx], parenNum, parenOps[parenNum], opStackTop);
                isFirst = 0;
            } else {
                isFirst = 0;
            }
            charType = typeChar(input[idx]);
            if (charType == 1) { //number just add it to the post fix, but add a * operator if it wasn't after an operator
                //printf("idx: %d, %c, %d\n",idx, input[idx], parenNum);
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                //printf("%d %c\n", postFixIdx-1, postFix[postFixIdx-1]);
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum] = 1;
                } // account for x2                
                lastCharOp = 0;
                continue;
            } else if (charType == 2) { // operator if it is a +,-, pop all of the current operators, then push the +,-
                if (input[idx] != '+' && input[idx] != '-') {
                    push(input[idx], opStack, &opStackTop);
                    parenOps[parenNum]++;
                    //printf("%c\n", input[idx]);
                    popNext[parenNum] = 1;
                    //printf("paren Num: %d\n", parenNum);
                } else {
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        postFixIdx++;
                        parenOps[parenNum]--;
                    }
                    popNext[parenNum] = 0;
                    if (parenNum == 0) {
                        push(oppositeOp(input[idx]), opStack, &opStackTop);
                    } else {
                        push(input[idx], opStack, &opStackTop);
                        //printf("%d %c\n", idx, input[idx]);
                    }
                    parenOps[parenNum]++;
                }
                lastCharOp = 1;
                continue;
            } else if (charType == 3) { // variable add the variable and all derivatives, and if it wasnt right before an operator, add a *
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                while(input[idx + 1] == '\'') {
                    postFix[postFixIdx] = input[idx+1];
                    postFixIdx++;
                    idx++;
                }
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum] = 1;
                } // account for 2x
                lastCharOp = 0;
                continue;
            } else if (input[idx] == '(') { // if it wasn't an operator before, add *, then enter a parenthesis
                if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                if (!lastCharOp) {
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                    popNext[parenNum]++;
                } // account for 2(x)
                parenNum++;
                parenOps[parenNum] = 0; // enter a parenthesis
                popNext[parenNum] = 0;
                lastCharOp = 1;
                isFirst = 1;
                numberParen++;
                skipParen[numberParen] = 1;
                continue;
            } else if (input[idx] == ')') { // pop all operators inside the parenthesis and leave parenthesis
                while (parenOps[parenNum] > 0) {
                    postFix[postFixIdx] = pop(opStack, &opStackTop);
                    postFixIdx++;
                    parenOps[parenNum]--;
                }
                parenNum--;
               if (popNext[parenNum]) {
                    //printf("%c %d\n", input[idx], idx);
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        popNext[parenNum] = 0; // do x^2*... instead of x^(2*...)
                        parenOps[parenNum]--;
                        postFixIdx++;
                    }
                }
                popNext[parenNum] = 0;
                lastCharOp = 0;
                continue;
            }
            /*if (input[idx] == '(') {
                while (popNext[parenNum] > 0) {
                    postFix[postFixIdx] = pop(opStack, &opStackTop);
                    postFixIdx++;
                    popNext[parenNum]--;
                    parenOps[parenNum]--;
                }
                parenNum++;
            }
            if (input[idx] == ')') {
                parenNum--;
            }
            if (parenNum != 0) {
                continue;
            }
            if (isFirst && (typeChar(input[idx]) == 1 || typeChar(input[idx]) == 3)) {
                push('+', opStack, &opStackTop);
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                parenOps[parenNum]++;
                isFirst = 0;
                lastCharOp = 0;
                continue;
            } else {
                isFirst = 0;
            }
            if (typeChar(input[idx]) == 1 || typeChar(input[idx]) == 3) {
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                while (input[idx + 1] == '\'') {
                    idx++;
                    postFix[postFixIdx] = input[idx];
                    postFixIdx++;
                }
                while (popNext[parenNum] > 0) {
                    postFix[postFixIdx] = pop(opStack, &opStackTop);
                    postFixIdx++;
                    popNext[parenNum]--;
                    parenOps[parenNum]--;
                }
                if (!lastCharOp) {
                    popNext[parenNum]++;
                    push('*', opStack, &opStackTop);
                    parenOps[parenNum]++;
                }
                lastCharOp = 0;
            } else if (typeChar(input[idx]) == 2) {
                if (input[idx] == '+' || input[idx] == '-') {
                    while (parenOps[parenNum] > 0) {
                        postFix[postFixIdx] = pop(opStack, &opStackTop);
                        postFixIdx++;
                        parenOps[parenNum]--;
                    }
                    push(oppositeOp(input[idx]), opStack, &opStackTop);
                    parenOps[parenNum]++;
                    lastOp = input[idx];
                } else {
                    popNext[parenNum]++;
                    push(input[idx], opStack, &opStackTop);
                    parenOps[parenNum]++;
                }
                lastCharOp = 1;
            }
*/       
        }
        }
        
        while (parenOps[parenNum] > 0) {
            int character = pop(opStack, &opStackTop);
            if (character != 0) {
                postFix[postFixIdx];
                postFixIdx++;
            }
            parenOps[parenNum]--;
        }
    }
    //this finishes the last chunk
    /*if (yIdx > -1) {
        idx = yIdx;
        if (input[idx] == '-') {
            postFix[postFixIdx] = '0';
            postFixIdx++;
            postFix[postFixIdx] = '1';
            postFixIdx++;
            postFix[postFixIdx] = '-';
            postFixIdx++;
            postFix[postFixIdx] = '/';
            postFixIdx++;
        }
        idx++;
        while (input[idx] != '+' && input[idx] != '-' && idx < idxNum) {
            if (input[idx] == ' ') {
                idx++;
                continue;
            }
            charType = typeChar(input[idx]);
            if (charType == 1) {
                postFix[postFixIdx] = input[idx];
                postFixIdx++;
                postFix[postFixIdx] = '/';
                postFixIdx++;
            } else if (charType == 2) {

            } else if (charType == 3) {
                int primeCount = -1;
                if (input[idx] == 'y') {
                    primeCount = 0;
                    while (input[idx + 1 + primeCount] == '\'') {
                        primeCount++;
                    }
                    if (primeCount == derivativeCount) {
                        idx++;
                        while (input[idx] == '\'') {
                            idx++;
                        }
                        continue;
                    } else {
                        postFix[postFixIdx] = input[idx];
                        postFixIdx++;
                        while (input[idx + 1] == '\'') {
                            idx++;
                            postFix[postFixIdx] = input[idx];
                            postFixIdx++;
                        }
                        postFix[postFixIdx] = '/';
                        postFixIdx++;
                    }
                } else {
                    postFix[postFixIdx] = input[idx];
                    postFixIdx++;
                    postFix[postFixIdx] = '/';
                    postFixIdx++;
                }
            }
            idx++;
        }
    }
    /*f (lastChar) {
        postFix[postFixIdx] = '/';
        postFixIdx++;
    }*/

    postFix[postFixIdx] = '\0';
    return;
}

int interpret(char *input, int inputIdxNum, char *postFix) {
    if (!checkValidity(input, inputIdxNum)) {
        printf("Not a valid equation");
        return 0;
    }

    int eqType = equationType(input, inputIdxNum);    
    char oneSideInput[INPUT_SIZE];
    int oneSideIdxNum = eqOneSide(input, oneSideInput, inputIdxNum, eqType);
    printf("%s\n", oneSideInput);

    generatePostFix(oneSideInput, oneSideIdxNum, postFix);
    printf("%s\n", postFix);

    return 1;
}