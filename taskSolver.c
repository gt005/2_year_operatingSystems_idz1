int taskSolver(char *inputString, int size) {
    int count = 0;
    int i = 0;
    while (i < size) {
        if (inputString[i] >= '0' && inputString[i] <= '9') {
            count++;
            while (inputString[i] >= '0' && inputString[i] <= '9') {
                i++;
            }
        }
        i++;
    }
    return count;
}