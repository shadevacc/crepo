#include "../main.h"

void l0_merge(int* nums1, int nums1Size, int m,
             int* nums2, int nums2Size, int n)
{
    char ch;
    printf("Enter a single character: ");
    scanf("%c", &ch);

    switch(ch)
    {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
        {
            printf("character %c is VOWEL\n", ch);
            break;
        }
        default:
        {
            printf("character %c is consonant\n", ch);
        }
    }
}