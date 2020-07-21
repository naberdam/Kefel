//
// Created by nerya on 04/12/2019.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define REGISTER_EDI "%edi"
#define REGISTER_EAX "%eax"
#define REGISTER_ECX "%ecx"
#define MOVL "movl %s,%s\n"
#define SHL "shl $%d,%s\n"
#define LEAL "leal (%s,%s,%d),%s\n"
#define ADDL "addl %s,%s\n"
#define SUBL "subl %s,%s\n"

int exponentOfTwo(int num) {
    int i = 0;
    for (i; num != 0; ++i) {
        if (num % 2 == 1) {
            break;
        }
        num /= 2;
    }
    return i;
}

void printNegInFile(FILE *inputTxt, bool negativeNumber) {
    if (negativeNumber) {
        fprintf(inputTxt, "neg %s\n", REGISTER_EAX);
    }
}

void positiveNumberToFile(FILE *inputTxt, int i, int j, bool isNotFirstTime) {
    if (isNotFirstTime) {
        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
        fprintf(inputTxt, SHL, i, REGISTER_ECX);
        fprintf(inputTxt, ADDL, REGISTER_ECX, REGISTER_EAX);
        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
        fprintf(inputTxt, SHL, j, REGISTER_ECX);
        fprintf(inputTxt, SUBL, REGISTER_ECX, REGISTER_EAX);
    } else {
        fprintf(inputTxt, SHL, i, REGISTER_EAX);
        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
        fprintf(inputTxt, SHL, j, REGISTER_ECX);
        fprintf(inputTxt, SUBL, REGISTER_ECX, REGISTER_EAX);
    }
}

void negativeNumberToFile(FILE *inputTxt, int i, int j, bool isNotFirstTime) {
    if (isNotFirstTime) {
        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
        fprintf(inputTxt, SHL, j, REGISTER_ECX);
        fprintf(inputTxt, ADDL, REGISTER_ECX, REGISTER_EAX);
        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
        fprintf(inputTxt, SHL, i, REGISTER_ECX);
        fprintf(inputTxt, SUBL, REGISTER_ECX, REGISTER_EAX);
    } else {
        fprintf(inputTxt, SHL, j, REGISTER_EAX);
        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
        fprintf(inputTxt, SHL, i, REGISTER_ECX);
        fprintf(inputTxt, SUBL, REGISTER_ECX, REGISTER_EAX);
    }
}

int countLinesToFile(int num, FILE *inputTxt, bool negIsInLastLine) {
    int i = 0, j = 0, mask = 1, k, countLine = 0;
    bool alreadyWasOne = false, bitNumberZero = false, isNotFirstTime = false;
    for (i; i < 32; ++i) {
        if (num & mask) {
            alreadyWasOne = true;
            mask <<= 1;
            continue;
        }
        if (alreadyWasOne) {
            if (i >= (j + 3)) {
                if (isNotFirstTime) {
                    countLine += 6;
                } else {
                    countLine += 4;
                }
                isNotFirstTime = true;
            } else {
                for (k = j; k <= i - 1; ++k) {
                    if (k == 0) {
                        if (negIsInLastLine) {
                            bitNumberZero = true;
                        } else {
                            countLine += 1;
                            isNotFirstTime = true;
                        }
                        continue;
                    }
                    if (isNotFirstTime) {
                        countLine += 3;
                    } else {
                        countLine += 1;
                    }
                    isNotFirstTime = true;
                }
            }
            j = i;
            alreadyWasOne = false;
        }
        mask <<= 1;
        j++;
    }
    if (bitNumberZero) {
        if (negIsInLastLine) {
            countLine += 2;
        }
    }
    return countLine;
}

