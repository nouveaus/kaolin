inline char dtoc(int i) { return i + 48; }

// code review 
inline void itos(int i, char *s, int size_s) {
    // get least significant number
    int j = 0;
    int neg = 0;
    char buf[size_s];

    if (i == 0) {
        s[0] = '0';
        s[1] = 0;
        return;
    } else if (i < 0) {
        neg = 1;
        i *= -1;
    } 

    // reverse order
    while (j < size_s && i > 0) {
        int rem = i % 10;

        buf[j] = dtoc(rem);
        i /= 10;
        j++;
    }

    int size = j - 1;
    j = 0;
    if (neg) {
        s[0] = '-';
        j++;
        size++;
    }

    while (j <= size) {
        s[j] = buf[size - j];
        j++;
    }
    s[j] = 0;
}
