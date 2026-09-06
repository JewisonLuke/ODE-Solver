#define INPUT_SIZE 100
#define numValids 19

int findIdx(char *input, int inputIdxNum, char op) {
    for (int i = 0; i < inputIdxNum; i++) {
        if (input[i] == op) {
            return i;
        }
    }
    return -1;
}

int checkValidity(char *input, int inputIdxNum) {
    char validChars[numValids] = "1234567890xy -+/*^"; // last char is the \0 char
    int valid = 1;
    int numEqual = 0;

    int parenCount = 0;

    for (int i = 0; i < inputIdxNum; i++) {
        int validChar = 0;
        if (input[i] == '=') { // = checked separately
            numEqual++;
            if (parenCount != 0) {
                valid = 0;
                break; // can't have (3x = 5)
            }
            parenCount = 0;
            continue;
        }
        else if (input[i] == '(') {
            parenCount++;
            continue;
        }
        else if (input[i] == ')') {
            parenCount--;
            if (parenCount < 0) {
                valid = 0;
                break;
            }
            continue;
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
            continue;
        }
        for (int j = 0; j < numValids - 1; j++) {
            if (input[i] == validChars[j]) {
                validChar = 1;
                if (i == 0 && j > 13) {
                    valid = 0; // first character must be a number, -, space, x, or y
                }
                break;
            }
        }
        if (!validChar) {
            valid = 0;
            break;
        }
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

    for (int i = 0; i < inputIdxNum; i++) {
        if ((input[i] == '+' || input[i] == '-' || input[i] == '=') && i != 0) {
            chunks[numChunks] = i - 1;
            numChunks += 1;
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

void eqOneSide(char *input, char *oneSideInput, int inputIdxNum) {
    int equalIdx = findIdx(input, inputIdxNum,'=');
    int chunks[INPUT_SIZE/2];
    int numChunks = findChunks(input, inputIdxNum, chunks);
    int idx = 0;

    addChunk(input, oneSideInput, 0, chunks[0], idx);
    idx += chunks[0] + 1;
    addChunk(input, oneSideInput, chunks[1] + 1, chunks[2], idx);
    for (int i = 1; i <= numChunks; i++) {
        addChunk(input, oneSideInput, chunks[i-1] + 1, chunks[i], idx);
        idx += chunks[i] - chunks[i-1];
    }

    return;
}

int interpret(char *input, int inputIdxNum) {
    if (!checkValidity(input, inputIdxNum)) {
        printf("Not a valid equation");
        return 0;
    }

    int eqType = equationType(input, inputIdxNum);    

    char oneSideInput[INPUT_SIZE];
    eqOneSide(input, oneSideInput, inputIdxNum);
    printf("%s\n", oneSideInput);

    return 1;
}