void printToAssembly(int num, FILE *inputTxt, bool positiveNumber) {
    int i = 0, j = 0, mask = 1, k;
    bool alreadyWasOne = false, bitNumberZero = false, isNotFirstTime = false;
    for (i; i < 32; ++i) {
        if (num & mask) {
            alreadyWasOne = true;
            mask <<= 1;
            continue;
        }
        if (alreadyWasOne) {
            if (i >= (j + 3)) {
                if (positiveNumber) {
                    positiveNumberToFile(inputTxt, i, j, isNotFirstTime);
                } else {
                    negativeNumberToFile(inputTxt, i, j, isNotFirstTime);
                }
                isNotFirstTime = true;
            } else {
                for (k = j; k <= i - 1; ++k) {
                    if (k == 0) {
                        if (positiveNumber) {
                            bitNumberZero = true;
                        } else {
                            printNegInFile(inputTxt, !positiveNumber);
                            isNotFirstTime = true;
                        }
                        continue;
                    }
                    if (isNotFirstTime) {
                        fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_ECX);
                        fprintf(inputTxt, SHL, k, REGISTER_ECX);
                        if (positiveNumber) {
                            fprintf(inputTxt, ADDL, REGISTER_ECX, REGISTER_EAX);
                        } else {
                            fprintf(inputTxt, SUBL, REGISTER_ECX, REGISTER_EAX);
                        }
                    } else {
                        fprintf(inputTxt, SHL, k, REGISTER_EAX);
                    }
                    isNotFirstTime = true;
                }
            }
            j = i;
            alreadyWasOne = false;
        }
        mask <<= 1;
        j++;
    }
    if (bitNumberZero) {
        if (positiveNumber) {
            fprintf(inputTxt, LEAL, REGISTER_EDI, REGISTER_EAX, 1, REGISTER_EAX);
        } /*else {
            fprintf(inputTxt, SUBL, REGISTER_EDI, REGISTER_EAX);
        }*/
    }
}

int main(int argc, char *argv[]) {
    int k = atoi(argv[1]);
    int countExponentOfTwo, kTemp, power = 1;
    bool negativeNumber = false, alreadyDone = false;
    FILE *inputTxt = fopen("kefel.s", "w+");
    if ((inputTxt) == NULL) {
        printf("error\n");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    fprintf(inputTxt, ".section .text\n");
    fprintf(inputTxt, ".global kefel\n");
    fprintf(inputTxt, "kefel: ");
    fprintf(inputTxt, MOVL, REGISTER_EDI, REGISTER_EAX);
    if (k == 0 || k == 1 || k == -1) {
        switch (k) {
            case 0:
                fprintf(inputTxt, SUBL, REGISTER_EAX, REGISTER_EAX);
                break;
            case 1:
                break;
            case -1:
                fprintf(inputTxt, "neg %s\n", REGISTER_EAX);
                break;
            default:
                break;
        }
    } else {
        if (k < 0) {
            k *= -1;
            negativeNumber = true;
        }
        countExponentOfTwo = exponentOfTwo(k);
        if (countExponentOfTwo) {
            power <<= countExponentOfTwo;
            kTemp = k / power;
            if (kTemp == 3 || kTemp == 5 || kTemp == 9) {
                fprintf(inputTxt, SHL, countExponentOfTwo, REGISTER_EAX);
                fprintf(inputTxt, LEAL, REGISTER_EAX, REGISTER_EAX, kTemp - 1, REGISTER_EAX);
                printNegInFile(inputTxt, negativeNumber);
                alreadyDone = true;
            } else if (kTemp == 1) {
                fprintf(inputTxt, SHL, countExponentOfTwo, REGISTER_EAX);
                printNegInFile(inputTxt, negativeNumber);
                alreadyDone = true;
            }
        }
        if (!alreadyDone) {
            if (k == 3 || k == 5 || k == 9) {
                fprintf(inputTxt, LEAL, REGISTER_EAX, REGISTER_EAX, k - 1, REGISTER_EAX);
                printNegInFile(inputTxt, negativeNumber);
            } else {
                if (negativeNumber) {
                    if (countLinesToFile(k, inputTxt, false)
                        <= countLinesToFile(k, inputTxt, true)) {
                        printToAssembly(k, inputTxt, false);
                    } else {
                        printToAssembly(k, inputTxt, true);
                        printNegInFile(inputTxt, negativeNumber);
                    }
                } else {
                    printToAssembly(k, inputTxt, true);
                }
            }
        }

    }
    fprintf(inputTxt, "ret\n");
    fclose(inputTxt);
    return 0;
}
