#ifndef STREAMCLASSIFIER_HASH_FUNCTIONS_H
#define STREAMCLASSIFIER_HASH_FUNCTIONS_H

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

uint32_t BOB(uint32_t key, uint32_t len) {
    const char * str = (const char *)&key;
    //register ub4 a,b,c,len;
    uint32_t a, b, c;
    uint32_t initval = 2;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = initval;         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12) {
        a += (str[0] + ((uint32_t) str[1] << 8) + ((uint32_t) str[2] << 16) + ((uint32_t) str[3] << 24));
        b += (str[4] + ((uint32_t) str[5] << 8) + ((uint32_t) str[6] << 16) + ((uint32_t) str[7] << 24));
        c += (str[8] + ((uint32_t) str[9] << 8) + ((uint32_t) str[10] << 16) + ((uint32_t) str[11] << 24));
        mix(a, b, c);
        str += 12;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch (len)              /* all the case statements fall through */{
    case 11: c += ((uint32_t) str[10] << 24);
        // fall through
    case 10: c += ((uint32_t) str[9] << 16);
        // fall through
    case 9 : c += ((uint32_t) str[8] << 8);
        /* the first byte of c is reserved for the length */
        // fall through
    case 8 : b += ((uint32_t) str[7] << 24);
        // fall through
    case 7 : b += ((uint32_t) str[6] << 16);
        // fall through
    case 6 : b += ((uint32_t) str[5] << 8);
        // fall through
    case 5 : b += str[4];
        // fall through
    case 4 : a += ((uint32_t) str[3] << 24);
        // fall through
    case 3 : a += ((uint32_t) str[2] << 16);
        // fall through
    case 2 : a += ((uint32_t) str[1] << 8);
        // fall through
    case 1 : a += str[0];
        /* case 0: nothing left to add */
        // fall through
    }
    mix(a, b, c);
    /*-------------------------------------------- report the result */
    return c;
}

#endif //STREAMCLASSIFIER_HASH_FUNCTIONS_H
