#include "cshell.h"

t_args *create_args_list(char **array)
{
    t_args *head;
    t_args *current;
    t_args *new;
    int i;

    if (!array || !array[0])
        return (NULL);
    
    head = NULL;
    i = 0;
    while (array[i])
    {
        new = (t_args *)dl_calloc(1, sizeof(t_args));
        if (!new)
        {
            free_args_list(head);
            return (NULL);
        }
        
        new->arg = dl_strdup(array[i]);
        if (!new->arg)
        {
            free(new);
            free_args_list(head);
            return (NULL);
        }
        
        new->next = NULL;
        
        if (!head)
            head = new;
        else
            current->next = new;
        
        current = new;
        i++;
    }
    
    return (head);
}

char **convert_args_to_array(t_args *list)
{
    char **array;
    t_args *current;
    int count;
    int i;

    if (!list)
        return (NULL);
    
    // Count arguments
    count = 0;
    current = list;
    while (current)
    {
        count++;
        current = current->next;
    }
    
    // Allocate array
    array = (char **)dl_calloc(count + 1, sizeof(char *));
    if (!array)
        return (NULL);
    
    // Fill array
    current = list;
    i = 0;
    while (current && i < count)
    {
        array[i] = dl_strdup(current->arg);
        if (!array[i])
        {
            free_string_array(array);
            return (NULL);
        }
        current = current->next;
        i++;
    }
    
    return (array);
}

void free_args_list(t_args *list)
{
    t_args *current;
    t_args *next;

    current = list;
    while (current)
    {
        next = current->next;
        if (current->arg)
            free(current->arg);
        free(current);
        current = next;
    }
}

void init_help(t_split *split)
{
    split->i = 0;
    split->j = 0;
    split->k = 0;
    split->l = 0;
    split->m = 0;
    split->n = 0;
    split->o = 0;
    split->p = 0;
    split->q = 0;
    split->r = 0;
    split->s = 0;
    split->t = 0;
    split->u = 0;
    split->v = 0;
    split->w = 0;
    split->x = 0;
    split->y = 0;
    split->z = 0;
}
