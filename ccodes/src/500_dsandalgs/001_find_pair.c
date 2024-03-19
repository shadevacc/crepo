/*
 * Given an limited size array. Find a pair whose sum is equal to target.
 */

#include "../main.h"
#include "dsandalgs.h"
void find_pair_call(int nums[], int nums_arr_size, int pair_sum_target)
{
    int found = 0;
    for (int i=0; i<nums_arr_size; i++)
    {
        for (int j=1; j<=nums_arr_size; j++)
        {
            if (nums[i] + nums[j] == pair_sum_target)
            {
                printf("Pair found = (%d, %d)\n", nums[i], nums[j]);
                found++;
            }
        }
    }
    if (found <= 0)
        printf("Pair not found\n");
}

int find_pair()
{
    int nums[]={ 8, 7, 2, 5, 3, 1 };
    int size_of_nums;
    int target_sum;

    target_sum = 30;
    printf("size_of_nums: %d\n", size_of_nums);
    find_pair_call(nums, size_of_nums, target_sum);

    return EXIT_SUCCESS;
}